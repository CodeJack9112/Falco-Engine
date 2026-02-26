#pragma once

#include "API.h"

class API_Camera
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Camera::INTERNAL_getViewMatrix", (void*)getViewMatrix);
		mono_add_internal_call("FalcoEngine.Camera::INTERNAL_getProjectionMatrixWithRSDepth", (void*)getProjectionMatrixWithRSDepth);
		mono_add_internal_call("FalcoEngine.Camera::INTERNAL_getWorldSpaceCorners", (void*)getWorldSpaceCorners);
		mono_add_internal_call("FalcoEngine.Camera::INTERNAL_worldToScreenPoint", (void*)worldToScreenPoint);
		mono_add_internal_call("FalcoEngine.Camera::INTERNAL_screenToWorldPoint", (void*)screenToWorldPoint);
		mono_add_internal_call("FalcoEngine.Camera::INTERNAL_isObjectVisible", (void*)isObjectVisible);

		mono_add_internal_call("FalcoEngine.Camera::get_nearClipDistance", (void*)getNearClipDistance);
		mono_add_internal_call("FalcoEngine.Camera::set_nearClipDistance", (void*)setNearClipDistance);
		mono_add_internal_call("FalcoEngine.Camera::get_farClipDistance", (void*)getFarClipDistance);
		mono_add_internal_call("FalcoEngine.Camera::set_farClipDistance", (void*)setFarClipDistance);

		mono_add_internal_call("FalcoEngine.Camera::get_main", (void*)getMainCamera);
		mono_add_internal_call("FalcoEngine.Camera::set_main", (void*)setMainCamera);
	}

private:
	//getViewMatrix
	static void getViewMatrix(MonoObject* this_ptr, bool ownFrustumOnly, API::Matrix4 * matrix);

	//getViewMatrix
	static void getProjectionMatrixWithRSDepth(MonoObject* this_ptr, API::Matrix4* matrix);

	//getWorldSpaceCorners
	static void getWorldSpaceCorners(MonoObject* this_ptr, API::Vector3 * nTopRight, API::Vector3* nTopLeft, API::Vector3* nBottomLeft, API::Vector3* nBottomRight,
		API::Vector3* fTopRight, API::Vector3* fTopLeft, API::Vector3* fBottomLeft, API::Vector3* fBottomRight);

	//worldToScreenPoint
	static void worldToScreenPoint(MonoObject* this_ptr, API::Vector3 * ref_world, API::Vector3* out_screen);

	//screenToWorldPoint
	static void screenToWorldPoint(MonoObject* this_ptr, API::Vector3 * ref_screen, API::Vector3* out_world);

	//getNearClipDistance
	static float getNearClipDistance(MonoObject* this_ptr);

	//setNearClipDistance
	static void setNearClipDistance(MonoObject* this_ptr, float value);

	//getFarClipDistance
	static float getFarClipDistance(MonoObject* this_ptr);

	//setFarClipDistance
	static void setFarClipDistance(MonoObject* this_ptr, float value);

	//isObjectVisible
	static bool isObjectVisible(MonoObject* this_ptr, MonoObject * obj);

	static MonoObject* getMainCamera();

	static void setMainCamera(MonoObject* obj);
};

