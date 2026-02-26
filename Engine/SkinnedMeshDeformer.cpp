#include "SkinnedMeshDeformer.h"
#include "SkeletonBone.h"
#include "Engine.h"

#include <OgreSkeletonManager.h>
#include <OgreEntity.h>
#include <OgreBone.h>

void SkinnedMeshDeformer::EnableAnimation(Ogre::SceneNode* boneNode, bool enable, bool recursive)
{
	SkeletonBone* bone = (SkeletonBone*)boneNode->GetComponent(SkeletonBone::COMPONENT_TYPE);
	if (bone != nullptr)
	{
		if (SkeletonManager::getSingleton().resourceExists(bone->GetModelFileName() + "_skeleton", "Assets"))
		{
			SceneNode* root = GetEngine->GetSceneManager()->getSceneNodeFast(bone->GetRootNodeName());

			if (root != nullptr)
			{
				SkeletonInstance* skeleton = nullptr;
				ActivateAnimation(enable, boneNode, root, bone->GetName(), skeleton, recursive);
			}
		}
	}
}

void SkinnedMeshDeformer::ActivateAnimation(bool activate, Ogre::SceneNode* boneNode, Ogre::SceneNode* root, std::string boneName, Ogre::SkeletonInstance*& skeletonRef, bool recursive)
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

		if (curNode->getAttachedObjects().size() > 0)
		{
			if (curNode->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
			{
				Entity* ent = (Entity*)curNode->getAttachedObject(0);
				AnimationStateSet* anims = ent->getAllAnimationStates();

				if (!ent->hasSkeleton())
					continue;

				SkeletonInstance* skeleton = ent->getSkeleton();
				skeletonRef = skeleton;

				Bone* _bone = skeleton->getBone(boneName);

				_bone->setManuallyControlled(!activate);
				_bone->setInheritOrientation(activate);

				if (anims != nullptr)
				{
					AnimationStateIterator ait = anims->getAnimationStateIterator();

					while (ait.hasMoreElements())
					{
						AnimationState* state = ait.getNext();

						if (!state->hasBlendMask())
						{
							if (!activate)
								state->createBlendMask(skeleton->getBones().size() - 1);
						}
						else
						{
							if (activate)
								state->destroyBlendMask();
						}

						if (!activate)
							state->setBlendMaskEntry(_bone->getHandle(), 0.0f);
					}
				}
			}
		}

		////////////////////

		if (recursive)
		{
			int j = 0;
			auto children = curNode->getChildren();
			for (auto it = children.begin(); it != children.end(); ++it, ++j)
			{
				SceneNode* nd = (SceneNode*)(*it);
				nstack.insert(nstack.begin() + j, nd);
			}
		}
	}
}

void SkinnedMeshDeformer::UpdateSkeleton(Ogre::SceneNode* mainRoot, Ogre::SceneNode* boneNode, Ogre::SceneNode* root, std::string boneName, Ogre::Vector3 position, Ogre::Quaternion rotation, Ogre::Vector3 scale)
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

		if (curNode->getAttachedObjects().size() > 0)
		{
			if (curNode->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
			{
				Entity* ent = (Entity*)curNode->getAttachedObject(0);

				if (ent->hasSkeleton())
				{
					SkeletonInstance* skeleton = ent->getSkeleton();
					Bone* _bone = skeleton->getBone(boneName);

					_bone->setPosition(position);

					Quaternion _rt = mainRoot->_getDerivedOrientation().Inverse() * rotation;

					_bone->setOrientation(_rt);
					_bone->setScale(scale);
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

void SkinnedMeshDeformer::UpdateSkeleton(SceneNode* node, bool useParentBuffer, Ogre::SceneNode* parentNode, bool recursive)
{
	std::vector<SceneNode*> nstack;
	nstack.push_back(node);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////
		SkeletonBone* bone = (SkeletonBone*)curNode->GetComponent(SkeletonBone::COMPONENT_TYPE);

		if (bone != nullptr)
		{
			SceneNode* root = GetEngine->GetSceneManager()->getSceneNodeFast(bone->GetRootNodeName());
			if (root != nullptr)
			{
				if (useParentBuffer)
				{
					if (parentNode == nullptr)
						parentNode = curNode->getParentSceneNode();

					Affine3 m1 = curNode->_getFullTransform();
					Affine3 m2 = parentNode->_getFullTransform();
					Affine3 m3 = m2.inverse() * m1;

					Vector3 pos = m3.getTrans();
					Vector3 ds = parentNode->_getDerivedScale();
					Vector3 scl = Vector3(1.0f / ds.x, 1.0f / ds.y, 1.0f / ds.z) * curNode->_getDerivedScale();

					SkinnedMeshDeformer::UpdateSkeleton(root, curNode, root, bone->GetName(), pos, curNode->_getDerivedOrientation(), scl);
				}
				else
				{
					SkinnedMeshDeformer::UpdateSkeleton(root, curNode, root, bone->GetName(), curNode->getPosition(), curNode->_getDerivedOrientation(), curNode->getScale());
				}
			}
		}

		if (recursive)
		{
			////////////////////
			parentNode = nullptr;

			int j = 0;
			auto children = curNode->getChildren();
			for (auto it = children.begin(); it != children.end(); ++it, ++j)
			{
				SceneNode* nd = (SceneNode*)(*it);
				nstack.insert(nstack.begin() + j, nd);
			}
		}
	}
}