#include "stdafx.h"
#include "MeshCollider.h"
#include "RigidBody.h"
#include <OgreSceneNode.h>
#include <OgreEntity.h>

std::string MeshCollider::COMPONENT_TYPE = "MeshCollider";

MeshCollider::MeshCollider(SceneNode * parent) : Collider(parent, GetEngine->GetMonoRuntime()->meshcollider_class)
{
	rebuild();
}

MeshCollider::~MeshCollider()
{

}

void MeshCollider::rebuild()
{
	if (collisionShape != nullptr)
		delete collisionShape;

	Vector3 center = Vector3::ZERO;

	if (GetParentSceneNode()->getAttachedObjects().size() > 0)
	{
		if (GetParentSceneNode()->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
		{
			Entity * obj = (Entity*)GetParentSceneNode()->getAttachedObject(0);

			size_t vertex_count;
			size_t index_count;
			Vector3 * vertices;
			unsigned long * indices;

			Vector3 scale = Vector3(1, 1, 1);

			MeshUtilities::GetMeshInformation(obj, vertex_count, vertices, index_count, indices, Vector3::ZERO, Quaternion::IDENTITY, scale);

			for (int i = 0; i < vertex_count; ++i)
			{
				center += vertices[i];
			}

			center *= pow(vertex_count, -1);
			SetOffset(center);

			if (convex)
			{
				collisionShape = new btConvexHullShape();

				for (int i = 0; i < vertex_count; ++i)
				{
					Vector3 pt = vertices[i] - center;

					((btConvexHullShape*)collisionShape)->addPoint(btVector3(btScalar(pt.x), btScalar(pt.y), btScalar(pt.z)));
				}

				((btConvexHullShape*)collisionShape)->recalcLocalAabb();
			}
			else
			{
				btTriangleMesh * mesh = new btTriangleMesh();

				for (int i = 0; i < vertex_count; i += 3)
				{
					Vector3 pt1 = vertices[i] - center;
					Vector3 pt2 = vertices[i + 1] - center;
					Vector3 pt3 = vertices[i + 2] - center;

					mesh->addTriangle(
						btVector3(btScalar(pt1.x), btScalar(pt1.y), btScalar(pt1.z)),
						btVector3(btScalar(pt2.x), btScalar(pt2.y), btScalar(pt2.z)),
						btVector3(btScalar(pt3.x), btScalar(pt3.y), btScalar(pt3.z)));
				}

				collisionShape = new btBvhTriangleMeshShape(mesh, true);

				((btBvhTriangleMeshShape*)collisionShape)->recalcLocalAabb();
			}

			SetCollisionShape(collisionShape);
		}
	}
}

void MeshCollider::SetConvex(bool c)
{
	convex = c;

	rebuild();
}
