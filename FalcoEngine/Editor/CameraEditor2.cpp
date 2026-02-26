#include "stdafx.h"
#include "CameraEditor2.h"

#include <OgreSceneNode.h>
#include <OgreCamera.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../Engine/Engine.h"
#include "../Engine/DeferredShading/DeferredShading.h"

#include "MainWindow.h"

#include "PropBool.h"
#include "PropFloat.h"
#include "PropColorPicker.h"
#include "PropComboBox.h"

CameraEditor2::CameraEditor2()
{
	setEditorName("CameraEditor");
}

CameraEditor2::~CameraEditor2()
{
}

void CameraEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	auto sceneNodes = getSceneNodes();
	SceneNode* sceneNode = sceneNodes[0];

	Camera* camera = (Camera*)sceneNode->getAttachedObject(0);

	//Camera
	Property* cameraProp = new Property(this, "Camera");
	cameraProp->icon = TextureManager::getSingleton().load("Icons/Hierarchy/camera.png", "Editor");

	PropBool * isMainCamera = new PropBool(this, "Main camera", camera->getMainCamera());
	isMainCamera->setOnChangeCallback([=](Property* prop, bool val) { onChangeIsMainCamera(prop, val); });

	PropFloat * nearClipPlane = new PropFloat(this, "Near clip plane", camera->getNearClipDistance());
	nearClipPlane->setOnChangeCallback([=](Property* prop, float val) { onChangeNearClipPlane(prop, val); });

	PropFloat* farClipPlane = new PropFloat(this, "Far clip plane", camera->getFarClipDistance());
	farClipPlane->setOnChangeCallback([=](Property* prop, float val) { onChangeFarClipPlane(prop, val); });

	PropFloat* fieldOfView = new PropFloat(this, "Field of view", camera->getFOVy().valueDegrees());
	fieldOfView->setOnChangeCallback([=](Property* prop, float val) { onChangeFOV(prop, val); });

	PropColorPicker* clearColor = new PropColorPicker(this, "Clear color", camera->getClearColor());
	clearColor->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeClearColor(prop, val); });

	PropComboBox* projectionType = new PropComboBox(this, "Projection type", { "Perspective", "Orthographic" });
	projectionType->setOnChangeCallback([=](Property* prop, int val) { onChangeProjectionType(prop, val); });
	projectionType->setCurrentItem(camera->getProjectionType() == ProjectionType::PT_PERSPECTIVE ? 0 : 1);

	PropFloat* orthoSize = new PropFloat(this, "Orthographic size", camera->getOrthographicSize());
	orthoSize->setOnChangeCallback([=](Property* prop, float val) { onChangeOrthoSize(prop, val); });

	cameraProp->addChild(isMainCamera);
	cameraProp->addChild(nearClipPlane);
	cameraProp->addChild(farClipPlane);
	cameraProp->addChild(fieldOfView);
	cameraProp->addChild(clearColor);
	cameraProp->addChild(projectionType);
	cameraProp->addChild(orthoSize);

	addProperty(cameraProp);
}

void CameraEditor2::update()
{
	__super::update();
}

void CameraEditor2::onChangeIsMainCamera(Property* prop, bool val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == "Camera")
		{
			Camera* camera = (Camera*)(*it)->getAttachedObject(0);
			camera->setMainCamera(val);
		}
	}
}

void CameraEditor2::onChangeNearClipPlane(Property* prop, float val)
{
	if (val > 0)
	{
		auto sceneNodes = getSceneNodes();

		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			if ((*it)->getAttachedObject(0)->getMovableType() == "Camera")
			{
				Camera* camera = (Camera*)(*it)->getAttachedObject(0);
				camera->setNearClipDistance(val);
			}
		}

		MainWindow::gizmo2->showWireframe(true);
	}
}

void CameraEditor2::onChangeFarClipPlane(Property* prop, float val)
{
	if (val > 0)
	{
		auto sceneNodes = getSceneNodes();

		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			if ((*it)->getAttachedObject(0)->getMovableType() == "Camera")
			{
				Camera* camera = (Camera*)(*it)->getAttachedObject(0);
				camera->setFarClipDistance(val);
			}
		}

		MainWindow::gizmo2->showWireframe(true);
	}
}

void CameraEditor2::onChangeFOV(Property* prop, float val)
{
	if (val > 0)
	{
		auto sceneNodes = getSceneNodes();

		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			if ((*it)->getAttachedObject(0)->getMovableType() == "Camera")
			{
				Camera* camera = (Camera*)(*it)->getAttachedObject(0);
				camera->setFOVy(Radian(Degree(val)));
			}
		}

		MainWindow::gizmo2->showWireframe(true);
	}
}

void CameraEditor2::onChangeClearColor(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == "Camera")
		{
			Camera* camera = (Camera*)(*it)->getAttachedObject(0);
			camera->setClearColor(val);
		}
	}

	GetEngine->GetDeferredShadingSystem()->setClearColor();
}

void CameraEditor2::onChangeOrthoSize(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == "Camera")
		{
			Camera* camera = (Camera*)(*it)->getAttachedObject(0);
			camera->setOrthographicSize(val);
		}
	}
}

void CameraEditor2::onChangeProjectionType(Property* prop, int val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == "Camera")
		{
			Camera* camera = (Camera*)(*it)->getAttachedObject(0);
			camera->setProjectionType(val == 0 ? ProjectionType::PT_PERSPECTIVE : ProjectionType::PT_ORTHOGRAPHIC);
		}
	}
}
