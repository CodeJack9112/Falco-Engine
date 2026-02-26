#include "stdafx.h"
#include "PhysicsManager.h"
#include "RigidBody.h"
#include "API_Physics.h"

void API_Physics::raycast(API::Vector3 * ref_from, API::Vector3 * ref_to, API::RaycastHit * out_hit)
{
	btVector3 btFrom(ref_from->x, ref_from->y, ref_from->z);
	btVector3 btTo(ref_to->x, ref_to->y, ref_to->z);
	btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

	GetEngine->GetPhysicsManager()->GetWorld()->rayTest(btFrom, btTo, res);

	btRigidBody* body = nullptr;

	if (res.m_collisionObject != nullptr)
		body = const_cast<btRigidBody*>(btRigidBody::upcast(res.m_collisionObject));

	out_hit->rigidBody = nullptr;

	std::vector<RigidBody*> bodies = GetEngine->GetPhysicsManager()->GetBodies();
	for (std::vector<RigidBody*>::iterator it = bodies.begin(); it != bodies.end(); ++it)
	{
		if ((*it)->GetNativeBody() == body)
		{
			out_hit->rigidBody = (*it)->GetMonoObject();
			break;
		}
	}

	API::Vector3 point;
	point.x = res.m_hitPointWorld.getX();
	point.y = res.m_hitPointWorld.getY();
	point.z = res.m_hitPointWorld.getZ();

	API::Vector3 normal;
	normal.x = res.m_hitNormalWorld.getX();
	normal.y = res.m_hitNormalWorld.getY();
	normal.z = res.m_hitNormalWorld.getZ();

	out_hit->hitPoint = point;
	out_hit->worldNormal = normal;
	out_hit->hasHit = res.hasHit();
}

MonoArray* API_Physics::overlapSphere(API::Vector3* center, float radius)
{
	btVector3 btFrom(center->x, center->y, center->z);
	btTransform transform1;
	btTransform transform2;
	transform1.setOrigin(btFrom);
	transform2.setOrigin(btFrom);

	btConvexShape* sphere = new btSphereShape(radius);

	struct TempCallback : public btDynamicsWorld::ConvexResultCallback
	{
		std::vector<const btCollisionObject*> mHits;

		btScalar addSingleResult(btDynamicsWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
		{
			if (find(mHits.begin(), mHits.end(), convexResult.m_hitCollisionObject) == mHits.end())
				mHits.push_back(convexResult.m_hitCollisionObject);

			return convexResult.m_hitFraction;
		}
	} res;

	GetEngine->GetPhysicsManager()->GetWorld()->convexSweepTest(sphere, transform1, transform2, res);

	std::vector<RigidBody*> bodies = GetEngine->GetPhysicsManager()->GetBodies();

	MonoArray* arr = mono_array_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->collider_class, res.mHits.size());

	int i = 0;
	for (auto _it = res.mHits.begin(); _it != res.mHits.end(); ++_it, ++i)
	{
		btRigidBody* body = const_cast<btRigidBody*>(btRigidBody::upcast(*_it));

		for (std::vector<RigidBody*>::iterator it = bodies.begin(); it != bodies.end(); ++it)
		{
			if ((*it)->GetNativeBody() == body)
			{
				mono_array_setref(arr, i, (*it)->GetMonoObject());

				break;
			}
		}
	}

	delete sphere;

	return arr;
}
