#include "stdafx.h"
#include "NavMeshAgent.h"
#include "Engine.h"

#include "NavigationManager.h"

#include "Pathfinding/InputGeom.h"
#include "Pathfinding/Detour/Include/DetourNavMeshQuery.h"
#include "Pathfinding/DebugUtils/Include/RecastDump.h"
#include "Pathfinding/DetourCrowd/Include/DetourCrowd.h"
#include "Pathfinding/Detour/Include/DetourCommon.h"

#include "DynamicLines.h"
#include "mathf.h"

std::string NavMeshAgent::COMPONENT_TYPE = "NavMeshAgent";

/* NavMesh Agent*/

NavMeshAgent::NavMeshAgent(SceneNode* parent) : Component(parent, GetEngine->GetMonoRuntime()->navmeshagent_class)
{
	GetEngine->GetNavigationManager()->AddAgent(this);
}

NavMeshAgent::~NavMeshAgent()
{
	GetEngine->GetNavigationManager()->RemoveAgent(this);
}

void NavMeshAgent::Init()
{
	
}

void NavMeshAgent::UpdateParams()
{
	
}

void NavMeshAgent::Update()
{
	if (!GetEnabled() || !GetParentSceneNode()->getVisible())
		return;

	float dt = GetEngine->GetDeltaTime();
	SceneNode* sceneNode = GetParentSceneNode();
	Vector3 currentPos = sceneNode->_getDerivedPosition();

	if (currentPath.VertCount > 0)
	{
		//***--------FIND PATH--------***//

		Vector3 targetPos = Vector3(currentPath.PosX[1], currentPath.PosY[1], currentPath.PosZ[1]);
		Vector3 dir = (targetPos - currentPos).normalisedCopy();
		Vector3 newPos = currentPos + ((dir * speed * 2) * dt * GetEngine->getTimeScale());

		//***--------HANDLE COLLISIONS WITH NEIGHBOURS--------***//

		auto agents = GetEngine->GetNavigationManager()->getAgentList();
		static const float COLLISION_RESOLVE_FACTOR = 0.7f;
		Vector3 vel = dir;

		auto _idx = find(agents.begin(), agents.end(), this);
		int idx0 = distance(agents.begin(), _idx);

		for (int iter = 0; iter < 4; ++iter)
		{
			Vector3 disp = Vector3::ZERO;

			float w = 0;

			for (auto it = agents.begin(); it != agents.end(); ++it)
			{
				NavMeshAgent* nei = *it;
				if (nei == this)
					continue;

				SceneNode* neiNode = nei->GetParentSceneNode();
				Vector3 neiPos = neiNode->_getDerivedPosition();
				float neiRadius = nei->GetRadius();

				if (neiPos.distance(sceneNode->_getDerivedPosition()) <= neiRadius + radius)
				{
					int idx1 = distance(agents.begin(), it);

					float diff[3];
					dtVsub(diff, newPos.ptr(), neiPos.ptr());
					diff[1] = 0;

					float dist = dtVlenSqr(diff);
					if (dist > dtSqr(radius + neiRadius))
						continue;
					dist = dtMathSqrtf(dist);
					float pen = (radius + neiRadius) - dist;
					if (dist < 0.0001f)
					{
						// Agents on top of each other, try to choose diverging separation directions.
						if (idx0 > idx1)
							dtVset(diff, -vel.z, 0, vel.x);
						else
							dtVset(diff, vel.z, 0, -vel.x);
						pen = 0.01f;
					}
					else
					{
						pen = (1.0f / dist) * (pen * 0.5f) * COLLISION_RESOLVE_FACTOR;
					}

					dtVmad(disp.ptr(), disp.ptr(), diff, pen);

					w += 1.0f;
				}
			}

			if (w > 0.0001f)
			{
				const float iw = 1.0f / w;
				dtVscale(disp.ptr(), disp.ptr(), iw);
			}

			newPos = newPos + disp;
			//newPos = Mathf::lerp(newPos, newPos + disp, COLLISION_RESOLVE_FACTOR * 10.0f * dt);
		}

		//***--------SET POSITION AND ROTATION--------***//

		Vector3 right(dir.z, 0, -dir.x);
		right.normalise();
		Vector3 up = dir.crossProduct(right);
		Quaternion quat = Quaternion(right, up, dir);

		sceneNode->_setDerivedPosition(newPos);

		Quaternion _r = sceneNode->_getDerivedOrientation();
		Quaternion r = Quaternion::Slerp((rotationSpeed * 2.0) * 0.01, _r, quat, true);

		if (!isnan(r.x) && !isnan(r.y) && !isnan(r.z) && !isnan(r.w))
			sceneNode->_setDerivedOrientation(r);
	}
}

void NavMeshAgent::SetRadius(float r)
{
	radius = r;

	UpdateParams();
}

void NavMeshAgent::SetHeight(float h)
{
	height = h;

	UpdateParams();
}

void NavMeshAgent::SetSpeed(float s)
{
	speed = s;

	UpdateParams();
}

void NavMeshAgent::SetAcceleration(float a)
{
	acceleration = a;

	UpdateParams();
}

void NavMeshAgent::SetTargetPosition(Vector3 pos)
{
	//if (targetPosition == pos)
	//	return;

	targetPosition = pos;
	
	Vector3 currPos = GetParentSceneNode()->_getDerivedPosition();
	dtNavMeshQuery * query = GetEngine->GetNavigationManager()->GetNavMeshQuery();
	currentPath = FindPath(currPos, pos);

	/*if (debugPath != nullptr)
		delete debugPath;

	debugPath = new DynamicLines(MaterialManager::getSingleton().getDefaultMaterial());

	for (int i = 0; i < m_PathStore[0].MaxVertex; ++i)
	{
		debugPath->addPoint(Vector3(m_PathStore[0].PosX[i], m_PathStore[0].PosY[i], m_PathStore[0].PosZ[i]));
	}

	debugPath->update();

	SceneNode * debugPathNode = nullptr;

	if (GetEngine->GetSceneManager()->hasSceneNode("PathDebug"))
	{
		GetEngine->GetSceneManager()->destroySceneNode("PathDebug");
	}

	debugPathNode = GetEngine->GetSceneManager()->getRootSceneNode()->createChildSceneNode("PathDebug");

	debugPathNode->attachObject(debugPath);*/
}

void NavMeshAgent::SetRotationSpeed(float s)
{
	rotationSpeed = s;
}

PathData NavMeshAgent::FindPath(Vector3 StartPos, Vector3 EndPos)
{
	PathData m_PathStore;

	dtStatus status;
	const float pExtents[3] = { 64, 64, 64 }; // size of box around start/end points to look for nav polygons
	dtPolyRef StartPoly;
	float StartNearest[3];
	dtPolyRef EndPoly;
	float EndNearest[3];
	dtPolyRef PolyPath[MAX_PATHPOLY];
	int nPathCount = 0;
	float StraightPath[MAX_PATHVERT * 3];
	int nVertCount = 0;

	dtNavMeshQuery * m_navQuery = GetEngine->GetNavigationManager()->GetNavMeshQuery();

	// setup the filter
	dtQueryFilter Filter;
	Filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	Filter.setExcludeFlags(0);
	Filter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
	Filter.setAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
	Filter.setAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
	Filter.setAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
	Filter.setAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
	Filter.setAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);

	// find the start polygon
	status = m_navQuery->findNearestPoly(StartPos.ptr(), pExtents, &Filter, &StartPoly, StartNearest);
	if ((status&DT_FAILURE) || (status&DT_STATUS_DETAIL_MASK))  m_PathStore.valid = false; // couldn't find a polygon

	// find the end polygon
	status = m_navQuery->findNearestPoly(EndPos.ptr(), pExtents, &Filter, &EndPoly, EndNearest);
	if ((status&DT_FAILURE) || (status&DT_STATUS_DETAIL_MASK))  m_PathStore.valid = false; // couldn't find a polygon

	status = m_navQuery->findPath(StartPoly, EndPoly, StartNearest, EndNearest, &Filter, PolyPath, &nPathCount, MAX_PATHPOLY);
	if ((status&DT_FAILURE) || (status&DT_STATUS_DETAIL_MASK)) m_PathStore.valid = false; // couldn't create a path
	if (nPathCount == 0) m_PathStore.valid = false; // couldn't find a path

	status = m_navQuery->findStraightPath(StartNearest, EndNearest, PolyPath, nPathCount, StraightPath, NULL, NULL, &nVertCount, MAX_PATHVERT);
	if ((status&DT_FAILURE) || (status&DT_STATUS_DETAIL_MASK))  m_PathStore.valid = false; // couldn't create a path
	if (nVertCount == 0) m_PathStore.valid = false; // couldn't find a path

	// At this point we have our path.  Copy it to the path store
	int nIndex = 0;
	for (int nVert = 0; nVert < nVertCount; nVert++)
	{
		m_PathStore.PosX[nVert] = StraightPath[nIndex++];
		m_PathStore.PosY[nVert] = StraightPath[nIndex++];
		m_PathStore.PosZ[nVert] = StraightPath[nIndex++];
	}

	m_PathStore.VertCount = nVertCount;

	return m_PathStore;

}
