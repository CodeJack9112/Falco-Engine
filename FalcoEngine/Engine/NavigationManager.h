#pragma once

#include "Pathfinding/Detour/Include/DetourNavMesh.h"
#include "Pathfinding/Recast/Include/Recast.h"
#include "Pathfinding/PerfTimer.h"

#include <vector>
#include <string>
#include <functional>

namespace Ogre
{
	class SceneNode;
}

class NavMeshAgent;
class NavMeshObstacle;
struct TileCacheData;

/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.
enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};
enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};
enum SamplePartitionType
{
	SAMPLE_PARTITION_WATERSHED,
	SAMPLE_PARTITION_MONOTONE,
	SAMPLE_PARTITION_LAYERS,
};

/// Recast build context.
class BuildContext : public rcContext
{
	TimeVal m_startTime[RC_MAX_TIMERS];
	TimeVal m_accTime[RC_MAX_TIMERS];

public:
	BuildContext();

protected:
	virtual void doResetTimers();
	virtual void doStartTimer(const rcTimerLabel label);
	virtual void doStopTimer(const rcTimerLabel label);
	virtual int doGetAccumulatedTime(const rcTimerLabel label) const;
};

class NavigationManager
{
public:
	NavigationManager();
	~NavigationManager();

	void BuildNavMesh(std::function<void(int progress, int totalIter, int currentIter)> buildProgressCallback = nullptr);
	void LoadNavMesh();
	void ClearNavMesh();

	void BuildVisualizationMesh();
	void ClearVisualizationMesh();
	bool hasNavMeshVisualizationObject();

	void Init();
	void InitAgents();
	void InitAgents(Ogre::SceneNode * root);
	void Update();
	void AddAgent(NavMeshAgent * agent);
	void RemoveAgent(NavMeshAgent * agent);
	void AddObstacle(NavMeshObstacle* obstacle);
	void RemoveObstacle(NavMeshObstacle* obstacle);
	std::vector<NavMeshAgent*>& getAgentList() { return agentList; }

	void SetLoadedScene(std::string scene) { loadedScene = scene; }
	std::string GetLoadedScene() { return loadedScene; }

	void setNavMeshIsDirty() { isNavMeshDirty = true; }

protected:
	class dtNavMesh* m_navMesh;
	class dtNavMeshQuery* m_navQuery;
	class dtTileCache* m_tileCache;

	struct LinearAllocator* m_talloc;
	struct FastLZCompressor* m_tcomp;
	struct MeshProcess* m_tmproc;

	std::string loadedScene = "";
	bool isNavMeshDirty = false;

	unsigned char m_navMeshDrawFlags;

	float walkableSlopeAngle = 45;
	float walkableHeight = 2.0;
	float walkableClimb = 3.9;
	float walkableRadius = 1.6;
	float maxEdgeLen = 12;
	float maxSimplificationError = 3.0;
	float minRegionArea = 4.0;
	float mergeRegionArea = 20.0;
	float cellSize = 1.00;
	float cellHeight = 0.20;

	int m_partitionType;

	bool m_filterLowHangingObstacles;
	bool m_filterLedgeSpans;
	bool m_filterWalkableLowHeightSpans;

	BuildContext * m_ctx = nullptr;

private:
	bool m_keepInterResults;
	float m_totalBuildTimeMs;

	std::vector<NavMeshAgent*> agentList;
	std::vector<NavMeshObstacle*> obstacleList;

	void Cleanup();
	void GetSceneNodesWithEntities(Ogre::SceneNode * root, std::vector<Ogre::SceneNode*> & outList);

	void saveAll(const char* path);
	void loadAll(const char* path);
	void loadAllFromBuffer(char * buf, size_t bufSize);

public:
	int rasterizeTileLayers(const int tx, const int ty, const rcConfig& cfg, TileCacheData* tiles, const int maxTiles, const float* verts, const int nverts, const int* tris, const int ntris);

	//dtCrowd * GetCrowd() { return m_crowd; }
	dtNavMesh * GetNavMesh() { return m_navMesh; }
	dtNavMeshQuery * GetNavMeshQuery() { return m_navQuery; }
	dtTileCache * GetTileCache() { return m_tileCache; }

	void SetWalkableSlopeAngle(float value) { walkableSlopeAngle = value; }
	void SetWalkableHeight(float value) { walkableHeight = value; }
	void SetWalkableClimb(float value) { walkableClimb = value; }
	void SetWalkableRadius(float value) { walkableRadius = value; }
	void SetMaxEdgeLen(float value) { maxEdgeLen = value; }
	void SetMaxSimplificationError(float value) { maxSimplificationError = value; }
	void SetMinRegionArea(float value) { minRegionArea = value; }
	void SetMergeRegionArea(float value) { mergeRegionArea = value; }
	void SetCellSize(float value) { cellSize = value; }
	void SetCellHeight(float value) { cellHeight = value; }

	float GetWalkableSlopeAngle() { return walkableSlopeAngle; }
	float GetWalkableHeight() { return walkableHeight; }
	float GetWalkableClimb() { return walkableClimb; }
	float GetWalkableRadius() { return walkableRadius; }
	float GetMaxEdgeLen() { return maxEdgeLen; }
	float GetMaxSimplificationError() { return maxSimplificationError; }
	float GetMinRegionArea() { return minRegionArea; }
	float GetMergeRegionArea() { return mergeRegionArea; }
	float GetCellSize() { return cellSize; }
	float GetCellHeight() { return cellHeight; }
};

