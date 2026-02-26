#include "NavMeshObstacle.h"

#include "Engine.h"
#include "NavigationManager.h"

#include "Pathfinding/DetourTileCache/Include/DetourTileCache.h"

std::string NavMeshObstacle::COMPONENT_TYPE = "NavMeshObstacle";

NavMeshObstacle::NavMeshObstacle(SceneNode* parent) : Component(parent, GetEngine->GetMonoRuntime()->navmeshobstacle_class)
{
	GetEngine->GetNavigationManager()->AddObstacle(this);
}

NavMeshObstacle::~NavMeshObstacle()
{
	GetEngine->GetNavigationManager()->RemoveObstacle(this);
	removeObstacle();
}

void NavMeshObstacle::SceneLoaded()
{
	if (GetEnabled() && GetParentSceneNode()->getVisible())
		addObstacle();
}

void NavMeshObstacle::StateChanged(bool active)
{
	if (!active)
	{
		removeObstacle();
	}
	else
	{
		if (GetParentSceneNode()->getVisible())
			addObstacle();
	}
}

void NavMeshObstacle::NodeStateChanged(bool active)
{
	if (!active)
	{
		removeObstacle();
	}
	else
	{
		if (GetEnabled())
			addObstacle();
	}
}

void NavMeshObstacle::update()
{
	if (!GetEnabled() || !GetParentSceneNode()->getVisible())
		return;

	SceneNode* parent = GetParentSceneNode();

	Vector3 pos = parent->_getDerivedPosition();
	Vector3 scale = parent->_getDerivedScale();
	Quaternion rot = parent->_getDerivedOrientation();

	if (oldSize != size)
	{
		oldSize = size;
		updateObstacle();
	}

	if (oldOffset != offset)
	{
		oldOffset = offset;
		updateObstacle();
	}

	if (oldPos != pos)
	{
		oldPos = pos;
		updateObstacle();
	}

	if (oldRot != rot)
	{
		oldRot = rot;
		updateObstacle();
	}

	if (oldScale != scale)
	{
		oldScale = scale;
		updateObstacle();
	}
}

void NavMeshObstacle::addObstacle()
{
	if (obstacleRef == 0)
	{
		NavigationManager* mgr = GetEngine->GetNavigationManager();
		dtTileCache* cache = mgr->GetTileCache();
		SceneNode* parent = GetParentSceneNode();
		Quaternion rot = parent->_getDerivedOrientation();

		Matrix4 ft = parent->_getFullTransform();

		Vector3 _minX = ft * (Vector3(size.x, size.y, size.z) + offset);
		Vector3 _minY = ft * (Vector3(size.x, -size.y, size.z) + offset);
		Vector3 _minZ = ft * (Vector3(-size.x, size.y, size.z) + offset);
		Vector3 _minW = ft * (Vector3(-size.x, -size.y, size.z) + offset);

		Vector3 _maxX = ft * (Vector3(size.x, size.y, -size.z) + offset);
		Vector3 _maxY = ft * (Vector3(size.x, -size.y, -size.z) + offset);
		Vector3 _maxZ = ft * (Vector3(-size.x, size.y, -size.z) + offset);
		Vector3 _maxW = ft * (Vector3(-size.x, -size.y, -size.z) + offset);

		AxisAlignedBox aab;
		
		aab.merge(_minX);
		aab.merge(_minY);
		aab.merge(_minZ);
		aab.merge(_minW);
		aab.merge(_maxX);
		aab.merge(_maxY);
		aab.merge(_maxZ);
		aab.merge(_maxW);

		cache->addBoxObstacle(aab.getMinimum().ptr(), aab.getMaximum().ptr(), &obstacleRef);
	}
}

void NavMeshObstacle::removeObstacle()
{
	if (obstacleRef != 0)
	{
		NavigationManager* mgr = GetEngine->GetNavigationManager();
		dtTileCache* cache = mgr->GetTileCache();
		cache->removeObstacle(obstacleRef);
		obstacleRef = 0;
	}
}

void NavMeshObstacle::updateObstacle()
{
	removeObstacle();
	addObstacle();

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		GetEngine->GetNavigationManager()->setNavMeshIsDirty();
}
