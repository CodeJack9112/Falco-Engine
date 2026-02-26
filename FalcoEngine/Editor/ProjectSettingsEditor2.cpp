#include "stdafx.h"
#include "ProjectSettingsEditor2.h"

#include <OgreHighLevelGpuProgramManager.h>
#include <OgreHighLevelGpuProgram.h>
#include <OgreTechnique.h>
#include <OgrePass.h>

#include "../Engine/ResourceMap.h"
#include "../Engine/DeferredShading/DeferredShading.h"
#include "../Engine/DeferredShading/DeferredLightCP.h"
#include "../Engine/DeferredShading/LightMaterialGenerator.h"

#include "PropString.h"
#include "PropBool.h"
#include "PropComboBox.h"
#include "MainWindow.h"
#include "TreeView.h"

ProjectSettingsEditor2::ProjectSettingsEditor2()
{
}

ProjectSettingsEditor2::~ProjectSettingsEditor2()
{
}

void ProjectSettingsEditor2::init(string path, bool loadSettings)
{
	__super::init();

	settingsPath = path;

	if (loadSettings)
		projectSettings.Load(settingsPath);

	PropString* prop = new PropString(this, "Startup scene", projectSettings.StartupScene);
	prop->setReadonly(true);
	prop->setSupportedFormats({ "scene" });
	prop->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropStartupScene(prop, from); });

	Property* shadowSettings = new Property(this, "Shadows");

	PropBool* shadowsEnabled = new PropBool(this, "Enable shadows", projectSettings.shadowsEnabled);

	PropComboBox* dirShadows = new PropComboBox(this, "Directional resolution", { "128", "256", "512", "1024", "2048" });
	dirShadows->setCurrentItem(to_string(projectSettings.directionalShadowResolution));

	PropComboBox* spotShadows = new PropComboBox(this, "Spot resolution", { "128", "256", "512", "1024", "2048" });
	spotShadows->setCurrentItem(to_string(projectSettings.spotShadowResolution));

	PropComboBox* pointShadows = new PropComboBox(this, "Point resolution", { "128", "256", "512", "1024", "2048" });
	pointShadows->setCurrentItem(to_string(projectSettings.pointShadowResolution));

	PropComboBox* shadowCascades = new PropComboBox(this, "Cascades count", { "2", "3", "4" });
	shadowCascades->setCurrentItem(to_string(projectSettings.shadowCascadesCount));

	PropComboBox* shadowSamples = new PropComboBox(this, "Samples count", { "1", "2", "4", "6" });
	shadowSamples->setCurrentItem(to_string(projectSettings.shadowSamplesCount));

	PropBool* shadowCascadesBlending = new PropBool(this, "Cascades blending", projectSettings.shadowCascadesBlending);

	shadowSettings->addChild(shadowsEnabled);
	shadowSettings->addChild(dirShadows);
	shadowSettings->addChild(spotShadows);
	shadowSettings->addChild(pointShadows);
	shadowSettings->addChild(shadowCascades);
	shadowSettings->addChild(shadowSamples);
	shadowSettings->addChild(shadowCascadesBlending);

	dirShadows->setOnChangeCallback([=](Property * prop, string val) { onChangeDirShadows(prop, val); });
	spotShadows->setOnChangeCallback([=](Property* prop, string val) { onChangeSpotShadows(prop, val); });
	pointShadows->setOnChangeCallback([=](Property* prop, string val) { onChangePointShadows(prop, val); });
	shadowCascades->setOnChangeCallback([=](Property* prop, string val) { onChangeCascades(prop, val); });
	shadowSamples->setOnChangeCallback([=](Property* prop, string val) { onChangeSamples(prop, val); });
	shadowsEnabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeShadowsEnabled(prop, val); });
	shadowCascadesBlending->setOnChangeCallback([=](Property* prop, bool val) { onChangeCascadesBlending(prop, val); });

	addProperty(prop);
	addProperty(shadowSettings);

	Property* tags = new Property(this, "Tags");
	tags->setHasButton(true);
	tags->setButtonText("+");
	tags->setOnButtonClickCallback([=](TreeNode* prop) { onAddTag(prop); });

	int i = 0;
	for (auto it = projectSettings.tags.begin(); it != projectSettings.tags.end(); ++it, ++i)
	{
		PropString* tag = new PropString(this, to_string(i), *it);
		tag->setUserData(reinterpret_cast<void*>(new int(i)));
		tag->setPopupMenu({ "Remove" }, [=](TreeNode* node, int val) { onTagPopup(node, val); });
		tag->setOnChangeCallback([=](Property* prop, std::string val) { onChangeTag(prop, val); });

		tags->addChild(tag);
	}

	addProperty(tags);

	Property* layers = new Property(this, "Layers");
	layers->setHasButton(true);
	layers->setButtonText("+");
	layers->setOnButtonClickCallback([=](TreeNode* prop) { onAddLayer(prop); });

	i = 0;
	for (auto it = projectSettings.layers.begin(); it != projectSettings.layers.end(); ++it, ++i)
	{
		PropString* layer = new PropString(this, to_string(i), *it);
		layer->setUserData(reinterpret_cast<void*>(new int(i)));
		layer->setPopupMenu({ "Remove" }, [=](TreeNode* node, int val) { onLayerPopup(node, val); });
		layer->setOnChangeCallback([=](Property* prop, std::string val) { onChangeLayer(prop, val); });

		layers->addChild(layer);
	}

	addProperty(layers);
}

void ProjectSettingsEditor2::update()
{
	__super::update();
}

void ProjectSettingsEditor2::updateEditor()
{
	MainWindow::AddOnEndUpdateCallback([=]() {
		getTreeView()->clear();
		init(settingsPath, false);
	});
}

void ProjectSettingsEditor2::onDropStartupScene(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	projectSettings.StartupScene = fullPath;
	projectSettings.StartupSceneGuid = ResourceMap::getResourceGuidFromName(projectSettings.StartupScene);
	projectSettings.Save(settingsPath);
}

void ProjectSettingsEditor2::onChangeShadowsEnabled(Property* prop, bool val)
{
	projectSettings.shadowsEnabled = val;
	projectSettings.Save(settingsPath);
}

void resetDirectionalShadows()
{
	auto it = GetEngine->GetSceneManager()->getMovableObjectIterator(LightFactory::FACTORY_TYPE_NAME);
	while (it.hasMoreElements())
	{
		Light* light = (Light*)it.getNext();
		light->deleteShadowMaps();
	}
}

void resetSpotShadows()
{
	auto it = GetEngine->GetSceneManager()->getMovableObjectIterator(LightFactory::FACTORY_TYPE_NAME);
	while (it.hasMoreElements())
	{
		Light* light = (Light*)it.getNext();
		light->deleteShadowMaps();
	}
}

void resetPointShadows()
{
	auto it = GetEngine->GetSceneManager()->getMovableObjectIterator(LightFactory::FACTORY_TYPE_NAME);
	while (it.hasMoreElements())
	{
		Light* light = (Light*)it.getNext();
		light->deleteShadowMaps();
	}
}

void resetLightShaders()
{
	MainWindow::AddOnEndUpdateCallback([=]() {
		GetEngine->GetDeferredShadingSystem()->resetLightShaders();
	});
}

void ProjectSettingsEditor2::onChangeDirShadows(Property* prop, std::string val)
{
	projectSettings.directionalShadowResolution = atoi(val.c_str());
	projectSettings.Save(settingsPath);

	SceneManager::dirShadowResolution = atoi(val.c_str());

	resetDirectionalShadows();
}

void ProjectSettingsEditor2::onChangeSpotShadows(Property* prop, std::string val)
{
	projectSettings.spotShadowResolution = atoi(val.c_str());
	projectSettings.Save(settingsPath);

	SceneManager::spotShadowResolution = atoi(val.c_str());

	resetSpotShadows();
}

void ProjectSettingsEditor2::onChangePointShadows(Property* prop, std::string val)
{
	projectSettings.pointShadowResolution = atoi(val.c_str());
	projectSettings.Save(settingsPath);

	SceneManager::pointShadowResolution = atoi(val.c_str());

	resetPointShadows();
}

void ProjectSettingsEditor2::onChangeCascades(Property* prop, std::string val)
{
	projectSettings.shadowCascadesCount = atoi(val.c_str());
	projectSettings.Save(settingsPath);

	SceneManager::shadowCascadesCount = atoi(val.c_str());

	GetEngine->GetDeferredShadingSystem()->RecalculateShadowCascades();

	resetDirectionalShadows();
}

void ProjectSettingsEditor2::onChangeSamples(Property* prop, std::string val)
{
	projectSettings.shadowSamplesCount = atoi(val.c_str());
	projectSettings.Save(settingsPath);

	SceneManager::shadowSamplesCount = atoi(val.c_str());
}

void ProjectSettingsEditor2::onChangeCascadesBlending(Property* prop, bool val)
{
	projectSettings.shadowCascadesBlending = val;
	projectSettings.Save(settingsPath);

	SceneManager::shadowCascadesBlending = val;

	resetLightShaders();
}

void ProjectSettingsEditor2::onChangeTag(Property* prop, std::string val)
{
	void* data = prop->getUserData();
	int i = *reinterpret_cast<int*>(data);

	auto it = projectSettings.tags.begin() + i;
	*it = val;

	projectSettings.Save(settingsPath);
}

void ProjectSettingsEditor2::onAddTag(TreeNode* prop)
{
	projectSettings.tags.push_back("");
	updateEditor();
}

void ProjectSettingsEditor2::onTagPopup(TreeNode* node, int val)
{
	if (val == 0)
	{
		void* data = node->getUserData();
		int i = *reinterpret_cast<int*>(data);

		MainWindow::AddOnEndUpdateCallback([=]() {
			auto it = projectSettings.tags.begin() + i;
			projectSettings.tags.erase(it);
			projectSettings.Save(settingsPath);

			updateEditor();
		});
	}
}

void ProjectSettingsEditor2::onChangeLayer(Property* prop, std::string val)
{
	void* data = prop->getUserData();
	int i = *reinterpret_cast<int*>(data);

	auto it = projectSettings.layers.begin() + i;
	*it = val;

	projectSettings.Save(settingsPath);
}

void ProjectSettingsEditor2::onAddLayer(TreeNode* prop)
{
	projectSettings.layers.push_back("");
	updateEditor();
}

void ProjectSettingsEditor2::onLayerPopup(TreeNode* node, int val)
{
	if (val == 0)
	{
		void* data = node->getUserData();
		int i = *reinterpret_cast<int*>(data);

		MainWindow::AddOnEndUpdateCallback([=]() {
			auto it = projectSettings.layers.begin() + i;
			projectSettings.layers.erase(it);
			projectSettings.Save(settingsPath);

			updateEditor();
		});
	}
}
