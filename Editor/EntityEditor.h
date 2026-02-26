#pragma once

#include "ObjectEditor2.h"

class TreeNode;

class EntityEditor : public ObjectEditor2
{
public:
	EntityEditor();
	~EntityEditor();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();
	virtual void updateEditor();

private:
	bool isMaterialsIdentical();
	void listMaterials(Property * parent);

	void onChangeCastShadows(Property * prop, bool val);
	void onChangeReceiveShadows(Property* prop, bool val);
	void onChangeUseCustomLightmapSize(Property* prop, bool val);
	void onChangeCustomLightmapSize(Property* prop, string val);

	void onDropMaterial(TreeNode * prop, TreeNode * from);
	void onClickMaterialFile(Property* prop);
};

