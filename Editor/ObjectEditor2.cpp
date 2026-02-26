#include "stdafx.h"

#include "ObjectEditor2.h"

#include "RigidbodyEditor2.h"
#include "AudioSourceEditor2.h"
#include "AudioListenerEditor2.h"
#include "MeshColliderEditor2.h"
#include "CapsuleColliderEditor2.h"
#include "BoxColliderEditor2.h"
#include "SphereColliderEditor2.h"
#include "FixedJointEditor2.h"
#include "HingeJointEditor2.h"
#include "CharacterJointEditor2.h"
#include "SkeletonBoneEditor2.h"
#include "AnimationListEditor2.h"
#include "NavMeshAgentEditor2.h"
#include "NavMeshObstacleEditor2.h"
#include "VehicleEditor2.h"

#include "TreeView.h"
#include "Property.h"
#include "PropEditorHost.h"
#include "PropVector2.h"
#include "PropVector3.h"
#include "PropVector4.h"
#include "PropString.h"
#include "PropBool.h"
#include "PropInt.h"
#include "PropFloat.h"
#include "PropGameObject.h"
#include "PropComboBox.h"
#include "MainWindow.h"
#include "InspectorWindow.h"
#include "HierarchyWindow.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include <OgreSceneNode.h>
#include <OgreTextureManager.h>
#include <Ogre/source/OgreMain/custom/Component.h>

#include "../Engine/Euler.h"
#include "../Engine/MonoRuntime.h"
#include "../Engine/Engine.h"
#include "../Engine/StringConverter.h"
#include "../Engine/IO.h"
#include "../Engine/ResourceMap.h"
#include "../Engine/SceneSerializer.h"
#include "../Engine/ProjectSettings.h"
#include "../Engine/NavigationManager.h"

#include "../Engine/AudioSource.h"
#include "../Engine/AudioListener.h"
#include "../Engine/RigidBody.h"
#include "../Engine/MeshCollider.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/BoxCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/SkeletonBone.h"
#include "../Engine/AnimationList.h"
#include "../Engine/FixedJoint.h"
#include "../Engine/HingeJoint.h"
#include "../Engine/CharacterJoint.h"
#include "../Engine/NavMeshAgent.h"
#include "../Engine/NavMeshObstacle.h"
#include "../Engine/Vehicle.h"

#include "../Engine/Mathf.h"
#include "../Engine/SkinnedMeshDeformer.h"
#include "../Engine/CloneTools.h"
#include "../Ogre/source/OgreMain/custom/MonoScript.h"

using namespace Ogre;
using namespace std;

bool ObjectEditor2::bufferFilled = false;
Vector3 ObjectEditor2::bufferPosition = Vector3::ZERO;
Vector3 ObjectEditor2::bufferScale = Vector3::ZERO;
Quaternion ObjectEditor2::bufferOrientation = Quaternion::IDENTITY;
Component* ObjectEditor2::bufferComponent = nullptr;
MonoScript* ObjectEditor2::bufferScript = nullptr;

void ChangeLightmapStaticRecursive(SceneNode* node, bool value);
void ChangeNavigationStaticRecursive(SceneNode* node, bool value);
void ChangeStaticBatchingRecursive(SceneNode* node, bool value);

struct ScriptPropData
{
public:
	int tagInt = 0;
	MonoScript::MonoFieldInfo fieldInfo;
};

ObjectEditor2::ObjectEditor2()
{
	setEditorName("ObjectEditor");
	getTreeView()->setOnEndUpdateCallback([=]() { onTreeViewEndUpdate(); });
}

ObjectEditor2::~ObjectEditor2()
{
	
}

void ObjectEditor2::init(std::vector<SceneNode*> nodes)
{
	sceneNodes = nodes;
	SceneNode* currentNode = nodes[0];

	GetEngine->loadProjectSettings();
	ProjectSettings* projectSettings = GetEngine->getProjectSettings();

	std::vector<string> tags = projectSettings->tags;
	std::vector<string> layers = projectSettings->layers;

	int _tag = currentNode->tag;
	int _layer = currentNode->layer;
	
	tags.insert(tags.begin(), "Untagged");
	layers.insert(layers.begin(), "Default");

	if (_tag < 0 || _tag > tags.size() - 1)
		_tag = 0;

	if (_layer < 0 || _layer > layers.size() - 1)
		_layer = 0;

	/* BASE */
	Vector3 eulerAngles = Mathf::toEuler(currentNode->getOrientation());

	PropBool* enabled = new PropBool(this, "Enabled", currentNode->getGlobalVisible());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropString* name = new PropString(this, "Name", CP_UNI(currentNode->getAlias()));
	name->setOnChangeCallback([=](Property* prop, string val) { onChangeName(prop, val); });

	PropComboBox* tag = new PropComboBox(this, "Tag", tags);
	tag->setCurrentItem(_tag);
	tag->setOnChangeCallback([=](Property* prop, int val) { onChangeTag(prop, val); });

	PropComboBox* layer = new PropComboBox(this, "Layer", layers);
	layer->setCurrentItem(_layer);
	layer->setOnChangeCallback([=](Property* prop, int val) { onChangeLayer(prop, val); });

	PropBool* staticBatching = new PropBool(this, "Static batching", currentNode->staticBatching);
	PropBool* lightmapStatic = new PropBool(this, "Lightmap Static", currentNode->lightmapStatic);
	PropBool* navigationStatic = new PropBool(this, "Navigation Static", currentNode->navigationStatic);

	Property* transform = new Property(this, "Transform");
	transform->icon = TextureManager::getSingleton().load("Icons/Inspector/transform.png", "Editor");
	transform->setPopupMenu({ "Copy values", "Paste values" }, [=](TreeNode* node, int val) { onTransformPopup(node, val); });

	position = new PropVector3(this, "Position", currentNode->getPosition());
	rotation = new PropVector3(this, "Rotation", eulerAngles);
	scale = new PropVector3(this, "Scale", currentNode->getScale());

	position->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangePosition(prop, val); });
	rotation->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeRotation(prop, val); });
	scale->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeScale(prop, val); });
	staticBatching->setOnChangeCallback([=](Property* prop, bool val) { onChangeStaticBatching(prop, val); });
	lightmapStatic->setOnChangeCallback([=](Property* prop, bool val) { onChangeLightmapStatic(prop, val); });
	navigationStatic->setOnChangeCallback([=](Property* prop, bool val) { onChangeNavigationStatic(prop, val); });

	transform->addChild(position);
	transform->addChild(rotation);
	transform->addChild(scale);

	addProperty(enabled);
	addProperty(name);
	addProperty(tag);
	addProperty(layer);
	addProperty(staticBatching);
	addProperty(lightmapStatic);
	addProperty(navigationStatic);
	addProperty(transform);
	/* BASE */

	/* SCRIPTS */
	if (isScriptsIdentical())
		listScripts();
	/* SCRIPTS */

	/* COMPONENTS */
	if (isComponentsIdentical())
		listComponents();
	/* COMPONENTS */
}

void ObjectEditor2::update()
{
	if (isPrefabSelected())
	{
		string _prefabName = ResourceMap::getResourceNameFromGuid(sceneNodes[0]->prefabGuid);
		if (_prefabName.empty())
			_prefabName = sceneNodes[0]->prefabName;

		ImVec4 color = ImVec4(0.61f, 0.79f, 0.90f, 1.0f);

		if (!IO::FileExists(MainWindow::GetProjectAssetsDir() + _prefabName))
			color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);

		ImGui::Text("Prefab:");
		ImGui::SameLine();
		ImGui::TextColored(color, _prefabName.c_str());
		if (ImGui::Button("Break"))
		{
			ImGui::OpenPopup("Break prefab?");
		}
		ImGui::SameLine();
		if (ImGui::Button("Revert"))
		{
			revertPrefab(_prefabName);
		}
		ImGui::SameLine();
		if (ImGui::Button("Apply"))
		{
			applyPrefab(_prefabName);
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
	}

	if (ImGui::BeginPopupModal("Break prefab?"))
	{
		ImGui::Text("You will lose this prefab connection.");
		ImGui::SetNextItemWidth(-1.0f);

		if (ImGui::Button("Yes", ImVec2(80, 0)))
		{
			breakPrefab();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No", ImVec2(80, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (changeStaticLightmapNode != nullptr)
	{
		if (!ImGui::IsPopupOpen("Lightmap static"))
			ImGui::OpenPopup("Lightmap static");
	}

	if (changeStaticNavMeshNode != nullptr)
	{
		if (!ImGui::IsPopupOpen("Navigation static"))
			ImGui::OpenPopup("Navigation static");
	}

	if (changeStaticBatchingNode != nullptr)
	{
		if (!ImGui::IsPopupOpen("Static batching"))
			ImGui::OpenPopup("Static batching");
	}

	if (ImGui::BeginPopupModal("Lightmap static"))
	{
		ImGui::Text("Do you want to change children?");
		ImGui::SetNextItemWidth(-1.0f);

		if (ImGui::Button("Yes", ImVec2(80, 0)))
		{
			ChangeLightmapStaticRecursive(changeStaticLightmapNode, changeStaticLightmapNode->lightmapStatic);
			changeStaticLightmapNode = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No", ImVec2(80, 0)))
		{
			changeStaticLightmapNode = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Navigation static"))
	{
		ImGui::Text("Do you want to change children?");
		ImGui::SetNextItemWidth(-1.0f);

		if (ImGui::Button("Yes", ImVec2(80, 0)))
		{
			ChangeNavigationStaticRecursive(changeStaticNavMeshNode, changeStaticNavMeshNode->navigationStatic);
			changeStaticNavMeshNode = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No", ImVec2(80, 0)))
		{
			changeStaticNavMeshNode = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Static batching"))
	{
		ImGui::Text("Do you want to change children?");
		ImGui::SetNextItemWidth(-1.0f);

		if (ImGui::Button("Yes", ImVec2(80, 0)))
		{
			ChangeStaticBatchingRecursive(changeStaticBatchingNode, changeStaticBatchingNode->staticBatching);
			changeStaticBatchingNode = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No", ImVec2(80, 0)))
		{
			changeStaticBatchingNode = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	__super::update();
}

void ObjectEditor2::updateEditor()
{
	MainWindow::AddOnEndUpdateCallback([=]() {
		float sp = MainWindow::GetInspectorWindow()->getScrollPos();
		MainWindow::GetInspectorWindow()->saveCollapsedProperties();

		auto props = getTreeView()->getRootNode()->children;

		for (auto it = props.begin(); it != props.end(); ++it)
			removeProperty((Property*)*it);

		init(sceneNodes);

		MainWindow::GetInspectorWindow()->loadCollapsedProperties();
		MainWindow::GetInspectorWindow()->setScrollPos(sp);
	});
}

void ObjectEditor2::updateTransform()
{
	SceneNode* currentNode = sceneNodes[0];
	SceneNode* parentNode = currentNode->getParentSceneNode();
	SceneNode* gizmoNode = MainWindow::gizmo2->getSelectionRootNode();

	if (gizmoNode->getChildren().size() > 0)
	{
		if (MainWindow::gizmo2->getParentBuffer()[currentNode] != nullptr)
			parentNode = MainWindow::gizmo2->getParentBuffer()[currentNode];
	}

	Affine3 m1 = currentNode->_getFullTransform();
	Affine3 m2 = parentNode->_getFullTransform();
	Affine3 m3 = m2.inverse() * m1;

	Quaternion q = parentNode->_getDerivedOrientation().Inverse() * currentNode->_getDerivedOrientation();
	Vector3 ds = parentNode->_getDerivedScale();
	Vector3 scl = Vector3(1.0f / ds.x, 1.0f / ds.y, 1.0f / ds.z) * currentNode->_getDerivedScale();

	Vector3 eulerAngles = Mathf::toEuler(q);
	
	position->setValue(m3.getTrans());
	rotation->setValue(eulerAngles);
	scale->setValue(scl);
}

void ObjectEditor2::resetBufferObjects()
{
	bufferFilled = false;
	bufferPosition = Vector3::ZERO;
	bufferScale = Vector3::ZERO;
	bufferOrientation = Quaternion::IDENTITY;
	bufferComponent = nullptr;
	bufferScript = nullptr;
}

bool ObjectEditor2::isPrefabSelected()
{
	bool _nodes = sceneNodes.size() == 1;
	bool _prefab = false;

	if (_nodes)
		_prefab = sceneNodes[0]->prefabGuid != "" || sceneNodes[0]->prefabName != "";

	return _nodes && _prefab;
}

void ObjectEditor2::revertPrefab(std::string prefab)
{
	MainWindow::AddOnEndUpdateCallback([=]() {
		SceneNode* node = sceneNodes[0];

		if (!prefab.empty())
		{
			string _path = MainWindow::GetProjectAssetsDir() + prefab;

			if (IO::FileExists(_path))
			{
				Vector3 _oldPos = node->getPosition();
				Quaternion _oldRot = node->getOrientation();
				Vector3 _oldScl = node->getScale();

				SceneNode* parent = (SceneNode*)node->getParent();

				auto childVec = parent->getChildrenList();
				int _pos = std::distance(childVec.begin(), std::find(childVec.begin(), childVec.end(), node));

				string objName = node->getName();

				MainWindow::gizmo2->clearSelection();

				parent->removeChild(node);
				GetEngine->Destroy(node);

				SceneSerializer serializer;
				SceneNode* prefabNode = serializer.DeserializeFromPrefab(GetEngine->GetSceneManager(), _path);
				prefabNode->setName(objName);

				prefabNode->getParent()->removeChild(prefabNode);
				parent->insertChild(prefabNode, _pos);

				prefabNode->setPosition(_oldPos);
				prefabNode->setScale(_oldScl);
				prefabNode->setOrientation(_oldRot);

				prefabNode->setAlias(IO::GetFileName(_path));

				MainWindow::GetHierarchyWindow()->refreshHierarchy();

				//MainWindow::Get()->ProcessSceneNode(prefabNode, true, false);
				MainWindow::gizmo2->selectObject(prefabNode, false);
			}
		}
	});
}

void ObjectEditor2::applyPrefab(std::string prefab)
{
	SceneNode* node = sceneNodes[0];

	if (!prefab.empty())
	{
		string _path = MainWindow::GetProjectAssetsDir() + prefab;

		if (!IO::isDir(_path))
		{
			SceneSerializer serializer;
			serializer.SerializeToPrefab(node, _path);
		}
	}
}

void ObjectEditor2::breakPrefab()
{
	SceneNode* node = sceneNodes[0];

	node->prefabName = "";
	node->prefabGuid = "";

	MainWindow::AddOnEndUpdateCallback([=]() {
		MainWindow::GetHierarchyWindow()->refreshHierarchy();
	});
}

void ObjectEditor2::onTreeViewEndUpdate()
{
}

bool ObjectEditor2::isScriptsIdentical()
{
	bool result = true;

	if (sceneNodes.size() == 0)
		return true;

	SceneNode* sceneNode = sceneNodes[0];

	if (result)
	{
		for (std::vector<SceneNode*>::iterator it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			if (!result)
				break;

			SceneNode* currNode = *it;

			if (currNode->monoScripts.size() != sceneNode->monoScripts.size())
			{
				result = false;
				break;
			}

			int i = 0;
			for (std::vector<MonoScript*>::iterator _it = sceneNode->monoScripts.begin(); _it != sceneNode->monoScripts.end(); ++_it, ++i)
			{
				MonoScript* currScript = *_it;

				if (currNode->monoScripts.at(i)->_class != currScript->_class)
				{
					result = false;
					break;
				}
			}
		}
	}

	return result;
}

bool ObjectEditor2::isComponentsIdentical()
{
	bool result = true;

	if (sceneNodes.size() == 0)
		return true;

	SceneNode* sceneNode = sceneNodes[0];

	if (result)
	{
		for (std::vector<SceneNode*>::iterator it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			if (!result)
				break;

			SceneNode* currNode = *it;

			if (currNode->components.size() != sceneNode->components.size())
			{
				result = false;
				break;
			}

			int i = 0;
			for (std::vector<Component*>::iterator _it = sceneNode->components.begin(); _it != sceneNode->components.end(); ++_it, ++i)
			{
				Component* currComponent = *_it;

				if (currNode->components.at(i)->GetComponentTypeName() != currComponent->GetComponentTypeName())
				{
					result = false;
					break;
				}
			}
		}
	}

	return result;
}

void ObjectEditor2::listScripts()
{
	Property* scripts = new Property(this, "Scripts");
	scripts->setSupportedFormats({ "cs" });
	scripts->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropScript(prop, from); });
	scripts->icon = TextureManager::getSingleton().load("Icons/Inspector/scripts.png", "Editor");
	scripts->setPopupMenu({ "Paste script" }, [=](TreeNode* node, int val) { onScriptsPopup(node, val); });

	SceneNode* sceneNode = sceneNodes[0];

	int i = 0;
	for (auto it = sceneNode->monoScripts.begin(); it < sceneNode->monoScripts.end(); ++it, ++i)
	{
		std::string _className = mono_class_get_name((*it)->_class);
		Property* scriptProp = new Property(this, _className);
		scriptProp->icon = TextureManager::getSingleton().load("Icons/Inspector/cs.png", "Editor");
		scriptProp->setUserData(static_cast<void*>(new int(i)));
		scriptProp->setPopupMenu({ "Copy script", "-", "Remove" }, [=](TreeNode* node, int val) { onScriptPopup(node, val); });

		PropBool* enabled = new PropBool(this, "Enabled", (*it)->enabled);
		enabled->setUserData(static_cast<void*>(new int(i)));
		enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeScriptEnabled(prop, val); });
		scriptProp->addChild(enabled);

		//List fields
		MonoRuntime::FieldList fields = GetEngine->GetMonoRuntime()->GetFields(*it);

		for (auto f = fields.begin(); f != fields.end(); ++f)
		{
			Property* fieldProp = nullptr;

			if (f->fieldType == "System.Int32")
			{
				fieldProp = new PropInt(this, CP_UNI(f->fieldName), f->fieldValue.intVal);
				((PropInt*)fieldProp)->setOnChangeCallback([=](Property* prop, int val) { onChangeScriptInt(prop, val); });
			}

			if (f->fieldType == "System.Single")
			{
				fieldProp = new PropFloat(this, CP_UNI(f->fieldName), f->fieldValue.floatVal);
				((PropFloat*)fieldProp)->setOnChangeCallback([=](Property* prop, float val) { onChangeScriptFloat(prop, val); });
			}

			if (f->fieldType == "System.Boolean")
			{
				fieldProp = new PropBool(this, CP_UNI(f->fieldName), f->fieldValue.boolVal);
				((PropBool*)fieldProp)->setOnChangeCallback([=](Property* prop, bool val) { onChangeScriptBool(prop, val); });
			}

			if (f->fieldType == "FalcoEngine.Vector2")
			{
				fieldProp = new PropVector2(this, CP_UNI(f->fieldName), f->fieldValue.vec2Val);
				((PropVector2*)fieldProp)->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeScriptVector2(prop, val); });
			}

			if (f->fieldType == "FalcoEngine.Vector3")
			{
				fieldProp = new PropVector3(this, CP_UNI(f->fieldName), f->fieldValue.vec3Val);
				((PropVector3*)fieldProp)->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeScriptVector3(prop, val); });
			}

			if (f->fieldType == "FalcoEngine.Vector4")
			{
				fieldProp = new PropVector4(this, CP_UNI(f->fieldName), f->fieldValue.vec4Val);
				((PropVector4*)fieldProp)->setOnChangeCallback([=](Property* prop, Vector4 val) { onChangeScriptVector4(prop, val); });
			}

			if (f->fieldType == "System.String")
			{
				fieldProp = new PropString(this, CP_UNI(f->fieldName), CP_UNI(f->fieldValue.stringVal));
				fieldProp->setSupportedFormats({ "::Asset" });
				((PropString*)fieldProp)->setOnChangeCallback([=](Property* prop, string val) { onChangeScriptString(prop, val); });
				((PropString*)fieldProp)->setOnDropCallback([=](TreeNode* prop, TreeNode * from) { onDropScriptString(prop, from); });
			}

			if (f->fieldType == "FalcoEngine.GameObject")
			{
				if (GetEngine->GetSceneManager()->hasSceneNode(f->fieldValue.objectVal))
				{
					SceneNode* node = GetEngine->GetSceneManager()->getSceneNode(f->fieldValue.objectVal);
					fieldProp = new PropGameObject(this, CP_UNI(f->fieldName), node);
				}
				else
				{
					fieldProp = new PropGameObject(this, CP_UNI(f->fieldName), nullptr);
				}

				fieldProp->setSupportedFormats({ "::SceneNode" });
				fieldProp->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropScriptGameObject(prop, from); });
			}

			if (fieldProp == nullptr)
				fieldProp = new PropString(this, CP_UNI(f->fieldName), "");

			ScriptPropData * propData = new ScriptPropData();
			propData->tagInt = i;
			propData->fieldInfo = *f;

			fieldProp->setUserData(static_cast<void*>(propData));

			scriptProp->addChild(fieldProp);
		}

		scripts->addChild(scriptProp);
	}

	addProperty(scripts);
}

void ObjectEditor2::listComponents()
{
	Property* components = new Property(this, "Components");
	components->icon = TextureManager::getSingleton().load("Icons/Inspector/components.png", "Editor");
	components->setPopupMenu({ "Paste component" }, [=](TreeNode* node, int val) { onComponentsPopup(node, val); });

	SceneNode* sceneNode = sceneNodes[0];

	int i = 0;
	for (auto it = sceneNode->components.begin(); it < sceneNode->components.end(); ++it, ++i)
	{
		PropEditorHost* component = nullptr;
		PropertyEditor* hostedEditor = nullptr;
		Component* comp = *it;

		/* AUDIO */
		if (comp->GetComponentTypeName() == AudioSource::COMPONENT_TYPE)
		{
			AudioSourceEditor2* ed = new AudioSourceEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, AudioSource::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/audio_source.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == AudioListener::COMPONENT_TYPE)
		{
			AudioListenerEditor2* ed = new AudioListenerEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, AudioListener::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/audio_listener.png", "Editor");
			hostedEditor = ed;
		}

		/* PHYSICS */
		if (comp->GetComponentTypeName() == RigidBody::COMPONENT_TYPE)
		{
			RigidbodyEditor2* ed = new RigidbodyEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, RigidBody::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/rigidbody.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == MeshCollider::COMPONENT_TYPE)
		{
			MeshColliderEditor2* ed = new MeshColliderEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, MeshCollider::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/mesh_collider.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == CapsuleCollider::COMPONENT_TYPE)
		{
			CapsuleColliderEditor2* ed = new CapsuleColliderEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, CapsuleCollider::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/capsule_collider.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == BoxCollider::COMPONENT_TYPE)
		{
			BoxColliderEditor2* ed = new BoxColliderEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, BoxCollider::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/box_collider.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == SphereCollider::COMPONENT_TYPE)
		{
			SphereColliderEditor2* ed = new SphereColliderEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, SphereCollider::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/sphere_collider.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == FixedJoint::COMPONENT_TYPE)
		{
			FixedJointEditor2* ed = new FixedJointEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, FixedJoint::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/fixed_joint.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == HingeJoint::COMPONENT_TYPE)
		{
			HingeJointEditor2* ed = new HingeJointEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, HingeJoint::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/hinge_joint.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == CharacterJoint::COMPONENT_TYPE)
		{
			CharacterJointEditor2* ed = new CharacterJointEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, CharacterJoint::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/character_joint.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == Vehicle::COMPONENT_TYPE)
		{
			VehicleEditor2* ed = new VehicleEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, Vehicle::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/vehicle.png", "Editor");
			hostedEditor = ed;
		}

		/* ANIMATION */
		if (comp->GetComponentTypeName() == SkeletonBone::COMPONENT_TYPE)
		{
			SkeletonBoneEditor2* ed = new SkeletonBoneEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, SkeletonBone::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/bone.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == AnimationList::COMPONENT_TYPE)
		{
			AnimationListEditor2* ed = new AnimationListEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, AnimationList::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/animation.png", "Editor");
			hostedEditor = ed;
		}

		/* NAVIGATION */
		if (comp->GetComponentTypeName() == NavMeshAgent::COMPONENT_TYPE)
		{
			NavMeshAgentEditor2* ed = new NavMeshAgentEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, NavMeshAgent::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/navmesh_agent.png", "Editor");
			hostedEditor = ed;
		}

		if (comp->GetComponentTypeName() == NavMeshObstacle::COMPONENT_TYPE)
		{
			NavMeshObstacleEditor2* ed = new NavMeshObstacleEditor2();
			ed->init(sceneNodes);

			component = new PropEditorHost(this, NavMeshObstacle::COMPONENT_TYPE, ed);
			component->icon = TextureManager::getSingleton().load("Icons/Inspector/navmesh_agent.png", "Editor");
			hostedEditor = ed;
		}

		if (component != nullptr)
		{
			if (hostedEditor != nullptr)
				hostedEditor->getTreeView()->getRootNode()->setUserData(static_cast<void*>(new int(i)));

			component->setUserData(static_cast<void*>(new int(i)));
			component->setPopupMenu({ "Copy component", "-", "Remove" }, [=](TreeNode* node, int val) { onComponentPopup(node, val); });

			components->addChild(component);
		}
	}

	addProperty(components);
}

void changeNodeColor(TreeNode* root, float alpha)
{
	if (alpha == 1.0f)
	{
		SceneNode* n = GetEngine->GetSceneManager()->getSceneNodeFast(root->name);
		if (n != nullptr)
		{
			if (n->getVisible())
			{
				root->color[3] = alpha;

				for (auto it = root->children.begin(); it != root->children.end(); ++it)
					changeNodeColor(*it, alpha);
			}
		}
	}
	else
	{
		root->color[3] = alpha;

		for (auto it = root->children.begin(); it != root->children.end(); ++it)
			changeNodeColor(*it, alpha);
	}
}

void ObjectEditor2::onChangeEnabled(Property* prop, bool val)
{
	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		node->setVisible(val);

		TreeView * tv = MainWindow::GetHierarchyWindow()->getTreeView();
		TreeNode * n = tv->getNodeByName(node->getName(), tv->getRootNode());
		if (n != nullptr)
		{
			if (val)
			{
				if (node->getParentSceneNode()->getVisible())
					changeNodeColor(n, 1.0f);
			}
			else
				changeNodeColor(n, 0.5f);
		}
	}

	if (val == true)
		MainWindow::gizmo2->showWireframe(true);

	GetEngine->GetNavigationManager()->setNavMeshIsDirty();
}

void ObjectEditor2::onChangeName(Property* prop, string val)
{
	if (val.empty())
		return;

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		node->setAlias(CP_SYS(val));

		TreeView* tree = MainWindow::GetHierarchyWindow()->getTreeView();
		TreeNode* treeNode = tree->getNodeByName(node->getName(), tree->getRootNode());
		if (treeNode != nullptr)
			treeNode->alias = val;
	}
}

void ObjectEditor2::onChangePosition(Property* prop, Vector3 val)
{
	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		node->setPosition(val);
	}

	MainWindow::gizmo2->updatePosition();

	MainWindow::AddOnEndUpdateCallback([=]() {
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SkinnedMeshDeformer::EnableAnimation(*it, false);
			SkinnedMeshDeformer::UpdateSkeleton(*it);
		}

		MainWindow::gizmo2->updatePosition();
	});
}

void ObjectEditor2::onChangeRotation(Property* prop, Vector3 val)
{
	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;

		Quaternion rotation = Mathf::toQuaternion(val);

		node->setOrientation(rotation);
	}

	MainWindow::gizmo2->updatePosition();

	MainWindow::AddOnEndUpdateCallback([=]() {
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SkinnedMeshDeformer::EnableAnimation(*it, false);
			SkinnedMeshDeformer::UpdateSkeleton(*it);
		}

		MainWindow::gizmo2->updatePosition();
	});
}

void ObjectEditor2::onChangeScale(Property* prop, Vector3 val)
{
	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		node->setScale(val);
	}

	MainWindow::gizmo2->updatePosition();

	MainWindow::AddOnEndUpdateCallback([=]() {
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SkinnedMeshDeformer::EnableAnimation(*it, false);
			SkinnedMeshDeformer::UpdateSkeleton(*it);
		}

		MainWindow::gizmo2->updatePosition();
	});
}

int GetChildCountExceptGizmos(SceneNode* node)
{
	int count = 0;

	VectorIterator it = node->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode* child = (SceneNode*)it.getNext();

		if (!GetEngine->IsEditorObject(child))
			++count;
	}

	return count;
}

void ChangeLightmapStaticRecursive(SceneNode* node, bool value)
{
	VectorIterator it = node->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode* child = (SceneNode*)it.getNext();
		child->lightmapStatic = value;

		ChangeLightmapStaticRecursive(child, value);
	}
}

void ChangeNavigationStaticRecursive(SceneNode* node, bool value)
{
	VectorIterator it = node->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode* child = (SceneNode*)it.getNext();
		child->navigationStatic = value;

		ChangeNavigationStaticRecursive(child, value);
	}
}

void ChangeStaticBatchingRecursive(SceneNode* node, bool value)
{
	VectorIterator it = node->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode* child = (SceneNode*)it.getNext();
		child->staticBatching = value;

		ChangeStaticBatchingRecursive(child, value);
	}
}

void ObjectEditor2::onChangeLightmapStatic(Property* prop, bool val)
{
	if (sceneNodes.size() == 1)
	{
		sceneNodes[0]->lightmapStatic = val;

		if (GetChildCountExceptGizmos(sceneNodes[0]) > 0)
		{
			changeStaticLightmapNode = sceneNodes[0];
		}
	}
	else
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;
			node->lightmapStatic = val;
		}
	}
}

void ObjectEditor2::onChangeNavigationStatic(Property* prop, bool val)
{
	if (sceneNodes.size() == 1)
	{
		sceneNodes[0]->navigationStatic = val;

		if (GetChildCountExceptGizmos(sceneNodes[0]) > 0)
		{
			changeStaticNavMeshNode = sceneNodes[0];
		}
	}
	else
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;
			node->navigationStatic = val;
		}
	}
}

void ObjectEditor2::onChangeStaticBatching(Property* prop, bool val)
{
	if (sceneNodes.size() == 1)
	{
		sceneNodes[0]->staticBatching = val;

		if (GetChildCountExceptGizmos(sceneNodes[0]) > 0)
		{
			changeStaticBatchingNode = sceneNodes[0];
		}
	}
	else
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;
			node->staticBatching = val;
		}
	}
}

void ObjectEditor2::onChangeScriptEnabled(Property* prop, bool val)
{
	void* data = prop->getUserData();
	int tagInt = *static_cast<int*>(data);

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		MonoScript* monoScript = node->monoScripts.at(tagInt);

		monoScript->enabled = val;
	}
}

void ObjectEditor2::onChangeScriptInt(Property* prop, int val)
{
	ScriptPropData* data = static_cast<ScriptPropData*>(prop->getUserData());
	int tagInt = data->tagInt;
	MonoScript::MonoFieldInfo fieldInfo = data->fieldInfo;

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		MonoScript* monoScript = node->monoScripts.at(tagInt);

		MonoClass* _class = mono_object_get_class((MonoObject*)monoScript->object);
		MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fieldInfo.fieldName).c_str());

		MonoScript::MonoFieldInfo inf;
		inf.fieldName = fieldInfo.fieldName;
		inf.fieldType = fieldInfo.fieldType;
		inf.fieldValue = MonoScript::ValueVariant(val);

		mono_field_set_value((MonoObject*)monoScript->object, _fld, &val);

		monoScript->fieldSerializeList[fieldInfo.fieldName] = inf;
	}
}

void ObjectEditor2::onChangeScriptFloat(Property* prop, float val)
{
	ScriptPropData* data = static_cast<ScriptPropData*>(prop->getUserData());
	int tagInt = data->tagInt;
	MonoScript::MonoFieldInfo fieldInfo = data->fieldInfo;

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		MonoScript* monoScript = node->monoScripts.at(tagInt);

		MonoClass* _class = mono_object_get_class((MonoObject*)monoScript->object);
		MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fieldInfo.fieldName).c_str());

		MonoScript::MonoFieldInfo inf;
		inf.fieldName = fieldInfo.fieldName;
		inf.fieldType = fieldInfo.fieldType;
		inf.fieldValue = MonoScript::ValueVariant(val);

		mono_field_set_value((MonoObject*)monoScript->object, _fld, &val);

		monoScript->fieldSerializeList[fieldInfo.fieldName] = inf;
	}
}

void ObjectEditor2::onChangeScriptBool(Property* prop, bool val)
{
	ScriptPropData* data = static_cast<ScriptPropData*>(prop->getUserData());
	int tagInt = data->tagInt;
	MonoScript::MonoFieldInfo fieldInfo = data->fieldInfo;

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		MonoScript* monoScript = node->monoScripts.at(tagInt);

		MonoClass* _class = mono_object_get_class((MonoObject*)monoScript->object);
		MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fieldInfo.fieldName).c_str());

		MonoScript::MonoFieldInfo inf;
		inf.fieldName = fieldInfo.fieldName;
		inf.fieldType = fieldInfo.fieldType;
		inf.fieldValue = MonoScript::ValueVariant(val);

		mono_field_set_value((MonoObject*)monoScript->object, _fld, &val);

		monoScript->fieldSerializeList[fieldInfo.fieldName] = inf;
	}
}

void ObjectEditor2::onChangeScriptString(Property* prop, string val)
{
	ScriptPropData* data = static_cast<ScriptPropData*>(prop->getUserData());
	int tagInt = data->tagInt;
	MonoScript::MonoFieldInfo fieldInfo = data->fieldInfo;

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		MonoScript* monoScript = node->monoScripts.at(tagInt);

		MonoClass* _class = mono_object_get_class((MonoObject*)monoScript->object);
		MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fieldInfo.fieldName).c_str());

		MonoScript::MonoFieldInfo inf;
		inf.fieldName = fieldInfo.fieldName;
		inf.fieldType = fieldInfo.fieldType;
		inf.fieldValue = MonoScript::ValueVariant(val);

		MonoString* _str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(val).c_str());
		mono_field_set_value((MonoObject*)monoScript->object, _fld, _str);

		monoScript->fieldSerializeList[fieldInfo.fieldName] = inf;
	}
}

void ObjectEditor2::onDropScriptGameObject(TreeNode* prop, TreeNode * from)
{
	ScriptPropData* data = static_cast<ScriptPropData*>(prop->getUserData());
	int tagInt = data->tagInt;
	MonoScript::MonoFieldInfo fieldInfo = data->fieldInfo;

	string objName = from->name;

	if (GetEngine->GetSceneManager()->hasSceneNode(objName))
	{
		SceneNode* _node = GetEngine->GetSceneManager()->getSceneNode(objName);
		((PropGameObject*)prop)->setValue(_node);

		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;
			MonoScript* monoScript = node->monoScripts.at(tagInt);

			MonoClass* _class = mono_object_get_class((MonoObject*)monoScript->object);
			MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fieldInfo.fieldName).c_str());

			if (_node->gameObjectMono != nullptr)
			{
				MonoScript::MonoFieldInfo inf;
				inf.fieldName = fieldInfo.fieldName;
				inf.fieldType = fieldInfo.fieldType;
				inf.fieldValue = MonoScript::ValueVariant();
				inf.fieldValue.objectVal = objName;

				mono_field_set_value((MonoObject*)monoScript->object, _fld, _node->gameObjectMono);

				monoScript->fieldSerializeList[fieldInfo.fieldName] = inf;
			}
		}
	}
}

void ObjectEditor2::onDropScriptString(TreeNode* prop, TreeNode* from)
{
	ScriptPropData* data = static_cast<ScriptPropData*>(prop->getUserData());
	int tagInt = data->tagInt;
	MonoScript::MonoFieldInfo fieldInfo = data->fieldInfo;

	string val = from->getPath();
	((PropString*)prop)->setValue(val);

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		MonoScript* monoScript = node->monoScripts.at(tagInt);

		MonoClass* _class = mono_object_get_class((MonoObject*)monoScript->object);
		MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fieldInfo.fieldName).c_str());

		MonoScript::MonoFieldInfo inf;
		inf.fieldName = fieldInfo.fieldName;
		inf.fieldType = fieldInfo.fieldType;
		inf.fieldValue = MonoScript::ValueVariant(val);

		MonoString* _str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(val).c_str());
		mono_field_set_value((MonoObject*)monoScript->object, _fld, _str);

		monoScript->fieldSerializeList[fieldInfo.fieldName] = inf;
	}
}

void ObjectEditor2::onChangeScriptVector2(Property* prop, Vector2 val)
{
	ScriptPropData* data = static_cast<ScriptPropData*>(prop->getUserData());
	int tagInt = data->tagInt;
	MonoScript::MonoFieldInfo fieldInfo = data->fieldInfo;

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		MonoScript* monoScript = node->monoScripts.at(tagInt);

		MonoClass* _class = mono_object_get_class((MonoObject*)monoScript->object);
		MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fieldInfo.fieldName).c_str());

		MonoScript::MonoFieldInfo inf;
		inf.fieldName = fieldInfo.fieldName;
		inf.fieldType = fieldInfo.fieldType;
		inf.fieldValue = MonoScript::ValueVariant(val);

		MonoRuntime* mono = GetEngine->GetMonoRuntime();
		MonoObject* vec2 = mono_object_new(mono->GetDomain(), mono->vector2_class);
		mono_field_set_value(vec2, mono->vector2_x, &val.x);
		mono_field_set_value(vec2, mono->vector2_y, &val.y);

		void* _vec2 = mono_object_unbox(vec2);
		mono_field_set_value((MonoObject*)monoScript->object, _fld, _vec2);

		monoScript->fieldSerializeList[fieldInfo.fieldName] = inf;
	}
}

void ObjectEditor2::onChangeScriptVector3(Property* prop, Vector3 val)
{
	ScriptPropData* data = static_cast<ScriptPropData*>(prop->getUserData());
	int tagInt = data->tagInt;
	MonoScript::MonoFieldInfo fieldInfo = data->fieldInfo;

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		MonoScript* monoScript = node->monoScripts.at(tagInt);

		MonoClass* _class = mono_object_get_class((MonoObject*)monoScript->object);
		MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fieldInfo.fieldName).c_str());

		MonoScript::MonoFieldInfo inf;
		inf.fieldName = fieldInfo.fieldName;
		inf.fieldType = fieldInfo.fieldType;
		inf.fieldValue = MonoScript::ValueVariant(val);

		MonoRuntime* mono = GetEngine->GetMonoRuntime();
		MonoObject* vec3 = mono_object_new(mono->GetDomain(), mono->vector3_class);

		mono_field_set_value(vec3, mono->vector3_x, &val.x);
		mono_field_set_value(vec3, mono->vector3_y, &val.y);
		mono_field_set_value(vec3, mono->vector3_z, &val.z);

		void* _vec3 = mono_object_unbox(vec3);
		mono_field_set_value((MonoObject*)monoScript->object, _fld, _vec3);

		monoScript->fieldSerializeList[fieldInfo.fieldName] = inf;
	}
}

void ObjectEditor2::onChangeScriptVector4(Property* prop, Vector4 val)
{
	ScriptPropData* data = static_cast<ScriptPropData*>(prop->getUserData());
	int tagInt = data->tagInt;
	MonoScript::MonoFieldInfo fieldInfo = data->fieldInfo;

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		MonoScript* monoScript = node->monoScripts.at(tagInt);

		MonoClass* _class = mono_object_get_class((MonoObject*)monoScript->object);
		MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fieldInfo.fieldName).c_str());

		MonoScript::MonoFieldInfo inf;
		inf.fieldName = fieldInfo.fieldName;
		inf.fieldType = fieldInfo.fieldType;
		inf.fieldValue = MonoScript::ValueVariant(val);

		MonoRuntime* mono = GetEngine->GetMonoRuntime();
		MonoObject* vec4 = mono_object_new(mono->GetDomain(), mono->vector4_class);
		mono_field_set_value(vec4, mono->vector4_x, &val.x);
		mono_field_set_value(vec4, mono->vector4_y, &val.y);
		mono_field_set_value(vec4, mono->vector4_z, &val.z);
		mono_field_set_value(vec4, mono->vector4_w, &val.w);

		void* _vec4 = mono_object_unbox(vec4);
		mono_field_set_value((MonoObject*)monoScript->object, _fld, _vec4);

		monoScript->fieldSerializeList[fieldInfo.fieldName] = inf;
	}
}

void ObjectEditor2::onDropScript(TreeNode* prop, TreeNode* from)
{
	string className = IO::GetFileName(CP_SYS(from->alias));

	MonoClass* _class = GetEngine->GetMonoRuntime()->FindClass(className);

	if (_class != nullptr)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* sceneNode = *it;

			MonoScript* script = new MonoScript(sceneNode);
			script->_class = _class;
			sceneNode->monoScripts.push_back(script);

			script->CreateInstance();
		}

		updateEditor();
	}
	else
	{
		MessageBox(MainWindow::GetWindowHandle(), StringConvert::s2ws("Can't find class \"" + className + "\". Check if the class name of this script is correspond to its file name and class is derived from MonoBehaviour.").c_str(), _T("Error"), MB_OK);
	}
}

void ObjectEditor2::onScriptPopup(TreeNode* node, int val)
{
	int index = *static_cast<int*>(node->getUserData());

	if (val == 0)
	{
		SceneNode* node = sceneNodes[0];
		MonoScript* script = node->monoScripts.at(index);
		bufferScript = script;
	}

	if (val == 1)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;
			MonoScript* script = node->monoScripts.at(index);
			node->monoScripts.erase(node->monoScripts.begin() + index);

			if (script == bufferScript)
				bufferScript = nullptr;

			delete script;
		}

		updateEditor();
	}
}

void ObjectEditor2::onComponentPopup(TreeNode* node, int val)
{
	int index = *static_cast<int*>(node->getUserData());

	if (val == 0)
	{
		SceneNode* node = sceneNodes[0];
		Component* comp = node->components.at(index);
		bufferComponent = comp;
	}

	if (val == 1)
	{
		MainWindow::AddOnEndUpdateCallback([=]() {
			for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
			{
				SceneNode* node = *it;
				Component* comp = node->components.at(index);
				node->components.erase(node->components.begin() + index);

				if (bufferComponent == comp)
					bufferComponent = nullptr;

				delete comp;
			}

			updateEditor();
			MainWindow::gizmo2->showWireframe(true);
			GetEngine->GetNavigationManager()->setNavMeshIsDirty();
		});
	}
}

void ObjectEditor2::onComponentsPopup(TreeNode* node, int val)
{
	if (val == 0)
	{
		if (bufferComponent != nullptr)
		{
			for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
			{
				SceneNode* node = *it;
				CloneTools::cloneComponent(bufferComponent, node);
			}

			updateEditor();
		}
	}
}

void ObjectEditor2::onScriptsPopup(TreeNode* node, int val)
{
	if (val == 0)
	{
		if (bufferScript != nullptr)
		{
			for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
			{
				SceneNode* node = *it;
				CloneTools::cloneScript(bufferScript, node);
			}

			updateEditor();
		}
	}
}

void ObjectEditor2::onTransformPopup(TreeNode* node, int val)
{
	if (val == 0)
	{
		bufferFilled = true;
		bufferPosition = sceneNodes[0]->getPosition();
		bufferOrientation = sceneNodes[0]->getOrientation();
		bufferScale = sceneNodes[0]->getScale();
	}

	if (val == 1)
	{
		if (bufferFilled)
		{
			for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
			{
				SceneNode* node = *it;
				node->setPosition(bufferPosition);
				node->setScale(bufferScale);
				node->setOrientation(bufferOrientation);
			}

			updateEditor();
			MainWindow::gizmo2->updatePosition();

			MainWindow::AddOnEndUpdateCallback([=]() {
				MainWindow::gizmo2->updatePosition();
			});
		}
	}
}

void ObjectEditor2::onChangeTag(Property* node, int val)
{
	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		node->tag = val;
	}
}

void ObjectEditor2::onChangeLayer(Property* node, int val)
{
	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		SceneNode* node = *it;
		node->layer = val;
	}
}