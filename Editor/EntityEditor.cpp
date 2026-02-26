#include "stdafx.h"
#include "EntityEditor.h"

#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "MainWindow.h"
#include "AssetsWindow.h"
#include "MaterialEditor2.h"
#include "PropVector3.h"
#include "PropComboBox.h"
#include "PropFloat.h"
#include "PropBool.h"
#include "PropString.h"
#include "PropEditorHost.h"
#include "PropButton.h"

#include "../Engine/Engine.h"
#include "../Engine/IO.h"

EntityEditor::EntityEditor()
{
	setEditorName("EntityEditor");
}

EntityEditor::~EntityEditor()
{
}

void EntityEditor::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	SceneNode* sceneNode = getSceneNodes()[0];
	
	Entity* entity = (Entity*)getSceneNodes()[0]->getAttachedObject(0);

	//Render props
	Property* entityGroup = new Property(this, "Entity");
	entityGroup->icon = TextureManager::getSingleton().load("Icons/Hierarchy/entity.png", "Editor");

	Property* renderGroup = new Property(this, "Shadows");
	renderGroup->icon = TextureManager::getSingleton().load("Icons/Toolbar/shadows.png", "Editor");

	PropBool * castShadowsProp = new PropBool(this, "Cast shadows", entity->getCastShadows());
	castShadowsProp->setOnChangeCallback([=](Property* prop, bool val) { onChangeCastShadows(prop, val); });

	PropBool * receiveShadowsProp = new PropBool(this, "Receive shadows", true);
	receiveShadowsProp->setOnChangeCallback([=](Property* prop, bool val) { onChangeReceiveShadows(prop, val); });

	Property * lightmapGroup = new Property(this, "Lightmap");
	lightmapGroup->icon = TextureManager::getSingleton().load("Icons/Inspector/lightmap.png", "Editor");
	
	PropBool* useCustomLightmapSize = new PropBool(this, "Use custom lightmap size", entity->getUseCustomLightmapSize());
	useCustomLightmapSize->setOnChangeCallback([=](Property* prop, bool val) { onChangeUseCustomLightmapSize(prop, val); });

	PropComboBox* customLightmapSize = new PropComboBox(this, "Custom lightmap size", { "64", "128", "256", "512", "1024", "2048" });
	customLightmapSize->setCurrentItem(to_string(entity->getCustomLightmapSize()));

	if (!entity->getUseCustomLightmapSize())
		customLightmapSize->setCurrentItem(to_string(GetEngine->GetDefaultLightmapSize()).c_str());

	customLightmapSize->setOnChangeCallback([=](Property* prop, string val) { onChangeCustomLightmapSize(prop, val); });

	lightmapGroup->addChild(useCustomLightmapSize);
	lightmapGroup->addChild(customLightmapSize);

	renderGroup->addChild(castShadowsProp);
	//renderGroup->AddSubItem(receiveShadowsProp);

	entityGroup->addChild(renderGroup);
	entityGroup->addChild(lightmapGroup);

	addProperty(entityGroup);

	//Get materials and their's parameters
	if (isMaterialsIdentical())
		listMaterials(entityGroup);
}

void EntityEditor::update()
{
	__super::update();
}

void EntityEditor::updateEditor()
{
	if (getSceneNodes().size() > 0)
	{
		Entity* entity = (Entity*)getSceneNodes()[0]->getAttachedObject(0);

		std::vector<SubEntity*> subentities = entity->getSubEntities();

		for (auto it = subentities.begin(); it < subentities.end(); ++it)
		{
			//Material group
			if ((*it)->getMaterial() != nullptr && (*it)->getOriginalMaterial() != nullptr)
			{
				if ((*it)->getOriginalMaterial()->getName() != "StandardMaterial")
				{
					MaterialEditor2* materialEditor = new MaterialEditor2();
					materialEditor->setMaterials((*it)->getMaterial(), (*it)->getOriginalMaterial());
					materialEditor->updateMaterialProps();
					materialEditor->updateMaterial((*it)->getMaterial(), (*it)->getOriginalMaterial());
					delete materialEditor;
				}
			}
		}
	}

	__super::updateEditor();
}

bool EntityEditor::isMaterialsIdentical()
{
	bool result = true;

	auto sceneNodes = getSceneNodes();

	SceneNode* sceneNode = sceneNodes[0];
	Entity* entity = (Entity*)sceneNode->getAttachedObject(0);
	std::vector<SubEntity*> subentities = entity->getSubEntities();

	auto end = std::unique(subentities.begin(), subentities.end(), [](SubEntity* l, SubEntity* r) {
		if (l->getOriginalMaterial() != nullptr && r->getOriginalMaterial() != nullptr)
			return l->getOriginalMaterial()->getName() == r->getOriginalMaterial()->getName();
		else
			return l->getMaterial()->getName() == r->getMaterial()->getName();
	});

	subentities.erase(end, subentities.end());

	if (result)
	{
		for (std::vector<SceneNode*>::iterator it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			if (!result)
				break;

			SceneNode* currNode = *it;
			Entity* currEntity = (Entity*)currNode->getAttachedObject(0);
			std::vector<SubEntity*> currSubentities = currEntity->getSubEntities();

			auto _end = std::unique(currSubentities.begin(), currSubentities.end(), [](SubEntity* l, SubEntity* r) {
				if (l->getOriginalMaterial() != nullptr && r->getOriginalMaterial() != nullptr)
					return l->getOriginalMaterial()->getName() == r->getOriginalMaterial()->getName();
				else
					return l->getMaterial()->getName() == r->getMaterial()->getName();
			});

			currSubentities.erase(_end, currSubentities.end());

			if (subentities.size() != currSubentities.size())
			{
				result = false;
				break;
			}

			int i = 0;
			for (std::vector<SubEntity*>::iterator _it = subentities.begin(); _it != subentities.end(); ++_it, ++i)
			{
				SubEntity* currSubentity = *_it;

				if (currSubentities.at(i)->getOriginalMaterial() != currSubentity->getOriginalMaterial())
				{
					result = false;
					break;
				}
			}
		}
	}

	return result;
}

void EntityEditor::listMaterials(Property* parent)
{
	auto sceneNodes = getSceneNodes();
	SceneNode* sceneNode = sceneNodes[0];

	//Materials props.
	//We need to read all shader's properties and produce them for editioning
	Property * materialsProp = new Property(this, "Materials");
	materialsProp->icon = TextureManager::getSingleton().load("Icons/Inspector/materials.png", "Editor");
	Entity* entity = (Entity*)sceneNode->getAttachedObject(0);

	std::vector<SubEntity*> subentities;
	std::vector<SubEntity*>::iterator it;

	subentities = entity->getSubEntities();

	//Property* allMaterialsProp = new Property(this, "Attached materials");
	int i = 0;
	for (it = subentities.begin(); it < subentities.end(); ++it, ++i)
	{
		MaterialPtr mat = (*it)->getOriginalMaterial();
		if (mat == nullptr) mat = (*it)->getMaterial();

		PropButton * mat_prop = new PropButton(this, "Material " + to_string(i), IO::GetFileNameWithExt(mat->getName()));
		mat_prop->setSupportedFormats({ "material" });
		mat_prop->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropMaterial(prop, from); });
		mat_prop->setUserData(static_cast<void*>(new int(i)));
		mat_prop->setOnClickCallback([=](Property* prop) { onClickMaterialFile(prop); });
		
		materialsProp->addChild(mat_prop);
	}

	auto end = std::unique(subentities.begin(), subentities.end(), [](SubEntity* l, SubEntity* r) {
		if (l->getOriginalMaterial() != nullptr && r->getOriginalMaterial() != nullptr)
			return l->getOriginalMaterial()->getName() == r->getOriginalMaterial()->getName();
		else
			return l->getMaterial()->getName() == r->getMaterial()->getName();
	});

	subentities.erase(end, subentities.end());

	for (it = subentities.begin(); it < subentities.end(); ++it)
	{
		//Material group
		if ((*it)->getMaterial() != nullptr && (*it)->getOriginalMaterial() != nullptr)
		{
			if ((*it)->getOriginalMaterial()->getName() != "BaseWhite")
			{
				MaterialEditor2* materialEditor = new MaterialEditor2();
				materialEditor->init((*it)->getMaterial(), (*it)->getOriginalMaterial(), false);
				PropEditorHost * mat_prop = new PropEditorHost(this, IO::GetFileName((*it)->getOriginalMaterial()->getName()), materialEditor);
				mat_prop->icon = TextureManager::getSingleton().load("Icons/Assets/material.png", "Editor");
				materialsProp->addChild(mat_prop);
			}
		}
	}

	//Add all properties to property grid
	parent->addChild(materialsProp);
}

void EntityEditor::onChangeCastShadows(Property* prop, bool val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		(*it)->getAttachedObject(0)->setCastShadows(val);
	}
}

void EntityEditor::onChangeReceiveShadows(Property* prop, bool val)
{

}

void EntityEditor::onChangeUseCustomLightmapSize(Property* prop, bool val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Entity * entity = (Entity*)(*it)->getAttachedObject(0);
		entity->setUseCustomLightmapSize(val);
	}
}

void EntityEditor::onChangeCustomLightmapSize(Property* prop, string val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Entity* entity = (Entity*)(*it)->getAttachedObject(0);
		entity->setCustomLightmapSize(atoi(val.c_str()));
	}
}

void EntityEditor::onDropMaterial(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	int index = *static_cast<int*>(prop->getUserData());
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Entity* entity = (Entity*)(*it)->getAttachedObject(0);
		std::vector<SubEntity*> subentities = entity->getSubEntities();

		string materialName = fullPath;

		if (!MaterialManager::getSingleton().resourceExists(materialName, "Assets"))
			materialName = IO::GetFileName(from->alias);

		//MaterialEditor2* materialEditor = new MaterialEditor2();
		//materialEditor->updateMaterial(subentities.at(index)->getMaterial(), subentities.at(index)->getOriginalMaterial());
		subentities.at(index)->setMaterialName(materialName, "Assets");
		subentities.at(index)->setOriginalMaterial(MaterialPtr());
		//delete materialEditor;
	}

	updateEditor();
}

void EntityEditor::onClickMaterialFile(Property* prop)
{
	int index = *static_cast<int*>(prop->getUserData());

	auto sceneNodes = getSceneNodes();
	Entity* entity = (Entity*)(sceneNodes[0])->getAttachedObject(0);
	std::vector<SubEntity*> subentities = entity->getSubEntities();

	MaterialPtr mt = subentities.at(index)->getOriginalMaterial();
	if (mt != nullptr)
	{
		std::string matName = mt->getName();
		MainWindow::GetAssetsWindow()->focusOnFile(matName);
	}
}
