#include "stdafx.h"
#include "API_Quaternion.h"
#include "Engine.h"
#include "Mathf.h"

void API_Quaternion::euler(API::Vector3 * ref_vec, API::Quaternion * out_rot)
{
	//Euler e = Euler(ref_vec->y, ref_vec->x, ref_vec->z);
	//Quaternion q = e.toQuaternion();
	Quaternion q = Mathf::toQuaternion(Vector3(ref_vec->x, ref_vec->y, ref_vec->z));

	out_rot->x = q.x;
	out_rot->y = q.y;
	out_rot->z = q.z;
	out_rot->w = q.w;
}

void API_Quaternion::angleAxis(float angle, API::Vector3 * ref_axis, API::Quaternion * out_rot)
{
	Vector3 axis = Vector3(ref_axis->x, ref_axis->y, ref_axis->z);
	Quaternion q;
	q.FromAngleAxis(Radian(Degree(angle)), axis);

	out_rot->x = q.x;
	out_rot->y = q.y;
	out_rot->z = q.z;
	out_rot->w = q.w;
}

void API_Quaternion::lookRotation(API::Vector3* direction, API::Quaternion* out_rot)
{
	Vector3 dir = Vector3(direction->x, direction->y, direction->z);
	dir.normalise();
	Vector3 right(dir.z, 0, -dir.x);
	right.normalise();
	Vector3 up = dir.crossProduct(right);
	Quaternion quat = Quaternion(right, up, dir);
	
	out_rot->x = quat.x;
	out_rot->y = quat.y;
	out_rot->z = quat.z;
	out_rot->w = quat.w;
}

void API_Quaternion::eulerAngles(API::Quaternion* quaternion, API::Vector3* ret)
{
	Quaternion q1 = Quaternion(quaternion->w, quaternion->x, quaternion->y, quaternion->z);
	
	Vector3 euler = Mathf::toEuler(q1);

	ret->x = euler.x;
	ret->y = euler.y;
	ret->z = euler.z;
}

void API_Quaternion::slerp(API::Quaternion* q1, API::Quaternion* q2, float t, API::Quaternion* ret)
{
	Quaternion qq1 = Quaternion(q1->w, q1->x, q1->y, q1->z);
	Quaternion qq2 = Quaternion(q2->w, q2->x, q2->y, q2->z);

	Quaternion rret = Quaternion::Slerp(t, qq1, qq2, true);

	ret->x = rret.x;
	ret->y = rret.y;
	ret->z = rret.z;
	ret->w = rret.w;
}
