#pragma once

#include <OgrePrerequisites.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include "PropertyEditor.h"

class TreeView;
class TreeNode;
class Property;
class PropVector3;
class MonoScript;

class ObjectEditor2 : public PropertyEditor
{
public:
	ObjectEditor2();
	~ObjectEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();
	virtual void updateEditor();

	void updateTransform();

	std::vector<Ogre::SceneNode*>& getSceneNodes() { return sceneNodes; }
	static void resetBufferObjects();

private:
	std::vector<Ogre::SceneNode*> sceneNodes;

	PropVector3* position = nullptr;
	PropVector3* rotation = nullptr;
	PropVector3* scale = nullptr;

	Ogre::SceneNode* changeStaticLightmapNode = nullptr;
	Ogre::SceneNode* changeStaticNavMeshNode = nullptr;
	Ogre::SceneNode* changeStaticBatchingNode = nullptr;

	static bool bufferFilled;
	static Ogre::Vector3 bufferPosition;
	static Ogre::Vector3 bufferScale;
	static Ogre::Quaternion bufferOrientation;
	static Ogre::Component* bufferComponent;
	static MonoScript* bufferScript;

	bool isPrefabSelected();
	void revertPrefab(std::string prefab);
	void applyPrefab(std::string prefab);
	void breakPrefab();
	
	void onTreeViewEndUpdate();

	bool isScriptsIdentical();
	bool isComponentsIdentical();

	void listScripts();
	void listComponents();

	void onChangeEnabled(Property* prop, bool val);
	void onChangeName(Property* prop, std::string val);
	void onChangePosition(Property* prop, Ogre::Vector3 val);
	void onChangeRotation(Property* prop, Ogre::Vector3 val);
	void onChangeScale(Property* prop, Ogre::Vector3 val);
	void onChangeLightmapStatic(Property* prop, bool val);
	void onChangeNavigationStatic(Property* prop, bool val);
	void onChangeStaticBatching(Property* prop, bool val);

	void onChangeScriptEnabled(Property* prop, bool val);

	void onChangeScriptInt(Property* prop, int val);
	void onChangeScriptFloat(Property* prop, float val);
	void onChangeScriptBool(Property* prop, bool val);
	void onChangeScriptVector2(Property* prop, Ogre::Vector2 val);
	void onChangeScriptVector3(Property* prop, Ogre::Vector3 val);
	void onChangeScriptVector4(Property* prop, Ogre::Vector4 val);
	void onChangeScriptString(Property* prop, std::string val);
	void onDropScriptGameObject(TreeNode* prop, TreeNode * from);
	void onDropScriptString(TreeNode* prop, TreeNode * from);

	void onDropScript(TreeNode * prop, TreeNode * from);

	void onScriptPopup(TreeNode* node, int val);
	void onComponentPopup(TreeNode* node, int val);
	void onTransformPopup(TreeNode* node, int val);
	void onComponentsPopup(TreeNode* node, int val);
	void onScriptsPopup(TreeNode* node, int val);

	void onChangeTag(Property* node, int val);
	void onChangeLayer(Property* node, int val);
};