#include "stdafx.h"
#include "RaycastGeometry.h"
#include <OgreCamera.h>
#include <OgreMovableObject.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreCamera.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>
#include <OgreViewport.h>
#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>

#include "../Engine/Engine.h"

RaycastGeometry::RaycastGeometry(SceneManager * mgr, Camera * cam)
{
	sceneManager = mgr;
	camera = cam;
}

RaycastGeometry::~RaycastGeometry()
{
}

void RaycastGeometry::SetTypeMask(uint32 m)
{
	typeMask = m;
}

void RaycastGeometry::SetMask(uint32 m)
{
	mask = m;
}

MovableObject * RaycastGeometry::RaycastFromPoint(Vector2 point, float &distance)
{
	float width = (float)this->camera->getViewport()->getActualWidth(); // viewport width
	float height = (float)this->camera->getViewport()->getActualHeight(); // viewport height

	// create the ray to test
	Ray ray = this->camera->getCameraToViewportRay((float)point.x / width, (float)point.y / height);

	return RaycastFromPoint(ray, distance);
}

MovableObject* RaycastGeometry::RaycastFromPoint(Ray& ray, float& distance)
{
	// create the ray scene query object
	RaySceneQuery* m_pray_scene_query = sceneManager->createRayQuery(ray);
	m_pray_scene_query->setSortByDistance(true);
	m_pray_scene_query->setQueryTypeMask(typeMask);
	m_pray_scene_query->setQueryMask(mask);

	// execute the query, returns a vector of hits
	m_pray_scene_query->execute();

	MovableObject* retEntity = NULL;

	Ogre::RaySceneQueryResult query_result = m_pray_scene_query->getLastResults();
	delete m_pray_scene_query;

	for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
	{
		// stop checking if we have found a raycast hit that is closer
		// than all remaining entities
		/*if ((distance >= 0.0f) && (distance < query_result[qr_idx].distance))
		{
			retEntity = query_result[qr_idx].movable;
			break;
		}*/

		// only check this result if its a hit against an entity
		if ((query_result[qr_idx].movable != NULL))
		{
			//Check the billboard type object
			if (query_result[qr_idx].movable->getMovableType().compare("BillboardSet") == 0)
			{
				Ogre::Entity* pentity = static_cast<Entity*>(query_result[qr_idx].movable->getParentSceneNode()->getParentSceneNode()->getAttachedObject(0));
				distance = query_result[qr_idx].distance;
				return pentity;
			}

			if (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0)
			{
				Ogre::Entity* pentity = static_cast<Entity*>(query_result[qr_idx].movable);

				//Check if the entity has gizmo parent
				/*if ((pentity->getParentSceneNode()->getParentSceneNode() != NULL && pentity->getParentSceneNode()->getParentSceneNode()->getName() == "Gizmo"))
				{
					distance = query_result[qr_idx].distance;
					return pentity;
				}*/

				// mesh data to retrieve         
				size_t vertex_count;
				size_t index_count;
				Ogre::Vector3* vertices;
				unsigned long* indices;

				// get the mesh information
				GetMeshInformation(pentity->getMesh(), vertex_count, vertices, index_count, indices,
					pentity->getParentNode()->_getDerivedPosition(),
					pentity->getParentNode()->_getDerivedOrientation(),
					pentity->getParentNode()->_getDerivedScale());

				// test for hitting individual triangles on the mesh
				for (int i = 0; i < static_cast<int>(index_count); i += 3)
				{
					// check for a hit against this triangle
					std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
						vertices[indices[i + 1]], vertices[indices[i + 2]], true, false);

					// if it was a hit check if its the closest
					if (hit.first)
					{
						if ((distance <= 0.0f) || (hit.second < distance))
						{
							// this is the closest so far, save it off
							distance = hit.second;
							retEntity = pentity;
							break;
						}
					}
				}

				// free the verticies and indicies memory
				delete[] vertices;
				delete[] indices;
			}

			if (query_result[qr_idx].movable->getMovableType().compare("UIButton") == 0 || query_result[qr_idx].movable->getMovableType().compare("UIText") == 0 || query_result[qr_idx].movable->getMovableType().compare("UIImage") == 0)
			{
				UIElement* pentity = (UIElement*)(query_result[qr_idx].movable);

				// mesh data to retrieve
				size_t vertex_count;
				size_t index_count;
				Ogre::Vector3* vertices;
				unsigned long* indices;

				MeshPtr tmpMesh = pentity->convertToMesh(pentity->getName(), "Assets");

				if (tmpMesh != nullptr)
				{
					// get the mesh information
					GetMeshInformation(tmpMesh, vertex_count, vertices, index_count, indices,
						pentity->getParentNode()->_getDerivedPosition(),
						pentity->getParentNode()->_getDerivedOrientation(),
						pentity->getParentNode()->_getDerivedScale());

					// test for hitting individual triangles on the mesh
					for (int i = 0; i < static_cast<int>(index_count); i += 3)
					{
						// check for a hit against this triangle
						std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
							vertices[indices[i + 1]], vertices[indices[i + 2]], true, false);

						// if it was a hit check if its the closest
						if (hit.first)
						{
							if ((distance < 0.0f) || (hit.second < distance))
							{
								// this is the closest so far, save it off
								distance = hit.second;
								retEntity = (MovableObject*)pentity;
							}
						}
					}

					MeshManager::getSingleton().remove(tmpMesh);

					// free the verticies and indicies memory
					delete[] vertices;
					delete[] indices;
				}
			}
		}
	}

	// return the result
	if (distance >= 0.0f)
	{
		// raycast success
		return retEntity;
	}
	else
	{
		// raycast failed
		return NULL;
	}
}

MovableObject* RaycastGeometry::RaycastFromPointGeometryOnly(Ray& ray, float& distance, float maxDistance, bool lightmapStaticOnly)
{
	// create the ray scene query object
	RaySceneQuery* m_pray_scene_query = sceneManager->createRayQuery(ray);
	m_pray_scene_query->setSortByDistance(true);
	m_pray_scene_query->setQueryTypeMask(typeMask);
	m_pray_scene_query->setQueryMask(mask);

	// execute the query, returns a vector of hits
	m_pray_scene_query->execute();

	Ogre::RaySceneQueryResult query_result = m_pray_scene_query->getLastResults();
	delete m_pray_scene_query;

	MovableObject* result = nullptr;

	for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
	{
		if (result != nullptr)
			break;

		// only check this result if its a hit against an entity
		if ((query_result[qr_idx].movable != NULL))
		{
			if (query_result[qr_idx].movable->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
			{
				if (query_result[qr_idx].movable->getParentSceneNode() == nullptr)
					continue;

				if (GetEngine->IsEditorObject(query_result[qr_idx].movable->getParentSceneNode()))
					continue;

				if (query_result[qr_idx].distance > maxDistance)
					continue;

				if (lightmapStaticOnly)
				{
					if (!query_result[qr_idx].movable->getParentSceneNode()->lightmapStatic)
						continue;
				}

				Ogre::Entity* pentity = static_cast<Entity*>(query_result[qr_idx].movable);

				// mesh data to retrieve         
				size_t vertex_count;
				size_t index_count;
				Ogre::Vector3* vertices;
				unsigned long* indices;

				// get the mesh information
				GetMeshInformation(pentity->getMesh(), vertex_count, vertices, index_count, indices,
					pentity->getParentSceneNode()->_getDerivedPosition(),
					pentity->getParentSceneNode()->_getDerivedOrientation(),
					pentity->getParentSceneNode()->_getDerivedScale());

				// test for hitting individual triangles on the mesh
				for (int i = 0; i < static_cast<int>(index_count); i += 3)
				{
					// check for a hit against this triangle
					std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]], true, false);

					// if it was a hit check if its the closest
					if (hit.first && hit.second < maxDistance)
					{
						result = query_result[qr_idx].movable;
						distance = hit.second;

						break;
					}
				}

				// free the verticies and indicies memory
				delete[] vertices;
				delete[] indices;
			}
		}
	}

	return result;
}

void RaycastGeometry::GetMeshInformation(const MeshPtr mesh, size_t & vertex_count, Vector3 *& vertices, size_t & index_count, unsigned long *& indices, const Vector3 & position, const Quaternion & orient, const Ogre::Vector3 & scale)
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;

	vertex_count = index_count = 0;

	// Calculate how many vertices and indices we're going to need
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

		// We only need to add the shared vertices once
		if (submesh->useSharedVertices)
		{
			if (!added_shared)
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}

		// Add the indices
		index_count += submesh->indexData->indexCount;
	}


	// Allocate space for the vertices and indices
	vertices = new Ogre::Vector3[vertex_count];
	indices = new unsigned long[index_count];

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

		Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

		if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
		{
			if (submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			//unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			unsigned char* vertex = new unsigned char[vbuf->getSizeInBytes()];
			vbuf->readData(0, vbuf->getSizeInBytes(), reinterpret_cast<unsigned char*>(vertex));
			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//      Ogre::Real* pReal;
			float* pReal;

			for (size_t j = 0; j < vertex_data->vertexCount; ++j/*, vertex += vbuf->getVertexSize()*/)
			{
				posElem->baseVertexPointerToElement(vertex + (vbuf->getVertexSize() * j), &pReal);

				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

				vertices[current_offset + j] = (orient * (pt * scale)) + position;
			}

			//vbuf->unlock();
			delete[] vertex;
			next_offset += vertex_data->vertexCount;
		}


		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
		if (ibuf == NULL) continue; // need to check if index buffer is valid (which will be not if the mesh doesn't have triangles like a pointcloud)

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		//unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned long* pLong = new unsigned long[ibuf->getSizeInBytes()];
		ibuf->readData(0, ibuf->getSizeInBytes(), pLong);

		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

		size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;
		size_t index_start = index_data->indexStart;
		size_t last_index = numTris * 3 + index_start;

		if (use32bitindexes)
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
			}

		else
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
			}

		//ibuf->unlock();
		delete[] pLong;
		current_offset = next_offset;
	}
}