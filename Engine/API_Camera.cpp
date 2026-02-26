#include "API_Camera.h"
#include "Engine.h"
#include "OgreCamera.h"
#include "OgreViewport.h"

void API_Camera::getViewMatrix(MonoObject* this_ptr, bool ownFrustumOnly, API::Matrix4* matrix)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	Matrix4 mat = cam->getViewMatrix(ownFrustumOnly);

	matrix->m00 = mat[0][0];
	matrix->m10 = mat[1][0];
	matrix->m20 = mat[2][0];
	matrix->m30 = mat[3][0];

	matrix->m01 = mat[0][1];
	matrix->m11 = mat[1][1];
	matrix->m21 = mat[2][1];
	matrix->m31 = mat[3][1];

	matrix->m02 = mat[0][2];
	matrix->m12 = mat[1][2];
	matrix->m22 = mat[2][2];
	matrix->m32 = mat[3][2];

	matrix->m03 = mat[0][3];
	matrix->m13 = mat[1][3];
	matrix->m23 = mat[2][3];
	matrix->m33 = mat[3][3];
}

void API_Camera::getProjectionMatrixWithRSDepth(MonoObject* this_ptr, API::Matrix4* matrix)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	Matrix4 mat = cam->getProjectionMatrixWithRSDepth();

	matrix->m00 = mat[0][0];
	matrix->m10 = mat[1][0];
	matrix->m20 = mat[2][0];
	matrix->m30 = mat[3][0];

	matrix->m01 = mat[0][1];
	matrix->m11 = mat[1][1];
	matrix->m21 = mat[2][1];
	matrix->m31 = mat[3][1];

	matrix->m02 = mat[0][2];
	matrix->m12 = mat[1][2];
	matrix->m22 = mat[2][2];
	matrix->m32 = mat[3][2];

	matrix->m03 = mat[0][3];
	matrix->m13 = mat[1][3];
	matrix->m23 = mat[2][3];
	matrix->m33 = mat[3][3];
}

void API_Camera::getWorldSpaceCorners(MonoObject* this_ptr, API::Vector3* nTopRight, API::Vector3* nTopLeft, API::Vector3* nBottomLeft, API::Vector3* nBottomRight,
	API::Vector3* fTopRight, API::Vector3* fTopLeft, API::Vector3* fBottomLeft, API::Vector3* fBottomRight)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	const Vector3* corners = cam->getWorldSpaceCorners();

	nTopRight->x = corners[0].x;
	nTopRight->y = corners[0].y;
	nTopRight->z = corners[0].z;

	nTopLeft->x = corners[1].x;
	nTopLeft->y = corners[1].y;
	nTopLeft->z = corners[1].z;

	nBottomLeft->x = corners[2].x;
	nBottomLeft->y = corners[2].y;
	nBottomLeft->z = corners[2].z;

	nBottomRight->x = corners[3].x;
	nBottomRight->y = corners[3].y;
	nBottomRight->z = corners[3].z;

	fTopRight->x = corners[4].x;
	fTopRight->y = corners[4].y;
	fTopRight->z = corners[4].z;

	fTopLeft->x = corners[5].x;
	fTopLeft->y = corners[5].y;
	fTopLeft->z = corners[5].z;

	fBottomLeft->x = corners[6].x;
	fBottomLeft->y = corners[6].y;
	fBottomLeft->z = corners[6].z;

	fBottomRight->x = corners[7].x;
	fBottomRight->y = corners[7].y;
	fBottomRight->z = corners[7].z;
}

void API_Camera::worldToScreenPoint(MonoObject* this_ptr, API::Vector3* ref_world, API::Vector3* out_screen)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	float width = (float)cam->getViewport()->getActualWidth();
	float height = (float)cam->getViewport()->getActualHeight();

	Vector3 iPoint = Vector3(ref_world->x, ref_world->y, ref_world->z);
	Vector3 point = cam->getProjectionMatrix() * (cam->getViewMatrix() * iPoint);

	Vector3 screenSpacePoint = Vector3::ZERO;
	screenSpacePoint.x = ((point.x / 2.f) + 0.5f) * width;
	screenSpacePoint.y = ((point.y / 2.f) + 0.5f) * height;
	screenSpacePoint.z = point.z;

	out_screen->x = screenSpacePoint.x;
	out_screen->y = screenSpacePoint.y;
	out_screen->z = screenSpacePoint.z;
}

void API_Camera::screenToWorldPoint(MonoObject* this_ptr, API::Vector3* ref_screen, API::Vector3* out_world)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	float width = (float)cam->getViewport()->getActualWidth();
	float height = (float)cam->getViewport()->getActualHeight();

	// Actual width of Viewport - I have set my camera(mCam) as viewPort
	float scrx = ref_screen->x / width;
	float scry = ref_screen->y / height;
	Ray ray = cam->getCameraToViewportRay((float)(scrx), (float)(scry));
	Vector3 vect = ray.getPoint(ref_screen->z);
	
	out_world->x = vect.x;
	out_world->y = vect.y;
	out_world->z = vect.z;
}

float API_Camera::getNearClipDistance(MonoObject* this_ptr)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	return cam->getNearClipDistance();
}

void API_Camera::setNearClipDistance(MonoObject* this_ptr, float value)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	cam->setNearClipDistance(value);
}

float API_Camera::getFarClipDistance(MonoObject* this_ptr)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	return cam->getFarClipDistance();
}

void API_Camera::setFarClipDistance(MonoObject* this_ptr, float value)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	cam->setFarClipDistance(value);
}

bool API_Camera::isObjectVisible(MonoObject* this_ptr, MonoObject* obj)
{
	Camera* cam;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	SceneNode* node = nullptr;
	mono_field_get_value(obj, GetEngine->GetMonoRuntime()->gameobject_native_ptr_field, reinterpret_cast<void*>(&node));

	if (node != nullptr)
	{
		// TODO
	}

	return false;
}

MonoObject* API_Camera::getMainCamera()
{
	Camera* mainCamera = GetEngine->GetMainCamera();

	if (mainCamera != nullptr)
		return mainCamera->objectMono;
	
	return nullptr;
}

void API_Camera::setMainCamera(MonoObject* obj)
{
	Camera* cam = nullptr;

	if (obj != nullptr)
		mono_field_get_value(obj, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&cam));

	GetEngine->SetMainCamera(cam);
}
