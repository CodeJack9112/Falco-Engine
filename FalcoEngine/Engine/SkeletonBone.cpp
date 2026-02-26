#include "stdafx.h"
#include "SkeletonBone.h"
#include <fbxsdk.h>
#include "FBXSceneManager.h"

#include "Engine.h"
#include <Engine\ResourceMap.h>

std::string SkeletonBone::COMPONENT_TYPE = "SkeletonBone";

SkeletonBone::SkeletonBone(SceneNode * parent) : Component(parent)
{

}

SkeletonBone::~SkeletonBone()
{
}

void SkeletonBone::SetModelFileName(std::string name)
{
	modelFileName = name;
	if (FBXSceneManager::getSingleton().resourceExists(name, "Assets"))
		modelFileGuid = ResourceMap::guidMap[FBXSceneManager::getSingleton().getByName(name, "Assets").getPointer()];
	else
		modelFileGuid = "";
}

void SkeletonBone::CreateGizmos()
{
	//MaterialPtr wireframeMaterial;

	//if (!MaterialManager::getSingleton().resourceExists("WireframeMaterial_111", "Assets"))
	//	wireframeMaterial = MaterialManager::getSingleton().create("WireframeMaterial_111", "Assets");
	//else
	//	wireframeMaterial = MaterialManager::getSingleton().getByName("WireframeMaterial_111", "Assets");

	//wireframeMaterial->setLightingEnabled(false);
	//wireframeMaterial->setReceiveShadows(false);
	//wireframeMaterial->setDepthBias(1, 0);
	//wireframeMaterial->removeAllTechniques();
	//wireframeMaterial->createTechnique();
	//wireframeMaterial->getTechnique(0)->createPass();
	//wireframeMaterial->getTechnique(0)->getPass(0)->setLineWidth(1.5);
	//wireframeMaterial->getTechnique(0)->getPass(0)->setPolygonMode(PolygonMode::PM_WIREFRAME);
	//wireframeMaterial->getTechnique(0)->getPass(0)->setSceneBlendingOperation(Ogre::SceneBlendOperation::SBO_ADD);
	//wireframeMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	//wireframeMaterial->getTechnique(0)->setSchemeName("NoGBuffer");

	//SceneNode * wireframeNode = GetParentSceneNode()->createChildSceneNode("bone" + GetName() + GetParentSceneNode()->getName() + to_string(rand()));

	//MeshPtr capsuleMesh = Procedural::CapsuleGenerator().setHeight(3).setRadius(0.5).realizeMesh("Sphere" + GetName() + GetParentSceneNode()->getName() + to_string(rand()));
	//Entity * capsuleEntity = GetEngine->GetSceneManager()->createEntity("Sphere_ent" + GetName() + GetParentSceneNode()->getName() + to_string(rand()), capsuleMesh);
	//capsuleEntity->setMaterial(wireframeMaterial);
	////capsuleEntity->setQueryFlags(1 << 3);
	////capsuleEntity->setRenderQueueGroup(RENDER_QUEUE_9 - 1);

	//wireframeNode->attachObject(capsuleEntity);
	//wireframeNode->_setDerivedPosition(GetBindingPosition());
	//wireframeNode->_setDerivedOrientation(GetBindingRotation());
}
