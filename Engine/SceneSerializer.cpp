#include "stdafx.h"
#include "SceneSerializer.h"

#include "../boost/serialization/export.hpp"

#include <OgreSubEntity.h>
#include <OgreParticleSystem.h>
#include <OgreParticleSystemManager.h>
#include <OgreParticleEmitter.h>
#include <OgreParticleAffector.h>
#include <OgreParticleSystemRenderer.h>
#include <OgreSkeletonManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreStaticGeometry.h>

#include "AudioSource.h"
#include "AudioListener.h"
#include "RigidBody.h"
#include "MeshCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "UIButton.h"
#include "UIText.h"
#include "SkeletonBone.h"
#include "AnimationList.h"
#include "HingeJoint.h"
#include "FixedJoint.h"
#include "CharacterJoint.h"
#include "NavMeshAgent.h"
#include "NavMeshObstacle.h"
#include "Vehicle.h"

#include "ResourceMap.h"

#include "FBXSceneManager.h"
#include "NavigationManager.h"
#include "IO.h"
#include "../boost/algorithm/string.hpp"
#include "DeferredShading/DeferredShading.h"
#include "TerrainManager.h"
#include "DeferredShading/DeferredLightCP.h"

#include "OgreCompositorManager.h"
#include "../boost/iostreams/stream.hpp"
#include "ZipHelper.h"
#include "SkinnedMeshDeformer.h"

std::map<std::string, SScene> SceneSerializer::prefabCache;

SVector2 toSVec2(Vector2 vec)
{
	return SVector2(vec.x, vec.y);
}

SVector3 toSVec3(Vector3 vec)
{
	return SVector3(vec.x, vec.y, vec.z);
}

SVector4 toSVec4(Vector4 vec)
{
	return SVector4(vec.x, vec.y, vec.z, vec.w);
}

SColor toSCol(ColourValue col)
{
	return SColor(col.r, col.g, col.b, col.a);
}

Vector2 toVec2(SVector2 vec)
{
	return Vector2(vec.x, vec.y);
}

Vector3 toVec3(SVector3 vec)
{
	return Vector3(vec.x, vec.y, vec.z);
}

Vector4 toVec4(SVector4 vec)
{
	return Vector4(vec.x, vec.y, vec.z, vec.w);
}

ColourValue toCol(SColor col)
{
	return ColourValue(col.r, col.g, col.b, col.a);
}

SceneSerializer::SceneSerializer()
{
}

SceneSerializer::~SceneSerializer()
{
}

void SceneSerializer::Serialize(SceneManager * manager, string path)
{
	//Prepare serialization
	std::ofstream ofs(path, std::ios::binary);
	boost::archive::binary_oarchive stream(ofs);
	//boost::archive::text_oarchive stream(ofs);

	//Scene class
	SScene scene = SScene();
	SaveSceneData(manager, scene);

	//Collect data
	SceneNode * root = manager->getRootSceneNode();

	GetEngine->UpdateSceneNodeIndexes();
	
	//Run recursive serialization of scene nodes
	SerializeChild(root, &scene);

	GetEngine->GetTerrainManager()->SaveAllTerrains();

	//Write file
	stream << scene;
	ofs.close();
}

SScene SceneSerializer::SerializeToMemory(SceneManager * manager)
{
	//Scene class
	SScene scene = SScene();
	SaveSceneData(manager, scene);

	//Collect data
	SceneNode * root = manager->getRootSceneNode();

	GetEngine->UpdateSceneNodeIndexes();

	//Run recursive serialization of scene nodes
	SerializeChild(root, &scene);

	return scene;
}

void SceneSerializer::SerializeChild(SceneNode * root, SScene * scene)
{
	std::vector<SceneNode*> nstack;
	auto children = root->getChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
		nstack.push_back((SceneNode*)*it);

	while (nstack.size() > 0)
	{
		SceneNode* child = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////
		Node* parent = child->getParent();
		string parentName = parent->getName();

		if (!GetEngine->IsEditorObject(child))
		{
			SceneNode::ObjectIterator it = ((SceneNode*)child)->getAttachedObjectIterator();

			if (child->getAttachedObjects().size() > 0)
			{
				MovableObject* movObj = child->getAttachedObject(0);

				//Setup object
				if (movObj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
				{
					SEntity node = SEntity();
					node.index = child->index;
					node.name = child->getName();
					node.prefabName = child->prefabName;
					node.prefabGuid = child->prefabGuid;
					node.position = SVector3(child->getPosition().x, child->getPosition().y, child->getPosition().z);
					node.rotation = SQuaternion(child->getOrientation().x, child->getOrientation().y, child->getOrientation().z, child->getOrientation().w);
					node.scale = SVector3(child->getScale().x, child->getScale().y, child->getScale().z);
					node.parentName = parentName;
					node.alias = child->getAlias();
					node.visible = ((SceneNode*)child)->getGlobalVisible();
					node.tag = to_string(child->tag);
					node.layer = to_string(child->layer);
					node.enabled = true;

					node.fbxSkeletonFrom = child->fbxSkeletonFrom;
					node.fbxCurvesFrom = child->fbxCurvesFrom;
					node.fbxSkeletonFromFile = child->fbxSkeletonFromFile;
					node.fbxCurvesFromFile = child->fbxCurvesFromFile;
					node.fbxSkeletonFromGuid = child->fbxSkeletonFromGuid;
					node.fbxCurvesFromGuid = child->fbxCurvesFromGuid;
					node.fbxFromFile = child->fbxFromFile;
					node.fbxFromGuid = child->fbxFromGuid;

					node.type = ObjectType::OT_Entity;
					node.lightmapStatic = child->lightmapStatic;
					node.navigationStatic = child->navigationStatic;
					node.staticBatching = child->staticBatching;
					node.useCustomLightmapSize = ((Entity*)movObj)->getUseCustomLightmapSize();
					node.customLightmapSize = ((Entity*)movObj)->getCustomLightmapSize();

					node.meshPath = ((Entity*)movObj)->getMesh()->getOrigin();
					node.meshGuid = ResourceMap::guidMap[((Entity*)movObj)->getMesh().getPointer()];
					node.mesh = ((Entity*)movObj)->getMesh()->getName();
					node.meshIndex = ((Entity*)movObj)->getMesh()->fbxIndex;
					node.isFbx = ((Entity*)movObj)->getMesh()->isFbx;

					node.castShadows = ((Entity*)movObj)->getCastShadows();
					node.skinned = child->skinned;

					std::vector<SubEntity*> subentities;
					std::vector<SubEntity*>::iterator it;
					subentities = ((Entity*)movObj)->getSubEntities();

					//Save subentities information
					int i = 0;
					for (it = subentities.begin(); it < subentities.end(); ++it)
					{
						MaterialPtr mat = (*it)->getOriginalMaterial();
						if (mat == nullptr) mat = (*it)->getMaterial();

						SSubEntity subEntity = SSubEntity();
						subEntity.index = i;
						subEntity.materialName = mat->getName();
						subEntity.materialGuid = ResourceMap::guidMap[mat.getPointer()];
						node.subEntities.push_back(subEntity);

						++i;
					}

					SceneNode* _node = (SceneNode*)child;
					//Save scripts
					SaveScripts(_node, node);
					//Save components
					SaveComponents(_node, node);

					//Add to list
					scene->entities.push_back(node);
				}

				if (movObj->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
				{
					Light* curLight = ((Light*)movObj);

					SLight node = SLight();
					node.index = child->index;
					node.name = child->getName();
					node.prefabName = child->prefabName;
					node.prefabGuid = child->prefabGuid;
					node.position = SVector3(child->getPosition().x, child->getPosition().y, child->getPosition().z);
					node.rotation = SQuaternion(child->getOrientation().x, child->getOrientation().y, child->getOrientation().z, child->getOrientation().w);
					node.scale = SVector3(child->getScale().x, child->getScale().y, child->getScale().z);
					node.parentName = parentName;
					node.alias = child->getAlias();
					node.visible = ((SceneNode*)child)->getGlobalVisible();
					node.tag = to_string(child->tag);
					node.layer = to_string(child->layer);
					node.enabled = true;

					node.fbxSkeletonFrom = ((SceneNode*)child)->fbxSkeletonFrom;
					node.fbxSkeletonFromFile = ((SceneNode*)child)->fbxSkeletonFromFile;
					node.fbxSkeletonFromGuid = node.fbxSkeletonFromGuid;
					node.fbxCurvesFrom = ((SceneNode*)child)->fbxCurvesFrom;
					node.fbxCurvesFromFile = ((SceneNode*)child)->fbxCurvesFromFile;
					node.fbxCurvesFromGuid = child->fbxCurvesFromGuid;
					node.fbxFromFile = child->fbxFromFile;
					node.fbxFromGuid = child->fbxFromGuid;

					node.type = ObjectType::OT_Light;
					node.lightmapStatic = ((SceneNode*)child)->lightmapStatic;
					node.navigationStatic = ((SceneNode*)child)->navigationStatic;
					node.staticBatching = child->staticBatching;
					node.range = curLight->getAttenuationRange();
					node.intensity = curLight->getPowerScale();
					node.shadowBias = curLight->getShadowBias();
					ColourValue dif_Color = curLight->getDiffuseColour();
					ColourValue spec_Color = curLight->getSpecularColour();
					node.diffuseColor = SColor(dif_Color.r, dif_Color.g, dif_Color.b, dif_Color.a);
					node.specularColor = SColor(spec_Color.r, spec_Color.g, spec_Color.b, spec_Color.a);
					node.castShadows = curLight->getCastShadows();
					node.parameters = SVector4(curLight->getCustomParameter(1).x, curLight->getCustomParameter(1).y, curLight->getCustomParameter(1).z, curLight->getCustomParameter(1).w);
					node.innerAngle = curLight->getSpotlightInnerAngle().valueDegrees();
					node.outerAngle = curLight->getSpotlightOuterAngle().valueDegrees();
					node.falloff = curLight->getSpotlightFalloff();
					node.mode = static_cast<int>(curLight->getMode());
					node.skinned = child->skinned;

					SceneNode* _node = (SceneNode*)child;
					//Save scripts
					SaveScripts(_node, node);
					//Save components
					SaveComponents(_node, node);

					//Add to list
					scene->lights.push_back(node);
				}

				if (movObj->getMovableType() == EmptyObjectFactory::FACTORY_TYPE_NAME)
				{
					SEmpty node = SEmpty();
					node.index = child->index;
					node.name = child->getName();
					node.prefabName = child->prefabName;
					node.prefabGuid = child->prefabGuid;
					node.position = SVector3(child->getPosition().x, child->getPosition().y, child->getPosition().z);
					node.rotation = SQuaternion(child->getOrientation().x, child->getOrientation().y, child->getOrientation().z, child->getOrientation().w);
					node.scale = SVector3(child->getScale().x, child->getScale().y, child->getScale().z);
					node.parentName = parentName;
					node.alias = child->getAlias();
					node.visible = ((SceneNode*)child)->getGlobalVisible();
					node.tag = to_string(child->tag);
					node.layer = to_string(child->layer);
					node.enabled = true;

					node.fbxSkeletonFrom = ((SceneNode*)child)->fbxSkeletonFrom;
					node.fbxSkeletonFromFile = ((SceneNode*)child)->fbxSkeletonFromFile;
					node.fbxSkeletonFromGuid = node.fbxSkeletonFromGuid;
					node.fbxCurvesFrom = ((SceneNode*)child)->fbxCurvesFrom;
					node.fbxCurvesFromFile = ((SceneNode*)child)->fbxCurvesFromFile;
					node.fbxSkeletonFromGuid = child->fbxCurvesFromGuid;
					node.fbxFromFile = child->fbxFromFile;
					node.fbxFromGuid = child->fbxFromGuid;

					node.type = ObjectType::OT_Empty;
					node.lightmapStatic = ((SceneNode*)child)->lightmapStatic;
					node.navigationStatic = ((SceneNode*)child)->navigationStatic;
					node.staticBatching = child->staticBatching;
					node.skinned = child->skinned;

					SceneNode* _node = (SceneNode*)child;
					//Save scripts
					SaveScripts(_node, node);
					//Save components
					SaveComponents(_node, node);

					//Add to list
					scene->empties.push_back(node);
				}

				if (dynamic_cast<Camera*>(movObj))
				{
					SCamera node = SCamera();
					node.index = child->index;
					node.name = child->getName();
					node.prefabName = child->prefabName;
					node.prefabGuid = child->prefabGuid;
					node.position = SVector3(child->getPosition().x, child->getPosition().y, child->getPosition().z);
					node.rotation = SQuaternion(child->getOrientation().x, child->getOrientation().y, child->getOrientation().z, child->getOrientation().w);
					node.scale = SVector3(child->getScale().x, child->getScale().y, child->getScale().z);
					node.parentName = parentName;
					node.alias = child->getAlias();
					node.visible = ((SceneNode*)child)->getGlobalVisible();
					node.tag = to_string(child->tag);
					node.layer = to_string(child->layer);
					node.enabled = true;

					node.fbxSkeletonFrom = ((SceneNode*)child)->fbxSkeletonFrom;
					node.fbxSkeletonFromFile = ((SceneNode*)child)->fbxSkeletonFromFile;
					node.fbxSkeletonFromGuid = node.fbxSkeletonFromGuid;
					node.fbxCurvesFrom = ((SceneNode*)child)->fbxCurvesFrom;
					node.fbxCurvesFromFile = ((SceneNode*)child)->fbxCurvesFromFile;
					node.fbxCurvesFromGuid = child->fbxCurvesFromGuid;
					node.fbxFromFile = child->fbxFromFile;
					node.fbxFromGuid = child->fbxFromGuid;

					node.type = ObjectType::OT_Camera;
					node.lightmapStatic = ((SceneNode*)child)->lightmapStatic;
					node.navigationStatic = ((SceneNode*)child)->navigationStatic;
					node.staticBatching = child->staticBatching;
					node.nearClipPlane = ((Camera*)movObj)->getNearClipDistance();
					node.farClipPlane = ((Camera*)movObj)->getFarClipDistance();
					node.fieldOfView = ((Camera*)movObj)->getFOVy().valueDegrees();
					node.isMainCamera = ((Camera*)movObj)->getMainCamera();
					node.clearColor = toSCol(((Camera*)movObj)->getClearColor());
					node.orthographic = ((Camera*)movObj)->getProjectionType() == ProjectionType::PT_ORTHOGRAPHIC;
					node.orthoSize = ((Camera*)movObj)->getOrthographicSize();
					node.skinned = child->skinned;

					SceneNode* _node = (SceneNode*)child;
					//Save scripts
					SaveScripts(_node, node);
					//Save components
					SaveComponents(_node, node);

					//Add to list
					scene->cameras.push_back(node);
				}

				if (movObj->getMovableType() == UICanvasFactory::FACTORY_TYPE_NAME)
				{
					SUICanvas node = SUICanvas();
					node.index = child->index;
					node.name = child->getName();
					node.prefabName = child->prefabName;
					node.prefabGuid = child->prefabGuid;
					node.position = SVector3(child->getPosition().x, child->getPosition().y, child->getPosition().z);
					node.rotation = SQuaternion(child->getOrientation().x, child->getOrientation().y, child->getOrientation().z, child->getOrientation().w);
					node.scale = SVector3(child->getScale().x, child->getScale().y, child->getScale().z);
					node.parentName = parentName;
					node.alias = child->getAlias();
					node.visible = ((SceneNode*)child)->getGlobalVisible();
					node.tag = to_string(child->tag);
					node.layer = to_string(child->layer);
					node.enabled = true;

					node.type = ObjectType::OT_UICanvas;
					node.lightmapStatic = ((SceneNode*)child)->lightmapStatic;
					node.navigationStatic = ((SceneNode*)child)->navigationStatic;
					node.staticBatching = child->staticBatching;
					node.refScreenWidth = ((UICanvas*)movObj)->GetReferenceScreenWidth();
					node.refScreenHeight = ((UICanvas*)movObj)->GetReferenceScreenHeight();
					node.scaleMode = (int)((UICanvas*)movObj)->GetScaleMode();
					node.skinned = child->skinned;

					SceneNode* _node = (SceneNode*)child;
					//Save scripts
					SaveScripts(_node, node);
					//Save components
					SaveComponents(_node, node);

					//Add to list
					scene->uiCanvases.push_back(node);
				}

				if (movObj->getMovableType() == UIButtonFactory::FACTORY_TYPE_NAME)
				{
					UIButton* uiElement = (UIButton*)movObj;
					SUIButton node = SUIButton();
					node.index = child->index;
					node.name = child->getName();
					node.prefabName = child->prefabName;
					node.prefabGuid = child->prefabGuid;
					node.position = SVector3(child->getPosition().x, child->getPosition().y, child->getPosition().z);
					node.rotation = SQuaternion(child->getOrientation().x, child->getOrientation().y, child->getOrientation().z, child->getOrientation().w);
					node.scale = SVector3(child->getScale().x, child->getScale().y, child->getScale().z);
					node.parentName = parentName;
					node.alias = child->getAlias();
					node.visible = ((SceneNode*)child)->getGlobalVisible();
					node.tag = to_string(child->tag);
					node.layer = to_string(child->layer);
					node.enabled = true;

					node.type = ObjectType::OT_UIButton;
					node.lightmapStatic = ((SceneNode*)child)->lightmapStatic;
					node.navigationStatic = ((SceneNode*)child)->navigationStatic;
					node.staticBatching = child->staticBatching;
					node.canvasAlignment = (int)((UIElement*)movObj)->GetCanvasAlignment();
					node.anchor = SVector2(((UIElement*)movObj)->GetAnchor().x, ((UIElement*)movObj)->GetAnchor().y);
					node.imageNormal = ((UIButton*)movObj)->GetImageNormalName();
					node.imageNormalGuid = ResourceMap::guidMap[((UIButton*)movObj)->GetImageNormal().getPointer()];
					node.imageHover = ((UIButton*)movObj)->GetImageHoverName();
					node.imageHoverGuid = ResourceMap::guidMap[((UIButton*)movObj)->GetImageHover().getPointer()];
					node.imagePressed = ((UIButton*)movObj)->GetImagePressedName();
					node.imagePressedGuid = ResourceMap::guidMap[((UIButton*)movObj)->GetImagePressed().getPointer()];
					node.imageDisabled = ((UIButton*)movObj)->GetImageDisabledName();
					node.imageDisabledGuid = ResourceMap::guidMap[((UIButton*)movObj)->GetImageDisabled().getPointer()];

					node.colorNormal = toSCol(uiElement->GetColorNormal());
					node.colorHover = toSCol(uiElement->GetColorHover());
					node.colorPressed = toSCol(uiElement->GetColorPressed());
					node.colorDisabled = toSCol(uiElement->GetColorDisabled());
					node.interactable = uiElement->GetInteractable();

					node.pointerDownEvent.functionName = ((UIButton*)movObj)->GetPointerDownEvent().functionName;
					node.pointerDownEvent.sceneNode = ((UIButton*)movObj)->GetPointerDownEvent().sceneNode;
					node.pointerUpEvent.functionName = ((UIButton*)movObj)->GetPointerUpEvent().functionName;
					node.pointerUpEvent.sceneNode = ((UIButton*)movObj)->GetPointerUpEvent().sceneNode;
					node.color = SColor(uiElement->getColor().r, uiElement->getColor().g, uiElement->getColor().b, uiElement->getColor().a);
					node.elementSize = toSVec2(uiElement->GetSize());
					node.skinned = child->skinned;

					SceneNode* _node = (SceneNode*)child;
					//Save scripts
					SaveScripts(_node, node);
					//Save components
					SaveComponents(_node, node);

					//Add to list
					scene->uiButtons.push_back(node);
				}

				if (movObj->getMovableType() == UITextFactory::FACTORY_TYPE_NAME)
				{
					UIText* uiElement = (UIText*)movObj;
					SUIText node = SUIText();
					node.index = child->index;
					node.name = child->getName();
					node.prefabName = child->prefabName;
					node.prefabGuid = child->prefabGuid;
					node.position = SVector3(child->getPosition().x, child->getPosition().y, child->getPosition().z);
					node.rotation = SQuaternion(child->getOrientation().x, child->getOrientation().y, child->getOrientation().z, child->getOrientation().w);
					node.scale = SVector3(child->getScale().x, child->getScale().y, child->getScale().z);
					node.parentName = parentName;
					node.alias = child->getAlias();
					node.visible = ((SceneNode*)child)->getGlobalVisible();
					node.tag = to_string(child->tag);
					node.layer = to_string(child->layer);
					node.enabled = true;

					node.type = ObjectType::OT_UIText;
					node.lightmapStatic = ((SceneNode*)child)->lightmapStatic;
					node.navigationStatic = ((SceneNode*)child)->navigationStatic;
					node.staticBatching = child->staticBatching;
					node.canvasAlignment = (int)((UIElement*)movObj)->GetCanvasAlignment();
					node.anchor = SVector2(((UIElement*)movObj)->GetAnchor().x, ((UIElement*)movObj)->GetAnchor().y);
					node.font = ((UIText*)movObj)->getFontName();
					node.fontGuid = ((UIText*)movObj)->getFontGuid();
					node.text = ((UIText*)movObj)->getText();
					node.color = SColor(uiElement->getColor().r, uiElement->getColor().g, uiElement->getColor().b, uiElement->getColor().a);
					node.size = ((UIText*)movObj)->getSize();
					node.elementSize = toSVec2(uiElement->GetSize());
					node.verticalAlignment = static_cast<int>(uiElement->getVerticalAlignment());
					node.horizontalAlignment = static_cast<int>(uiElement->getHorizontalAlignment());
					node.skinned = child->skinned;

					SceneNode* _node = (SceneNode*)child;
					//Save scripts
					SaveScripts(_node, node);
					//Save components
					SaveComponents(_node, node);

					//Add to list
					scene->uiTexts.push_back(node);
				}

				if (movObj->getMovableType() == UIImageFactory::FACTORY_TYPE_NAME)
				{
					UIImage* uiElement = (UIImage*)movObj;
					SUIImage node = SUIImage();
					node.index = child->index;
					node.name = child->getName();
					node.prefabName = child->prefabName;
					node.prefabGuid = child->prefabGuid;
					node.position = SVector3(child->getPosition().x, child->getPosition().y, child->getPosition().z);
					node.rotation = SQuaternion(child->getOrientation().x, child->getOrientation().y, child->getOrientation().z, child->getOrientation().w);
					node.scale = SVector3(child->getScale().x, child->getScale().y, child->getScale().z);
					node.parentName = parentName;
					node.alias = child->getAlias();
					node.visible = ((SceneNode*)child)->getGlobalVisible();
					node.tag = to_string(child->tag);
					node.layer = to_string(child->layer);
					node.enabled = true;

					node.type = ObjectType::OT_UIImage;
					node.lightmapStatic = ((SceneNode*)child)->lightmapStatic;
					node.navigationStatic = ((SceneNode*)child)->navigationStatic;
					node.staticBatching = child->staticBatching;
					node.canvasAlignment = (int)((UIElement*)movObj)->GetCanvasAlignment();
					node.anchor = SVector2(((UIElement*)movObj)->GetAnchor().x, ((UIElement*)movObj)->GetAnchor().y);
					node.image = ((UIImage*)movObj)->GetImageName();
					node.imageGuid = ResourceMap::guidMap[((UIImage*)movObj)->GetImage().getPointer()];
					node.color = SColor(uiElement->getColor().r, uiElement->getColor().g, uiElement->getColor().b, uiElement->getColor().a);
					node.elementSize = toSVec2(uiElement->GetSize());
					node.skinned = child->skinned;

					SceneNode* _node = (SceneNode*)child;
					//Save scripts
					SaveScripts(_node, node);
					//Save components
					SaveComponents(_node, node);

					//Add to list
					scene->uiImages.push_back(node);
				}

				if (movObj->getMovableType() == ParticleSystemFactory::FACTORY_TYPE_NAME)
				{
					ParticleSystem* ps = (ParticleSystem*)movObj;
					SParticleSystem node = SParticleSystem();
					node.index = child->index;
					node.name = child->getName();
					node.prefabName = child->prefabName;
					node.prefabGuid = child->prefabGuid;
					node.position = SVector3(child->getPosition().x, child->getPosition().y, child->getPosition().z);
					node.rotation = SQuaternion(child->getOrientation().x, child->getOrientation().y, child->getOrientation().z, child->getOrientation().w);
					node.scale = SVector3(child->getScale().x, child->getScale().y, child->getScale().z);
					node.parentName = parentName;
					node.alias = child->getAlias();
					node.visible = ((SceneNode*)child)->getGlobalVisible();
					node.tag = to_string(child->tag);
					node.layer = to_string(child->layer);
					node.enabled = true;

					node.type = ObjectType::OT_ParticleSystem;
					node.lightmapStatic = ((SceneNode*)child)->lightmapStatic;
					node.navigationStatic = ((SceneNode*)child)->navigationStatic;
					node.staticBatching = child->staticBatching;
					node.castShadows = ps->getCastShadows();
					node.emit = ps->getEmitting();
					node.keepLocal = ps->getKeepParticlesInLocalSpace();
					node.material = ps->getMaterialName();
					node.materialGuid = ResourceMap::guidMap[ps->getMaterial().getPointer()];
					node.size = SVector2(ps->getDefaultWidth(), ps->getDefaultHeight());
					node.speedFactor = ps->getSpeedFactor();
					node.billboardType = ps->getRenderer()->getParameter("billboard_type");
					node.skinned = child->skinned;

					for (int i = 0; i < ps->getNumEmitters(); ++i)
					{
						ParticleEmitter* _emitter = ps->getEmitter(i);
						SParticleEmitter emitter = SParticleEmitter();
						emitter.enabled = _emitter->getEnabled();
						emitter.type = _emitter->getType();
						emitter.angle = _emitter->getAngle().valueDegrees();
						emitter.startColor = SColor(_emitter->getColour().r, _emitter->getColour().g, _emitter->getColour().b, _emitter->getColour().a);
						emitter.direction = SVector3(_emitter->getDirection().x, _emitter->getDirection().y, _emitter->getDirection().z);
						emitter.emissionRate = _emitter->getEmissionRate();
						emitter.minDuration = _emitter->getMinDuration();
						emitter.maxDuration = _emitter->getMaxDuration();
						emitter.minVelocity = _emitter->getMinParticleVelocity();
						emitter.maxVelocity = _emitter->getMaxParticleVelocity();
						emitter.minRepeatDelay = _emitter->getMinRepeatDelay();
						emitter.maxRepeatDelay = _emitter->getMaxRepeatDelay();
						emitter.minLifeTime = _emitter->getMinTimeToLive();
						emitter.maxLifeTime = _emitter->getMaxTimeToLive();

						//Specific parameters
						if (_emitter->getType() == "Box" || _emitter->getType() == "Cylinder" || _emitter->getType() == "Ellipsoid")
						{
							emitter.parameters.push_back(SParticleSystemParameter("width", _emitter->getParameter("width")));
							emitter.parameters.push_back(SParticleSystemParameter("height", _emitter->getParameter("height")));
							emitter.parameters.push_back(SParticleSystemParameter("depth", _emitter->getParameter("depth")));
						}

						if (_emitter->getType() == "HollowEllipsoid")
						{
							emitter.parameters.push_back(SParticleSystemParameter("inner_width", _emitter->getParameter("inner_width")));
							emitter.parameters.push_back(SParticleSystemParameter("inner_height", _emitter->getParameter("inner_height")));
							emitter.parameters.push_back(SParticleSystemParameter("inner_depth", _emitter->getParameter("inner_depth")));
						}

						if (_emitter->getType() == "Ring")
						{
							emitter.parameters.push_back(SParticleSystemParameter("inner_width", _emitter->getParameter("inner_width")));
							emitter.parameters.push_back(SParticleSystemParameter("inner_height", _emitter->getParameter("inner_height")));
						}

						node.emitters.push_back(emitter);
					}

					for (int i = 0; i < ps->getNumAffectors(); ++i)
					{
						ParticleAffector* _affector = ps->getAffector(i);
						SParticleAffector affector = SParticleAffector();
						affector.type = _affector->getType();

						//Properties
						if (_affector->getType() == "LinearForce")
						{
							affector.parameters.push_back(SParticleSystemParameter("force_vector", _affector->getParameter("force_vector")));
							affector.parameters.push_back(SParticleSystemParameter("force_application", _affector->getParameter("force_application")));
						}

						if (_affector->getType() == "ColourFader")
						{
							affector.parameters.push_back(SParticleSystemParameter("red", _affector->getParameter("red")));
							affector.parameters.push_back(SParticleSystemParameter("green", _affector->getParameter("green")));
							affector.parameters.push_back(SParticleSystemParameter("blue", _affector->getParameter("blue")));
							affector.parameters.push_back(SParticleSystemParameter("alpha", _affector->getParameter("alpha")));
						}

						if (_affector->getType() == "ColourFader2")
						{
							affector.parameters.push_back(SParticleSystemParameter("red1", _affector->getParameter("red1")));
							affector.parameters.push_back(SParticleSystemParameter("green1", _affector->getParameter("green1")));
							affector.parameters.push_back(SParticleSystemParameter("blue1", _affector->getParameter("blue1")));
							affector.parameters.push_back(SParticleSystemParameter("alpha1", _affector->getParameter("alpha1")));
							affector.parameters.push_back(SParticleSystemParameter("red2", _affector->getParameter("red2")));
							affector.parameters.push_back(SParticleSystemParameter("green2", _affector->getParameter("green2")));
							affector.parameters.push_back(SParticleSystemParameter("blue2", _affector->getParameter("blue2")));
							affector.parameters.push_back(SParticleSystemParameter("alpha2", _affector->getParameter("alpha2")));
							affector.parameters.push_back(SParticleSystemParameter("state_change", _affector->getParameter("state_change")));
						}

						if (_affector->getType() == "Scaler")
						{
							affector.parameters.push_back(SParticleSystemParameter("rate", _affector->getParameter("rate")));
						}

						if (_affector->getType() == "Rotator")
						{
							affector.parameters.push_back(SParticleSystemParameter("rotation_speed_range_start", _affector->getParameter("rotation_speed_range_start")));
							affector.parameters.push_back(SParticleSystemParameter("rotation_speed_range_end", _affector->getParameter("rotation_speed_range_end")));
							affector.parameters.push_back(SParticleSystemParameter("rotation_range_start", _affector->getParameter("rotation_range_start")));
							affector.parameters.push_back(SParticleSystemParameter("rotation_range_end", _affector->getParameter("rotation_range_end")));
						}

						if (_affector->getType() == "ColourInterpolator")
						{
							affector.parameters.push_back(SParticleSystemParameter("time0", _affector->getParameter("time0")));
							affector.parameters.push_back(SParticleSystemParameter("colour0", _affector->getParameter("colour0")));
							affector.parameters.push_back(SParticleSystemParameter("time1", _affector->getParameter("time1")));
							affector.parameters.push_back(SParticleSystemParameter("colour1", _affector->getParameter("colour1")));
							affector.parameters.push_back(SParticleSystemParameter("time2", _affector->getParameter("time2")));
							affector.parameters.push_back(SParticleSystemParameter("colour2", _affector->getParameter("colour2")));
							affector.parameters.push_back(SParticleSystemParameter("time3", _affector->getParameter("time3")));
							affector.parameters.push_back(SParticleSystemParameter("colour3", _affector->getParameter("colour3")));
							affector.parameters.push_back(SParticleSystemParameter("time4", _affector->getParameter("time4")));
							affector.parameters.push_back(SParticleSystemParameter("colour4", _affector->getParameter("colour4")));
							affector.parameters.push_back(SParticleSystemParameter("time5", _affector->getParameter("time5")));
							affector.parameters.push_back(SParticleSystemParameter("colour5", _affector->getParameter("colour5")));
						}

						if (_affector->getType() == "DirectionRandomiser")
						{
							affector.parameters.push_back(SParticleSystemParameter("randomness", _affector->getParameter("randomness")));
							affector.parameters.push_back(SParticleSystemParameter("scope", _affector->getParameter("scope")));
							affector.parameters.push_back(SParticleSystemParameter("keep_velocity", _affector->getParameter("keep_velocity")));
						}

						node.affectors.push_back(affector);
					}

					SceneNode* _node = (SceneNode*)child;
					//Save scripts
					SaveScripts(_node, node);
					//Save components
					SaveComponents(_node, node);

					//Add to list
					scene->particleSystems.push_back(node);
				}
			}
			////////////////////

			int j = 0;
			auto children = child->getChildren();
			for (auto it = children.begin(); it != children.end(); ++it, ++j)
			{
				SceneNode* nd = (SceneNode*)(*it);
				nstack.insert(nstack.begin() + j, nd);
			}
		}
	}
}

void SceneSerializer::RestoreObjects(SScene& scene, SceneManager* manager, std::vector<std::pair<SceneNode*, SSceneNode>>& allNodes, std::map<std::string, SceneNode*>& remapList, bool onlyMeshes)
{
	//------------Restore objects-------------//

	//Restore entities
	for (std::vector<SEntity>::iterator ientity = scene.entities.begin(); ientity < scene.entities.end(); ++ientity)
	{
		Entity* model = nullptr;
		SceneNode* pNode = nullptr;

		if (ientity->name.empty())
			continue;

		if (ientity->isFbx)
		{
			std::string nm = GenName(ientity->name, manager);

			std::string _meshPath = ResourceMap::getResourceNameFromGuid(ientity->meshGuid);
			if (_meshPath.empty())
				_meshPath = ientity->meshPath;

			if (FBXSceneManager::getSingleton().resourceExists(_meshPath, "Assets"))
			{
				FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(_meshPath, "Assets");
				SceneManager* prevMgr = fbx->GetSceneManager();
				fbx->SetSceneManager(manager);
				pNode = fbx->ConvertToNativeFormat(nm, ientity->meshIndex);
				fbx->SetSceneManager(prevMgr);
				if (pNode->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
					model = (Entity*)pNode->getAttachedObject(0);
			}
			else
			{
				pNode = manager->createSceneNode(nm);
				Empty* empty = (Empty*)manager->createMovableObject(nm, EmptyObjectFactory::FACTORY_TYPE_NAME);
				pNode->attachObject(empty);
			}
		}

		pNode->setAlias(ientity->alias);
		
		pNode->lightmapStatic = ientity->lightmapStatic;
		pNode->navigationStatic = ientity->navigationStatic;
		pNode->staticBatching = ientity->staticBatching;
		pNode->tag = atoi(ientity->tag.c_str());
		pNode->layer = atoi(ientity->layer.c_str());

		pNode->prefabName = ResourceMap::getResourceNameFromGuid(ientity->prefabGuid);
		if (pNode->prefabName.empty())
			pNode->prefabName = ientity->prefabName;
		pNode->prefabGuid = ientity->prefabGuid;

		if (ientity->fbxSkeletonFrom > -1)
			pNode->fbxSkeletonFrom = ientity->fbxSkeletonFrom;

		pNode->fbxCurvesFrom = ientity->fbxCurvesFrom;

		std::string _fbxSkeletonPath = ResourceMap::getResourceNameFromGuid(ientity->fbxSkeletonFromGuid);
		if (_fbxSkeletonPath.empty())
			_fbxSkeletonPath = ientity->fbxSkeletonFromFile;

		std::string _fbxCurvesPath = ResourceMap::getResourceNameFromGuid(ientity->fbxCurvesFromGuid);
		if (_fbxCurvesPath.empty())
			_fbxCurvesPath = ientity->fbxCurvesFromFile;

		pNode->fbxSkeletonFromFile = _fbxSkeletonPath;
		pNode->fbxCurvesFromFile = _fbxCurvesPath;
		pNode->fbxCurvesFromGuid = ientity->fbxCurvesFromGuid;
		pNode->fbxSkeletonFromGuid = ientity->fbxSkeletonFromGuid;

		std::string _fbxFromFile = ResourceMap::getResourceNameFromGuid(ientity->fbxFromGuid);
		if (_fbxFromFile.empty())
			_fbxFromFile = ientity->fbxFromFile;

		pNode->fbxFromFile = _fbxFromFile;
		pNode->fbxFromGuid = ientity->fbxFromGuid;

		pNode->skinned = ientity->skinned;

		if (model != nullptr)
		{
			model->setCastShadows(ientity->castShadows);
			model->setUseCustomLightmapSize(ientity->useCustomLightmapSize);
			model->setCustomLightmapSize(ientity->customLightmapSize);

			std::vector<SSubEntity>::iterator se_it;

			//Load subentities information
			for (se_it = ientity->subEntities.begin(); se_it < ientity->subEntities.end(); ++se_it)
			{
				std::string _materialPath = ResourceMap::getResourceNameFromGuid(se_it->materialGuid);
				if (_materialPath.empty())
					_materialPath = se_it->materialName;

				if (se_it->index < model->getNumSubEntities())
				{
					model->getSubEntity(se_it->index)->setMaterialName(_materialPath, "Assets");

					if (IO::GetFileName(GetEngine->loadedScene) != "scene_temp" && GetEngine->loadedScene != "")
					{
						//Load lightmaps
						std::string scenePath = IO::RemovePart(GetEngine->loadedScene, GetEngine->GetAssetsPath());
						std::string saveTo = IO::GetFilePath(scenePath) + IO::GetFileName(scenePath) + "/Lightmaps/";
						std::string texName = saveTo + pNode->getName() + to_string(se_it->index) + ".png";
						if (GetEngine->GetUseUnpackedResources())
						{
							if (IO::FileExists(GetEngine->GetAssetsPath() + texName))
							{
								TexturePtr ltex = TextureManager::getSingleton().load(texName, "Assets", TEX_TYPE_2D, -1, 1.0, false, PixelFormat::PF_R8G8B8, false);
								model->getSubEntity(se_it->index)->setLightmapTexture(ltex);
							}
						}
						else
						{
							if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), texName))
							{
								TexturePtr ltex;

								if (!TextureManager::getSingleton().resourceExists(texName, "Assets"))
								{
									int sz = 0;
									char* buffer = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), texName, sz);
									MemoryDataStream* memStream = new Ogre::MemoryDataStream(reinterpret_cast<void*>(buffer), sz);
									DataStreamPtr data_stream(memStream);

									String ext = IO::GetFileExtension(texName);
									Image image;
									image.load(data_stream, ext);

									data_stream->close();
									delete[] buffer;
									memStream->close();

									ltex = TextureManager::getSingleton().create(texName, "Assets", true);
									ltex->loadImage(image);
									//ltex = TextureManager::getSingleton().load(texName, "Assets", TEX_TYPE_2D);
								}
								else
								{
									ltex = TextureManager::getSingleton().getByName(texName, "Assets");
									//ltex = TextureManager::getSingleton().load(texName, "Assets", TEX_TYPE_2D);
								}

								model->getSubEntity(se_it->index)->setLightmapTexture(ltex);
							}
						}
					}
				}
			}
		}

		remapList[ientity->name] = pNode;
		allNodes.push_back(std::make_pair(pNode, *ientity));
	}

	//Restore lights
	for (std::vector<SLight>::iterator ilight = scene.lights.begin(); ilight < scene.lights.end(); ++ilight)
	{
		Ogre::String name = GenName(ilight->name, manager);

		SceneNode* lightNode = manager->createSceneNode(name);
		Light* light = manager->createLight(name);

		if (ilight->parameters.x == 0)
			light->setType(Light::LightTypes::LT_POINT);
		if (ilight->parameters.x == 1)
			light->setType(Light::LightTypes::LT_SPOTLIGHT);
		if (ilight->parameters.x == 2)
			light->setType(Light::LightTypes::LT_DIRECTIONAL);

		light->setAttenuation(ilight->range, 1.0, 0.045, 0.0075);
		light->setPowerScale(ilight->intensity);
		light->setShadowBias(ilight->shadowBias);
		light->setDiffuseColour(ColourValue(ilight->diffuseColor.r, ilight->diffuseColor.g, ilight->diffuseColor.b, ilight->diffuseColor.a));
		light->setSpecularColour(ColourValue(ilight->specularColor.r, ilight->specularColor.g, ilight->specularColor.b, ilight->specularColor.a));
		light->setCastShadows(ilight->castShadows);
		light->setCustomParameter(1, Vector4(ilight->parameters.x, ilight->parameters.y, ilight->parameters.z, ilight->parameters.w));
		light->setMode(static_cast<Light::LightMode>(ilight->mode));

		light->setSpotlightInnerAngle(Radian(Degree(ilight->innerAngle)));
		light->setSpotlightOuterAngle(Radian(Degree(ilight->outerAngle)));
		light->setSpotlightFalloff(ilight->falloff);

		lightNode->attachObject(light);

		lightNode->setAlias(ilight->alias);

		lightNode->prefabName = ResourceMap::getResourceNameFromGuid(ilight->prefabGuid);
		if (lightNode->prefabName.empty())
			lightNode->prefabName = ilight->prefabName;
		lightNode->prefabGuid = ilight->prefabGuid;

		lightNode->lightmapStatic = ilight->lightmapStatic;
		lightNode->navigationStatic = ilight->navigationStatic;
		lightNode->staticBatching = ilight->staticBatching;
		lightNode->tag = atoi(ilight->tag.c_str());
		lightNode->layer = atoi(ilight->layer.c_str());

		if (ilight->fbxSkeletonFrom > -1)
			lightNode->fbxSkeletonFrom = ilight->fbxSkeletonFrom;

		lightNode->fbxCurvesFrom = ilight->fbxCurvesFrom;

		std::string _fbxSkeletonPath = ResourceMap::getResourceNameFromGuid(ilight->fbxSkeletonFromGuid);
		if (_fbxSkeletonPath.empty())
			_fbxSkeletonPath = ilight->fbxSkeletonFromFile;

		std::string _fbxCurvesPath = ResourceMap::getResourceNameFromGuid(ilight->fbxCurvesFromGuid);
		if (_fbxCurvesPath.empty())
			_fbxCurvesPath = ilight->fbxCurvesFromFile;

		lightNode->fbxSkeletonFromFile = _fbxSkeletonPath;
		lightNode->fbxCurvesFromFile = _fbxCurvesPath;
		lightNode->fbxCurvesFromGuid = ilight->fbxCurvesFromGuid;
		lightNode->fbxSkeletonFromGuid = ilight->fbxSkeletonFromGuid;

		std::string _fbxFromFile = ResourceMap::getResourceNameFromGuid(ilight->fbxFromGuid);
		if (_fbxFromFile.empty())
			_fbxFromFile = ilight->fbxFromFile;

		lightNode->fbxFromFile = _fbxFromFile;
		lightNode->fbxFromGuid = ilight->fbxFromGuid;

		lightNode->skinned = ilight->skinned;

		remapList[ilight->name] = lightNode;
		allNodes.push_back(std::make_pair(lightNode, *ilight));
	}

	//Restore empties
	for (std::vector<SEmpty>::iterator iempty = scene.empties.begin(); iempty < scene.empties.end(); ++iempty)
	{
		Ogre::String name = GenName(iempty->name, manager);

		SceneNode* emptyNode = manager->createSceneNode(name);
		Empty* empty = (Empty*)manager->createMovableObject(name, EmptyObjectFactory::FACTORY_TYPE_NAME);

		emptyNode->attachObject(empty);

		emptyNode->setAlias(iempty->alias);
		
		emptyNode->prefabName = ResourceMap::getResourceNameFromGuid(iempty->prefabGuid);
		if (emptyNode->prefabName.empty())
			emptyNode->prefabName = iempty->prefabName;

		emptyNode->lightmapStatic = iempty->lightmapStatic;
		emptyNode->navigationStatic = iempty->navigationStatic;
		emptyNode->staticBatching = iempty->staticBatching;
		emptyNode->tag = atoi(iempty->tag.c_str());
		emptyNode->layer = atoi(iempty->layer.c_str());

		if (iempty->fbxSkeletonFrom > -1)
			emptyNode->fbxSkeletonFrom = iempty->fbxSkeletonFrom;
		emptyNode->prefabGuid = iempty->prefabGuid;
		
		emptyNode->fbxCurvesFrom = iempty->fbxCurvesFrom;

		std::string _fbxSkeletonPath = ResourceMap::getResourceNameFromGuid(iempty->fbxSkeletonFromGuid);
		if (_fbxSkeletonPath.empty())
			_fbxSkeletonPath = iempty->fbxSkeletonFromFile;

		std::string _fbxCurvesPath = ResourceMap::getResourceNameFromGuid(iempty->fbxCurvesFromGuid);
		if (_fbxCurvesPath.empty())
			_fbxCurvesPath = iempty->fbxCurvesFromFile;

		emptyNode->fbxSkeletonFromFile = _fbxSkeletonPath;
		emptyNode->fbxCurvesFromFile = _fbxCurvesPath;
		emptyNode->fbxCurvesFromGuid = iempty->fbxCurvesFromGuid;
		emptyNode->fbxSkeletonFromGuid = iempty->fbxSkeletonFromGuid;

		std::string _fbxFromFile = ResourceMap::getResourceNameFromGuid(iempty->fbxFromGuid);
		if (_fbxFromFile.empty())
			_fbxFromFile = iempty->fbxFromFile;

		emptyNode->fbxFromFile = _fbxFromFile;
		emptyNode->fbxFromGuid = iempty->fbxFromGuid;

		emptyNode->skinned = iempty->skinned;

		remapList[iempty->name] = emptyNode;
		allNodes.push_back(std::make_pair(emptyNode, *iempty));
	}

	if (!onlyMeshes)
	{
		//Restore cameras
		for (std::vector<SCamera>::iterator icamera = scene.cameras.begin(); icamera < scene.cameras.end(); ++icamera)
		{
			Ogre::String name = GenName(icamera->name, manager);

			SceneNode* cameraNode = manager->createSceneNode(name);
			Camera* camera = manager->createCamera(name);
			camera->setNearClipDistance(icamera->nearClipPlane);
			camera->setFarClipDistance(icamera->farClipPlane);
			camera->setFOVy(Radian(Degree(icamera->fieldOfView)));
			camera->setMainCamera(icamera->isMainCamera);
			camera->setClearColor(toCol(icamera->clearColor));
			camera->setProjectionType(icamera->orthographic ? ProjectionType::PT_ORTHOGRAPHIC : ProjectionType::PT_PERSPECTIVE);
			camera->setOrthographicSize(icamera->orthoSize);

			cameraNode->attachObject(camera);

			cameraNode->setAlias(icamera->alias);

			cameraNode->prefabName = ResourceMap::getResourceNameFromGuid(icamera->prefabGuid);
			if (cameraNode->prefabName.empty())
				cameraNode->prefabName = icamera->prefabName;
			cameraNode->prefabGuid = icamera->prefabGuid;

			cameraNode->lightmapStatic = icamera->lightmapStatic;
			cameraNode->navigationStatic = icamera->navigationStatic;
			cameraNode->staticBatching = icamera->staticBatching;
			cameraNode->tag = atoi(icamera->tag.c_str());
			cameraNode->layer = atoi(icamera->layer.c_str());

			if (icamera->fbxSkeletonFrom > -1)
				cameraNode->fbxSkeletonFrom = icamera->fbxSkeletonFrom;

			cameraNode->fbxCurvesFrom = icamera->fbxCurvesFrom;

			std::string _fbxSkeletonPath = ResourceMap::getResourceNameFromGuid(icamera->fbxSkeletonFromGuid);
			if (_fbxSkeletonPath.empty())
				_fbxSkeletonPath = icamera->fbxSkeletonFromFile;

			std::string _fbxCurvesPath = ResourceMap::getResourceNameFromGuid(icamera->fbxCurvesFromGuid);
			if (_fbxCurvesPath.empty())
				_fbxCurvesPath = icamera->fbxCurvesFromFile;

			cameraNode->fbxSkeletonFromFile = _fbxSkeletonPath;
			cameraNode->fbxCurvesFromFile = _fbxCurvesPath;
			cameraNode->fbxCurvesFromGuid = icamera->fbxCurvesFromGuid;
			cameraNode->fbxSkeletonFromGuid = icamera->fbxSkeletonFromGuid;

			std::string _fbxFromFile = ResourceMap::getResourceNameFromGuid(icamera->fbxFromGuid);
			if (_fbxFromFile.empty())
				_fbxFromFile = icamera->fbxFromFile;

			cameraNode->fbxFromFile = _fbxFromFile;
			cameraNode->fbxFromGuid = icamera->fbxFromGuid;

			cameraNode->skinned = icamera->skinned;

			remapList[icamera->name] = cameraNode;
			allNodes.push_back(std::make_pair(cameraNode, *icamera));
		}
	}

	//Restore ui canvases
	for (std::vector<SUICanvas>::iterator iuicanvas = scene.uiCanvases.begin(); iuicanvas < scene.uiCanvases.end(); ++iuicanvas)
	{
		Ogre::String name = GenName(iuicanvas->name, manager);

		SceneNode* canvasNode = manager->createSceneNode(name);

		SceneManager* mgr = UICanvasFactory::manager;
		UICanvasFactory::manager = manager;
		UICanvas* canvas = (UICanvas*)manager->createMovableObject(name, UICanvasFactory::FACTORY_TYPE_NAME);
		UICanvasFactory::manager = mgr;

		canvas->SetReferenceScreenSize(iuicanvas->refScreenWidth, iuicanvas->refScreenHeight);
		canvas->SetScaleMode((UICanvas::ScaleMode)iuicanvas->scaleMode);

		canvasNode->attachObject((MovableObject*)canvas);

		canvasNode->setAlias(iuicanvas->alias);
		
		canvasNode->prefabName = ResourceMap::getResourceNameFromGuid(iuicanvas->prefabGuid);
		if (canvasNode->prefabName.empty())
			canvasNode->prefabName = iuicanvas->prefabName;
		canvasNode->prefabGuid = iuicanvas->prefabGuid;

		canvasNode->lightmapStatic = iuicanvas->lightmapStatic;
		canvasNode->navigationStatic = iuicanvas->navigationStatic;
		canvasNode->staticBatching = iuicanvas->staticBatching;
		canvasNode->skinned = iuicanvas->skinned;
		canvasNode->tag = atoi(iuicanvas->tag.c_str());
		canvasNode->layer = atoi(iuicanvas->layer.c_str());

		remapList[iuicanvas->name] = canvasNode;
		allNodes.push_back(std::make_pair(canvasNode, *iuicanvas));
	}

	//Restore ui buttons
	for (std::vector<SUIButton>::iterator iuibutton = scene.uiButtons.begin(); iuibutton < scene.uiButtons.end(); ++iuibutton)
	{
		Ogre::String name = GenName(iuibutton->name, manager);

		SceneNode* elementNode = manager->createSceneNode(name);
		UIButton* element = (UIButton*)manager->createMovableObject(name, UIButtonFactory::FACTORY_TYPE_NAME);

		elementNode->setAlias(iuibutton->alias);
		
		elementNode->prefabName = ResourceMap::getResourceNameFromGuid(iuibutton->prefabGuid);
		if (elementNode->prefabName.empty())
			elementNode->prefabName = iuibutton->prefabName;
		elementNode->prefabGuid = iuibutton->prefabGuid;

		elementNode->lightmapStatic = iuibutton->lightmapStatic;
		elementNode->navigationStatic = iuibutton->navigationStatic;
		elementNode->staticBatching = iuibutton->staticBatching;
		elementNode->skinned = iuibutton->skinned;
		elementNode->tag = atoi(iuibutton->tag.c_str());
		elementNode->layer = atoi(iuibutton->layer.c_str());

		elementNode->attachObject((MovableObject*)element);

		element->SetCanvasAlignment((UIElement::Alignment)iuibutton->canvasAlignment);
		element->SetAnchor(Vector2(iuibutton->anchor.x, iuibutton->anchor.y));

		std::string _imageNormalPath = ResourceMap::getResourceNameFromGuid(iuibutton->imageNormalGuid);
		if (_imageNormalPath.empty()) _imageNormalPath = iuibutton->imageNormal;
		std::string _imageHoverPath = ResourceMap::getResourceNameFromGuid(iuibutton->imageHoverGuid);
		if (_imageHoverPath.empty()) _imageHoverPath = iuibutton->imageHover;
		std::string _imagePressedPath = ResourceMap::getResourceNameFromGuid(iuibutton->imagePressedGuid);
		if (_imagePressedPath.empty()) _imagePressedPath = iuibutton->imagePressed;
		std::string _imageDisabledPath = ResourceMap::getResourceNameFromGuid(iuibutton->imageDisabledGuid);
		if (_imageDisabledPath.empty()) _imageDisabledPath = iuibutton->imageDisabled;

		element->SetImageNormal(_imageNormalPath);
		element->SetImageHover(_imageHoverPath);
		element->SetImagePressed(_imagePressedPath);
		element->SetImageDisabled(_imageDisabledPath);

		element->SetColorNormal(toCol(iuibutton->colorNormal));
		element->SetColorHover(toCol(iuibutton->colorHover));
		element->SetColorPressed(toCol(iuibutton->colorPressed));
		element->SetColorDisabled(toCol(iuibutton->colorDisabled));

		element->setColor(ColourValue(iuibutton->color.r, iuibutton->color.g, iuibutton->color.b, iuibutton->color.a));
		element->SetSize(toVec2(iuibutton->elementSize));

		element->SetInteractable(iuibutton->interactable);

		UIEvent downEvent;
		downEvent.functionName = iuibutton->pointerDownEvent.functionName;
		downEvent.sceneNode = iuibutton->pointerDownEvent.sceneNode;

		UIEvent upEvent;
		upEvent.functionName = iuibutton->pointerUpEvent.functionName;
		upEvent.sceneNode = iuibutton->pointerUpEvent.sceneNode;

		element->SetPointerDownEvent(downEvent);
		element->SetPointerUpEvent(upEvent);

		remapList[iuibutton->name] = elementNode;
		allNodes.push_back(std::make_pair(elementNode, *iuibutton));
	}

	//Restore ui texts
	for (std::vector<SUIText>::iterator iuitext = scene.uiTexts.begin(); iuitext < scene.uiTexts.end(); ++iuitext)
	{
		Ogre::String name = GenName(iuitext->name, manager);

		SceneNode* elementNode = manager->createSceneNode(name);
		UIText* element = (UIText*)manager->createMovableObject(name, UITextFactory::FACTORY_TYPE_NAME);

		elementNode->setAlias(iuitext->alias);
		
		elementNode->prefabName = ResourceMap::getResourceNameFromGuid(iuitext->prefabGuid);
		if (elementNode->prefabName.empty())
			elementNode->prefabName = iuitext->prefabName;
		elementNode->prefabGuid = iuitext->prefabGuid;

		elementNode->lightmapStatic = iuitext->lightmapStatic;
		elementNode->staticBatching = iuitext->staticBatching;
		elementNode->skinned = iuitext->skinned;
		elementNode->tag = atoi(iuitext->tag.c_str());
		elementNode->layer = atoi(iuitext->layer.c_str());

		elementNode->attachObject((MovableObject*)element);

		std::string _fontPath = ResourceMap::getResourceNameFromGuid(iuitext->fontGuid);
		if (_fontPath.empty()) _fontPath = iuitext->font;

		element->SetCanvasAlignment((UIElement::Alignment)iuitext->canvasAlignment);
		element->SetAnchor(Vector2(iuitext->anchor.x, iuitext->anchor.y));
		element->setFontName(_fontPath);
		element->setColor(ColourValue(iuitext->color.r, iuitext->color.g, iuitext->color.b, iuitext->color.a));
		element->setText(iuitext->text);
		element->setFontSize(iuitext->size);
		element->SetSize(toVec2(iuitext->elementSize));
		element->setVerticalAlignment(static_cast<UIText::VerticalAlignment>(iuitext->verticalAlignment));
		element->setHorizontalAlignment(static_cast<UIText::HorizontalAlignment>(iuitext->horizontalAlignment));

		remapList[iuitext->name] = elementNode;
		allNodes.push_back(std::make_pair(elementNode, *iuitext));
	}

	//Restore ui images
	for (std::vector<SUIImage>::iterator iuiimage = scene.uiImages.begin(); iuiimage < scene.uiImages.end(); ++iuiimage)
	{
		Ogre::String name = GenName(iuiimage->name, manager);

		SceneNode* elementNode = manager->createSceneNode(name);
		UIImage* element = (UIImage*)manager->createMovableObject(name, UIImageFactory::FACTORY_TYPE_NAME);

		elementNode->setAlias(iuiimage->alias);
		
		elementNode->prefabName = ResourceMap::getResourceNameFromGuid(iuiimage->prefabGuid);
		if (elementNode->prefabName.empty())
			elementNode->prefabName = iuiimage->prefabName;
		elementNode->prefabGuid = iuiimage->prefabGuid;

		elementNode->lightmapStatic = iuiimage->lightmapStatic;
		elementNode->navigationStatic = iuiimage->navigationStatic;
		elementNode->staticBatching = iuiimage->staticBatching;
		elementNode->skinned = iuiimage->skinned;
		elementNode->tag = atoi(iuiimage->tag.c_str());
		elementNode->layer = atoi(iuiimage->layer.c_str());

		elementNode->attachObject((MovableObject*)element);

		std::string _imagePath = ResourceMap::getResourceNameFromGuid(iuiimage->imageGuid);
		if (_imagePath.empty()) _imagePath = iuiimage->image;

		element->SetCanvasAlignment((UIElement::Alignment)iuiimage->canvasAlignment);
		element->SetAnchor(Vector2(iuiimage->anchor.x, iuiimage->anchor.y));
		element->SetImage(_imagePath);
		element->setColor(ColourValue(iuiimage->color.r, iuiimage->color.g, iuiimage->color.b, iuiimage->color.a));
		element->SetSize(toVec2(iuiimage->elementSize));

		remapList[iuiimage->name] = elementNode;
		allNodes.push_back(std::make_pair(elementNode, *iuiimage));
	}

	//Restore particle systems
	for (std::vector<SParticleSystem>::iterator it = scene.particleSystems.begin(); it < scene.particleSystems.end(); ++it)
	{
		Ogre::String name = GenName(it->name, manager);

		SceneNode* elementNode = manager->createSceneNode(name);
		ParticleSystem* element = (ParticleSystem*)manager->createParticleSystem(name, 10000, "Assets");

		elementNode->setAlias(it->alias);
		
		elementNode->prefabName = ResourceMap::getResourceNameFromGuid(it->prefabGuid);
		if (elementNode->prefabName.empty())
			elementNode->prefabName = it->prefabName;
		elementNode->prefabGuid = it->prefabGuid;

		elementNode->lightmapStatic = it->lightmapStatic;
		elementNode->navigationStatic = it->navigationStatic;
		elementNode->staticBatching = it->staticBatching;
		elementNode->skinned = it->skinned;
		elementNode->tag = atoi(it->tag.c_str());
		elementNode->layer = atoi(it->layer.c_str());

		elementNode->attachObject(element);

		std::string _materialPath = ResourceMap::getResourceNameFromGuid(it->materialGuid);
		if (_materialPath.empty()) _materialPath = it->material;

		element->setCastShadows(it->castShadows);
		element->setEmitting(it->emit);
		element->setKeepParticlesInLocalSpace(it->keepLocal);
		element->setMaterialName(_materialPath);
		element->setDefaultWidth(it->size.x);
		element->setDefaultHeight(it->size.y);
		element->setSpeedFactor(it->speedFactor);
		element->getRenderer()->setParameter("billboard_type", it->billboardType);

		for (std::vector<SParticleEmitter>::iterator em = it->emitters.begin(); em != it->emitters.end(); ++em)
		{
			if (em->type.empty())
				continue;

			ParticleEmitter* emitter = element->addEmitter(em->type);
			emitter->setAngle(Radian(Degree(em->angle)));
			emitter->setColour(ColourValue(em->startColor.r, em->startColor.g, em->startColor.b, em->startColor.a));
			emitter->setDirection(Vector3(em->direction.x, em->direction.y, em->direction.z));
			emitter->setEmissionRate(em->emissionRate);
			emitter->setEnabled(true/*em->enabled*/);
			emitter->setMinDuration(em->minDuration);
			emitter->setMinParticleVelocity(em->minVelocity);
			emitter->setMinRepeatDelay(em->minRepeatDelay);
			emitter->setMinTimeToLive(em->minLifeTime);
			emitter->setMaxDuration(em->maxDuration);
			emitter->setMaxParticleVelocity(em->maxVelocity);
			emitter->setMaxRepeatDelay(em->maxRepeatDelay);
			emitter->setMaxTimeToLive(em->maxLifeTime);

			for (std::vector<SParticleSystemParameter>::iterator p = em->parameters.begin(); p != em->parameters.end(); ++p)
			{
				emitter->setParameter(p->name, p->value);
			}
		}

		for (std::vector<SParticleAffector>::iterator af = it->affectors.begin(); af != it->affectors.end(); ++af)
		{
			if (af->type.empty())
				continue;

			ParticleAffector* affector = element->addAffector(af->type);

			for (std::vector<SParticleSystemParameter>::iterator p = af->parameters.begin(); p != af->parameters.end(); ++p)
			{
				affector->setParameter(p->name, p->value);
			}
		}

		remapList[it->name] = elementNode;
		allNodes.push_back(std::make_pair(elementNode, *it));
	}
}

void SceneSerializer::Deserialize(SceneManager* manager, string path)
{
	if (!GetEngine->GetUseUnpackedResources())
	{
		string _path = IO::RemovePart(path, GetEngine->GetAssetsPath());

		if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), _path))
		{
			int sz = 0;
			auto buf = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), _path, sz);

			SceneSerializer sceneSerializer;
			sceneSerializer.DeserializeFromBuffer(manager, buf, sz, _path);
		}
		else
		{
			MessageBoxA(nullptr, "Error opening scene", "Error. Can't open", MB_OK | MB_ICONERROR);
		}
	}
	else
	{
		if (path.empty())
			return;

		if (!IO::FileExists(path))
			return;

		//-----------------------------------//
		//----Load scene from binary file----//

		try
		{
			std::ifstream ofs(path, std::ios::binary);
			boost::archive::binary_iarchive stream(ofs);

			SScene scene;

			stream >> scene;
			ofs.close();

			Deserialize(manager, scene, path);
		}
		catch (Exception e)
		{
			GetEngine->ClearScene(manager);
			MessageBoxA(nullptr, "The scene you are trying to open is corrupted or it was created with previous version of the editor.", "Error. Can't open", MB_OK | MB_ICONERROR);
			MessageBoxA(nullptr, e.getFullDescription().c_str(), "Error. Can't open", MB_OK | MB_ICONERROR);
		}
		catch (boost::archive::archive_exception e)
		{
			GetEngine->ClearScene(manager);
			MessageBoxA(nullptr, "The scene you are trying to open is corrupted or it was created with previous version of the editor.", "Error. Can't open", MB_OK | MB_ICONERROR);
			MessageBoxA(nullptr, e.what(), "Error. Can't open", MB_OK | MB_ICONERROR);
		}
	}
}

void SceneSerializer::DeserializeFromBuffer(SceneManager* manager, char* buffer, int bufSize, string path)
{
	//-----------------------------------//
	//----Load scene from buffer----//

	try
	{
		boost::iostreams::stream<boost::iostreams::array_source> is(buffer, bufSize);
		boost::archive::binary_iarchive stream(is);
		
		SScene scene;

		stream >> scene;

		is.close();
		delete[] buffer;

		Deserialize(manager, scene, path);
	}
	catch (Exception e)
	{
		GetEngine->ClearScene(manager);
		MessageBoxA(nullptr, "The scene you are trying to open is corrupted or it was created with previous version of the editor.", "Error. Can't open", MB_OK | MB_ICONERROR);
		MessageBoxA(nullptr, e.getFullDescription().c_str(), "Error. Can't open", MB_OK | MB_ICONERROR);
	}
	catch (boost::archive::archive_exception e)
	{
		GetEngine->ClearScene(manager);
		MessageBoxA(nullptr, "The scene you are trying to open is corrupted or it was created with previous version of the editor.", "Error. Can't open", MB_OK | MB_ICONERROR);
		MessageBoxA(nullptr, e.what(), "Error. Can't open", MB_OK | MB_ICONERROR);
	}
}

void SceneSerializer::Deserialize(SceneManager * manager, SScene & scene, string path)
{
	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		//Init deferred shading
		//GetEngine->GetDeferredShadingSystem()->Cleanup();
		GetEngine->GetDeferredShadingSystem()->removeViewports();
	}

	GetEngine->GetDeferredShadingSystem()->resetLights();

	//Clear current scene
	Engine::ClearScene(manager);
	GetEngine->nameGenerator->reset();

	if (!path.empty())
	{
		GetEngine->loadedScene = path;
		GetEngine->GetNavigationManager()->SetLoadedScene(path);
	}

	//------------Restore objects-------------//

	std::vector<std::pair<SceneNode*, SSceneNode>> allNodes;
	std::map<std::string, SceneNode*> remapList;
	RestoreObjects(scene, manager, allNodes, remapList, false);

	//--------------Restore order---------------//

	std::sort(allNodes.begin(), allNodes.end(), [](const std::pair<SceneNode*, SSceneNode> & a, const std::pair<SceneNode*, SSceneNode> & b) -> bool
	{
		return a.second.index < b.second.index;
	});

	for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		ssi->first->index = ssi->second.index;
		manager->getRootSceneNode()->addChild(ssi->first);
	}

	for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		Vector3 pos = Vector3(ssi->second.position.x, ssi->second.position.y, ssi->second.position.z);
		Quaternion rot = Quaternion(ssi->second.rotation.w, ssi->second.rotation.x, ssi->second.rotation.y, ssi->second.rotation.z);
		Vector3 scl = Vector3(ssi->second.scale.x, ssi->second.scale.y, ssi->second.scale.z);

		RestoreNodeRelationship(manager, ssi->second.name, ssi->second.parentName, pos, rot, scl);

		ssi->first->setGlobalVisible(ssi->second.visible);

		//Restore scripts
		RestoreScripts(ssi->first, ssi->second, remapList);
		//Restore components
		RestoreComponents(ssi->first, ssi->second, remapList);
	}

	for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		if (ssi->first->getParent() == manager->getRootSceneNode())
		{
			ssi->first->setVisible(ssi->second.visible);
		}
	}

	for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		AnimationList* lst = (AnimationList*)ssi->first->GetComponent(AnimationList::COMPONENT_TYPE);

		if (lst != nullptr)
			GenIndexedPointers(lst, ssi->first);

		if (ssi->first->getVisible())
		{
			if (ssi->first->getAttachedObject(0)->getMovableType() == "Camera")
			{
				if (GetEngine->GetEnvironment() == Engine::Environment::Player)
				{
					Camera* cam = (Camera*)ssi->first->getAttachedObject(0);
					if (cam->getMainCamera())
						GetEngine->SetMainCamera(cam);
				}
			}
		}
	}

	//Build static geometry
	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		std::string scenePath = IO::RemovePart(GetEngine->loadedScene, GetEngine->GetAssetsPath());
		std::string saveTo = GetEngine->GetAssetsPath() + IO::GetFilePath(scenePath) + IO::GetFileName(scenePath) + "/Lightmaps/";

		GetEngine->buildStaticGeometry(saveTo);
	}
	//

	for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		SceneNode* node = ssi->first;
		LoadAnimationAndSkinningData(node);
	}

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
		{
			SkinnedMeshDeformer::EnableAnimation(ssi->first, false);
			SkinnedMeshDeformer::UpdateSkeleton(ssi->first);
		}
	}

	GetEngine->UpdateSceneNodeIndexes();

	GetEngine->GetNavigationManager()->LoadNavMesh();
	GetEngine->GetNavigationManager()->Init();

	/**/
	//GetEngine->UpdateMaterials();
	//GetEngine->ClearMaterialsInstances();
	/**/

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		if (GetEngine->GetMainCamera() != nullptr)
		{
			//Init deferred shading
			GetEngine->GetDeferredShadingSystem()->addViewport(GetEngine->GetMainCamera()->getViewport());
			GetEngine->GetDeferredShadingSystem()->setCamera(GetEngine->GetMainCamera());
			//GetEngine->GetDeferredShadingSystem()->initialize();
		}

		GetEngine->GetTerrainManager()->InitPagedGeometry();
		RestoreSceneData(manager, scene);
		GetEngine->GetTerrainManager()->SetPagedGeometryCamera(GetEngine->GetMainCamera());

		//GetEngine->GetNavigationManager()->BuildNavMesh();
		//GetEngine->GetNavigationManager()->Init();
		GetEngine->GetNavigationManager()->InitAgents();
		GetEngine->GetPhysicsManager()->Setup();

		GetEngine->GetMonoRuntime()->MonoScriptSetFieldsAllNodes(manager->getRootSceneNode());

		//Call components post load function
		//Rigidbodies first
		for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
		{
			SceneNode* node = ssi->first;
			for (SceneNode::ComponentList::iterator it = node->components.begin(); it != node->components.end(); ++it)
			{
				if ((*it)->GetComponentTypeName() == RigidBody::COMPONENT_TYPE)
					(*it)->SceneLoaded();
			}
		}
		//Then others
		for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
		{
			SceneNode * node = ssi->first;
			for (SceneNode::ComponentList::iterator it = node->components.begin(); it != node->components.end(); ++it)
			{
				if ((*it)->GetComponentTypeName() != RigidBody::COMPONENT_TYPE)
					(*it)->SceneLoaded();
			}
		}

		GetEngine->UpdateMaterials();

		//Execute start function
		GetEngine->GetMonoRuntime()->Execute("Start");
	}
	else
	{
		for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
		{
			SceneNode* node = ssi->first;
			for (SceneNode::ComponentList::iterator it = node->components.begin(); it != node->components.end(); ++it)
			{
				if ((*it)->GetComponentTypeName() == NavMeshObstacle::COMPONENT_TYPE)
					(*it)->SceneLoaded();
			}
		}

		GetEngine->GetTerrainManager()->InitPagedGeometry();
		RestoreSceneData(manager, scene);
		GetEngine->GetMonoRuntime()->MonoScriptSetFieldsAllNodes(manager->getRootSceneNode());
		GetEngine->UpdateMaterials();
	}

	remapList.clear();

	MapIterator it = FBXSceneManager::getSingleton().getResourceIterator();
	while (it.hasMoreElements())
	{
		FBXScenePtr fbx = static_pointer_cast<FBXScene>(it.getNext());
		fbx->saveCache();
	}

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		DeferredLightRenderOperation::UpdateStaticShadowmaps();
		Root::getSingleton().renderOneFrame();
	}

	for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		if (ssi->first->getAttachedObjects().size() > 0)
		{
			if (ssi->first->getAttachedObject(0)->getMovableType() == UITextFactory::FACTORY_TYPE_NAME)
			{
				UIText* txt = (UIText*)ssi->first->getAttachedObject(0);
				txt->update();
			}
		}
	}

	//mono_gc_collect(mono_gc_max_generation());
}

void SceneSerializer::SerializeToPrefab(SceneNode * root, string path)
{
	std::ofstream ofs(path, std::ios::binary);
	boost::archive::binary_oarchive stream(ofs);

	SScene scene = SScene();
	GetEngine->UpdateSceneNodeIndexes();

	auto childVec = root->getParent()->getChildrenList();
	int pos = std::distance(childVec.begin(), std::find(childVec.begin(), childVec.end(), root));

	/*Detect if this object is the child of another prefab*/
	SceneNode* _parent = root->getParentSceneNode();
	bool _prefab = _parent->prefabGuid != "" || _parent->prefabName != "";

	while (_prefab == false && _parent != nullptr)
	{
		_parent = _parent->getParentSceneNode();
		if (_parent != nullptr)
			_prefab = _parent->prefabGuid != "" || _parent->prefabName != "";
	}
	/*End*/

	SceneNode * parent = (SceneNode*)root->getParent();
	parent->removeChild(root);
	SceneNode * temp = GetEngine->GetSceneManager()->createSceneNode();
	temp->addChild(root);

	bool isPrefab = false;
	if (root->prefabName != "" || root->prefabGuid != "")
		isPrefab = true;

	string _path = IO::RemovePart(path, GetEngine->GetAssetsPath());

	if (!isPrefab)
	{
		string pGuid = ResourceMap::genGuid();

		if (!_prefab)
		{
			root->prefabName = _path;
			root->prefabGuid = pGuid;
		}

		ResourceMap::addResource(_path, pGuid);
	}
	else
	{
		string _ppath = ResourceMap::getResourceNameFromGuid(root->prefabGuid);
		if (_ppath.empty())
			_ppath = root->prefabName;

		if (_ppath != path)
		{
			root->prefabName = _path;
			root->prefabGuid = ResourceMap::genGuid();
			ResourceMap::addResource(_path, root->prefabGuid);
		}
	}

	ClearPrefabLinks(root);

	//Run recursive serialization of scene nodes
	SerializeChild(temp, &scene);

	temp->removeChild(root);
	parent->insertChild(root, pos);

	GetEngine->GetSceneManager()->destroySceneNode(temp);

	//Write file
	stream << scene;
	ofs.close();

	prefabCache.erase(path);
}

SceneNode * SceneSerializer::DeserializeFromPrefab(SceneManager * manager, string path, bool onlyMeshes)
{
	if (path.empty())
		return nullptr;

	if (GetEngine->GetUseUnpackedResources())
	{
		if (!IO::FileExists(path))
			return nullptr;
	}

	SScene scene;

	if (prefabCache.find(path) != prefabCache.end())
	{
		//Load from cache
		scene = prefabCache[path];
	}
	else
	{
		//-----------------------------------//
		//----Load prefab from binary file----//

		if (!GetEngine->GetUseUnpackedResources())
		{
			string _path = IO::RemovePart(path, GetEngine->GetAssetsPath());

			if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), _path))
			{
				try
				{
					int sz = 0;
					auto buf = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), _path, sz);

					boost::iostreams::stream<boost::iostreams::array_source> is(buf, sz);
					boost::archive::binary_iarchive stream(is);

					stream >> scene;

					is.close();
					delete[] buf;
					stream.delete_created_pointers();

					prefabCache[path] = scene;
				}
				catch (...)
				{
					SceneNode* errorNode = manager->getRootSceneNode()->createChildSceneNode(GenName("NotSupported", manager));
					Empty* empty = (Empty*)manager->createMovableObject(errorNode->getName(), EmptyObjectFactory::FACTORY_TYPE_NAME);

					errorNode->setAlias("NotSupported_Object");
					errorNode->attachObject(empty);

					return errorNode;
				}
			}
		}
		else
		{
			try
			{
				std::ifstream ofs(path, std::ios::binary);
				boost::archive::binary_iarchive stream(ofs);

				stream >> scene;
				ofs.close();
				stream.delete_created_pointers();

				//Save to cache in RAM
				prefabCache[path] = scene;
			}
			catch (...)
			{
				SceneNode* errorNode = manager->getRootSceneNode()->createChildSceneNode(GenName("NotSupported", manager));
				Empty* empty = (Empty*)manager->createMovableObject(errorNode->getName(), EmptyObjectFactory::FACTORY_TYPE_NAME);

				errorNode->setAlias("NotSupported_Object");
				errorNode->attachObject(empty);

				return errorNode;
			}
		}
	}

	SceneNode * rootReturn = nullptr;

	//------------Restore objects-------------//

	std::vector<std::pair<SceneNode*, SSceneNode>> allNodes;
	std::map<std::string, SceneNode*> remapList;
	RestoreObjects(scene, manager, allNodes, remapList, onlyMeshes);

	//--------------Restore order---------------//

	std::sort(allNodes.begin(), allNodes.end(), [](const std::pair<SceneNode*, SSceneNode> & a, const std::pair<SceneNode*, SSceneNode> & b) -> bool
	{
		return a.second.index < b.second.index;
	});

	if (rootReturn == nullptr)
	{
		if (allNodes.size() > 0)
			rootReturn = allNodes.at(0).first;
	}
	
	for (std::vector<std::pair<SceneNode*, SSceneNode>>::iterator ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		ssi->first->index = ssi->second.index;
		manager->getRootSceneNode()->addChild(ssi->first);
	}

	for (std::vector<std::pair<SceneNode*, SSceneNode>>::iterator ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		Vector3 pos = Vector3(ssi->second.position.x, ssi->second.position.y, ssi->second.position.z);
		Quaternion rot = Quaternion(ssi->second.rotation.w, ssi->second.rotation.x, ssi->second.rotation.y, ssi->second.rotation.z);
		Vector3 scl = Vector3(ssi->second.scale.x, ssi->second.scale.y, ssi->second.scale.z);

		RestoreNodeRelationship(manager, remapList[ssi->second.name]->getName(), remapList[ssi->second.parentName] != nullptr ? remapList[ssi->second.parentName]->getName() : "", pos, rot, scl);
		ssi->first->setGlobalVisible(ssi->second.visible);

		if (!onlyMeshes)
		{
			//Restore scripts
			RestoreScripts(remapList[ssi->second.name], ssi->second, remapList);
			//Restore components
			RestoreComponents(remapList[ssi->second.name], ssi->second, remapList);
		}
	}

	for (std::vector<std::pair<SceneNode*, SSceneNode>>::iterator ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		if (ssi->first->getParent() == manager->getRootSceneNode())
		{
			ssi->first->setVisible(ssi->second.visible);
		}
	}

	for (std::vector<std::pair<SceneNode*, SSceneNode>>::iterator ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		AnimationList* lst = (AnimationList*)ssi->first->GetComponent(AnimationList::COMPONENT_TYPE);

		if (lst != nullptr)
			GenIndexedPointers(lst, ssi->first);
	}

	//if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	//{
	for (std::vector<std::pair<SceneNode*, SSceneNode>>::iterator ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		SceneNode* node = remapList[ssi->second.name];
		LoadAnimationAndSkinningData(node);
	}
	//}

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
		{
			SkinnedMeshDeformer::EnableAnimation(ssi->first, false);
			SkinnedMeshDeformer::UpdateSkeleton(ssi->first);
		}
	}

	GetEngine->UpdateSceneNodeIndexes();

	//Execute start function
	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		//GetEngine->GetNavigationManager()->Init();

		if (rootReturn != nullptr)
		{
			GetEngine->GetNavigationManager()->InitAgents(rootReturn);
			GetEngine->GetPhysicsManager()->Setup(rootReturn);

			GetEngine->GetMonoRuntime()->MonoScriptSetFieldsAllNodes(rootReturn);
		}

		//Call components post load function
		//Rigidbodies first
		for (std::vector<std::pair<SceneNode*, SSceneNode>>::iterator ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
		{
			SceneNode* node = remapList[ssi->second.name];
			for (SceneNode::ComponentList::iterator it = node->components.begin(); it != node->components.end(); ++it)
			{
				if ((*it)->GetComponentTypeName() == RigidBody::COMPONENT_TYPE)
					(*it)->SceneLoaded();
			}
		}
		//Then others
		for (std::vector<std::pair<SceneNode*, SSceneNode>>::iterator ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
		{
			SceneNode* node = remapList[ssi->second.name];
			for (SceneNode::ComponentList::iterator it = node->components.begin(); it != node->components.end(); ++it)
			{
				if ((*it)->GetComponentTypeName() != RigidBody::COMPONENT_TYPE)
					(*it)->SceneLoaded();
			}

			if (node->getAttachedObjects().size() > 0)
			{
				if (node->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
				{
					Entity* ent = (Entity*)node->getAttachedObject(0);
					GetEngine->UpdateMaterials(ent);
				}
			}
		}
		//Call start event
		for (std::vector<std::pair<SceneNode*, SSceneNode>>::iterator ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
		{
			SceneNode* node = remapList[ssi->second.name];
			GetEngine->GetMonoRuntime()->ExecuteForNode(node, "Start");
		}
	}
	else
	{
		for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
		{
			SceneNode* node = ssi->first;
			for (SceneNode::ComponentList::iterator it = node->components.begin(); it != node->components.end(); ++it)
			{
				if ((*it)->GetComponentTypeName() == NavMeshObstacle::COMPONENT_TYPE)
					(*it)->SceneLoaded();
			}
		}

		if (!onlyMeshes)
		{
			if (rootReturn != nullptr)
				GetEngine->GetMonoRuntime()->MonoScriptSetFieldsAllNodes(rootReturn);
		}
	}

	remapList.clear();

	if (rootReturn != nullptr)
	{
		rootReturn->prefabName = IO::RemovePart(path, GetEngine->GetAssetsPath());
		rootReturn->prefabGuid = ResourceMap::getResourceGuidFromName(rootReturn->prefabName);
	}

	MapIterator it = FBXSceneManager::getSingleton().getResourceIterator();
	while (it.hasMoreElements())
	{
		FBXScenePtr fbx = static_pointer_cast<FBXScene>(it.getNext());
		fbx->saveCache();
	}

	return rootReturn;
}

void SceneSerializer::RestoreNodeRelationship(SceneManager * manager, string nodeName, string parentName, Vector3 position, Quaternion rotation, Vector3 scale)
{
	SceneNode *emptyNode = manager->getSceneNode(nodeName);

	//Restore parent and transform
	if (parentName.empty())
	{
		SceneNode * oldParent = emptyNode->getParentSceneNode();
		if (oldParent != nullptr)
			oldParent->removeChild(nodeName);
		manager->getRootSceneNode()->addChild(emptyNode);
	}
	else if (parentName != "Ogre/SceneRoot")
	{
		SceneNode * parent = manager->getSceneNode(parentName);
		SceneNode * oldParent = emptyNode->getParentSceneNode();
		if (oldParent != nullptr)
			oldParent->removeChild(nodeName);
		parent->addChild(emptyNode);
	}

	emptyNode->setPosition(position);
	emptyNode->setOrientation(rotation);
	emptyNode->setScale(scale);

	emptyNode->setInitialState();
}

void SceneSerializer::SaveScripts(SceneNode * _node, SSceneNode & node)
{
	for (std::vector<MonoScript*>::iterator scr_it = _node->monoScripts.begin(); scr_it != _node->monoScripts.end(); ++scr_it)
	{
		SMonoScript script;
		script.className = CP_SYS(mono_class_get_name((*scr_it)->_class));
		script.enabled = (*scr_it)->enabled;

		MonoScript * scr = *scr_it;

		MonoRuntime::FieldList lst = GetEngine->GetMonoRuntime()->GetFields(scr);

		//Remove incorrect fields
		std::vector<std::string> toRemove;

		for (MonoRuntime::FieldList::iterator it = lst.begin(); it != lst.end(); ++it)
		{
			for (std::map<std::string, MonoScript::MonoFieldInfo>::iterator it2 = scr->fieldSerializeList.begin(); it2 != scr->fieldSerializeList.end(); ++it2)
			{
				if (it->fieldName == it2->first)
				{
					if (it->fieldType != it2->second.fieldType)
						toRemove.push_back(it->fieldName);
				}
			}
		}

		for (std::map<std::string, MonoScript::MonoFieldInfo>::iterator it2 = scr->fieldSerializeList.begin(); it2 != scr->fieldSerializeList.end(); ++it2)
		{
			MonoRuntime::FieldList::iterator it = find_if(lst.begin(), lst.end(), [it2](const MonoScript::MonoFieldInfo& s) { return s.fieldName == it2->first; });

			if (it == lst.end())
				toRemove.push_back(it2->first);
		}

		for (std::vector<std::string>::iterator it = toRemove.begin(); it != toRemove.end(); ++it)
		{
			scr->fieldSerializeList.erase(*it);
		}

		toRemove.clear();

		//Store fields
		for (std::map<std::string, MonoScript::MonoFieldInfo>::iterator it = scr->fieldSerializeList.begin(); it != scr->fieldSerializeList.end(); ++it)
		{
			SMonoFieldInfo inf;
			inf.fieldName = it->second.fieldName;
			inf.fieldType = it->second.fieldType;
			inf.boolVal = it->second.fieldValue.boolVal;
			inf.floatVal = it->second.fieldValue.floatVal;
			inf.intVal = it->second.fieldValue.intVal;
			inf.stringVal = it->second.fieldValue.stringVal;
			inf.objectVal = it->second.fieldValue.objectVal;
			inf.vec2Val = toSVec2(it->second.fieldValue.vec2Val);
			inf.vec3Val = toSVec3(it->second.fieldValue.vec3Val);
			inf.vec4Val = toSVec4(it->second.fieldValue.vec4Val);

			script.monoFieldList.push_back(inf);
		}

		node.monoScripts.push_back(script);
	}
}

void SceneSerializer::RestoreScripts(SceneNode * _node, SSceneNode & node, std::map<std::string, SceneNode*>& remapList)
{
	for (std::vector<SMonoScript>::iterator scr_it = node.monoScripts.begin(); scr_it != node.monoScripts.end(); ++scr_it)
	{
		MonoClass* _class = GetEngine->GetMonoRuntime()->FindClass(scr_it->className);

		if (_class == nullptr)
			continue;

		MonoScript * monoScript = new MonoScript(_node);
		//monoScript->className = scr_it->className;
		monoScript->enabled = scr_it->enabled;
		monoScript->_class = _class;

		monoScript->CreateInstance();

		//GetEngine->GetMonoRuntime()->CreateInstance(_node, monoScript);

		for (std::vector<SMonoFieldInfo>::iterator it = scr_it->monoFieldList.begin(); it != scr_it->monoFieldList.end(); ++it)
		{
			std::string objectVal = remapList[it->objectVal] != nullptr ? remapList[it->objectVal]->getName() : it->objectVal;

			MonoScript::MonoFieldInfo inf;
			inf.fieldName = it->fieldName;
			inf.fieldType = it->fieldType;
			MonoScript::ValueVariant val;
			val.boolVal = it->boolVal;
			val.floatVal = it->floatVal;
			val.intVal = it->intVal;
			val.stringVal = it->stringVal;
			val.objectVal = objectVal;
			val.vec2Val = toVec2(it->vec2Val);
			val.vec3Val = toVec3(it->vec3Val);
			val.vec4Val = toVec4(it->vec4Val);

			inf.fieldValue = val;

			monoScript->fieldSerializeList[inf.fieldName] = inf;
		}

		//GetEngine->GetMonoRuntime()->MonoScriptSetFields(monoScript);

		_node->monoScripts.push_back(monoScript);
	}
}

void SceneSerializer::SaveComponents(SceneNode * _node, SSceneNode & node)
{
	for (std::vector<Component*>::iterator it = _node->components.begin(); it != _node->components.end(); ++it)
	{
		//Audio sources
		if (dynamic_cast<AudioSource*>(*it))
		{
			SAudioSource component;
			AudioSource * audio = (AudioSource*)*it;

			component.fileName = audio->GetFileName();
			component.fileGuid = audio->GetFileGuid();
			component.playOnStart = audio->GetPlayOnStart();
			component.loop = audio->GetLooped();
			component.enabled = audio->GetEnabled();
			component.volume = audio->GetVolume();
			component.minDistance = audio->GetMinDistance();
			component.maxDistance = audio->GetMaxDistance();
			component.is2D = audio->GetIs2D();

			node.audioSourceComponents.push_back(component);
		}

		//Audio listeners
		if (dynamic_cast<AudioListener*>(*it))
		{
			SAudioListener component;
			AudioListener * listener = (AudioListener*)*it;

			component.enabled = listener->GetEnabled();
			component.volume = listener->GetVolume();

			node.audioListenerComponents.push_back(component);
		}

		//Rigidbodies
		if (dynamic_cast<RigidBody*>(*it))
		{
			SRigidBody component;
			RigidBody * rigidBody = (RigidBody*)*it;
			component.mass = rigidBody->GetMass();
			component.freezePositionX = rigidBody->GetFreezePositionX();
			component.freezePositionY = rigidBody->GetFreezePositionY();
			component.freezePositionZ = rigidBody->GetFreezePositionZ();
			component.freezeRotationX = rigidBody->GetFreezeRotationX();
			component.freezeRotationY = rigidBody->GetFreezeRotationY();
			component.freezeRotationZ = rigidBody->GetFreezeRotationZ();
			component.isKinematic = rigidBody->GetIsKinematic();
			component.isStatic = rigidBody->GetIsStatic();
			component.useOwnGravity = rigidBody->GetUseOwnGravity();
			component.gravity = toSVec3(rigidBody->GetGravity());
			component.friction = rigidBody->GetFriction();
			component.bounciness = rigidBody->GetBounciness();
			component.linearDamping = rigidBody->GetLinearDamping();
			component.angularDamping = rigidBody->GetAngularDamping();
			component.enabled = rigidBody->GetEnabled();

			node.rigidbodyComponents.push_back(component);
		}

		//Mesh colliders
		if (dynamic_cast<MeshCollider*>(*it))
		{
			SMeshCollider component;
			MeshCollider * collider = (MeshCollider*)*it;
			component.convex = collider->GetConvex();
			component.isTrigger = collider->GetIsTrigger();
			component.enabled = collider->GetEnabled();

			node.meshColliderComponents.push_back(component);
		}

		//Capsule colliders
		if (dynamic_cast<CapsuleCollider*>(*it))
		{
			SCapsuleCollider component;
			CapsuleCollider * collider = (CapsuleCollider*)*it;
			component.radius = collider->GetRadius();
			component.height = collider->GetHeight();
			component.offset = SVector3(collider->GetOffset().x, collider->GetOffset().y, collider->GetOffset().z);
			component.rotation = SQuaternion(collider->GetRotation().x, collider->GetRotation().y, collider->GetRotation().z, collider->GetRotation().w);
			component.isTrigger = collider->GetIsTrigger();
			component.enabled = collider->GetEnabled();

			node.capsuleColliderComponents.push_back(component);
		}

		//Box colliders
		if (dynamic_cast<BoxCollider*>(*it))
		{
			SBoxCollider component;
			BoxCollider * collider = (BoxCollider*)*it;
			component.boxSize = SVector3(collider->GetBoxSize().x, collider->GetBoxSize().y, collider->GetBoxSize().z);
			component.offset = SVector3(collider->GetOffset().x, collider->GetOffset().y, collider->GetOffset().z);
			component.rotation = SQuaternion(collider->GetRotation().x, collider->GetRotation().y, collider->GetRotation().z, collider->GetRotation().w);
			component.isTrigger = collider->GetIsTrigger();
			component.enabled = collider->GetEnabled();

			node.boxColliderComponents.push_back(component);
		}

		//Sphere colliders
		if (dynamic_cast<SphereCollider*>(*it))
		{
			SSphereCollider component;
			SphereCollider * collider = (SphereCollider*)*it;
			component.radius = collider->GetRadius();
			component.offset = SVector3(collider->GetOffset().x, collider->GetOffset().y, collider->GetOffset().z);
			component.isTrigger = collider->GetIsTrigger();
			component.enabled = collider->GetEnabled();

			node.sphereColliderComponents.push_back(component);
		}

		//Skeleton bone
		if (dynamic_cast<SkeletonBone*>(*it))
		{
			SSkeletonBone component;
			SkeletonBone * skelBone = (SkeletonBone*)*it;
			component.index = skelBone->GetIndex();
			component.name = skelBone->GetName();
			component.bindingPosition = SVector3(skelBone->GetBindingPosition().x, skelBone->GetBindingPosition().y, skelBone->GetBindingPosition().z);
			component.bindingRotation = SQuaternion(skelBone->GetBindingRotation().x, skelBone->GetBindingRotation().y, skelBone->GetBindingRotation().z, skelBone->GetBindingRotation().w);
			component.bindingScale = SVector3(skelBone->GetBindingScale().x, skelBone->GetBindingScale().y, skelBone->GetBindingScale().z);
			component.position = SVector3(skelBone->GetPosition().x, skelBone->GetPosition().y, skelBone->GetPosition().z);
			component.rotation = SQuaternion(skelBone->GetRotation().x, skelBone->GetRotation().y, skelBone->GetRotation().z, skelBone->GetRotation().w);
			component.scale = SVector3(skelBone->GetScale().x, skelBone->GetScale().y, skelBone->GetScale().z);
			component.skeletonName = skelBone->GetModelFileName();
			component.skeletonGuid = skelBone->GetModelFileGuid();
			component.rootNodeName = skelBone->GetRootNodeName();
			component.isRootBone = skelBone->GetIsRootBone();
			component.enabled = skelBone->GetEnabled();

			node.skeletonBoneComponents.push_back(component);
		}

		//Hinge joint
		if (dynamic_cast<HingeJoint*>(*it))
		{
			SHingeJoint component;
			HingeJoint * joint = (HingeJoint*)*it;
			component.connectedObjectName = joint->GetConnectedObjectName();
			component.anchor = toSVec3(joint->GetAnchor());
			component.connectedAnchor = toSVec3(joint->GetConnectedAnchor());
			component.axis = toSVec3(joint->GetAxis());
			component.limitMin = joint->GetLimitMin();
			component.limitMax = joint->GetLimitMax();
			component.linkedBodiesCollision = joint->GetLinkedBodiesCollision();

			component.enabled = joint->GetEnabled();
			
			node.hingeJointComponents.push_back(component);
		}
		
		//Fixed joint
		if (dynamic_cast<FixedJoint*>(*it))
		{
			SFixedJoint component;
			FixedJoint* joint = (FixedJoint*)*it;
			component.connectedObjectName = joint->GetConnectedObjectName();
			component.anchor = toSVec3(joint->GetAnchor());
			component.connectedAnchor = toSVec3(joint->GetConnectedAnchor());
			component.linkedBodiesCollision = joint->GetLinkedBodiesCollision();

			component.enabled = joint->GetEnabled();

			node.fixedJointComponents.push_back(component);
		}

		//Character joint
		if (dynamic_cast<CharacterJoint*>(*it))
		{
			SCharacterJoint component;
			CharacterJoint * joint = (CharacterJoint*)*it;
			component.connectedObjectName = joint->GetConnectedObjectName();
			component.anchor = toSVec3(joint->GetAnchor());
			component.connectedAnchor = toSVec3(joint->GetConnectedAnchor());
			component.limitMin = toSVec3(joint->GetLimitMin());
			component.limitMax = toSVec3(joint->GetLimitMax());
			component.axis = toSVec3(joint->GetAxis());
			component.linearAxis = toSVec3(joint->GetLinearAxis());
			component.linkedBodiesCollision = joint->GetLinkedBodiesCollision();
			component.enabled = joint->GetEnabled();

			node.characterJointComponents.push_back(component);
		}

		//Animation List
		if (dynamic_cast<AnimationList*>(*it))
		{
			SAnimationList component;
			AnimationList * animList = (AnimationList*)*it;
			component.fbxFileName = animList->GetFBXFileName();
			component.fbxFileGuid = animList->GetFBXFileGuid();
			component.defaultAnimation = animList->GetDefaultAnimation();
			component.playAtStart = animList->GetPlayAtStart();
			component.enabled = animList->GetEnabled();
			
			for (AnimationList::AnimationDataList::iterator ait = animList->GetAnimationDataList().begin(); ait != animList->GetAnimationDataList().end(); ++ait)
			{
				SAnimationData dt;
				dt.name = ait->name;
				dt.fileName = ait->fileName;
				dt.fileGuid = ait->fileGuid;
				dt.startFrame = ait->startFrame;
				dt.endFrame = ait->endFrame;
				dt.loop = ait->loop;
				dt.speed = ait->speed;

				component.animationData.push_back(dt);
			}

			node.animationListComponents.push_back(component);
		}

		//NavMesh Agent
		if (dynamic_cast<NavMeshAgent*>(*it))
		{
			SNavMeshAgent component;
			NavMeshAgent * navMeshAgent = (NavMeshAgent*)*it;
			component.acceleration = navMeshAgent->GetAcceleration();
			component.height = navMeshAgent->GetHeight();
			component.radius = navMeshAgent->GetRadius();
			component.speed = navMeshAgent->GetSpeed();
			component.enabled = navMeshAgent->GetEnabled();
			component.rotationSpeed = navMeshAgent->GetRotationSpeed();

			node.navMeshAgentComponents.push_back(component);
		}

		//NavMesh Obstacle
		if ((*it)->GetComponentTypeName() == NavMeshObstacle::COMPONENT_TYPE)
		{
			SNavMeshObstacle component;
			NavMeshObstacle* navMeshObstacle = (NavMeshObstacle*)*it;
			component.size = toSVec3(navMeshObstacle->getSize());
			component.offset = toSVec3(navMeshObstacle->getOffset());

			node.navMeshObstacleComponents.push_back(component);
		}

		//Vehicle
		if ((*it)->GetComponentTypeName() == Vehicle::COMPONENT_TYPE)
		{
			SVehicle component;
			Vehicle* vehicle = (Vehicle*)*it;
			
			std::vector<Vehicle::WheelInfo>& wheels = vehicle->getWheels();

			for (auto _it = wheels.begin(); _it != wheels.end(); ++_it)
			{
				SWheelInfo inf;
				inf.m_connectedObjectName = _it->m_connectedObjectName;
				inf.m_axle = toSVec3(_it->m_axle);
				inf.m_connectionPoint = toSVec3(_it->m_connectionPoint);
				inf.m_direction = toSVec3(_it->m_direction);
				inf.m_friction = _it->m_friction;
				inf.m_isFrontWheel = _it->m_isFrontWheel;
				inf.m_radius = _it->m_radius;
				inf.m_rollInfluence = _it->m_rollInfluence;
				inf.m_suspensionCompression = _it->m_suspensionCompression;
				inf.m_suspensionDamping = _it->m_suspensionDamping;
				inf.m_suspensionRestLength = _it->m_suspensionRestLength;
				inf.m_suspensionStiffness = _it->m_suspensionStiffness;
				inf.m_width = _it->m_width;

				component.wheels.push_back(inf);
			}

			component.vehicleAxis = toSVec3(vehicle->getAxis());
			component.invertForward = vehicle->getInvertForward();

			component.enabled = vehicle->GetEnabled();

			node.vehicleComponents.push_back(component);
		}
	}
}

void SceneSerializer::RestoreComponents(SceneNode* _node, SSceneNode& node, std::map<std::string, SceneNode*>& remapList)
{
	//Audio sources
	for (auto it = node.audioSourceComponents.begin(); it != node.audioSourceComponents.end(); ++it)
	{
		AudioSource* audio = new AudioSource(_node);

		std::string _audioPath = ResourceMap::getResourceNameFromGuid(it->fileGuid);
		if (_audioPath.empty())
			_audioPath = it->fileName;

		audio->SetPlayOnStart(it->playOnStart);
		audio->SetLoop(it->loop);
		audio->SetFileName(_audioPath);
		audio->SetFileGuid(it->fileGuid);
		audio->SetEnabled(it->enabled);
		audio->SetVolume(it->volume);
		audio->SetMinDistance(it->minDistance);
		audio->SetMaxDistance(it->maxDistance);
		audio->SetIs2D(it->is2D);

		_node->components.push_back(audio);
	}

	//Audio listeners
	for (auto it = node.audioListenerComponents.begin(); it != node.audioListenerComponents.end(); ++it)
	{
		AudioListener* listener = new AudioListener(_node);
		listener->SetEnabled(it->enabled);
		listener->SetVolume(it->volume);

		_node->components.push_back(listener);
	}

	//Rigidbodies
	for (auto it = node.rigidbodyComponents.begin(); it != node.rigidbodyComponents.end(); ++it)
	{
		RigidBody* rigidBody = new RigidBody(_node);

		rigidBody->SetMass(it->mass);
		rigidBody->SetFreezePositionX(it->freezePositionX);
		rigidBody->SetFreezePositionY(it->freezePositionY);
		rigidBody->SetFreezePositionZ(it->freezePositionZ);
		rigidBody->SetFreezeRotationX(it->freezeRotationX);
		rigidBody->SetFreezeRotationY(it->freezeRotationY);
		rigidBody->SetFreezeRotationZ(it->freezeRotationZ);
		rigidBody->SetIsKinematic(it->isKinematic);
		rigidBody->SetIsStatic(it->isStatic);
		rigidBody->SetUseOwnGravity(it->useOwnGravity);
		rigidBody->SetGravity(toVec3(it->gravity));
		rigidBody->SetFriction(it->friction);
		rigidBody->SetBounciness(it->bounciness);
		rigidBody->SetLinearDamping(it->linearDamping);
		rigidBody->SetAngularDamping(it->angularDamping);
		rigidBody->SetEnabled(it->enabled);

		_node->components.push_back(rigidBody);
	}

	//Mesh colliders
	for (auto it = node.meshColliderComponents.begin(); it != node.meshColliderComponents.end(); ++it)
	{
		MeshCollider* meshCollider = new MeshCollider(_node);
		meshCollider->SetConvex(it->convex);
		meshCollider->SetIsTrigger(it->isTrigger);
		meshCollider->SetEnabled(it->enabled);

		_node->components.push_back(meshCollider);
	}

	//Capsule colliders
	for (auto it = node.capsuleColliderComponents.begin(); it != node.capsuleColliderComponents.end(); ++it)
	{
		CapsuleCollider* capsuleCollider = new CapsuleCollider(_node);
		capsuleCollider->SetRadius(it->radius);
		capsuleCollider->SetHeight(it->height);
		capsuleCollider->SetOffset(Vector3(it->offset.x, it->offset.y, it->offset.z));
		capsuleCollider->SetRotation(Quaternion(it->rotation.w, it->rotation.x, it->rotation.y, it->rotation.z));
		capsuleCollider->SetIsTrigger(it->isTrigger);
		capsuleCollider->SetEnabled(it->enabled);

		_node->components.push_back(capsuleCollider);
	}

	//Box colliders
	for (auto it = node.boxColliderComponents.begin(); it != node.boxColliderComponents.end(); ++it)
	{
		BoxCollider* boxCollider = new BoxCollider(_node);
		boxCollider->SetBoxSize(Vector3(it->boxSize.x, it->boxSize.y, it->boxSize.z));
		boxCollider->SetOffset(Vector3(it->offset.x, it->offset.y, it->offset.z));
		boxCollider->SetRotation(Quaternion(it->rotation.w, it->rotation.x, it->rotation.y, it->rotation.z));
		boxCollider->SetIsTrigger(it->isTrigger);
		boxCollider->SetEnabled(it->enabled);

		_node->components.push_back(boxCollider);
	}

	//Sphere colliders
	for (auto it = node.sphereColliderComponents.begin(); it != node.sphereColliderComponents.end(); ++it)
	{
		SphereCollider* sphereCollider = new SphereCollider(_node);
		sphereCollider->SetRadius(it->radius);
		sphereCollider->SetOffset(Vector3(it->offset.x, it->offset.y, it->offset.z));
		sphereCollider->SetIsTrigger(it->isTrigger);
		sphereCollider->SetEnabled(it->enabled);

		_node->components.push_back(sphereCollider);
	}

	//Skeleton bones
	for (auto it = node.skeletonBoneComponents.begin(); it != node.skeletonBoneComponents.end(); ++it)
	{
		std::string rootNodeName = remapList[it->rootNodeName] != nullptr ? remapList[it->rootNodeName]->getName() : it->rootNodeName;

		std::string _skeletonPath = ResourceMap::getResourceNameFromGuid(it->skeletonGuid);
		if (_skeletonPath.empty())
			_skeletonPath = it->skeletonName;

		SkeletonBone* skelBone = new SkeletonBone(_node);
		skelBone->SetIndex(it->index);
		skelBone->SetName(it->name);
		skelBone->SetBindingPosition(Vector3(it->bindingPosition.x, it->bindingPosition.y, it->bindingPosition.z));
		skelBone->SetBindingRotation(Quaternion(it->bindingRotation.w, it->bindingRotation.x, it->bindingRotation.y, it->bindingRotation.z));
		skelBone->SetBindingScale(Vector3(it->bindingScale.x, it->bindingScale.y, it->bindingScale.z));
		skelBone->SetPosition(Vector3(it->position.x, it->position.y, it->position.z));
		skelBone->SetRotation(Quaternion(it->rotation.w, it->rotation.x, it->rotation.y, it->rotation.z));
		skelBone->SetScale(Vector3(it->scale.x, it->scale.y, it->scale.z));
		skelBone->SetModelFileName(_skeletonPath);
		skelBone->SetRootNodeName(rootNodeName);
		skelBone->SetEnabled(it->enabled);
		skelBone->SetIsRootBone(it->isRootBone);

		_node->components.push_back(skelBone);
	}

	//Hinge joints
	for (auto it = node.hingeJointComponents.begin(); it != node.hingeJointComponents.end(); ++it)
	{
		std::string connectedBody = remapList[it->connectedObjectName] != nullptr ? remapList[it->connectedObjectName]->getName() : it->connectedObjectName;

		HingeJoint* joint = new HingeJoint(_node);
		joint->SetConnectedObjectName(connectedBody);
		joint->SetAnchor(toVec3(it->anchor));
		joint->SetConnectedAnchor(toVec3(it->connectedAnchor));
		joint->SetAxis(toVec3(it->axis));
		joint->SetLimitMin(it->limitMin);
		joint->SetLimitMax(it->limitMax);
		joint->SetLinkedBodiesCollision(it->linkedBodiesCollision);
		joint->SetEnabled(it->enabled);

		_node->components.push_back(joint);
	}

	//Fixed joints
	for (auto it = node.fixedJointComponents.begin(); it != node.fixedJointComponents.end(); ++it)
	{
		std::string connectedBody = remapList[it->connectedObjectName] != nullptr ? remapList[it->connectedObjectName]->getName() : it->connectedObjectName;

		FixedJoint* joint = new FixedJoint(_node);
		joint->SetConnectedObjectName(connectedBody);
		joint->SetAnchor(toVec3(it->anchor));
		joint->SetConnectedAnchor(toVec3(it->connectedAnchor));
		joint->SetLinkedBodiesCollision(it->linkedBodiesCollision);
		joint->SetEnabled(it->enabled);

		_node->components.push_back(joint);
	}

	//Character joints
	for (auto it = node.characterJointComponents.begin(); it != node.characterJointComponents.end(); ++it)
	{
		std::string connectedBody = remapList[it->connectedObjectName] != nullptr ? remapList[it->connectedObjectName]->getName() : it->connectedObjectName;

		CharacterJoint* joint = new CharacterJoint(_node);
		joint->SetConnectedObjectName(connectedBody);
		joint->SetAnchor(toVec3(it->anchor));
		joint->SetConnectedAnchor(toVec3(it->connectedAnchor));
		joint->SetLimitMin(toVec3(it->limitMin));
		joint->SetLimitMax(toVec3(it->limitMax));
		joint->SetAxis(toVec3(it->axis));
		joint->SetLinearAxis(toVec3(it->linearAxis));
		joint->SetLinkedBodiesCollision(it->linkedBodiesCollision);
		joint->SetEnabled(it->enabled);

		_node->components.push_back(joint);
	}

	//Animation lists
	for (auto it = node.animationListComponents.begin(); it != node.animationListComponents.end(); ++it)
	{
		std::string _fbxPath = ResourceMap::getResourceNameFromGuid(it->fbxFileGuid);
		if (_fbxPath.empty())
			_fbxPath = it->fbxFileName;

		AnimationList* animList = new AnimationList(_node);
		animList->SetFBXFileName(_fbxPath);
		animList->SetDefaultAnimation(it->defaultAnimation);
		animList->SetPlayAtStart(it->playAtStart);
		animList->SetEnabled(it->enabled);

		for (std::vector<SAnimationData>::iterator ait = it->animationData.begin(); ait != it->animationData.end(); ++ait)
		{
			_fbxPath = ResourceMap::getResourceNameFromGuid(ait->fileGuid);
			if (_fbxPath.empty())
				_fbxPath = ait->fileName;

			animList->AddAnimationData(ait->name, _fbxPath, ait->startFrame, ait->endFrame, ait->loop, ait->speed);
		}

		_node->components.push_back(animList);
	}

	//NavMesh agents
	for (auto it = node.navMeshAgentComponents.begin(); it != node.navMeshAgentComponents.end(); ++it)
	{
		NavMeshAgent* navMeshAgent = new NavMeshAgent(_node);
		navMeshAgent->SetAcceleration(it->acceleration);
		navMeshAgent->SetHeight(it->height);
		navMeshAgent->SetRadius(it->radius);
		navMeshAgent->SetSpeed(it->speed);
		navMeshAgent->SetEnabled(it->enabled);
		navMeshAgent->SetRotationSpeed(it->rotationSpeed);

		_node->components.push_back(navMeshAgent);
	}

	//NavMesh obstacles
	for (auto it = node.navMeshObstacleComponents.begin(); it != node.navMeshObstacleComponents.end(); ++it)
	{
		NavMeshObstacle* navMeshObstacle = new NavMeshObstacle(_node);
		navMeshObstacle->setSize(toVec3(it->size));
		navMeshObstacle->setOffset(toVec3(it->offset));

		_node->components.push_back(navMeshObstacle);
	}

	//Vehicle
	for (auto it = node.vehicleComponents.begin(); it != node.vehicleComponents.end(); ++it)
	{
		//std::string connectedBody = remapList[it->connectedObjectName] != nullptr ? remapList[it->connectedObjectName]->getName() : it->connectedObjectName;

		Vehicle* vehicle = new Vehicle(_node);

		std::vector<Vehicle::WheelInfo>& wheels = vehicle->getWheels();

		for (auto _it = it->wheels.begin(); _it != it->wheels.end(); ++_it)
		{
			Vehicle::WheelInfo inf;
			inf.m_connectedObjectName = _it->m_connectedObjectName;
			inf.m_axle = toVec3(_it->m_axle);
			inf.m_connectionPoint = toVec3(_it->m_connectionPoint);
			inf.m_direction = toVec3(_it->m_direction);
			inf.m_friction = _it->m_friction;
			inf.m_isFrontWheel = _it->m_isFrontWheel;
			inf.m_radius = _it->m_radius;
			inf.m_rollInfluence = _it->m_rollInfluence;
			inf.m_suspensionCompression = _it->m_suspensionCompression;
			inf.m_suspensionDamping = _it->m_suspensionDamping;
			inf.m_suspensionRestLength = _it->m_suspensionRestLength;
			inf.m_suspensionStiffness = _it->m_suspensionStiffness;
			inf.m_width = _it->m_width;

			wheels.push_back(inf);
		}
		
		vehicle->setAxis(toVec3(it->vehicleAxis));
		vehicle->setInvertForward(it->invertForward);

		vehicle->SetEnabled(it->enabled);

		_node->components.push_back(vehicle);
	}
}

void SceneSerializer::SaveSceneData(SceneManager * manager, SScene & scene)
{
	//Lighting settings
	scene.ambientColor = SColor(manager->getAmbientLight().r, manager->getAmbientLight().g, manager->getAmbientLight().b, manager->getAmbientLight().a);
	if (manager->getSkyBoxMaterial() != NULL)
	{
		scene.skyboxMaterialName = manager->getSkyBoxMaterial()->getName();
		scene.skyboxMaterialGuid = ResourceMap::guidMap[manager->getSkyBoxMaterial().getPointer()];
	}

	scene.lightmapSettings = SLightmapSettings();
	scene.lightmapSettings.defaultLightmapSize = GetEngine->GetDefaultLightmapSize();
	scene.lightmapSettings.lightmapBlurRadius = GetEngine->GetLightmapBlurRadius();
	scene.lightmapSettings.regionLightmapSize = GetEngine->GetRegionLightmapSize();

	//Save navigation data
	NavigationManager * navMgr = GetEngine->GetNavigationManager();

	SNavMeshSettings navMeshSettings = SNavMeshSettings();
	navMeshSettings.walkableSlopeAngle = navMgr->GetWalkableSlopeAngle();
	navMeshSettings.walkableRadius = navMgr->GetWalkableRadius();
	navMeshSettings.walkableHeight = navMgr->GetWalkableHeight();
	navMeshSettings.walkableClimb = navMgr->GetWalkableClimb();
	navMeshSettings.minRegionArea = navMgr->GetMinRegionArea();
	navMeshSettings.mergeRegionArea = navMgr->GetMergeRegionArea();
	navMeshSettings.maxSimplificationError = navMgr->GetMaxSimplificationError();
	navMeshSettings.maxEdgeLen = navMgr->GetMaxEdgeLen();
	navMeshSettings.cellSize = navMgr->GetCellSize();
	navMeshSettings.cellHeight = navMgr->GetCellHeight();

	scene.navMeshSettings = navMeshSettings;

	//Save terrain
	TerrainManager * terrMgr = GetEngine->GetTerrainManager();
	for (std::map<Terrain*, std::string>::iterator it = terrMgr->GetTerrainList().begin(); it != terrMgr->GetTerrainList().end(); ++it)
	{
		STerrain terr;
		terr.fileName = it->second;
		terr.fileGuid = ResourceMap::getResourceGuidFromName(terr.fileName);
		//terr.position = toSVec3(it->first->getPosition());
		//terr.size = it->first->getSize();
		//terr.worldSize = it->first->getWorldSize();
		scene.terrains.push_back(terr);
	}

	//Save detail textures
	std::vector<TerrainManager::TextureData> textures = GetEngine->GetTerrainManager()->GetTextures();
	for (std::vector<TerrainManager::TextureData>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		STextureData dmbData;
		dmbData.diffusePath = it->diffusePath;
		dmbData.diffuseGuid = it->diffuseGuid;
		dmbData.normalPath = it->normalPath;
		dmbData.normalGuid = it->normalGuid;
		dmbData.worldSize = it->worldSize;

		scene.terrainTextures.push_back(dmbData);
	}

	//Save detail mesh brushes
	std::vector<TerrainManager::DetailMeshData> detailBrushes = GetEngine->GetTerrainManager()->GetDetailMeshes();
	for (std::vector<TerrainManager::DetailMeshData>::iterator it = detailBrushes.begin(); it != detailBrushes.end(); ++it)
	{
		SDetailMeshDataBrush dmbData;
		dmbData.meshPath = it->path;
		dmbData.meshGuid = it->guid;
		dmbData.scaleMin = it->minScale;
		dmbData.scaleMax = it->maxScale;

		scene.detailMeshDataBrushes.push_back(dmbData);
	}

	//Save grass brushes
	std::vector<TerrainManager::GrassData> grassBrushes = GetEngine->GetTerrainManager()->GetGrass();
	int gi = 0;
	for (std::vector<TerrainManager::GrassData>::iterator it = grassBrushes.begin(); it != grassBrushes.end(); ++it)
	{
		SGrassDataBrush gbData;
		gbData.materialPath = it->path;
		gbData.materialGuid = it->guid;
		gbData.minSize = SVector2(it->minSize.x, it->minSize.y);
		gbData.maxSize = SVector2(it->maxSize.x, it->maxSize.y);
		gbData.animation = it->animation;
		gbData.swayDistribution = it->swayDistribution;
		gbData.swayLength = it->swayLength;
		gbData.swaySpeed = it->swaySpeed;
		gbData.density = it->density;

		GrassLayer* layer = GetEngine->GetTerrainManager()->GetGrassLayer(gi);
		Terrain * terr = terrMgr->GetTerrainGroup()->getTerrain(0, 0);
		for (int i = 0; i < terr->getSize(); ++i)
		{
			for (int j = 0; j < terr->getSize(); ++j)
			{
				gbData.densityMap[i][j] = layer->getDensityMap()->getDensityAtRaw(i, j, FloatRect(0, 0, terr->getSize(), terr->getSize()));
			}
		}

		scene.grassDataBrushes.push_back(gbData);
		++gi;
	}

	//Save detail meshes
	TreeLoader2D * treeLoader = GetEngine->GetTerrainManager()->GetTreeLoader();
	if (treeLoader != nullptr)
	{
		TreeIterator2D tit = treeLoader->getTrees();

		while (tit.hasMoreElements())
		{
			TreeRef tree = tit.getNext();
			TerrainManager::DetailMeshUserData* dmuData = static_cast<TerrainManager::DetailMeshUserData*>(tree.getUserData());

			SDetailMeshData dmData;
			dmData.position = SVector3(tree.getPosition().x, tree.getPosition().y, tree.getPosition().z);
			dmData.scale = tree.getScale();
			dmData.yaw = tree.getYaw().valueDegrees();
			if (dmuData != nullptr)
			{
				dmData.meshPath = dmuData->path;
				dmData.meshGuid = dmuData->guid;
			}

			scene.detailMeshesData.push_back(dmData);
		}
	}

	//Terrain settings
	scene.detailMeshesDrawDistance = terrMgr->GetDetailMeshesDrawDistance();
	scene.grassDrawDistance = terrMgr->GetGrassDrawDistance();
}

void SceneSerializer::RestoreSceneData(SceneManager * manager, SScene & scene)
{
	//Load lighting settings
	manager->setAmbientLight(ColourValue(scene.ambientColor.r, scene.ambientColor.g, scene.ambientColor.b, scene.ambientColor.a));
	if (!scene.skyboxMaterialName.empty())
	{
		std::string _skyMat = ResourceMap::getResourceNameFromGuid(scene.skyboxMaterialGuid);
		if (_skyMat.empty())
			_skyMat = scene.skyboxMaterialName;

		if (MaterialManager::getSingleton().resourceExists(_skyMat, "Assets"))
			manager->setSkyBox(true, _skyMat, 1300, true, Quaternion::IDENTITY, "Assets");
	}

	GetEngine->SetDefaultLightmapSize(scene.lightmapSettings.defaultLightmapSize);
	GetEngine->SetRegionLightmapSize(scene.lightmapSettings.regionLightmapSize);
	GetEngine->SetLightmapBlurRadius(scene.lightmapSettings.lightmapBlurRadius);

	//Load navigation data
	NavigationManager * navMgr = GetEngine->GetNavigationManager();

	SNavMeshSettings & navMeshSettings = scene.navMeshSettings;
	navMgr->SetWalkableSlopeAngle(navMeshSettings.walkableSlopeAngle);
	navMgr->SetWalkableRadius(navMeshSettings.walkableRadius);
	navMgr->SetWalkableHeight(navMeshSettings.walkableHeight);
	navMgr->SetWalkableClimb(navMeshSettings.walkableClimb);
	navMgr->SetMinRegionArea(navMeshSettings.minRegionArea);
	navMgr->SetMergeRegionArea(navMeshSettings.mergeRegionArea);
	navMgr->SetMaxSimplificationError(navMeshSettings.maxSimplificationError);
	navMgr->SetMaxEdgeLen(navMeshSettings.maxEdgeLen);
	navMgr->SetCellSize(navMeshSettings.cellSize);
	navMgr->SetCellHeight(navMeshSettings.cellHeight);

	//Load terrain
	TerrainManager * terrMgr = GetEngine->GetTerrainManager();
	for (std::vector<STerrain>::iterator it = scene.terrains.begin(); it != scene.terrains.end(); ++it)
	{
		std::string _terrPath = ResourceMap::getResourceNameFromGuid(it->fileGuid);
		if (_terrPath.empty())
			_terrPath = it->fileName;

		terrMgr->CreateTerrain(it->x, it->y, GetEngine->GetAssetsPath() + _terrPath);
		//terrMgr->GetTerrainGroup()->getTerrain(it->x, it->y)->setPosition(toVec3(it->position));
		//terrMgr->GetTerrainGroup()->getTerrain(it->x, it->y)->setSize(it->size);
		//terrMgr->GetTerrainGroup()->getTerrain(it->x, it->y)->setWorldSize(it->worldSize);
	}

	TerrainGroup* terrGroup = terrMgr->GetTerrainGroup();
	Terrain* terrain = terrGroup->getTerrain(0, 0);

	//Load terrain textures
	for (std::vector<STextureData>::iterator it = scene.terrainTextures.begin(); it != scene.terrainTextures.end(); ++it)
	{
		std::string _diffusePath = ResourceMap::getResourceNameFromGuid(it->diffuseGuid);
		if (_diffusePath.empty())
			_diffusePath = it->diffusePath;

		std::string _normalPath = ResourceMap::getResourceNameFromGuid(it->normalGuid);
		if (_normalPath.empty())
			_normalPath = it->normalPath;

		TerrainManager::TextureData dt;
		dt.diffusePath = _diffusePath;
		dt.normalPath = _normalPath;
		dt.diffuseGuid = it->diffuseGuid;
		dt.normalGuid = it->normalGuid;
		dt.worldSize = it->worldSize;

		terrMgr->AddTexture(dt);

		StringVector lst;
		lst.resize(2);
		if (GetEngine->GetUseUnpackedResources())
		{
			if (!_diffusePath.empty() && _diffusePath != "None" && IO::FileExists(GetEngine->GetAssetsPath() + _diffusePath))
				lst[0] = _diffusePath;
			else
				lst[0] = "Terrain/default_diffuse.jpg";

			if (!_normalPath.empty() && _normalPath != "None" && IO::FileExists(GetEngine->GetAssetsPath() + _normalPath))
				lst[1] = _normalPath;
			else
				lst[1] = "Terrain/default_normal.jpg";
		}
		else
		{
			if (!_diffusePath.empty() && _diffusePath != "None" && ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), _diffusePath))
				lst[0] = _diffusePath;
			else
				lst[0] = "Terrain/default_diffuse.jpg";

			if (!_normalPath.empty() && _normalPath != "None" && ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), _normalPath))
				lst[1] = _normalPath;
			else
				lst[1] = "Terrain/default_normal.jpg";
		}

		//terrain->addLayer(dt.worldSize, &lst);
		int i = distance(scene.terrainTextures.begin(), it);
		terrain->replaceLayer(i, true, dt.worldSize, &lst);
		terrain->load();
	}

	//Load detail mesh brushes
	for (std::vector<SDetailMeshDataBrush>::iterator it = scene.detailMeshDataBrushes.begin(); it != scene.detailMeshDataBrushes.end(); ++it)
	{
		std::string _meshPath = ResourceMap::getResourceNameFromGuid(it->meshGuid);
		if (_meshPath.empty())
			_meshPath = it->meshPath;

		TerrainManager::DetailMeshData dt;
		dt.path = _meshPath;
		dt.guid = it->meshGuid;
		dt.minScale = it->scaleMin;
		dt.maxScale = it->scaleMax;

		terrMgr->AddDetailMesh(dt);
	}

	//Load grass brushes
	int gi = 0;
	for (std::vector<SGrassDataBrush>::iterator it = scene.grassDataBrushes.begin(); it != scene.grassDataBrushes.end(); ++it)
	{
		std::string _matPath = ResourceMap::getResourceNameFromGuid(it->materialGuid);
		if (_matPath.empty())
			_matPath = it->materialPath;

		TerrainManager::GrassData dt;
		dt.path = _matPath;
		dt.guid = it->materialGuid;
		dt.minSize = Vector2(it->minSize.x, it->minSize.y);
		dt.maxSize = Vector2(it->maxSize.x, it->maxSize.y);
		dt.animation = it->animation;
		dt.swayDistribution = it->swayDistribution;
		dt.swayLength = it->swayLength;
		dt.swaySpeed = it->swaySpeed;
		dt.density = it->density;

		terrMgr->AddGrass(dt);

		GrassLayer* layer = GetEngine->GetTerrainManager()->GetGrassLayer(gi);
		Terrain* terr = terrMgr->GetTerrainGroup()->getTerrain(0, 0);
		for (int i = 0; i < terr->getSize(); ++i)
		{
			for (int j = 0; j < terr->getSize(); ++j)
			{
				layer->getDensityMap()->setDensityAtRaw(i, j, FloatRect(0, 0, terr->getSize(), terr->getSize()), it->densityMap[i][j]);
			}
		}

		++gi;
	}

	terrMgr->GetGrassPagedGeometry()->reloadGeometry();

	//Load detail meshes
	for (std::vector<SDetailMeshData>::iterator it = scene.detailMeshesData.begin(); it != scene.detailMeshesData.end(); ++it)
	{
		std::string _meshPath = ResourceMap::getResourceNameFromGuid(it->meshGuid);
		if (_meshPath.empty())
			_meshPath = it->meshPath;

		Vector3 pos = Vector3(it->position.x, it->position.y, it->position.z);
		terrMgr->PlaceDetailMesh(_meshPath, pos, it->scale, Degree(it->yaw));
	}

	//Terrain settings
	terrMgr->SetDetailMeshesDrawDistance(scene.detailMeshesDrawDistance);
	terrMgr->SetGrassDrawDistance(scene.grassDrawDistance);
}

void SceneSerializer::LoadAnimationAndSkinningData(SceneNode* node)
{
	AnimationList* list = (AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE);
	SkeletonBone* bone = (SkeletonBone*)node->GetComponent(SkeletonBone::COMPONENT_TYPE);

	if (list != nullptr)
	{
		if (!node->skeletonLoaded)
		{
			//RestoreAnimations(list);
			string _fbxPath = ResourceMap::getResourceNameFromGuid(list->GetFBXFileGuid());
			if (_fbxPath.empty())
				_fbxPath = list->GetFBXFileName();

			FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(_fbxPath, "Assets");

			if (fbx != nullptr)
			{
				fbx->PostLoad(node);
				Entity* _ent = nullptr;
				SkeletonPtr skeleton = SkeletonManager::getSingleton().getByName(fbx->getName() + "_skeleton", "Assets");
				fbx->SetSkeleton(node, skeleton, _ent);

				if (GetEngine->GetEnvironment() == Engine::Environment::Player)
				{
					if (list->GetPlayAtStart())
					{
						if (list->GetAnimationDataList().size() > list->GetDefaultAnimation())
						{
							Engine::AnimationPlay(node, list->GetAnimationDataAt(list->GetDefaultAnimation()).name, true);
						}
					}
				}
			}
		}
	}
	else if (bone != nullptr)
	{
		SceneNode* boneRootNode = GetEngine->GetSceneManager()->getSceneNodeFast(bone->GetRootNodeName());
		if (boneRootNode != nullptr)
		{
			if (boneRootNode->GetComponent(AnimationList::COMPONENT_TYPE) == nullptr)
			{
				if (boneRootNode->skinned)
				{
					if (!boneRootNode->skeletonLoaded)
					{
						string _fbxPath = ResourceMap::getResourceNameFromGuid(bone->GetModelFileGuid());
						if (_fbxPath.empty())
							_fbxPath = bone->GetModelFileName();

						FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(_fbxPath, "Assets");

						if (fbx != nullptr)
						{
							fbx->PostLoad(boneRootNode);
							Entity* _ent = nullptr;
							SkeletonPtr skeleton = SkeletonManager::getSingleton().getByName(fbx->getName() + "_skeleton", "Assets");
							fbx->SetSkeleton(boneRootNode, skeleton, _ent);
						}
					}
				}
			}
		}
	}
}

void SceneSerializer::RestoreAnimations(AnimationList * animList, std::string animName)
{
	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		return;

	SceneNode * root = animList->GetParentSceneNode();
	RestoreAnimationsRecursive(animList, root, animName);

	FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(animList->GetFBXFileName(), "Assets");
	if (fbx != nullptr)
	{
		//fbx->PostLoad(root);

		if (animName.empty())
		{
			std::vector<FBXScenePtr> importedAnims;

			for (AnimationList::AnimationDataList::iterator it = animList->GetAnimationDataList().begin(); it != animList->GetAnimationDataList().end(); ++it)
			{
				FBXScenePtr fbxAnim = FBXSceneManager::getSingleton().getByName(it->fileName, "Assets");

				if (find(importedAnims.begin(), importedAnims.end(), fbxAnim) == importedAnims.end())
				{
					SkeletonPtr skeleton = SkeletonManager::getSingleton().getByName(fbx->getName() + "_skeleton", "Assets");

					if (skeleton != nullptr)
					{
						//fbxAnim->PostLoad(root, skeleton);
						fbxAnim->ImportKeyFrames(animList, root, skeleton);
						Entity * _ent = nullptr;
						fbx->SetSkeleton(root, skeleton, _ent);

						importedAnims.push_back(fbxAnim);
					}

					fbxAnim->ClearAnimationData();
				}
			}

			importedAnims.clear();
		}
		else
		{
			AnimationList::AnimationDataList::iterator it = find_if(animList->GetAnimationDataList().begin(), animList->GetAnimationDataList().end(), [animName](const AnimationList::AnimationData & lst) -> bool {
				return lst.name == animName;
			});

			if (it != animList->GetAnimationDataList().end())
			{
				FBXScenePtr fbxAnim = FBXSceneManager::getSingleton().getByName(it->fileName, "Assets");

				SkeletonPtr skeleton = SkeletonManager::getSingleton().getByName(fbx->getName() + "_skeleton", "Assets");

				if (skeleton != nullptr)
				{
					fbxAnim->ImportKeyFrames(animList, root, skeleton);
					Entity* _ent = nullptr;
					fbx->SetSkeleton(root, skeleton, _ent);
				}

				fbxAnim->ClearAnimationData();
			}
		}

		fbx->ClearAnimationData();
	}
}

void SceneSerializer::RestoreAnimationsRecursive(AnimationList* animList, SceneNode* root, std::string animName)
{
	std::vector<std::pair<SceneNode*, SceneNode*>> nstack;
	auto children = root->getChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
		nstack.push_back(make_pair((SceneNode*)*it, root));

	while (nstack.size() > 0)
	{
		SceneNode* node = nstack.begin()->first;
		SceneNode* curRoot = nstack.begin()->second;
		nstack.erase(nstack.begin());

		////////////////////
		bool valid = false;
		if (node->fbxFromFile == curRoot->fbxFromFile && node->fbxFromGuid == curRoot->fbxFromGuid)
		{
			valid = true;
		}

		if (!valid)
			continue;

		if (node->fbxCurvesFrom > -1)
		{
			if (animName.empty())
			{
				for (AnimationList::AnimationDataList::iterator it = animList->GetAnimationDataList().begin(); it != animList->GetAnimationDataList().end(); ++it)
				{
					FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(it->fileName, "Assets");

					if (fbx != nullptr)
					{
						fbx->LoadCurves(node, node->fbxCurvesFrom, animList, *it);
					}
				}
			}
			else
			{
				AnimationList::AnimationDataList::iterator it = find_if(animList->GetAnimationDataList().begin(), animList->GetAnimationDataList().end(), [animName](const AnimationList::AnimationData& lst) -> bool {
					return lst.name == animName;
					});

				if (it != animList->GetAnimationDataList().end())
				{
					FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(it->fileName, "Assets");

					if (fbx != nullptr)
					{
						fbx->LoadCurves(node, node->fbxCurvesFrom, animList, *it);
					}
				}
			}
		}

		if (node->fbxSkeletonFrom > -1)
		{
			if (animName.empty())
			{
				for (AnimationList::AnimationDataList::iterator it = animList->GetAnimationDataList().begin(); it != animList->GetAnimationDataList().end(); ++it)
				{
					FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(it->fileName, "Assets");

					if (fbx != nullptr)
					{
						fbx->LoadSkeletal(node, node->fbxSkeletonFrom, animList, *it);
					}
				}
			}
			else
			{
				AnimationList::AnimationDataList::iterator it = find_if(animList->GetAnimationDataList().begin(), animList->GetAnimationDataList().end(), [animName](const AnimationList::AnimationData& lst) -> bool {
					return lst.name == animName;
				});

				if (it != animList->GetAnimationDataList().end())
				{
					FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(it->fileName, "Assets");

					if (fbx != nullptr)
					{
						fbx->LoadSkeletal(node, node->fbxSkeletonFrom, animList, *it);
					}
				}
			}
		}

		////////////////////
		int j = 0;
		auto children = node->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			nstack.insert(nstack.begin() + j, make_pair(nd, node));
		}
	}
}

std::string SceneSerializer::GenName(std::string srcName, SceneManager* manager)
{
	std::string nm = srcName;

	while (manager->hasSceneNode(nm))
	{
		nm = srcName;
		nm = nm + "_" + GetEngine->GenerateNewName(manager);
	}

	return nm;
}

void SceneSerializer::GenIndexedPointers(AnimationList * animList, SceneNode * root)
{
	std::vector<SceneNode*> nstack;
	nstack.push_back(root);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////

		animList->indexedPointers[curNode->getName()] = curNode;

		////////////////////

		int j = 0;
		auto children = curNode->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			nstack.insert(nstack.begin() + j, nd);
		}
	}
}

void SceneSerializer::ClearPrefabLinks(SceneNode* root)
{
	std::vector<SceneNode*> nstack;
	auto children = root->getChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
		nstack.push_back((SceneNode*)*it);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////

		curNode->prefabName = "";
		curNode->prefabGuid = "";

		////////////////////

		int j = 0;
		auto children = curNode->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			nstack.insert(nstack.begin() + j, nd);
		}
	}

	//VectorIterator it = root->getChildIterator();

	/*while (it.hasMoreElements())
	{
		SceneNode* node = (SceneNode*)it.getNext();

		node->prefabName = "";
		node->prefabGuid = "";

		ClearPrefabLinks(node);
	}*/
}

void SceneSerializer::GetAllSceneNodes(SceneNode* root, std::vector<SceneNode*> & outList)
{
	std::vector<SceneNode*> nstack;
	auto children = root->getChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
		nstack.push_back((SceneNode*)*it);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////

		outList.push_back(curNode);

		////////////////////

		int j = 0;
		auto children = curNode->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			nstack.insert(nstack.begin() + j, nd);
		}
	}

	/*VectorIterator it = root->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode* node = (SceneNode*)it.getNext();
		outList.push_back(node);

		GetAllSceneNodes(node, outList);
	}*/
}
