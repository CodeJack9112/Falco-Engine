#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#include "Engine.h"
#include "NavigationManager.h"
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include "Pathfinding/InputGeom.h"
#include "Pathfinding/Detour/Include/DetourNavMeshBuilder.h"
#include "Pathfinding/Detour/Include/DetourNavMeshQuery.h"
#include "Pathfinding/DebugUtils/Include/RecastDump.h"
#include "Pathfinding/DetourCrowd/Include/DetourCrowd.h"
#include "Pathfinding/Detour/Include/DetourCommon.h"
#include "Pathfinding/DetourTileCache/Include/DetourTileCache.h"
#include "Pathfinding/DetourTileCache/Include/DetourTileCacheBuilder.h"
#include "fastlz/fastlz.h"

#include "NavMeshAgent.h"
#include "NavMeshObstacle.h"
#include "ZipHelper.h"

#include "MeshUtilities.h"
#include "IO.h"

#include "TerrainManager.h"

#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_iarchive.hpp>

static const int MAX_LAYERS = 32;
static const int EXPECTED_LAYERS_PER_TILE = 4;

struct FastLZCompressor : public dtTileCacheCompressor
{
	virtual int maxCompressedSize(const int bufferSize)
	{
		return (int)(bufferSize * 1.05f);
	}

	virtual dtStatus compress(const unsigned char* buffer, const int bufferSize,
		unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize)
	{
		*compressedSize = fastlz_compress((const void* const)buffer, bufferSize, compressed);
		return DT_SUCCESS;
	}

	virtual dtStatus decompress(const unsigned char* compressed, const int compressedSize,
		unsigned char* buffer, const int maxBufferSize, int* bufferSize)
	{
		*bufferSize = fastlz_decompress(compressed, compressedSize, buffer, maxBufferSize);
		return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
	}
};

struct LinearAllocator : public dtTileCacheAlloc
{
	unsigned char* buffer;
	size_t capacity;
	size_t top;
	size_t high;

	LinearAllocator(const size_t cap) : buffer(0), capacity(0), top(0), high(0)
	{
		resize(cap);
	}

	~LinearAllocator()
	{
		dtFree(buffer);
	}

	void resize(const size_t cap)
	{
		if (buffer) dtFree(buffer);
		buffer = (unsigned char*)dtAlloc(cap, DT_ALLOC_PERM);
		capacity = cap;
	}

	virtual void reset()
	{
		high = dtMax(high, top);
		top = 0;
	}

	virtual void* alloc(const size_t size)
	{
		if (!buffer)
			return 0;
		if (top + size > capacity)
			return 0;
		unsigned char* mem = &buffer[top];
		top += size;
		return mem;
	}

	virtual void free(void* /*ptr*/)
	{
		// Empty
	}
};

struct MeshProcess : public dtTileCacheMeshProcess
{
	InputGeom* m_geom;

	inline MeshProcess() : m_geom(0)
	{
	}

	inline void init(InputGeom* geom)
	{
		m_geom = geom;
	}

	virtual void process(struct dtNavMeshCreateParams* params,
		unsigned char* polyAreas, unsigned short* polyFlags)
	{
		// Update poly flags from areas.
		for (int i = 0; i < params->polyCount; ++i)
		{
			if (polyAreas[i] == DT_TILECACHE_WALKABLE_AREA)
				polyAreas[i] = SAMPLE_POLYAREA_GROUND;

			if (polyAreas[i] == SAMPLE_POLYAREA_GROUND ||
				polyAreas[i] == SAMPLE_POLYAREA_GRASS ||
				polyAreas[i] == SAMPLE_POLYAREA_ROAD)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (polyAreas[i] == SAMPLE_POLYAREA_WATER)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (polyAreas[i] == SAMPLE_POLYAREA_DOOR)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}

		// Pass in off-mesh connections.
		if (m_geom)
		{
			params->offMeshConVerts = m_geom->getOffMeshConnectionVerts();
			params->offMeshConRad = m_geom->getOffMeshConnectionRads();
			params->offMeshConDir = m_geom->getOffMeshConnectionDirs();
			params->offMeshConAreas = m_geom->getOffMeshConnectionAreas();
			params->offMeshConFlags = m_geom->getOffMeshConnectionFlags();
			params->offMeshConUserID = m_geom->getOffMeshConnectionId();
			params->offMeshConCount = m_geom->getOffMeshConnectionCount();
		}
	}
};

struct TileCacheData
{
	unsigned char* data;
	int dataSize;
};

struct RasterizationContext
{
	RasterizationContext() :
		solid(0),
		triareas(0),
		lset(0),
		chf(0),
		ntiles(0)
	{
		memset(tiles, 0, sizeof(TileCacheData) * MAX_LAYERS);
	}

	~RasterizationContext()
	{
		rcFreeHeightField(solid);
		delete[] triareas;
		rcFreeHeightfieldLayerSet(lset);
		rcFreeCompactHeightfield(chf);
		for (int i = 0; i < MAX_LAYERS; ++i)
		{
			dtFree(tiles[i].data);
			tiles[i].data = 0;
		}
	}

	rcHeightfield* solid;
	unsigned char* triareas;
	rcHeightfieldLayerSet* lset;
	rcCompactHeightfield* chf;
	TileCacheData tiles[MAX_LAYERS];
	int ntiles;
};

struct TileCacheBuildContext
{
	inline TileCacheBuildContext(struct dtTileCacheAlloc* a) : layer(0), lcset(0), lmesh(0), alloc(a) {}
	inline ~TileCacheBuildContext() { purge(); }
	void purge()
	{
		dtFreeTileCacheLayer(alloc, layer);
		layer = 0;
		dtFreeTileCacheContourSet(alloc, lcset);
		lcset = 0;
		dtFreeTileCachePolyMesh(alloc, lmesh);
		lmesh = 0;
	}
	struct dtTileCacheLayer* layer;
	struct dtTileCacheContourSet* lcset;
	struct dtTileCachePolyMesh* lmesh;
	struct dtTileCacheAlloc* alloc;
};

bool m_filterLowHangingObstacles = false;
bool m_filterLedgeSpans = false;
bool m_filterWalkableLowHeightSpans = false;

int NavigationManager::rasterizeTileLayers(const int tx, const int ty, const rcConfig& cfg, TileCacheData* tiles, const int maxTiles, const float* verts, const int nverts, const int* tris, const int ntris)
{
	FastLZCompressor comp;
	RasterizationContext rc;

	rcChunkyTriMesh* chunkyMesh = new rcChunkyTriMesh();
	rcCreateChunkyTriMesh(verts, tris, ntris, 2, chunkyMesh);

	// Tile bounds.
	const float tcs = cfg.tileSize * cfg.cs;

	rcConfig tcfg;
	memcpy(&tcfg, &cfg, sizeof(tcfg));

	tcfg.bmin[0] = cfg.bmin[0] + tx * tcs;
	tcfg.bmin[1] = cfg.bmin[1];
	tcfg.bmin[2] = cfg.bmin[2] + ty * tcs;
	tcfg.bmax[0] = cfg.bmin[0] + (tx + 1) * tcs;
	tcfg.bmax[1] = cfg.bmax[1];
	tcfg.bmax[2] = cfg.bmin[2] + (ty + 1) * tcs;
	tcfg.bmin[0] -= tcfg.borderSize * tcfg.cs;
	tcfg.bmin[2] -= tcfg.borderSize * tcfg.cs;
	tcfg.bmax[0] += tcfg.borderSize * tcfg.cs;
	tcfg.bmax[2] += tcfg.borderSize * tcfg.cs;

	// Allocate voxel heightfield where we rasterize our input data to.
	rc.solid = rcAllocHeightfield();
	if (!rc.solid)
	{
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		delete chunkyMesh;
		return 0;
	}
	if (!rcCreateHeightfield(m_ctx, *rc.solid, tcfg.width, tcfg.height, tcfg.bmin, tcfg.bmax, tcfg.cs, tcfg.ch))
	{
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		delete chunkyMesh;
		return 0;
	}

	// Allocate array that can hold triangle flags.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	rc.triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
	if (!rc.triareas)
	{
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
		delete chunkyMesh;
		return 0;
	}

	float tbmin[2], tbmax[2];
	tbmin[0] = tcfg.bmin[0];
	tbmin[1] = tcfg.bmin[2];
	tbmax[0] = tcfg.bmax[0];
	tbmax[1] = tcfg.bmax[2];
	int cid[512];// TODO: Make grow when returning too many items.
	const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
	if (!ncid)
	{
		delete chunkyMesh;
		return 0; // empty
	}

	for (int i = 0; i < ncid; ++i)
	{
		const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
		const int* tris = &chunkyMesh->tris[node.i * 3];
		const int ntris = node.n;

		memset(rc.triareas, 0, ntris * sizeof(unsigned char));
		rcMarkWalkableTriangles(m_ctx, tcfg.walkableSlopeAngle,
			verts, nverts, tris, ntris, rc.triareas);

		if (!rcRasterizeTriangles(m_ctx, verts, nverts, tris, rc.triareas, ntris, *rc.solid, tcfg.walkableClimb))
		{
			delete chunkyMesh;
			return 0;
		}
	}

	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	if (m_filterLowHangingObstacles)
		rcFilterLowHangingWalkableObstacles(m_ctx, tcfg.walkableClimb, *rc.solid);
	if (m_filterLedgeSpans)
		rcFilterLedgeSpans(m_ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid);
	if (m_filterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(m_ctx, tcfg.walkableHeight, *rc.solid);


	rc.chf = rcAllocCompactHeightfield();
	if (!rc.chf)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		delete chunkyMesh;
		return 0;
	}
	if (!rcBuildCompactHeightfield(m_ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid, *rc.chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		delete chunkyMesh;
		return 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, tcfg.walkableRadius, *rc.chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		delete chunkyMesh;
		return 0;
	}

	// (Optional) Mark areas.
	/*const ConvexVolume* vols = m_geom->getConvexVolumes();
	for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
	{
		rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts,
			vols[i].hmin, vols[i].hmax,
			(unsigned char)vols[i].area, *rc.chf);
	}*/

	rc.lset = rcAllocHeightfieldLayerSet();
	if (!rc.lset)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'lset'.");
		delete chunkyMesh;
		return 0;
	}
	if (!rcBuildHeightfieldLayers(m_ctx, *rc.chf, tcfg.borderSize, tcfg.walkableHeight, *rc.lset))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build heighfield layers.");
		delete chunkyMesh;
		return 0;
	}

	rc.ntiles = 0;
	for (int i = 0; i < rcMin(rc.lset->nlayers, MAX_LAYERS); ++i)
	{
		TileCacheData* tile = &rc.tiles[rc.ntiles++];
		const rcHeightfieldLayer* layer = &rc.lset->layers[i];

		// Store header
		dtTileCacheLayerHeader header;
		header.magic = DT_TILECACHE_MAGIC;
		header.version = DT_TILECACHE_VERSION;

		// Tile layer location in the navmesh.
		header.tx = tx;
		header.ty = ty;
		header.tlayer = i;
		dtVcopy(header.bmin, layer->bmin);
		dtVcopy(header.bmax, layer->bmax);

		// Tile info.
		header.width = (unsigned char)layer->width;
		header.height = (unsigned char)layer->height;
		header.minx = (unsigned char)layer->minx;
		header.maxx = (unsigned char)layer->maxx;
		header.miny = (unsigned char)layer->miny;
		header.maxy = (unsigned char)layer->maxy;
		header.hmin = (unsigned short)layer->hmin;
		header.hmax = (unsigned short)layer->hmax;

		dtStatus status = dtBuildTileCacheLayer(&comp, &header, layer->heights, layer->areas, layer->cons,
			&tile->data, &tile->dataSize);
		if (dtStatusFailed(status))
		{
			delete chunkyMesh;
			return 0;
		}
	}

	// Transfer ownsership of tile data from build context to the caller.
	int n = 0;
	for (int i = 0; i < rcMin(rc.ntiles, maxTiles); ++i)
	{
		tiles[n++] = rc.tiles[i];
		rc.tiles[i].data = 0;
		rc.tiles[i].dataSize = 0;
	}

	delete chunkyMesh;

	return n;
}

static int calcLayerBufferSize(const int gridWidth, const int gridHeight)
{
	const int headerSize = dtAlign4(sizeof(dtTileCacheLayerHeader));
	const int gridSize = gridWidth * gridHeight;
	return headerSize + gridSize * 4;
}

BuildContext::BuildContext()
{
	resetTimers();
}

void BuildContext::doResetTimers()
{
	for (int i = 0; i < RC_MAX_TIMERS; ++i)
		m_accTime[i] = -1;
}

void BuildContext::doStartTimer(const rcTimerLabel label)
{
	m_startTime[label] = getPerfTime();
}

void BuildContext::doStopTimer(const rcTimerLabel label)
{
	const TimeVal endTime = getPerfTime();
	const TimeVal deltaTime = endTime - m_startTime[label];
	if (m_accTime[label] == -1)
		m_accTime[label] = deltaTime;
	else
		m_accTime[label] += deltaTime;
}

int BuildContext::doGetAccumulatedTime(const rcTimerLabel label) const
{
	return getPerfTimeUsec(m_accTime[label]);
}

NavigationManager::NavigationManager()
{
	m_navQuery = new dtNavMeshQuery();
	m_ctx = new BuildContext();

	m_talloc = new LinearAllocator(32000);
	m_tcomp = new FastLZCompressor;
	m_tmproc = new MeshProcess;
}

NavigationManager::~NavigationManager()
{
	delete m_navQuery;
	agentList.clear();

	dtFreeTileCache(m_tileCache);
}

struct ArraySlice
{
public:
	NavigationManager* mgr = nullptr;
	int start = 0;
	int end = 0;
	float y = 0;
	rcConfig cfg;
	float* verts = nullptr;
	int nverts = 0;
	int* tris = nullptr;
	int ntris = 0;
	dtStatus status;
	dtTileCache* m_tileCache = nullptr;
	//int m_cacheLayerCount = 0;
	//int m_cacheCompressedSize = 0;
	//int m_cacheRawSize = 0;
	dtTileCacheParams tcparams;
};

void rasterizeThread(void* param)
{
	ArraySlice* slice = (ArraySlice*)param;
	//if (!slice) return;

	for (int x = slice->start; x < slice->end; ++x)
	{
		TileCacheData tiles[MAX_LAYERS];
		memset(tiles, 0, sizeof(tiles));
		int ntiles = slice->mgr->rasterizeTileLayers(x, slice->y, slice->cfg, tiles, MAX_LAYERS, slice->verts, slice->nverts, slice->tris, slice->ntris);

		for (int i = 0; i < ntiles; ++i)
		{
			TileCacheData* tile = &tiles[i];
			slice->status = slice->m_tileCache->addTile(tile->data, tile->dataSize, DT_COMPRESSEDTILE_FREE_DATA, 0);
			if (dtStatusFailed(slice->status))
			{
				dtFree(tile->data);
				tile->data = 0;
				continue;
			}

			//slice->m_cacheLayerCount += 1;
			//slice->m_cacheCompressedSize += tile->dataSize;
			//slice->m_cacheRawSize += calcLayerBufferSize(slice->tcparams.width, slice->tcparams.height);
		}
	}

	ExitThread(0);
}

void NavigationManager::BuildNavMesh(std::function<void(int progress, int totalIter, int currentIter)> buildProgressCallback)
{
	Cleanup();

	std::vector<SceneNode*> staticNodes;
	GetSceneNodesWithEntities(GetEngine->GetSceneManager()->getRootSceneNode(), staticNodes);

	int _nverts = 0;
	int _ntris = 0;

	std::vector<float> allVerts;
	std::vector<int> allIndices;

	float _xmin = 0, _ymin = 0, _zmin = 0;
	float _xmax = 0, _ymax = 0, _zmax = 0;

	int jj = 0;

	for (auto it = staticNodes.begin(); it != staticNodes.end(); ++it)
	{
		Entity * entity = (Entity*)(*it)->getAttachedObject(0);
		MeshPtr mesh = entity->getMesh();

		if (mesh != nullptr)
		{
			size_t vertex_count;
			size_t index_count;
			Vector3 * vertices;
			unsigned long * indices;

			MeshUtilities::GetMeshInformation(entity, vertex_count, vertices, index_count, indices, (*it)->_getDerivedPosition(), (*it)->_getDerivedOrientation(), (*it)->_getDerivedScale());
			
			_nverts += vertex_count;
			_ntris += index_count / 3;

			for (int i = 0; i < vertex_count; ++i)
			{
				allVerts.push_back(vertices[i].x);
				allVerts.push_back(vertices[i].y);
				allVerts.push_back(vertices[i].z);

				allIndices.push_back(jj);
				jj += 1;

				//Find bounds min
				if (vertices[i].x < _xmin) _xmin = vertices[i].x;
				if (vertices[i].y < _ymin) _ymin = vertices[i].y;
				if (vertices[i].z < _zmin) _zmin = vertices[i].z;

				//Find bounds max
				if (vertices[i].x > _xmax) _xmax = vertices[i].x;
				if (vertices[i].y > _ymax) _ymax = vertices[i].y;
				if (vertices[i].z > _zmax) _zmax = vertices[i].z;
			}
		}
	}

	auto terrains = GetEngine->GetTerrainManager()->GetTerrainList();

	for (auto it = terrains.begin(); it != terrains.end(); ++it)
	{
		Terrain * terr = it->first;

		float* mapptr = terr->getHeightData();
		float WorldSize = terr->getWorldSize();
		int MapSize = terr->getSize();

		float DeltaPos = (WorldSize / 2.0f);

		Ogre::AxisAlignedBox tileBox = terr->getWorldAABB();
		float DeltaX = tileBox.getMinimum().x;
		float DeltaZ = tileBox.getMaximum().z;

		float Scale = WorldSize / (float)(MapSize - 1);

		std::vector<Vector3> vecs;

		int max = MapSize;
		int z = 0;
		int tri = 0;

		for (int z = 0; z < MapSize; ++z)
		{
			for (int x = 0; x < MapSize; ++x)
			{
				Vector3 vv = Vector3((Scale * x) + DeltaX, mapptr[(MapSize * z) + x], (Scale * -z) + DeltaZ);
				vecs.push_back(vv);

				//Find bounds min
				if (vv.x < _xmin) _xmin = vv.x;
				if (vv.y < _ymin) _ymin = vv.y;
				if (vv.z < _zmin) _zmin = vv.z;

				//Find bounds max
				if (vv.x > _xmax) _xmax = vv.x;
				if (vv.y > _ymax) _ymax = vv.y;
				if (vv.z > _zmax) _zmax = vv.z;
			}
		}
		
		for (int x = 0; x < MapSize * MapSize; ++x)
		{
			// skip rightmost vertices
			if ((x + 1) % MapSize == 0)
			{
				continue;
			}

			// make a square of 2 triangles
			Vector3 v1 = vecs[x];
			Vector3 v2 = vecs[x + 1];
			Vector3 v3 = vecs[x + MapSize];
			Vector3 v4 = vecs[x + 1];
			Vector3 v5 = vecs[x + 1 + MapSize];
			Vector3 v6 = vecs[x + MapSize];

			allVerts.push_back(v1.x);
			allVerts.push_back(v1.y);
			allVerts.push_back(v1.z);
			allVerts.push_back(v2.x);
			allVerts.push_back(v2.y);
			allVerts.push_back(v2.z);
			allVerts.push_back(v3.x);
			allVerts.push_back(v3.y);
			allVerts.push_back(v3.z);
			allVerts.push_back(v4.x);
			allVerts.push_back(v4.y);
			allVerts.push_back(v4.z);
			allVerts.push_back(v5.x);
			allVerts.push_back(v5.y);
			allVerts.push_back(v5.z);
			allVerts.push_back(v6.x);
			allVerts.push_back(v6.y);
			allVerts.push_back(v6.z);

			allIndices.push_back(jj);
			allIndices.push_back(jj + 1);
			allIndices.push_back(jj + 2);
			allIndices.push_back(jj + 3);
			allIndices.push_back(jj + 4);
			allIndices.push_back(jj + 5);
			jj += 6;

			_nverts += 6;
			_ntris += 2;
		}
	}

	staticNodes.clear();

	const float* bmin = new float[3]{ _xmin, _ymin, _zmin };
	const float* bmax = new float[3]{ _xmax, _ymax, _zmax };

	const float* verts = &allVerts[0];
	const int nverts = _nverts;
	const int* tris = &allIndices[0];
	const int ntris = _ntris;

	dtStatus status;

	int tileSize = 48;
	int vertsPerPoly = 6;
	int sampleDistance = 6;
	int sampleMaxError = 1;

	m_tmproc->init(nullptr);

	// Init cache
	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, cellSize, &gw, &gh);
	const int ts = (int)tileSize;
	const int tw = (gw + ts - 1) / ts;
	const int th = (gh + ts - 1) / ts;

	// Generation params.
	rcConfig cfg;
	memset(&cfg, 0, sizeof(cfg));
	cfg.cs = cellSize;
	cfg.ch = cellHeight;
	cfg.walkableSlopeAngle = walkableSlopeAngle;
	cfg.walkableHeight = (int)ceilf(walkableHeight / cfg.ch);
	cfg.walkableClimb = (int)floorf(walkableClimb / cfg.ch);
	cfg.walkableRadius = (int)ceilf(walkableRadius / cfg.cs);
	cfg.maxEdgeLen = (int)(maxEdgeLen / cellSize);
	cfg.maxSimplificationError = maxSimplificationError;
	cfg.minRegionArea = (int)rcSqr(minRegionArea);		// Note: area = size*size
	cfg.mergeRegionArea = (int)rcSqr(mergeRegionArea);	// Note: area = size*size
	cfg.maxVertsPerPoly = (int)vertsPerPoly;
	cfg.tileSize = (int)tileSize;
	cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
	cfg.width = cfg.tileSize + cfg.borderSize * 2;
	cfg.height = cfg.tileSize + cfg.borderSize * 2;
	cfg.detailSampleDist = sampleDistance < 0.9f ? 0 : cellSize * sampleDistance;
	cfg.detailSampleMaxError = cellHeight * sampleMaxError;
	rcVcopy(cfg.bmin, bmin);
	rcVcopy(cfg.bmax, bmax);

	// Tile cache params.
	dtTileCacheParams tcparams;
	memset(&tcparams, 0, sizeof(tcparams));
	rcVcopy(tcparams.orig, bmin);
	tcparams.cs = cellSize;
	tcparams.ch = cellHeight;
	tcparams.width = (int)tileSize;
	tcparams.height = (int)tileSize;
	tcparams.walkableHeight = walkableHeight;
	tcparams.walkableRadius = walkableRadius;
	tcparams.walkableClimb = walkableClimb;
	tcparams.maxSimplificationError = maxSimplificationError;
	tcparams.maxTiles = tw * th * EXPECTED_LAYERS_PER_TILE;
	tcparams.maxObstacles = 128;

	dtFreeTileCache(m_tileCache);

	m_tileCache = dtAllocTileCache();
	if (!m_tileCache)
	{

	}

	status = m_tileCache->init(&tcparams, m_talloc, m_tcomp, m_tmproc);
	if (dtStatusFailed(status))
	{

	}

	dtFreeNavMesh(m_navMesh);

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{

	}

	int tileBits = rcMin((int)dtIlog2(dtNextPow2(tw * th * EXPECTED_LAYERS_PER_TILE)), 14);
	if (tileBits > 14) tileBits = 14;
	int polyBits = 22 - tileBits;
	int maxTiles = 1 << tileBits;
	int maxPolysPerTile = 1 << polyBits;

	dtNavMeshParams params;
	memset(&params, 0, sizeof(params));
	rcVcopy(params.orig, bmin);
	params.tileWidth = tileSize * cellSize;
	params.tileHeight = tileSize * cellSize;
	params.maxTiles = maxTiles;
	params.maxPolys = maxPolysPerTile;

	status = m_navMesh->init(&params);
	if (dtStatusFailed(status))
	{

	}

	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{

	}

	// Preprocess tiles.

	m_ctx->resetTimers();

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	const int numCPU = sysinfo.dwNumberOfProcessors + 1;

	//int m_cacheLayerCount = 0;
	//int m_cacheCompressedSize = 0;
	//int m_cacheRawSize = 0;
	int totalIter = th * tw;
	int iterations = 0;

	for (int y = 0; y < th; ++y)
	{
		//Multithreaded
		int arrayLength = tw;
		size_t maxPerThread = arrayLength / numCPU;

		std::vector<HANDLE> _threads;
		_threads.resize(numCPU);

		std::vector<ArraySlice*> slices;

		for (size_t i = 0; i < numCPU; ++i)
		{
			size_t start = i * maxPerThread;
			size_t end = std::min((int)(start + maxPerThread), arrayLength);

			if (i == numCPU - 1)
			{
				if (end < arrayLength)
					end = arrayLength;
			}

			ArraySlice* slice = new ArraySlice();
			slice->cfg = cfg;
			slice->start = start;
			slice->end = end;
			slice->mgr = this;
			slice->m_tileCache = m_tileCache;
			slice->ntris = ntris;
			slice->nverts = nverts;
			slice->tcparams = tcparams;
			slice->tris = const_cast<int*>(tris);
			slice->verts = const_cast<float*>(verts);
			slice->y = y;
			slices.push_back(slice);

			_threads[i] = (HANDLE)_beginthread(rasterizeThread, 0, (void*)slice);
		}

		iterations += tw;

		WaitForMultipleObjects(numCPU, &_threads[0], TRUE, INFINITE);

		for (auto it = slices.begin(); it != slices.end(); ++it)
		{
			//m_cacheCompressedSize += (*it)->m_cacheCompressedSize;
			//m_cacheLayerCount += (*it)->m_cacheLayerCount;
			//m_cacheRawSize += (*it)->m_cacheRawSize;
			//iterations += (*it)->iterations;

			delete* it;
		}

		slices.clear();
		_threads.clear();

		int progress = (float)iterations / (float)totalIter * 100.0f;
		if (progress > 100) progress = 100;
		if (iterations > totalIter) iterations = totalIter;
		if (buildProgressCallback != nullptr)
			buildProgressCallback(progress, totalIter, iterations);

		//Single thread
		/*for (int x = 0; x < tw; ++x)
		{
			TileCacheData tiles[MAX_LAYERS];
			memset(tiles, 0, sizeof(tiles));
			int ntiles = rasterizeTileLayers(x, y, cfg, tiles, MAX_LAYERS, verts, nverts, tris, ntris);

			for (int i = 0; i < ntiles; ++i)
			{
				TileCacheData* tile = &tiles[i];
				status = m_tileCache->addTile(tile->data, tile->dataSize, DT_COMPRESSEDTILE_FREE_DATA, 0);
				if (dtStatusFailed(status))
				{
					dtFree(tile->data);
					tile->data = 0;
					continue;
				}

				m_cacheLayerCount++;
				m_cacheCompressedSize += tile->dataSize;
				m_cacheRawSize += calcLayerBufferSize(tcparams.width, tcparams.height);
			}
		}*/
	}

	// Build initial meshes
	m_ctx->startTimer(RC_TIMER_TOTAL);
	for (int y = 0; y < th; ++y)
		for (int x = 0; x < tw; ++x)
			m_tileCache->buildNavMeshTilesAt(x, y, m_navMesh);

	m_ctx->stopTimer(RC_TIMER_TOTAL);

	IO::CreateDir(IO::GetFilePath(loadedScene) + "/" + IO::GetFileName(loadedScene) + "/");
	std::string saveTo = IO::GetFilePath(loadedScene) + "/" + IO::GetFileName(loadedScene) + "/" + IO::GetFileName(loadedScene) + ".navmesh";

	saveAll(saveTo.c_str());

	BuildVisualizationMesh();
}

void NavigationManager::LoadNavMesh()
{
	Cleanup();

	std::string path = IO::GetFilePath(loadedScene) + IO::GetFileName(loadedScene) + "/" + IO::GetFileName(loadedScene) + ".navmesh";

	if (GetEngine->GetUseUnpackedResources())
	{
		if (!IO::FileExists(path))
			return;
	}
	else
	{
		path = IO::RemovePart(path, GetEngine->GetAssetsPath());
		if (!ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), path))
		{
			return;
		}
	}

	//if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		if (GetEngine->GetUseUnpackedResources())
		{
			//_cache.Load(path);
			loadAll(path.c_str());
		}
		else
		{
			int sz = 0;
			char* buffer = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), path, sz);
			loadAllFromBuffer(buffer, sz);
			//_cache.LoadFromBuffer(buffer, sz);
		}
	}

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		BuildVisualizationMesh();
	}
}

void NavigationManager::ClearNavMesh()
{
	Cleanup();
}

void NavigationManager::BuildVisualizationMesh()
{
	std::string path = IO::GetFilePath(loadedScene) + "/" + IO::GetFileName(loadedScene) + "/" + IO::GetFileName(loadedScene) + ".navmesh";

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		MaterialPtr navMeshMaterial = MaterialManager::getSingleton().create("NavMeshMaterial", "Editor");
		navMeshMaterial->setLightingEnabled(false);
		navMeshMaterial->setReceiveShadows(false);
		navMeshMaterial->removeAllTechniques();
		navMeshMaterial->createTechnique();
		navMeshMaterial->getTechnique(0)->createPass();
		navMeshMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
		//navMeshMaterial->getTechnique(0)->getPass(0)->setPolygonMode(PolygonMode::PM_WIREFRAME);
		navMeshMaterial->getTechnique(0)->getPass(0)->setPointSize(3);
		navMeshMaterial->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);

		navMeshMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
		navMeshMaterial->getTechnique(0)->getPass(0)->setVertexProgram("transparent_vs");
		navMeshMaterial->getTechnique(0)->getPass(0)->setFragmentProgram("transparent_fs");
		navMeshMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(0.0f, 0.815f, 1.0f, 0.35));

		//Create engine mesh to visualize navigation mesh
		ManualObject * _navMesh = new ManualObject("NavigationMesh_gizmo");
		_navMesh->setCastShadows(false);
		_navMesh->setQueryFlags(1 << 3);

		SceneNode * _navMeshNode = GetEngine->GetSceneManager()->getRootSceneNode()->createChildSceneNode("NavigationMesh_node_gizmo");
		_navMeshNode->attachObject(_navMesh);

		//Copy navigation mesh
		if (m_tileCache != nullptr)
		{
			dtTileCacheAlloc* talloc = m_tileCache->getAlloc();
			dtTileCacheCompressor* tcomp = m_tileCache->getCompressor();
			const dtTileCacheParams* params = m_tileCache->getParams();

			_navMesh->begin("NavMeshMaterial", RenderOperation::OperationType::OT_TRIANGLE_LIST, "Editor");

			//for (int i = 0; i < m_tileCache->getTileCount(); ++i)
			//{
			//	talloc->reset();

			//	const dtCompressedTile* tile = m_tileCache->getTile(i);
			//	TileCacheBuildContext bc(talloc);
			//	const int walkableClimbVx = (int)(params->walkableClimb / params->ch);
			//	dtStatus status;

			//	status = dtDecompressTileCacheLayer(talloc, tcomp, tile->data, tile->dataSize, &bc.layer);
			//	if (dtStatusFailed(status))
			//		continue;

			//	// Build navmesh
			//	status = dtBuildTileCacheRegions(talloc, *bc.layer, walkableClimbVx);
			//	if (dtStatusFailed(status))
			//		continue;

			//	bc.lcset = dtAllocTileCacheContourSet(talloc);
			//	if (!bc.lcset)
			//		continue;
			//	status = dtBuildTileCacheContours(talloc, *bc.layer, walkableClimbVx, params->maxSimplificationError, *bc.lcset);
			//	if (dtStatusFailed(status))
			//		continue;

			//	bc.lmesh = dtAllocTileCachePolyMesh(talloc);
			//	if (!bc.lmesh)
			//		continue;
			//	status = dtBuildTileCachePolyMesh(talloc, *bc.lcset, *bc.lmesh);
			//	if (dtStatusFailed(status))
			//		continue;

			//	dtTileCachePolyMesh lmesh = *bc.lmesh;

			//	const int nvp = lmesh.nvp;
			//	const float* orig = tile->header->bmin;

			//	for (int i = 0; i < lmesh.npolys; ++i)
			//	{
			//		const unsigned short* p = &lmesh.polys[i * nvp * 2];
			//		const unsigned char area = lmesh.areas[i];

			//		unsigned short vi[3];
			//		for (int j = 2; j < nvp; ++j)
			//		{
			//			if (p[j] == DT_TILECACHE_NULL_IDX) break;
			//			vi[0] = p[0];
			//			vi[1] = p[j - 1];
			//			vi[2] = p[j];
			//			for (int k = 0; k < 3; ++k)
			//			{
			//				const unsigned short* v = &lmesh.verts[vi[k] * 3];
			//				const float x = orig[0] + v[0] * params->cs;
			//				const float y = orig[1] + (v[1] + 1) * params->ch;
			//				const float z = orig[2] + v[2] * params->cs;

			//				_navMesh->position(Vector3(x, y, z));
			//			}
			//		}
			//	}
			//}

			for (int i = 0; i < m_navMesh->getMaxTiles(); ++i)
			{
				const dtMeshTile* tile = m_navMesh->getTile(i);
				if (!tile->header) continue;
				dtPolyRef base = m_navMesh->getPolyRefBase(tile);

				for (int j = 0; j < tile->header->polyCount; ++j)
				{
					const dtPoly* p = &tile->polys[j];
					//if ((p->flags & SAMPLE_POLYFLAGS_DISABLED) == 0) continue;

					const unsigned int ip = (unsigned int)(p - tile->polys);
					const dtPolyDetail* pd = &tile->detailMeshes[ip];

					for (int i = 0; i < pd->triCount; ++i)
					{
						const unsigned char* t = &tile->detailTris[(pd->triBase + i) * 4];
						for (int j = 0; j < 3; ++j)
						{
							if (t[j] < p->vertCount)
								_navMesh->position(Vector3(&tile->verts[p->verts[t[j]] * 3]));
							else
								_navMesh->position(Vector3(&tile->detailVerts[(pd->vertBase + t[j] - p->vertCount) * 3]));
						}
					}
				}
			}

			_navMesh->end();
		}
	}
}

void NavigationManager::ClearVisualizationMesh()
{
	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		if (MaterialManager::getSingleton().resourceExists("NavMeshMaterial", "Editor"))
			MaterialManager::getSingleton().remove("NavMeshMaterial", "Editor");

		if (GetEngine->GetSceneManager()->hasSceneNode("NavigationMesh_node_gizmo"))
		{
			Engine::Destroy(GetEngine->GetSceneManager()->getSceneNode("NavigationMesh_node_gizmo"));
		}
	}
}

bool NavigationManager::hasNavMeshVisualizationObject()
{
	SceneNode* navVis = GetEngine->GetSceneManager()->getSceneNodeFast("NavigationMesh_node_gizmo");
	if (navVis != nullptr)
	{
		if (navVis->getAttachedObjects().size() > 0)
		{
			MovableObject* obj = navVis->getAttachedObject(0);

			return obj->isVisible();
		}
	}

	return false;
}

void NavigationManager::Init()
{
	
}

void NavigationManager::InitAgents()
{
	if (m_navMesh == nullptr)
		return;

	for (std::vector<NavMeshAgent*>::iterator it = agentList.begin(); it != agentList.end(); ++it)
	{
		(*it)->Init();
	}
}

void NavigationManager::InitAgents(SceneNode * root)
{
	if (m_navMesh == nullptr)
		return;

	NavMeshAgent * agent = (NavMeshAgent*)root->GetComponent(NavMeshAgent::COMPONENT_TYPE);

	if (agent != nullptr)
	{
		agent->Init();
	}

	VectorIterator it = root->getChildIterator();
	while (it.hasMoreElements())
	{
		SceneNode * node = (SceneNode*)it.getNext();

		InitAgents(node);
	}
}

void NavigationManager::Update()
{
	if (m_navMesh == nullptr)
		return;

	for (auto it = obstacleList.begin(); it != obstacleList.end(); ++it)
		(*it)->update();

	if (m_tileCache)
		m_tileCache->update(GetEngine->GetDeltaTime(), m_navMesh);

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		for (auto it = agentList.begin(); it != agentList.end(); ++it)
			(*it)->Update();
	}

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		if (isNavMeshDirty)
		{
			isNavMeshDirty = false;

			if (hasNavMeshVisualizationObject())
			{
				ClearVisualizationMesh();
				BuildVisualizationMesh();
			}
		}
	}
}

void NavigationManager::AddAgent(NavMeshAgent * agent)
{
	agentList.push_back(agent);
}

void NavigationManager::RemoveAgent(NavMeshAgent * agent)
{
	agentList.erase(std::remove(agentList.begin(), agentList.end(), agent), agentList.end());
}

void NavigationManager::AddObstacle(NavMeshObstacle* obstacle)
{
	obstacleList.push_back(obstacle);
}

void NavigationManager::RemoveObstacle(NavMeshObstacle* obstacle)
{
	obstacleList.erase(std::remove(obstacleList.begin(), obstacleList.end(), obstacle), obstacleList.end());
}

void NavigationManager::Cleanup()
{
	/* Cleanup */
	dtFreeNavMesh(m_navMesh);
	m_navMesh = 0;
	/* Cleanup */

	ClearVisualizationMesh();
}

void NavigationManager::GetSceneNodesWithEntities(SceneNode * root, std::vector<SceneNode*> & outList)
{
	VectorIterator it = root->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode * node = (SceneNode*)it.getNext();

		if (GetEngine->IsEditorObject(node))
			continue;

		if (node->navigationStatic)
		{
			if (node->getAttachedObjects().size() > 0)
			{
				if (node->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
				{
					if (node->getAttachedObject(0)->getQueryFlags() == 1 << 1)
					{
						outList.push_back(node);
					}
				}
			}
		}

		GetSceneNodesWithEntities(node, outList);
	}
}

static const int TILECACHESET_MAGIC = 'T' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'TSET';
static const int TILECACHESET_VERSION = 1;

struct TileCacheSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams meshParams;
	dtTileCacheParams cacheParams;
};

struct TileCacheTileHeader
{
	dtCompressedTileRef tileRef;
	int dataSize;
};

void NavigationManager::saveAll(const char* path)
{
	if (!m_tileCache) return;

	FILE* fp = fopen(path, "wb");
	if (!fp)
		return;

	// Store header.
	TileCacheSetHeader header;
	header.magic = TILECACHESET_MAGIC;
	header.version = TILECACHESET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < m_tileCache->getTileCount(); ++i)
	{
		const dtCompressedTile* tile = m_tileCache->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header.numTiles++;
	}
	memcpy(&header.cacheParams, m_tileCache->getParams(), sizeof(dtTileCacheParams));
	memcpy(&header.meshParams, m_navMesh->getParams(), sizeof(dtNavMeshParams));
	fwrite(&header, sizeof(TileCacheSetHeader), 1, fp);

	// Store tiles.
	for (int i = 0; i < m_tileCache->getTileCount(); ++i)
	{
		const dtCompressedTile* tile = m_tileCache->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		TileCacheTileHeader tileHeader;
		tileHeader.tileRef = m_tileCache->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

		fwrite(tile->data, tile->dataSize, 1, fp);
	}

	fclose(fp);
}

void NavigationManager::loadAll(const char* path)
{
	FILE* fp = fopen(path, "rb");
	if (!fp) return;

	// Read header.
	TileCacheSetHeader header;
	size_t headerReadReturnCode = fread(&header, sizeof(TileCacheSetHeader), 1, fp);
	if (headerReadReturnCode != 1)
	{
		// Error or early EOF
		fclose(fp);
		return;
	}
	if (header.magic != TILECACHESET_MAGIC)
	{
		fclose(fp);
		return;
	}
	if (header.version != TILECACHESET_VERSION)
	{
		fclose(fp);
		return;
	}

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		fclose(fp);
		return;
	}

	dtStatus status = m_navMesh->init(&header.meshParams);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return;
	}

	m_tileCache = dtAllocTileCache();
	if (!m_tileCache)
	{
		fclose(fp);
		return;
	}

	m_tmproc->init(nullptr);

	status = m_tileCache->init(&header.cacheParams, m_talloc, m_tcomp, m_tmproc);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		TileCacheTileHeader tileHeader;
		size_t tileHeaderReadReturnCode = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (tileHeaderReadReturnCode != 1)
		{
			// Error or early EOF
			fclose(fp);
			return;
		}
		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		size_t tileDataReadReturnCode = fread(data, tileHeader.dataSize, 1, fp);
		if (tileDataReadReturnCode != 1)
		{
			// Error or early EOF
			dtFree(data);
			fclose(fp);
			return;
		}

		dtCompressedTileRef tile = 0;
		dtStatus addTileStatus = m_tileCache->addTile(data, tileHeader.dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tile);
		if (dtStatusFailed(addTileStatus))
		{
			dtFree(data);
		}

		if (tile)
		{
			m_tileCache->buildNavMeshTile(tile, m_navMesh);
		}
	}

	fclose(fp);

	delete m_navQuery;
	m_navQuery = new dtNavMeshQuery();
	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return;
	}
}

void NavigationManager::loadAllFromBuffer(char* buf, size_t bufSize)
{
	boost::iostreams::stream<boost::iostreams::array_source> is(buf, bufSize);

	// Read header.
	TileCacheSetHeader header;
	is.read((char*)&header, sizeof(TileCacheSetHeader));

	if (header.magic != TILECACHESET_MAGIC)
	{
		is.close();
		delete[] buf;
		return;
	}

	if (header.version != TILECACHESET_VERSION)
	{
		is.close();
		delete[] buf;
		return;
	}

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		is.close();
		delete[] buf;
		return;
	}

	dtStatus status = m_navMesh->init(&header.meshParams);
	if (dtStatusFailed(status))
	{
		is.close();
		delete[] buf;
		return;
	}

	m_tileCache = dtAllocTileCache();
	if (!m_tileCache)
	{
		is.close();
		delete[] buf;
		return;
	}

	m_tmproc->init(nullptr);

	status = m_tileCache->init(&header.cacheParams, m_talloc, m_tcomp, m_tmproc);
	if (dtStatusFailed(status))
	{
		is.close();
		delete[] buf;
		return;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		TileCacheTileHeader tileHeader;

		is.read((char*)&tileHeader, sizeof(tileHeader));

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);

		is.read((char*)data, tileHeader.dataSize);

		dtCompressedTileRef tile = 0;
		dtStatus addTileStatus = m_tileCache->addTile(data, tileHeader.dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tile);
		if (dtStatusFailed(addTileStatus))
		{
			dtFree(data);
		}

		if (tile)
		{
			m_tileCache->buildNavMeshTile(tile, m_navMesh);
		}
	}

	delete m_navQuery;
	m_navQuery = new dtNavMeshQuery();
	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{
		is.close();
		delete[] buf;
		return;
	}

	is.close();
	delete[] buf;
}