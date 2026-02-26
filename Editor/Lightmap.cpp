#include "stdafx.h"
#include "LightMap.h"

#include <algorithm>

#include <OgreSubEntity.h>
#include <OgreSubMesh.h>
#include <OgreSceneNode.h>
#include <OgreMesh.h>
#include <OgreStringConverter.h>
#include <OgreSceneManager.h>

#include "../Engine/Engine.h"
#include "../Engine/mathf.h"

#include "MainWindow.h"

std::vector<std::pair<int, int> > CLightMap::m_SearchPattern;
int CLightMap::m_iLightMapCounter = 0;

CLightMap::CLightMap(SubEntity* pSubEntity, int entNum, std::string objName, std::string saveDir, Real PixelsPerUnit, int iTexSize, bool bDebugLightmaps)
	: m_pSubEntity(pSubEntity)
	, m_iTexSize(iTexSize)
	, m_PixelsPerUnit(PixelsPerUnit)
	, m_bDebugLightmaps(bDebugLightmaps)
{
	saveTo = saveDir;
	objectName = objName;
	entityNum = entNum;

	if (m_SearchPattern.empty())
		BuildSearchPattern();

	m_LightMapName = "LightMap" + StringConverter::toString(m_iLightMapCounter++);

	if (CalculateLightMap())
		SaveLightmap();
}

CLightMap::~CLightMap(void)
{

}

void CLightMap::ResetCounter()
{
	m_iLightMapCounter = 0;
}

String CLightMap::GetName()
{
	return m_LightMapName;
}

struct ArraySlice
{
public:

	ArraySlice(std::vector<unsigned long> & _pBuffer, std::vector<unsigned short>& _pBuffer16, size_t start, size_t end, CLightMap* _lightmap, std::vector<Vector3> & _MeshVertices, std::vector<Vector3> & _MeshNormals, std::vector<Vector2> & _MeshTextureCoords)
	{
		pBuffer = _pBuffer;
		pBuffer16 = _pBuffer16;
		pBegin = start;
		pEnd = end;
		lightmap = _lightmap;
		MeshVertices = _MeshVertices;
		MeshNormals = _MeshNormals;
		MeshTextureCoords = _MeshTextureCoords;
	}

	~ArraySlice()
	{
		pBuffer.clear();
		pBuffer16.clear();
		MeshVertices.clear();
		MeshNormals.clear();
		MeshTextureCoords.clear();
	}

	int pBegin, pEnd;

	int * nThread = 0;
	bool bit32 = false;
	CLightMap* lightmap;
	std::vector<unsigned long> pBuffer;
	std::vector<unsigned short> pBuffer16;
	std::vector<Vector3> MeshVertices;
	std::vector<Vector3> MeshNormals;
	std::vector<Vector2> MeshTextureCoords;
};

void myThread(void* param)
{
	ArraySlice* slice = (ArraySlice*)param;
	if (!slice) return;

	unsigned long Indices[3];
	unsigned short Indices16[3];

	for (int i = slice->pBegin, j = slice->pEnd; i < j; i += 3)
	{
		for (int k = 0; k < 3; ++k)
		{
			if (slice->bit32)
				Indices[k] = slice->pBuffer[i + k];
			else
				Indices16[k] = slice->pBuffer16[i + k];
		}

		if (slice->bit32)
		{
			slice->lightmap->LightTriangle(slice->MeshVertices[Indices[0]], slice->MeshVertices[Indices[1]], slice->MeshVertices[Indices[2]],
				slice->MeshNormals[Indices[0]], slice->MeshNormals[Indices[1]], slice->MeshNormals[Indices[2]],
				slice->MeshTextureCoords[Indices[0]], slice->MeshTextureCoords[Indices[1]], slice->MeshTextureCoords[Indices[2]]);
		}
		else
		{
			slice->lightmap->LightTriangle(slice->MeshVertices[Indices16[0]], slice->MeshVertices[Indices16[1]], slice->MeshVertices[Indices16[2]],
				slice->MeshNormals[Indices16[0]], slice->MeshNormals[Indices16[1]], slice->MeshNormals[Indices16[2]],
				slice->MeshTextureCoords[Indices16[0]], slice->MeshTextureCoords[Indices16[1]], slice->MeshTextureCoords[Indices16[2]]);
		}
	}

	ExitThread(0);
}

bool CLightMap::CalculateLightMap()
{
	// Get the submesh
	SubMesh* submesh = m_pSubEntity->getSubMesh();
	Entity* entity = m_pSubEntity->getParent();
	SceneNode* meshNode = entity->getParentSceneNode();

	Matrix4 WorldTransform;
	//m_pSubEntity->getWorldTransforms(&WorldTransform);
	WorldTransform = meshNode->_getFullTransform();

	// Get vertex positions
	std::vector<Vector3> MeshVertices;
	{
		try
		{
			VertexData* vertex_data = submesh->useSharedVertices ? submesh->parent->sharedVertexData : submesh->vertexData;
			const VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
			HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			float* pReal;

			MeshVertices.resize(vertex_data->vertexCount);

			for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);
				MeshVertices[j] = WorldTransform * Vector3(pReal[0], pReal[1], pReal[2]);
			}

			vbuf->unlock();
		}
		catch (...)
		{
			return false;
		}
	}

	// Get vertex normals
	Quaternion Rotation = meshNode->_getDerivedOrientation();

	std::vector<Vector3> MeshNormals;
	{
		try
		{
			VertexData* vertex_data = submesh->useSharedVertices ? submesh->parent->sharedVertexData : submesh->vertexData;
			const VertexElement* normalElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);
			HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(normalElem->getSource());
			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			float* pReal;

			MeshNormals.resize(vertex_data->vertexCount);

			for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				normalElem->baseVertexPointerToElement(vertex, &pReal);
				MeshNormals[j] = Rotation * Vector3(pReal[0], pReal[1], pReal[2]);
			}

			vbuf->unlock();
		}
		catch (...)
		{
			return false;
		}
	}

	// Get vertex UV coordinates
	std::vector<Vector2> MeshTextureCoords;
	{
		try
		{
			VertexData* vertex_data = submesh->useSharedVertices ? submesh->parent->sharedVertexData : submesh->vertexData;
			// Get last set of texture coordinates
			int i = 0;
			const VertexElement* texcoordElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES, 1);

			if (!texcoordElem)
				return false;

			HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(texcoordElem->getSource());
			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			float* pReal;

			MeshTextureCoords.resize(vertex_data->vertexCount);

			for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				texcoordElem->baseVertexPointerToElement(vertex, &pReal);
				MeshTextureCoords[j] = Vector2(pReal[0], pReal[1]);
			}

			vbuf->unlock();
		}
		catch (...)
		{
			return false;
		}
	}

	IndexData* index_data = submesh->indexData;

	size_t numTris = index_data->indexCount / 3;
	HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

	bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

	unsigned long Indices[3];
	unsigned long* pBuffer = new unsigned long[ibuf->getSizeInBytes()];
	//ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);

	ibuf->readData(0, ibuf->getSizeInBytes(), reinterpret_cast<unsigned long*>(pBuffer));
	unsigned short* pBuffer16 = reinterpret_cast<unsigned short*>(pBuffer);

	// Calculate the lightmap texture size
	if (m_PixelsPerUnit && m_LightMap == nullptr)
	{
		Real SurfaceArea = 0;
		for (size_t k = 0; k < numTris * 3; k += 3)
		{
			for (int i = 0; i < 3; ++i)
			{
				if (use32bitindexes)
					Indices[i] = ((unsigned long*)pBuffer)[k + i];
				else
					Indices[i] = ((unsigned short*)pBuffer)[k + i];
			}

			SurfaceArea += GetTriangleArea(MeshVertices[Indices[0]], MeshVertices[Indices[1]], MeshVertices[Indices[2]]);
		}
		Real TexSize = Math::Sqrt(SurfaceArea) * m_PixelsPerUnit;

		int iTexSize = 1;
		while (iTexSize < TexSize)
			iTexSize *= 2;

		m_iTexSize = iTexSize;
	}

	// Create the texture with the new size
	CreateTexture();

	// Fill in the lightmap
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	const int numCPU = sysinfo.dwNumberOfProcessors + 1;

	int arrayLength = numTris * 3;
	size_t maxPerThread = arrayLength / numCPU;
	size_t sliceLength = 0;

	while (sliceLength + 3 <= maxPerThread)
	{
		sliceLength += 3;
	}

	int* threads = new int[numCPU];
	HANDLE g_hMainThread = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());

	std::vector<unsigned long> pBuf;
	if (use32bitindexes)
		std::copy(pBuffer, pBuffer + (numTris * 3), std::back_inserter(pBuf));

	std::vector<unsigned short> pBuf16;
	if (!use32bitindexes)
		std::copy(pBuffer16, pBuffer16 + (numTris * 3), std::back_inserter(pBuf16));

	std::vector<HANDLE> _threads;
	_threads.resize(numCPU);

	std::vector<ArraySlice*> slices;

	for (size_t i = 0; i < numCPU; i++)
	{
		threads[i] = 0;

		size_t start = i * sliceLength;
		size_t end = std::min((int)(start + sliceLength), arrayLength);

		if (i == numCPU - 1)
			end = arrayLength;

		ArraySlice* slice = new ArraySlice(pBuf, pBuf16, start, end, this, MeshVertices, MeshNormals, MeshTextureCoords);
		slice->bit32 = use32bitindexes;
		slice->nThread = &threads[i];
		slices.push_back(slice);

		_threads[i] = (HANDLE)_beginthread(myThread, 0, (void*)slice);
	}

	//SuspendThread(g_hMainThread);
	WaitForMultipleObjects(numCPU, &_threads[0], TRUE, INFINITE);

	for (auto it = slices.begin(); it != slices.end(); ++it)
		delete* it;

	slices.clear();
	_threads.clear();

	pBuf.clear();
	pBuf16.clear();

	/*for (size_t k = 0; k < numTris * 3; k += 3)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (use32bitindexes)
				Indices[i] = ((unsigned int*)pBuffer)[k + i];
			else
				Indices[i] = ((unsigned short*)pBuffer)[k + i];
		}

		LightTriangle(MeshVertices[Indices[0]], MeshVertices[Indices[1]], MeshVertices[Indices[2]],
			MeshNormals[Indices[0]], MeshNormals[Indices[1]], MeshNormals[Indices[2]],
			MeshTextureCoords[Indices[0]], MeshTextureCoords[Indices[1]], MeshTextureCoords[Indices[2]]);
	}*/

	//ibuf->unlock();
	delete[] pBuffer;
	delete[] threads;

	FillInvalidPixels();
	//m_LightMap->blur(1.0);

	return true;
}

void CLightMap::FillInvalidPixels()
{
	int i, j;
	int x, y;
	std::vector<std::pair<int, int> >::iterator itSearchPattern;
	for (i = 0; i < m_iTexSize; ++i)
	{
		for (j = 0; j < m_iTexSize; ++j)
		{
			// Invalid pixel found
			//if ((*m_LightMap)(i, j, 0, 1) == 0)
			if (m_LightMap[i * m_iTexSize + j].a == 0)
			{
				for (itSearchPattern = m_SearchPattern.begin(); itSearchPattern != m_SearchPattern.end(); ++itSearchPattern)
				{
					x = i + itSearchPattern->first;
					y = j + itSearchPattern->second;
					if (x < 0 || x >= m_iTexSize)
						continue;
					if (y < 0 || y >= m_iTexSize)
						continue;
					// If search pixel is valid assign it to the invalid pixel and stop searching
					if (m_LightMap[x * m_iTexSize + y].a == 1)
					{
						PixelInfo _col = m_LightMap[x * m_iTexSize + y];
						_col.a = 1;

						//m_LightMap->setColourAt(_col, i, j, 0);
						m_LightMap[i * m_iTexSize + j] = _col;
						break;
					}
				}
			}
		}
	}
}

void CLightMap::BuildSearchPattern()
{
	m_SearchPattern.clear();
	const int iSize = 5;
	int i, j;
	for (i = -iSize; i <= iSize; ++i)
	{
		for (j = -iSize; j <= iSize; ++j)
		{
			if (i == 0 && j == 0)
				continue;
			m_SearchPattern.push_back(std::make_pair(i, j));
		}
	}
	sort(m_SearchPattern.begin(), m_SearchPattern.end(), SortCoordsByDistance());
}

Vector3 CLightMap::GetBarycentricCoordinates(const Vector2& P1, const Vector2& P2, const Vector2& P3, const Vector2& P)
{
	Vector3 Coordinates(0.0);
	Real denom = (-P1.x * P3.y - P2.x * P1.y + P2.x * P3.y + P1.y * P3.x + P2.y * P1.x - P2.y * P3.x);

	if (fabs(denom) >= 1e-6)
	{
		Coordinates.x = (P2.x * P3.y - P2.y * P3.x - P.x * P3.y + P3.x * P.y - P2.x * P.y + P2.y * P.x) / denom;
		Coordinates.y = -(-P1.x * P.y + P1.x * P3.y + P1.y * P.x - P.x * P3.y + P3.x * P.y - P1.y * P3.x) / denom;
		//Coordinates.z = (-P1.x * P.y + P2.y * P1.x + P2.x * P.y - P2.x * P1.y - P2.y * P.x + P1.y * P.x) / denom;
	}
	Coordinates.z = 1 - Coordinates.x - Coordinates.y;

	return Coordinates;
}

Real CLightMap::GetTriangleArea(const Vector3& P1, const Vector3& P2, const Vector3& P3)
{
	return 0.5 * (P2 - P1).crossProduct(P3 - P1).length();
}

void CLightMap::LightTriangle(const Vector3 & P1, const Vector3 & P2, const Vector3 & P3,
	const Vector3 & N1, const Vector3 & N2, const Vector3 & N3,
	const Vector2 & T1, const Vector2 & T2, const Vector2 & T3)
{
	Vector2 TMin = T1, TMax = T1;
	TMin.makeFloor(T2);
	TMin.makeFloor(T3);
	TMax.makeCeil(T2);
	TMax.makeCeil(T3);
	int iMinX = GetPixelCoordinate(TMin.x);
	int iMinY = GetPixelCoordinate(TMin.y);
	int iMaxX = GetPixelCoordinate(TMax.x);
	int iMaxY = GetPixelCoordinate(TMax.y);
	int i, j;
	Vector2 TextureCoord;
	Vector3 BarycentricCoords;
	Vector3 Pos;
	Vector3 Normal;
	for (i = iMinX; i <= iMaxX; ++i)
	{
		for (j = iMinY; j <= iMaxY; ++j)
		{
			TextureCoord.x = GetTextureCoordinate(i);
			TextureCoord.y = GetTextureCoordinate(j);
			BarycentricCoords = GetBarycentricCoordinates(T1, T2, T3, TextureCoord);
			Pos = BarycentricCoords.x * P1 + BarycentricCoords.y * P2 + BarycentricCoords.z * P3;
			Normal = BarycentricCoords.x * N1 + BarycentricCoords.y * N2 + BarycentricCoords.z * N3;
			Normal.normalise();

			if (m_LightMap[i * m_iTexSize + j].a == 1 || BarycentricCoords.x < 0 || BarycentricCoords.y < 0 || BarycentricCoords.z < 0)
			{
				continue;
			}

			PixelInfo c = GetLightIntensity(Pos, Normal);
			c.a = 1;

			m_LightMap[i * m_iTexSize + j] = c;
		}
	}

}

int CLightMap::GetPixelCoordinate(Real TextureCoord)
{
	int iPixel = TextureCoord * m_iTexSize;
	if (iPixel < 0)
		iPixel = 0;
	if (iPixel >= m_iTexSize)
		iPixel = m_iTexSize - 1;
	return iPixel;
}

Real CLightMap::GetTextureCoordinate(int iPixelCoord)
{
	return (Real(iPixelCoord) + 0.5) / Real(m_iTexSize);
}

CLightMap::PixelInfo CLightMap::GetLightIntensity(const Vector3 & Position, const Vector3 & Normal)
{
	Camera* camera = MainWindow::mainCamera;
	SceneManager* sceneManager = GetEngine->GetSceneManager();

	Real Tolerance = 0.001;
	ColourValue ambCol = sceneManager->getAmbientLight();
	PixelInfo AmbientValue = PixelInfo(0.2, 0.2, 0.2, 0.0f);// PixelInfo(ambCol.r, ambCol.g, ambCol.b, 0.0f);

	RaycastGeometry* raycast = new RaycastGeometry(sceneManager, camera);
	raycast->SetTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
	raycast->SetMask(1 << 1);
	float distance = 0;

	MapIterator it = GetEngine->GetSceneManager()->getMovableObjectIterator(LightFactory::FACTORY_TYPE_NAME);

	PixelInfo outColor;

	while (it.hasMoreElements())
	{
		Light* light = (Light*)it.getNext();

		if (light->getMode() == Light::LightMode::LM_REALTIME)
			continue;

		SceneNode* lightNode = light->getParentSceneNode();

		if (light->getType() == Light::LightTypes::LT_DIRECTIONAL)
		{
			float Distance = 1000;
			
			Vector3 LightDirection = lightNode->_getDerivedOrientation()* Vector3::NEGATIVE_UNIT_Z;
			LightDirection.normalise();

			float _Intensity = -LightDirection.dotProduct(Normal) * light->getPowerScale();
			
			if (_Intensity <= 0.0f)
				continue;

			PixelInfo Intensity = PixelInfo(_Intensity, _Intensity, _Intensity, 0.0f);
			Intensity = Intensity * (PixelInfo(1.0f, 1.0f, 1.0f, 0.0f) - AmbientValue);

			Vector3 Origin = Position - Distance * LightDirection;
			MovableObject* obj = nullptr;
			
			if (light->getCastShadows())
				obj = raycast->RaycastFromPointGeometryOnly(Ray(Origin, LightDirection), distance, Distance - Tolerance, true);

			ColourValue lc = light->getDiffuseColour();
			PixelInfo _color = PixelInfo(lc.r, lc.g, lc.b, 0.0f) * Intensity;

			if (obj == nullptr)
			{
				outColor += _color;
			}
			else
			{
				outColor -= _color;

				if (outColor.r < 0) outColor.r = 0;
				if (outColor.g < 0) outColor.g = 0;
				if (outColor.b < 0) outColor.b = 0;
			}
		}

		if (light->getType() == Light::LightTypes::LT_POINT)
		{
			Vector3 Origin = lightNode->_getDerivedPosition();
			float Distance = Origin.distance(Position);

			Vector3 lightVector = (Origin - Position).normalisedCopy();

			float len_sq = lightVector.dotProduct(lightVector);
			float len = sqrt(len_sq);
			float att = clamp(1.0f - Distance / (light->getAttenuationRange() * 1.2f), 0.0f, len) * light->getPowerScale();

			float _Intensity = clamp(lightVector.dotProduct(Normal) * att, 0.0f, 1.0f);

			if (_Intensity <= 0.0f)
				continue;

			PixelInfo Intensity = PixelInfo(_Intensity, _Intensity, _Intensity, 0.0f);
			Intensity = Intensity * (PixelInfo(1.0f, 1.0f, 1.0f, 0.0f) - AmbientValue);

			MovableObject* obj = nullptr;
			
			if (light->getCastShadows())
				obj = raycast->RaycastFromPointGeometryOnly(Ray(Origin, -lightVector), distance, Distance - Tolerance, true);

			ColourValue lc = light->getDiffuseColour();
			PixelInfo _color = PixelInfo(lc.r, lc.g, lc.b, 0.0f) * Intensity;

			if (obj == nullptr)
			{
				outColor += _color;
			}
			else
			{
				outColor -= _color;

				if (outColor.r < 0) outColor.r = 0;
				if (outColor.g < 0) outColor.g = 0;
				if (outColor.b < 0) outColor.b = 0;
			}
		}

		if (light->getType() == Light::LightTypes::LT_SPOTLIGHT)
		{
			Vector3 Origin = lightNode->_getDerivedPosition();
			float Distance = Origin.distance(Position);

			Vector3 lightVector = (Origin - Position).normalisedCopy();
			Vector3 LightDirection = light->getDerivedDirection();
			LightDirection.normalise();

			float len_sq = lightVector.dotProduct(lightVector);
			float len = sqrt(len_sq);
			float att = clamp(1.0f - Distance / (light->getAttenuationRange() * 1.2f), 0.0f, len) * light->getPowerScale();

			float spotlightAngle = clamp(lightVector.dotProduct(-LightDirection), 0.0f, 1.0f);
			float spotParamX = cos(light->getSpotlightInnerAngle().valueRadians() / 2.0f);
			float spotParamY = cos(light->getSpotlightOuterAngle().valueRadians() / 2.0f);
			float spotFalloff = clamp((spotlightAngle - spotParamX) / (spotParamY - spotParamX), 0.0f, 1.0f);
			float spot = (1.0 - spotFalloff);

			float _Intensity = clamp(lightVector.dotProduct(Normal) * att * spot, 0.0f, 1.0f);

			if (_Intensity <= 0.0f)
				continue;

			PixelInfo Intensity = PixelInfo(_Intensity, _Intensity, _Intensity, 0.0f);
			Intensity = Intensity * (PixelInfo(1.0f, 1.0f, 1.0f, 0.0f) - AmbientValue);

			MovableObject* obj = nullptr;
			
			if (light->getCastShadows())
				obj = raycast->RaycastFromPointGeometryOnly(Ray(Origin, -lightVector), distance, Distance - Tolerance, true);

			ColourValue lc = light->getDiffuseColour();
			PixelInfo _color = PixelInfo(lc.r, lc.g, lc.b, 0.0f) * Intensity;

			if (obj == nullptr)
			{
				outColor += _color;
			}
			else
			{
				outColor -= _color;

				if (outColor.r < 0) outColor.r = 0;
				if (outColor.g < 0) outColor.g = 0;
				if (outColor.b < 0) outColor.b = 0;
			}
		}
	}

	delete raycast;

	PixelInfo finalColor = AmbientValue + outColor;
	finalColor.r = clamp(finalColor.r, AmbientValue.r, 1.0f);
	finalColor.g = clamp(finalColor.g, AmbientValue.g, 1.0f);
	finalColor.b = clamp(finalColor.b, AmbientValue.b, 1.0f);
	finalColor.a = 0.0f;

	return finalColor;
}

void CLightMap::SetLightIntensity(int i, int j, PixelInfo color)
{
}

void CLightMap::CreateTexture()
{
	if (TextureManager::getSingleton().resourceExists(m_LightMapName, "Assets"))
		TextureManager::getSingleton().remove(m_LightMapName, "Assets");

	m_LightMap = new PixelInfo[m_iTexSize * m_iTexSize];
}

void CLightMap::SaveLightmap()
{
	// Create the texture
	TexturePtr m_Texture = TextureManager::getSingleton().createManual(
		m_LightMapName,
		"Assets",
		TEX_TYPE_2D,
		m_iTexSize, m_iTexSize,
		-1,
		PF_R8G8B8,
		TU_DEFAULT);

	Image buffer;
	m_Texture->convertToImage(buffer);
	TextureManager::getSingleton().remove((ResourcePtr)m_Texture);

	float r = GetEngine->GetLightmapBlurRadius();
	if (r > 3)
		r = 3;

	if (r > 0)
		BlurLightmap(r);

	for (int i = 0; i < m_iTexSize; ++i)
	{
		for (int j = 0; j < m_iTexSize; ++j)
		{
			PixelInfo _c = m_LightMap[i * m_iTexSize + j];
			ColourValue c = ColourValue(_c.r, _c.g, _c.b, 1.0f);
			buffer.setColourAt(c, i, j, 0);
		}
	}

	std::string texName = saveTo + objectName + to_string(entityNum) + ".png";

	buffer.save(texName);
	
	if (TextureManager::getSingleton().getByName(texName, "Assets") == nullptr)
	{
		TexturePtr ltex = TextureManager::getSingleton().loadImage(texName, "Assets", buffer);
		m_pSubEntity->setLightmapTexture(ltex);
	}

	buffer.freeMemory();
	delete[] m_LightMap;
}

void CLightMap::BlurLightmap(float radius)
{
	PixelInfo * data = new PixelInfo[m_iTexSize * m_iTexSize];

	for (int i = 0; i < m_iTexSize; ++i)
	{
		for (int j = 0; j < m_iTexSize; ++j)
		{
			data[i * m_iTexSize + j] = m_LightMap[i * m_iTexSize + j] * 255.0f;
		}
	}

	int w = m_iTexSize;
	int h = m_iTexSize;

	float rs = ceil(radius * 2.57f); // significant radius
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			PixelInfo val, wsum;
			for (int iy = i - rs; iy < i + rs + 1; iy++)
			{
				for (int ix = j - rs; ix < j + rs + 1; ix++)
				{
					int x = min(w - 1, max(0, ix));
					int y = min(h - 1, max(0, iy));
					float dsq = (ix - j) * (ix - j) + (iy - i) * (iy - i);
					float wght = exp(-dsq / (2 * radius * radius)) / (Math::PI * 2 * radius * radius);
					val += data[y * w + x] * wght; wsum += wght;
				}
			}
			float r = round(val.r / wsum.r) / 255.0f;
			float g = round(val.g / wsum.g) / 255.0f;
			float b = round(val.b / wsum.b) / 255.0f;
			m_LightMap[i * w + j] = PixelInfo(r, g, b, 1.0);
		}
	}

	delete[] data;
}