#include "stdafx.h"
#include "LightEditor2.h"

#include <OgreSceneNode.h>
#include <OgreLight.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "MainWindow.h"

#include "PropVector3.h"
#include "PropComboBox.h"
#include "PropFloat.h"
#include "PropBool.h"
#include "PropColorPicker.h"
#include "../Engine/DeferredShading/DeferredLightCP.cpp"

LightEditor2::LightEditor2()
{
	setEditorName("LightEditor");
}

LightEditor2::~LightEditor2()
{
}

bool LightEditor2::isLightTypesIdentical(std::vector<SceneNode*>& nodes)
{
	bool result = true;

	SceneNode* node = nodes.at(0);
	Light* light = (Light*)node->getAttachedObject(0);

	for (std::vector<SceneNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		Light* obj = (Light*)(*it)->getAttachedObject(0);

		if (obj->getType() != light->getType())
		{
			result = false;
			break;
		}
	}

	return result;
}

void LightEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	if (!isLightTypesIdentical(nodes))
		return;

	SceneNode* currentNode = getSceneNodes()[0];

	Light* light = (Light*)currentNode->getAttachedObject(0);

	string lightType = "Light";
	string lightMode = "Realtime";

	if (light->getType() == Light::LightTypes::LT_DIRECTIONAL)
		lightType = "Directional";
	if (light->getType() == Light::LightTypes::LT_POINT)
		lightType = "Point";
	if (light->getType() == Light::LightTypes::LT_SPOTLIGHT)
		lightType = "Spot";

	if (light->getMode() == Light::LightMode::LM_BAKED)
		lightMode = "Baked";
	if (light->getMode() == Light::LightMode::LM_REALTIME)
		lightMode = "Realtime";
	if (light->getMode() == Light::LightMode::LM_MIXED)
		lightMode = "Mixed";

	//Light
	Property* lightProp = new Property(this, "Light");
	lightProp->icon = TextureManager::getSingleton().load("Icons/Hierarchy/light.png", "Editor");

	PropComboBox* lightTypeProp = new PropComboBox(this, "Type", { "Directional", "Point", "Spot" });
	lightTypeProp->setCurrentItem(lightType);

	PropComboBox* lightModeProp = new PropComboBox(this, "Mode", { "Realtime", "Baked", "Mixed" });
	lightModeProp->setCurrentItem(lightMode);

	lightTypeProp->setOnChangeCallback([=](Property* prop, string val) { onChangeType(prop, val); });
	lightModeProp->setOnChangeCallback([=](Property* prop, string val) { onChangeMode(prop, val); });

	PropFloat* shadowBias = new PropFloat(this, "Shadow bias", light->getShadowBias());
	shadowBias->setOnChangeCallback([=](Property* prop, float val) { onChangeShadowBias(prop, val); });

	PropFloat * lightIntensity = new PropFloat(this, "Intensity", light->getPowerScale());
	lightIntensity->setOnChangeCallback([=](Property* prop, float val) { onChangeIntensity(prop, val); });

	PropFloat* lightRadius = nullptr;

	if (light->getType() != Light::LightTypes::LT_DIRECTIONAL)
	{
		lightRadius = new PropFloat(this, "Radius", light->getAttenuationRange());
		lightRadius->setOnChangeCallback([=](Property* prop, float val) { onChangeRadius(prop, val); });
	}

	PropBool * castShadowsProp = new PropBool(this, "Cast shadows", light->getCastShadows());
	castShadowsProp->setOnChangeCallback([=](Property* prop, bool val) { onChangeCastShadows(prop, val); });

	PropColorPicker * diffuseColor = new PropColorPicker(this, "Diffuse color", light->getDiffuseColour());
	diffuseColor->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeDiffuseColor(prop, val); });

	PropColorPicker * specularColor = new PropColorPicker(this, "Specular color", light->getSpecularColour());
	specularColor->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeSpecularColor(prop, val); });

	//Spotlight properties
	PropFloat * lightSpotInnerAngle = new PropFloat(this, "Inner spotlight angle", light->getSpotlightInnerAngle().valueDegrees());
	lightSpotInnerAngle->setOnChangeCallback([=](Property* prop, float val) { onChangeInnerAngle(prop, val); });

	PropFloat * lightSpotOuterAngle = new PropFloat(this, "Outer spotlight angle", light->getSpotlightOuterAngle().valueDegrees());
	lightSpotOuterAngle->setOnChangeCallback([=](Property* prop, float val) { onChangeOuterAngle(prop, val); });

	PropFloat * lightSpotFalloff = new PropFloat(this, "Spotlight falloff", light->getSpotlightFalloff());
	lightSpotFalloff->setOnChangeCallback([=](Property* prop, float val) { onChangeFalloff(prop, val); });

	//Add properties to grid
	lightProp->addChild(lightTypeProp);
	lightProp->addChild(lightModeProp);
	lightProp->addChild(lightIntensity);

	if (light->getType() != Light::LightTypes::LT_DIRECTIONAL)
		lightProp->addChild(lightRadius);

	if (light->getType() == Light::LightTypes::LT_SPOTLIGHT)
	{
		lightProp->addChild(lightSpotInnerAngle);
		lightProp->addChild(lightSpotOuterAngle);
		lightProp->addChild(lightSpotFalloff);
	}

	lightProp->addChild(castShadowsProp);
	lightProp->addChild(shadowBias);
	lightProp->addChild(diffuseColor);
	lightProp->addChild(specularColor);

	addProperty(lightProp);
}

void LightEditor2::update()
{
	__super::update();
}

void LightEditor2::onChangeDiffuseColor(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setDiffuseColour(val);
		}
	}
}

void LightEditor2::onChangeSpecularColor(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setSpecularColour(val);
		}
	}
}

void LightEditor2::onChangeIntensity(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setPowerScale(val);
		}
	}
}

void LightEditor2::onChangeRadius(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setAttenuation(val, light->getAttenuationConstant(), light->getAttenuationLinear(), light->getAttenuationQuadric());
		}
	}

	MainWindow::gizmo2->showWireframe(true);
}

void LightEditor2::onChangeInnerAngle(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setSpotlightInnerAngle(Radian(Degree(val)));
		}
	}

	MainWindow::gizmo2->showWireframe(true);
}

void LightEditor2::onChangeOuterAngle(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setSpotlightOuterAngle(Radian(Degree(val)));
		}
	}

	MainWindow::gizmo2->showWireframe(true);
}

void LightEditor2::onChangeFalloff(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setSpotlightFalloff(val);
		}
	}

	MainWindow::gizmo2->showWireframe(true);
}

void LightEditor2::onChangeCastShadows(Property* prop, bool val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setCastShadows(val);
		}
	}

	MainWindow::AddOnEndUpdateCallback([=]() { DeferredLightRenderOperation::UpdateStaticShadowmaps(); });
}

void LightEditor2::onChangeType(Property* prop, string val)
{
	auto sceneNodes = getSceneNodes();

	Light::LightTypes type = Light::LightTypes::LT_DIRECTIONAL;
	int _type = 0;

	if (val == "Directional") { type = Light::LightTypes::LT_DIRECTIONAL; _type = 2; }
	if (val == "Point") { type = Light::LightTypes::LT_POINT; _type = 0; }
	if (val == "Spot") { type = Light::LightTypes::LT_SPOTLIGHT; _type = 1; }

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->deleteShadowMaps();
			light->setType(type);
			light->setCustomParameter(1, Vector4(_type, 0.0, 0.0, 0.0));
		}
	}

	MainWindow::AddOnEndUpdateCallback([=]() {
		GetEngine->GetDeferredShadingSystem()->resetLightShaders();
	});

	GetEngine->GetDeferredShadingSystem()->resetLights();

	MainWindow::gizmo2->showWireframe(true);
	updateEditor();

	MainWindow::AddOnEndUpdateCallback([=]() { DeferredLightRenderOperation::UpdateStaticShadowmaps(); });
}

void LightEditor2::onChangeMode(Property* prop, string val)
{
	auto sceneNodes = getSceneNodes();

	Light::LightMode mode = Light::LightMode::LM_REALTIME;

	if (val == "Realtime") mode = Light::LightMode::LM_REALTIME;
	if (val == "Baked") mode = Light::LightMode::LM_BAKED;
	if (val == "Mixed") mode = Light::LightMode::LM_MIXED;

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setMode(mode);
		}
	}

	MainWindow::AddOnEndUpdateCallback([=]() { DeferredLightRenderOperation::UpdateStaticShadowmaps(); });
}

void LightEditor2::onChangeShadowBias(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if ((*it)->getAttachedObject(0)->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
		{
			Light* light = (Light*)(*it)->getAttachedObject(0);
			light->setShadowBias(val);
		}
	}
}
