#include "stdafx.h"
#include "CloneTools.h"
#include "AudioSource.h"
#include "AudioListener.h"
#include "RigidBody.h"
#include "MeshCollider.h"
#include "SkeletonBone.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "AnimationList.h"
#include "HingeJoint.h"
#include "FixedJoint.h"
#include "CharacterJoint.h"
#include "NavMeshAgent.h"
#include "NavMeshObstacle.h"
#include "Vehicle.h"
#include "UIButton.h"
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreParticleSystem.h>
#include <OgreParticleSystemManager.h>
#include <OgreCamera.h>
#include <OgreParticleEmitter.h>
#include <OgreParticleAffector.h>
#include <OgreParticleSystemRenderer.h>
#include <OgreSubEntity.h>
#include "StringConverter.h"

std::map<std::string, std::string> CloneTools::remapList;

CloneTools::CloneTools()
{
}

CloneTools::~CloneTools()
{
}

SceneNode * CloneTools::CloneSceneNode(SceneNode * node, SceneNode * cloneTo)
{
	remapList.clear();

	if (node->getAttachedObjects().size() > 0)
	{
		MovableObject * attachedObject = node->getAttachedObject(0);
		if (attachedObject->getMovableType() == UICanvasFactory::FACTORY_TYPE_NAME)
		{
			return nullptr;
		}
	}

	SceneNode * newNode = CloneSceneNodeRecursive(node, cloneTo);
	newNode->setVisible(node->getVisible());
	newNode->setGlobalVisible(node->getGlobalVisible());

	ReassignParamsRecursive(newNode);

	GetEngine->UpdateSceneNodeIndexes();

	return newNode;
}

SceneNode * CloneTools::CloneSceneNodeRecursive(SceneNode * node, SceneNode * cloneTo)
{
	if (node == nullptr)
		return nullptr;

	//Clone the node itself
	SceneNode* newNode = nullptr;
	SceneNode* ret = nullptr;

	std::vector<std::pair<SceneNode*, SceneNode*>> nstack;
	nstack.push_back(make_pair(node, cloneTo));

	while (nstack.size() > 0)
	{
		SceneNode* curNode = nstack.begin()->first;
		SceneNode* curParent = nstack.begin()->second;
		nstack.erase(nstack.begin());

		////////////////////

		std::string newName = GetEngine->GenerateNewName(GetEngine->GetSceneManager(), "Clone_");
		remapList[curNode->getName()] = newName;

		if (curParent == NULL)
			newNode = curNode->getParentSceneNode()->createChildSceneNode(newName, curNode->getPosition(), curNode->getOrientation());
		else
			newNode = curParent->createChildSceneNode(newName, curNode->getPosition(), curNode->getOrientation());

		if (ret == nullptr)
			ret = newNode;

		newNode->setAlias(curNode->getAlias());
		newNode->setScale(curNode->getScale());
		newNode->fbxSkeletonFrom = curNode->fbxSkeletonFrom;
		newNode->fbxCurvesFrom = curNode->fbxCurvesFrom;
		newNode->fbxSkeletonFromFile = curNode->fbxSkeletonFromFile;
		newNode->fbxCurvesFromFile = curNode->fbxCurvesFromFile;
		newNode->skinned = curNode->skinned;
		newNode->setGlobalVisible(curNode->getGlobalVisible());
		newNode->prefabGuid = curNode->prefabGuid;
		newNode->prefabName = curNode->prefabName;
		newNode->lightmapStatic = curNode->lightmapStatic;
		newNode->navigationStatic = curNode->navigationStatic;

		MovableObject* attachedObject = NULL;

		if (curNode->getAttachedObjects().size() > 0)
		{
			attachedObject = curNode->getAttachedObject(0);

			if (dynamic_cast<Entity*>(attachedObject))
			{
				Entity* newEntity = ((Entity*)attachedObject)->clone(newName);

				for (int i = 0; i < newEntity->getNumSubEntities(); ++i)
				{
					SubEntity* sub = newEntity->getSubEntity(i);

					MaterialPtr mat = ((Entity*)attachedObject)->getSubEntity(i)->getMaterial();
					MaterialPtr origMat = ((Entity*)attachedObject)->getSubEntity(i)->getOriginalMaterial();

					if (origMat != nullptr)
					{
						sub->setMaterial(MaterialPtr());
						sub->setOriginalMaterial(origMat);
					}
					else
					{
						sub->setMaterial(mat);
						sub->setOriginalMaterial(origMat);
					}
				}

				newEntity->setQueryFlags(1 << 1);
				newEntity->detachFromParent();
				newNode->attachObject(newEntity);
			}

			if (dynamic_cast<Light*>(attachedObject))
			{
				Light* newLight = GetEngine->GetSceneManager()->createLight(newName);

				newLight->setDiffuseColour(((Light*)attachedObject)->getDiffuseColour());
				newLight->setType(((Light*)attachedObject)->getType());
				newLight->setAttenuation(((Light*)attachedObject)->getAttenuationRange(), ((Light*)attachedObject)->getAttenuationConstant(), ((Light*)attachedObject)->getAttenuationLinear(), ((Light*)attachedObject)->getAttenuationQuadric());
				newLight->setSpecularColour(((Light*)attachedObject)->getSpecularColour());
				newLight->setPowerScale(((Light*)attachedObject)->getPowerScale());
				newLight->setShadowBias(((Light*)attachedObject)->getShadowBias());
				newLight->setCastShadows(((Light*)attachedObject)->getCastShadows());
				newLight->setCustomParameter(1, ((Light*)attachedObject)->getCustomParameter(1));

				if (newLight->getCustomParameter(1).x == 0)
					newLight->setType(Light::LightTypes::LT_POINT);
				if (newLight->getCustomParameter(1).x == 1)
					newLight->setType(Light::LightTypes::LT_SPOTLIGHT);
				if (newLight->getCustomParameter(1).x == 2)
					newLight->setType(Light::LightTypes::LT_DIRECTIONAL);

				newLight->setSpotlightInnerAngle(Radian(Degree(((Light*)attachedObject)->getSpotlightInnerAngle())));
				newLight->setSpotlightOuterAngle(Radian(Degree(((Light*)attachedObject)->getSpotlightOuterAngle())));
				newLight->setSpotlightFalloff(((Light*)attachedObject)->getSpotlightFalloff());

				newLight->setMode(((Light*)attachedObject)->getMode());

				newNode->attachObject(newLight);
			}

			if (dynamic_cast<Empty*>(attachedObject))
			{
				//Empty * newEmpty = (Empty*)GetEngine->GetEmptyObjectFactory()->createInstance(newName, GetEngine->GetSceneManager());
				Empty* newEmpty = (Empty*)GetEngine->GetSceneManager()->createMovableObject(newName, EmptyObjectFactory::FACTORY_TYPE_NAME);
				newNode->attachObject(newEmpty);
			}

			if (attachedObject->getMovableType() == "Camera")
			{
				Camera* newCamera = GetEngine->GetSceneManager()->createCamera(newName);

				newCamera->setNearClipDistance(((Camera*)attachedObject)->getNearClipDistance());
				newCamera->setFarClipDistance(((Camera*)attachedObject)->getFarClipDistance());
				newCamera->setFOVy(((Camera*)attachedObject)->getFOVy());
				newCamera->setProjectionType(((Camera*)attachedObject)->getProjectionType());
				newCamera->setOrthographicSize(((Camera*)attachedObject)->getOrthographicSize());
				newCamera->setClearColor(((Camera*)attachedObject)->getClearColor());
				//newCamera->setMainCamera(((Camera*)attachedObject)->getMainCamera());

				newNode->attachObject(newCamera);
			}

			if (attachedObject->getMovableType() == UIButtonFactory::FACTORY_TYPE_NAME)
			{
				UIButton* newUIElement = (UIButton*)GetEngine->GetSceneManager()->createMovableObject(newName, UIButtonFactory::FACTORY_TYPE_NAME);
				newUIElement->SetSize(((UIElement*)attachedObject)->GetSize());
				newUIElement->SetCanvasAlignment(((UIButton*)attachedObject)->GetCanvasAlignment());
				newUIElement->SetAnchor(((UIButton*)attachedObject)->GetAnchor());
				newUIElement->SetImageNormal(((UIButton*)attachedObject)->GetImageNormalName());
				newUIElement->SetImageHover(((UIButton*)attachedObject)->GetImageHoverName());
				newUIElement->SetImagePressed(((UIButton*)attachedObject)->GetImagePressedName());
				newUIElement->SetImageDisabled(((UIButton*)attachedObject)->GetImageDisabledName());
				newUIElement->SetColorNormal(((UIButton*)attachedObject)->GetColorNormal());
				newUIElement->SetColorHover(((UIButton*)attachedObject)->GetColorHover());
				newUIElement->SetColorPressed(((UIButton*)attachedObject)->GetColorPressed());
				newUIElement->SetColorDisabled(((UIButton*)attachedObject)->GetColorDisabled());
				newUIElement->SetPointerDownEvent(((UIButton*)attachedObject)->GetPointerDownEvent());
				newUIElement->SetPointerUpEvent(((UIButton*)attachedObject)->GetPointerUpEvent());

				newNode->attachObject((MovableObject*)newUIElement);
			}

			if (attachedObject->getMovableType() == UITextFactory::FACTORY_TYPE_NAME)
			{
				UIText* newUIElement = (UIText*)GetEngine->GetSceneManager()->createMovableObject(newName, UITextFactory::FACTORY_TYPE_NAME);
				newUIElement->SetSize(((UIElement*)attachedObject)->GetSize());
				newUIElement->SetCanvasAlignment(((UIText*)attachedObject)->GetCanvasAlignment());
				newUIElement->SetAnchor(((UIText*)attachedObject)->GetAnchor());
				newUIElement->setFontName(((UIText*)attachedObject)->getFontName());
				newUIElement->setText(((UIText*)attachedObject)->getText());
				newUIElement->setColor(((UIText*)attachedObject)->getColor());
				newUIElement->setFontSize(((UIText*)attachedObject)->getSize());
				newUIElement->setHorizontalAlignment(((UIText*)attachedObject)->getHorizontalAlignment());
				newUIElement->setVerticalAlignment(((UIText*)attachedObject)->getVerticalAlignment());

				newNode->attachObject((MovableObject*)newUIElement);
			}

			if (attachedObject->getMovableType() == UIImageFactory::FACTORY_TYPE_NAME)
			{
				UIImage* newUIElement = (UIImage*)GetEngine->GetSceneManager()->createMovableObject(newName, UIImageFactory::FACTORY_TYPE_NAME);
				newUIElement->SetSize(((UIElement*)attachedObject)->GetSize());
				newUIElement->SetCanvasAlignment(((UIImage*)attachedObject)->GetCanvasAlignment());
				newUIElement->SetAnchor(((UIImage*)attachedObject)->GetAnchor());
				newUIElement->SetImage(((UIImage*)attachedObject)->GetImageName());
				newUIElement->setColor(((UIImage*)attachedObject)->getColor());

				newNode->attachObject((MovableObject*)newUIElement);
			}

			if (attachedObject->getMovableType() == ParticleSystemFactory::FACTORY_TYPE_NAME)
			{
				ParticleSystem* ps = (ParticleSystem*)attachedObject;
				ParticleSystem* newPS = (ParticleSystem*)GetEngine->GetSceneManager()->createParticleSystem(newName, ps->getParticleQuota(), "Assets");

				newPS->setCastShadows(ps->getCastShadows());
				newPS->setEmitting(ps->getEmitting());
				newPS->setKeepParticlesInLocalSpace(ps->getKeepParticlesInLocalSpace());
				newPS->setMaterialName(ps->getMaterialName());
				newPS->setDefaultWidth(ps->getDefaultWidth());
				newPS->setDefaultHeight(ps->getDefaultHeight());
				newPS->setSpeedFactor(ps->getSpeedFactor());
				newPS->getRenderer()->setParameter("billboard_type", ps->getRenderer()->getParameter("billboard_type"));

				for (int i = 0; i < ps->getNumEmitters(); ++i)
				{
					ParticleEmitter* em = ps->getEmitter(i);
					ParticleEmitter* emitter = newPS->addEmitter(em->getType());
					emitter->setAngle(Radian(Degree(em->getAngle())));
					emitter->setColour(em->getColour());
					emitter->setDirection(em->getDirection());
					emitter->setEmissionRate(em->getEmissionRate());
					emitter->setEnabled(true/*em->enabled*/);
					emitter->setMinDuration(em->getMinDuration());
					emitter->setMinParticleVelocity(em->getMinParticleVelocity());
					emitter->setMinRepeatDelay(em->getMinRepeatDelay());
					emitter->setMinTimeToLive(em->getMinTimeToLive());
					emitter->setMaxDuration(em->getMaxDuration());
					emitter->setMaxParticleVelocity(em->getMaxParticleVelocity());
					emitter->setMaxRepeatDelay(em->getMaxRepeatDelay());
					emitter->setMaxTimeToLive(em->getMaxTimeToLive());

					for (ParameterList::const_iterator p = em->getParameters().begin(); p != em->getParameters().end(); ++p)
					{
						emitter->setParameter(p->name, em->getParameter(p->name));
					}
				}

				for (int i = 0; i < ps->getNumAffectors(); ++i)
				{
					ParticleAffector* af = ps->getAffector(i);
					ParticleAffector* affector = newPS->addAffector(af->getType());

					for (ParameterList::const_iterator p = af->getParameters().begin(); p != af->getParameters().end(); ++p)
					{
						affector->setParameter(p->name, af->getParameter(p->name));
					}
				}

				newNode->attachObject(newPS);
			}
		}

		//Clone scripts
		for (auto scr_it = curNode->monoScripts.begin(); scr_it != curNode->monoScripts.end(); ++scr_it)
		{
			cloneScript(*scr_it, newNode);
		}

		//Clone components
		for (auto it = curNode->components.begin(); it != curNode->components.end(); ++it)
		{
			cloneComponent(*it, newNode);
		}

		////////////////////

		int j = 0;
		auto children = curNode->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			
			if (GetEngine->IsEditorObject(nd))
			{
				--j;
				continue;
			}

			nstack.insert(nstack.begin() + j, make_pair(nd, newNode));
		}
	}

	return ret;
}

void CloneTools::ReassignParamsRecursive(SceneNode * node)
{
	std::vector<SceneNode*> nstack;
	nstack.push_back(node);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////

		//Clone scripts
		for (std::vector<MonoScript*>::iterator scr_it = curNode->monoScripts.begin(); scr_it != curNode->monoScripts.end(); ++scr_it)
		{

		}

		//Clone components
		for (std::vector<Component*>::iterator it = curNode->components.begin(); it != curNode->components.end(); ++it)
		{
			//Audio source
			if ((*it)->GetComponentTypeName() == AudioSource::COMPONENT_TYPE)
			{
				AudioSource* comp = (AudioSource*)(*it);
			}

			//Audio listener
			if ((*it)->GetComponentTypeName() == AudioListener::COMPONENT_TYPE)
			{
				AudioListener* comp = (AudioListener*)*it;
			}

			//Rigidbody
			if ((*it)->GetComponentTypeName() == RigidBody::COMPONENT_TYPE)
			{
				RigidBody* comp = (RigidBody*)*it;
			}

			//Mesh collider
			if ((*it)->GetComponentTypeName() == MeshCollider::COMPONENT_TYPE)
			{
				MeshCollider* comp = (MeshCollider*)*it;
			}

			//Capsule collider
			if ((*it)->GetComponentTypeName() == CapsuleCollider::COMPONENT_TYPE)
			{
				CapsuleCollider* comp = (CapsuleCollider*)*it;
			}

			//Box collider
			if ((*it)->GetComponentTypeName() == BoxCollider::COMPONENT_TYPE)
			{
				BoxCollider* comp = (BoxCollider*)*it;
			}

			//Sphere collider
			if ((*it)->GetComponentTypeName() == SphereCollider::COMPONENT_TYPE)
			{
				SphereCollider* comp = (SphereCollider*)*it;
			}

			//Skeleton bone
			if ((*it)->GetComponentTypeName() == SkeletonBone::COMPONENT_TYPE)
			{
				SkeletonBone* comp = (SkeletonBone*)*it;

				std::string rootNodeName = remapList[comp->GetRootNodeName()] != "" ? remapList[comp->GetRootNodeName()] : comp->GetRootNodeName();

				comp->SetRootNodeName(rootNodeName);
			}

			//Animation list
			if ((*it)->GetComponentTypeName() == AnimationList::COMPONENT_TYPE)
			{
				AnimationList* comp = (AnimationList*)*it;
			}

			//Hinge joint
			if ((*it)->GetComponentTypeName() == HingeJoint::COMPONENT_TYPE)
			{
				HingeJoint* comp = (HingeJoint*)*it;

				std::string connectedBody = remapList[comp->GetConnectedObjectName()] != "" ? remapList[comp->GetConnectedObjectName()] : comp->GetConnectedObjectName();

				comp->SetConnectedObjectName(connectedBody);
			}

			//Fixed joint
			if ((*it)->GetComponentTypeName() == FixedJoint::COMPONENT_TYPE)
			{
				FixedJoint* comp = (FixedJoint*)*it;

				std::string connectedBody = remapList[comp->GetConnectedObjectName()] != "" ? remapList[comp->GetConnectedObjectName()] : comp->GetConnectedObjectName();

				comp->SetConnectedObjectName(connectedBody);
			}

			//Character joint
			if ((*it)->GetComponentTypeName() == CharacterJoint::COMPONENT_TYPE)
			{
				CharacterJoint* comp = (CharacterJoint*)*it;

				std::string connectedBody = remapList[comp->GetConnectedObjectName()] != "" ? remapList[comp->GetConnectedObjectName()] : comp->GetConnectedObjectName();

				comp->SetConnectedObjectName(connectedBody);
			}

			//Vehicle
			if ((*it)->GetComponentTypeName() == Vehicle::COMPONENT_TYPE)
			{
				Vehicle* comp = (Vehicle*)*it;

				std::vector<Vehicle::WheelInfo>& wheels = comp->getWheels();

				for (auto it = wheels.begin(); it != wheels.end(); ++it)
				{
					Vehicle::WheelInfo & inf = *it;
					std::string connectedObject = remapList[inf.m_connectedObjectName] != "" ? remapList[inf.m_connectedObjectName] : inf.m_connectedObjectName;
					inf.m_connectedObjectName = connectedObject;
				}
			}
		}
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

void CloneTools::cloneScript(MonoScript* script, SceneNode* copyTo)
{
	MonoScript* newScript = new MonoScript(copyTo);
	newScript->_class = script->_class;
	newScript->enabled = script->enabled;

	newScript->CreateInstance();

	for (std::map<std::string, MonoScript::MonoFieldInfo>::iterator fit = script->fieldSerializeList.begin(); fit != script->fieldSerializeList.end(); ++fit)
	{
		newScript->fieldSerializeList[fit->first] = fit->second;

		MonoClass* _class = mono_object_get_class((MonoObject*)newScript->object);
		MonoClassField* _fld = mono_class_get_field_from_name(_class, CP_UNI(fit->second.fieldName).c_str());

		if (fit->second.fieldType == "System.Int32")
		{
			mono_field_set_value((MonoObject*)newScript->object, _fld, &fit->second.fieldValue.intVal);
		}
		if (fit->second.fieldType == "System.Single")
		{
			mono_field_set_value((MonoObject*)newScript->object, _fld, &fit->second.fieldValue.floatVal);
		}
		if (fit->second.fieldType == "System.Boolean")
		{
			mono_field_set_value((MonoObject*)newScript->object, _fld, &fit->second.fieldValue.boolVal);
		}
		if (fit->second.fieldType == "System.String")
		{
			MonoString* _str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(fit->second.fieldValue.stringVal).c_str());
			mono_field_set_value((MonoObject*)newScript->object, _fld, _str);
		}
		if (fit->second.fieldType == "FalcoEngine.GameObject")
		{
			if (GetEngine->GetSceneManager()->hasSceneNode(fit->second.fieldValue.objectVal))
			{
				SceneNode* _node = GetEngine->GetSceneManager()->getSceneNode(fit->second.fieldValue.objectVal);
				if (_node->gameObjectMono != nullptr)
				{
					mono_field_set_value((MonoObject*)newScript->object, _fld, _node->gameObjectMono);
				}
			}
		}
		if (fit->second.fieldType == "FalcoEngine.Vector2")
		{
			MonoObject* vec2 = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->vector2_class);
			mono_field_set_value(vec2, GetEngine->GetMonoRuntime()->vector2_x, &fit->second.fieldValue.vec2Val.x);
			mono_field_set_value(vec2, GetEngine->GetMonoRuntime()->vector2_y, &fit->second.fieldValue.vec2Val.y);

			void* _vec2 = mono_object_unbox(vec2);
			mono_field_set_value((MonoObject*)newScript->object, _fld, _vec2);
		}
		if (fit->second.fieldType == "FalcoEngine.Vector3")
		{
			MonoObject* vec3 = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->vector3_class);
			mono_field_set_value(vec3, GetEngine->GetMonoRuntime()->vector3_x, &fit->second.fieldValue.vec3Val.x);
			mono_field_set_value(vec3, GetEngine->GetMonoRuntime()->vector3_y, &fit->second.fieldValue.vec3Val.y);
			mono_field_set_value(vec3, GetEngine->GetMonoRuntime()->vector3_z, &fit->second.fieldValue.vec3Val.z);

			void* _vec3 = mono_object_unbox(vec3);
			mono_field_set_value((MonoObject*)newScript->object, _fld, _vec3);
		}
		if (fit->second.fieldType == "FalcoEngine.Vector4")
		{
			MonoObject* vec4 = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->vector4_class);
			mono_field_set_value(vec4, GetEngine->GetMonoRuntime()->vector4_x, &fit->second.fieldValue.vec4Val.x);
			mono_field_set_value(vec4, GetEngine->GetMonoRuntime()->vector4_y, &fit->second.fieldValue.vec4Val.y);
			mono_field_set_value(vec4, GetEngine->GetMonoRuntime()->vector4_z, &fit->second.fieldValue.vec4Val.z);
			mono_field_set_value(vec4, GetEngine->GetMonoRuntime()->vector4_w, &fit->second.fieldValue.vec4Val.w);

			void* _vec4 = mono_object_unbox(vec4);
			mono_field_set_value((MonoObject*)newScript->object, _fld, _vec4);
		}
	}

	copyTo->monoScripts.push_back(newScript);
}

void CloneTools::cloneComponent(Component* component, SceneNode* copyTo)
{
	//Audio source
	if (component->GetComponentTypeName() == AudioSource::COMPONENT_TYPE)
	{
		AudioSource* comp = (AudioSource*)(component);
		AudioSource* audio = new AudioSource(copyTo);

		audio->SetFileName(comp->GetFileName());
		audio->SetPlayOnStart(comp->GetPlayOnStart());
		audio->SetLoop(comp->GetLooped());
		audio->SetEnabled(comp->GetEnabled());
		audio->SetVolume(comp->GetVolume());
		audio->SetMinDistance(comp->GetMinDistance());
		audio->SetMaxDistance(comp->GetMaxDistance());
		audio->SetIs2D(comp->GetIs2D());

		copyTo->components.push_back(audio);
	}

	//Audio listener
	if (component->GetComponentTypeName() == AudioListener::COMPONENT_TYPE)
	{
		AudioListener* comp = (AudioListener*)component;
		AudioListener* audio = new AudioListener(copyTo);
		audio->SetEnabled(comp->GetEnabled());
		audio->SetVolume(comp->GetVolume());

		copyTo->components.push_back(audio);
	}

	//Rigidbody
	if (component->GetComponentTypeName() == RigidBody::COMPONENT_TYPE)
	{
		RigidBody* comp = (RigidBody*)component;
		RigidBody* rigidBody = new RigidBody(copyTo);

		rigidBody->SetMass(comp->GetMass());
		rigidBody->SetFreezePositionX(comp->GetFreezePositionX());
		rigidBody->SetFreezePositionY(comp->GetFreezePositionY());
		rigidBody->SetFreezePositionZ(comp->GetFreezePositionZ());
		rigidBody->SetFreezeRotationX(comp->GetFreezeRotationX());
		rigidBody->SetFreezeRotationY(comp->GetFreezeRotationY());
		rigidBody->SetFreezeRotationZ(comp->GetFreezeRotationZ());
		rigidBody->SetIsKinematic(comp->GetIsKinematic());
		rigidBody->SetIsStatic(comp->GetIsStatic());
		rigidBody->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(rigidBody);
	}

	//Mesh collider
	if (component->GetComponentTypeName() == MeshCollider::COMPONENT_TYPE)
	{
		MeshCollider* comp = (MeshCollider*)component;
		MeshCollider* collider = new MeshCollider(copyTo);

		collider->SetConvex(comp->GetConvex());
		collider->SetIsTrigger(comp->GetIsTrigger());
		collider->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(collider);
	}

	//Capsule collider
	if (component->GetComponentTypeName() == CapsuleCollider::COMPONENT_TYPE)
	{
		CapsuleCollider* comp = (CapsuleCollider*)component;
		CapsuleCollider* collider = new CapsuleCollider(copyTo);

		collider->SetHeight(comp->GetHeight());
		collider->SetRadius(comp->GetRadius());
		collider->SetOffset(comp->GetOffset());
		collider->SetRotation(comp->GetRotation());
		collider->SetIsTrigger(comp->GetIsTrigger());
		collider->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(collider);
	}

	//Box collider
	if (component->GetComponentTypeName() == BoxCollider::COMPONENT_TYPE)
	{
		BoxCollider* comp = (BoxCollider*)component;
		BoxCollider* collider = new BoxCollider(copyTo);

		collider->SetBoxSize(comp->GetBoxSize());
		collider->SetOffset(comp->GetOffset());
		collider->SetRotation(comp->GetRotation());
		collider->SetIsTrigger(comp->GetIsTrigger());
		collider->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(collider);
	}

	//Sphere collider
	if (component->GetComponentTypeName() == SphereCollider::COMPONENT_TYPE)
	{
		SphereCollider* comp = (SphereCollider*)component;
		SphereCollider* collider = new SphereCollider(copyTo);

		collider->SetRadius(comp->GetRadius());
		collider->SetOffset(comp->GetOffset());
		collider->SetIsTrigger(comp->GetIsTrigger());
		collider->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(collider);
	}

	//Skeleton bone
	if (component->GetComponentTypeName() == SkeletonBone::COMPONENT_TYPE)
	{
		SkeletonBone* comp = (SkeletonBone*)component;
		SkeletonBone* bone = new SkeletonBone(copyTo);

		bone->SetBindingPosition(comp->GetBindingPosition());
		bone->SetBindingRotation(comp->GetBindingRotation());
		bone->SetBindingScale(comp->GetBindingScale());
		bone->SetIndex(comp->GetIndex());
		bone->SetName(comp->GetName());
		bone->SetPosition(comp->GetPosition());
		bone->SetRotation(comp->GetRotation());
		bone->SetScale(comp->GetScale());
		bone->SetModelFileName(comp->GetModelFileName());
		bone->SetRootNodeName(comp->GetRootNodeName());
		bone->SetIsRootBone(comp->GetIsRootBone());
		bone->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(bone);
	}

	//Animation list
	if (component->GetComponentTypeName() == AnimationList::COMPONENT_TYPE)
	{
		AnimationList* comp = (AnimationList*)component;
		AnimationList* animList = new AnimationList(copyTo);

		animList->SetDefaultAnimation(comp->GetDefaultAnimation());
		animList->SetFBXFileName(comp->GetFBXFileName());
		animList->SetPlayAtStart(comp->GetPlayAtStart());
		animList->SetEnabled(comp->GetEnabled());

		for (AnimationList::AnimationDataList::iterator ait = comp->GetAnimationDataList().begin(); ait != comp->GetAnimationDataList().end(); ++ait)
		{
			animList->AddAnimationData(ait->name, ait->fileName, ait->startFrame, ait->endFrame, ait->loop, ait->speed);
		}

		copyTo->components.push_back(animList);
	}

	//Hinge joint
	if (component->GetComponentTypeName() == HingeJoint::COMPONENT_TYPE)
	{
		HingeJoint* comp = (HingeJoint*)component;
		HingeJoint* joint = new HingeJoint(copyTo);

		joint->SetAnchor(comp->GetAnchor());
		joint->SetAxis(comp->GetAxis());
		joint->SetConnectedAnchor(comp->GetConnectedAnchor());
		joint->SetConnectedObjectName(comp->GetConnectedObjectName());
		joint->SetLimitMin(comp->GetLimitMin());
		joint->SetLimitMax(comp->GetLimitMax());
		joint->SetLinkedBodiesCollision(comp->GetLinkedBodiesCollision());
		joint->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(joint);
	}

	//Fixed joint
	if (component->GetComponentTypeName() == FixedJoint::COMPONENT_TYPE)
	{
		FixedJoint* comp = (FixedJoint*)component;
		FixedJoint* joint = new FixedJoint(copyTo);

		joint->SetAnchor(comp->GetAnchor());
		joint->SetConnectedAnchor(comp->GetConnectedAnchor());
		joint->SetConnectedObjectName(comp->GetConnectedObjectName());
		joint->SetLinkedBodiesCollision(comp->GetLinkedBodiesCollision());
		joint->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(joint);
	}

	//Character joint
	if (component->GetComponentTypeName() == CharacterJoint::COMPONENT_TYPE)
	{
		CharacterJoint* comp = (CharacterJoint*)component;
		CharacterJoint* joint = new CharacterJoint(copyTo);

		joint->SetAnchor(comp->GetAnchor());
		joint->SetAxis(comp->GetAxis());
		joint->SetConnectedAnchor(comp->GetConnectedAnchor());
		joint->SetConnectedObjectName(comp->GetConnectedObjectName());
		joint->SetLimitMin(comp->GetLimitMin());
		joint->SetLimitMax(comp->GetLimitMax());
		joint->SetLinkedBodiesCollision(comp->GetLinkedBodiesCollision());
		joint->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(joint);
	}

	//NavMesh agent
	if (component->GetComponentTypeName() == NavMeshAgent::COMPONENT_TYPE)
	{
		NavMeshAgent* comp = (NavMeshAgent*)component;
		NavMeshAgent* agent = new NavMeshAgent(copyTo);

		agent->SetAcceleration(comp->GetAcceleration());
		agent->SetHeight(comp->GetHeight());
		agent->SetRadius(comp->GetRadius());
		agent->SetRotationSpeed(comp->GetRotationSpeed());
		agent->SetSpeed(comp->GetSpeed());
		agent->SetTargetPosition(comp->GetTargetPosition());
		agent->SetEnabled(comp->GetEnabled());

		copyTo->components.push_back(agent);
	}

	//NavMesh obstacle
	if (component->GetComponentTypeName() == NavMeshObstacle::COMPONENT_TYPE)
	{
		NavMeshObstacle* comp = (NavMeshObstacle*)component;
		NavMeshObstacle* obstacle = new NavMeshObstacle(copyTo);

		obstacle->SetEnabled(comp->GetEnabled());
		obstacle->setSize(comp->getSize());
		obstacle->setOffset(comp->getOffset());

		copyTo->components.push_back(obstacle);
	}

	//Vehicle
	if (component->GetComponentTypeName() == Vehicle::COMPONENT_TYPE)
	{
		Vehicle* comp = (Vehicle*)component;
		Vehicle* vehicle = new Vehicle(copyTo);

		vehicle->SetEnabled(comp->GetEnabled());
		vehicle->setAxis(comp->getAxis());
		vehicle->setInvertForward(comp->getInvertForward());

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		std::vector<Vehicle::WheelInfo> & wheelsNew = vehicle->getWheels();

		for (auto it = wheels.begin(); it != wheels.end(); ++it)
		{
			Vehicle::WheelInfo inf;
			inf.m_axle = it->m_axle;
			inf.m_connectedObjectName = it->m_connectedObjectName;
			inf.m_connectedObjectRef = nullptr;
			inf.m_connectionPoint = it->m_connectionPoint;
			inf.m_direction = it->m_direction;
			inf.m_friction = it->m_friction;
			inf.m_isFrontWheel = it->m_isFrontWheel;
			inf.m_radius = it->m_radius;
			inf.m_rollInfluence = it->m_rollInfluence;
			inf.m_suspensionCompression = it->m_suspensionCompression;
			inf.m_suspensionDamping = it->m_suspensionDamping;
			inf.m_suspensionRestLength = it->m_suspensionRestLength;
			inf.m_suspensionStiffness = it->m_suspensionStiffness;
			inf.m_width = it->m_width;

			wheelsNew.push_back(inf);
		}

		copyTo->components.push_back(vehicle);
	}
}
