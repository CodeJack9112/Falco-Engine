#pragma once

#include <OgreMaterial.h>
#include "PropertyEditor.h"

using namespace std;
using namespace Ogre;

class DialogTextInput;
class TreeNode;

class MaterialEditor2 : public PropertyEditor
{
public:
	MaterialEditor2();
	~MaterialEditor2();

	virtual void init() {}
	virtual void init(MaterialPtr mat, MaterialPtr originalMat, bool updateMat = true);
	virtual void update();
	virtual void updateEditor();

	void setMaterials(MaterialPtr mat, MaterialPtr originalMat);

	void updateMaterial(MaterialPtr mat, MaterialPtr origMat);
	void updateMaterialProps();
	void setOnEditorUpdateCallback(std::function<void()> callback) { onUpdateCallback = callback; }

	MaterialPtr getMaterial() { return material; }

private:
	MaterialPtr material;
	MaterialPtr originalMaterial;

	DialogTextInput* dialogWindow = nullptr;

	std::function<void()> onUpdateCallback = nullptr;

	void onTreeViewEndUpdate();

	void serializeMaterial(MaterialPtr mat);
	void iterateParameters(GpuProgramParametersSharedPtr params, GpuNamedConstants vars, Property* mat_prop, Pass* pass, Pass* opass, MaterialPtr mat, MaterialPtr origMat);

	void onAddPass(TreeNode* prop);
	void onAddPPD(TreeNode* prop);
	void onAddTextureUnit(TreeNode* prop);

	void onChangeRenderType(Property* prop, std::string val);
	void onChangeBlendingMode(Property* prop, std::string val);
	void onChangeIteration(Property* prop, std::string val);
	void onChangeBackfaceCulling(Property* prop, bool val);
	void onChangeDepthWrite(Property* prop, bool val);
	void onChangeDepthCheck(Property* prop, bool val);
	void onChangeLighting(Property* prop, bool val);

	void onChangePreprocessorDefine(Property* prop, bool val);

	void onClickTextureUnitFile(Property* prop);

	void onChangeSampler2D(Property* prop, int val);
	void onChangeSamplerCube(Property* prop, int val);
	void onChangeVector4(Property* prop, Vector4 val);
	void onChangeVector3(Property* prop, Vector3 val);
	void onChangeVector2(Property* prop, Vector2 val);
	void onChangeFloat(Property* prop, float val);
	void onChangeInteger(Property* prop, int val);
	void onChangeMatrix3(Property* prop, Matrix3 val);
	void onChangeMatrix4(Property* prop, Matrix4 val);
	void onChangeAutoParam(Property* prop, std::string val);

	void onChangeContentType(Property* prop, int val);
	void onChangeMaxAnisotropy(Property* prop, int val);
	void onChangeMinFilter(Property* prop, std::string val);
	void onChangeMagFilter(Property* prop, std::string val);
	void onChangeMipFilter(Property* prop, std::string val);

	void onPopupAddPPD(TreeNode* prop, int val);
	void onPopupPPD(TreeNode* prop, int val);
	void onPopupPass(TreeNode* prop, int val);
	void onPopupSelectedTextureUnit(TreeNode* prop, int val);
	void onPopupSelectedVertexShader(TreeNode* prop, int val);
	void onPopupSelectedFragmentShader(TreeNode* prop, int val);
	void onPopupSelectedParameterType(TreeNode* prop, int val);

	void onDropTexture(TreeNode* prop, TreeNode* from);
	void onDropVertexShader(TreeNode* prop, TreeNode* from);
	void onDropFragmentShader(TreeNode* prop, TreeNode* from);
};