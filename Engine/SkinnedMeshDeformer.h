#pragma once

#include <OgreSceneNode.h>
#include <OgreSkeletonInstance.h>

class SkinnedMeshDeformer
{
public:
	static void EnableAnimation(Ogre::SceneNode* boneNode, bool enable, bool recursive = true);
	static void UpdateSkeleton(SceneNode* node, bool useParentBuffer = false, Ogre::SceneNode* parentNode = nullptr, bool recursive = true);

private:
	static void ActivateAnimation(bool activate, Ogre::SceneNode* boneNode, Ogre::SceneNode* root, std::string boneName, Ogre::SkeletonInstance*& skeletonRef, bool recursive = true);

	//Position - local, Rotation - world, Scale - local
	static void UpdateSkeleton(Ogre::SceneNode* mainRoot, Ogre::SceneNode* boneNode, Ogre::SceneNode* root, std::string boneName, Ogre::Vector3 position, Ogre::Quaternion rotation, Ogre::Vector3 scale);
};

