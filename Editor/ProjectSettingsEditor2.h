#pragma once

#include "PropertyEditor.h"
#include "../Engine/ProjectSettings.h"

class TreeNode;
class Property;

class ProjectSettingsEditor2 : public PropertyEditor
{
public:
	ProjectSettingsEditor2();
	~ProjectSettingsEditor2();

	virtual void init(std::string path, bool loadSettings = true);
	virtual void update();
	virtual void updateEditor();

private:
	string settingsPath;
	ProjectSettings projectSettings;

	void onDropStartupScene(TreeNode * prop, TreeNode * from);
	void onChangeShadowsEnabled(Property * prop, bool val);
	void onChangeDirShadows(Property * prop, std::string val);
	void onChangeSpotShadows(Property* prop, std::string val);
	void onChangePointShadows(Property* prop, std::string val);
	void onChangeCascades(Property* prop, std::string val);
	void onChangeSamples(Property* prop, std::string val);
	void onChangeCascadesBlending(Property* prop, bool val);

	void onChangeTag(Property* prop, std::string val);
	void onAddTag(TreeNode* prop);
	void onTagPopup(TreeNode* node, int val);

	void onChangeLayer(Property* prop, std::string val);
	void onAddLayer(TreeNode* prop);
	void onLayerPopup(TreeNode* node, int val);
};