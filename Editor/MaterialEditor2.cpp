#include "stdafx.h"
#include "MaterialEditor2.h"
#include "DialogTextInput.h"

#include <OgreMaterialSerializer.h>
#include <OgreMaterialManager.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreCamera.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreTechnique.h>
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreGLSLProgram.h>

#include "MainWindow.h"
#include "InspectorWindow.h"
#include "ConsoleWindow.h"
#include "AssetsWindow.h"
#include "PreviewWindow.h"
#include "TreeView.h"
#include "PropString.h"
#include "PropFloat.h"
#include "PropInt.h"
#include "PropBool.h"
#include "PropComboBox.h"
#include "PropVector2.h"
#include "PropVector3.h"
#include "PropVector4.h"
#include "PropMatrix4.h"
#include "PropMatrix3.h"
#include "PropButton.h"

#include "../Engine/Engine.h"
#include "../Engine/IO.h"
#include "../Engine/Procedural/ProceduralBoxGenerator.h"
#include "../Engine/DeferredShading/DeferredLightCP.h"
#include "../Engine/TerrainManager.h"
#include "../Engine/PagedGeometry/include/BatchPage.h"

#include <OgreHardwarePixelBuffer.h>
#include <OgreRenderTarget.h>
#include <OgreRenderTexture.h>
#include <OgreViewport.h>

enum MaterialParamType { PT_MANUAL, PT_AUTO };

struct MaterialPropData
{
public:
	MaterialPropData() = default;

	MaterialPropData(MaterialPtr mat, MaterialPtr origMat, Pass * ps)
	{
		material = mat; originalMaterial = origMat; pass = ps;
	}
	MaterialPropData(MaterialPtr mat, MaterialPtr origMat, Pass* ps, string var, MaterialParamType type)
	{
		material = mat; originalMaterial = origMat; pass = ps; varName = var; paramType = type;
	}
	MaterialPropData(MaterialPtr mat, MaterialPtr origMat, Pass* ps, TextureUnitState * tex)
	{
		material = mat; originalMaterial = origMat; pass = ps; textureUnitState = tex;
	}
	MaterialPropData(MaterialPtr mat, MaterialPtr origMat, Pass* ps, int t)
	{
		material = mat; originalMaterial = origMat; pass = ps; tag = t;
	}
	MaterialPropData(MaterialPtr mat, MaterialPtr origMat, Pass* ps, string var, int t)
	{
		material = mat; originalMaterial = origMat; pass = ps; tag = t; varName = var;
	}
	MaterialPropData(MaterialPtr mat, MaterialPtr origMat, Pass* ps, string var)
	{
		material = mat; originalMaterial = origMat; pass = ps; varName = var;
	}
	MaterialPropData(MaterialPtr mat, MaterialPtr origMat, Pass* ps, TextureUnitState* tex, int t)
	{
		material = mat; originalMaterial = origMat; pass = ps; textureUnitState = tex; tag = t;
	}

	MaterialPtr material;
	MaterialPtr originalMaterial;
	Pass* pass = nullptr;
	TextureUnitState* textureUnitState = nullptr;
	MaterialParamType paramType = PT_MANUAL;
	int tag = 0;
	string varName = "";
};

void* voidCast(MaterialPropData* dt) { return static_cast<void*>(dt); }
MaterialPropData* dataCast(void* dt) { return static_cast<MaterialPropData*>(dt); }

MaterialEditor2::MaterialEditor2()
{
	getTreeView()->setOnEndUpdateCallback([=]() { onTreeViewEndUpdate(); });
}

MaterialEditor2::~MaterialEditor2()
{
}

void MaterialEditor2::updateMaterialProps()
{
	//Trick to update material for Deferred shading
	MeshPtr boxMesh = Procedural::BoxGenerator(1, 1, 1).realizeMesh("__material_mesh_temp", "Assets");
	Entity* boxEntity = GetEngine->GetSceneManager()->createEntity("__material_entity_temp", boxMesh->getName(), "Assets");
	boxEntity->setMaterial(MaterialPtr());
	boxEntity->getSubEntity(0)->setOriginalMaterial(originalMaterial);

	SceneNode* node = GetEngine->GetSceneManager()->getRootSceneNode()->createChildSceneNode("__material_scene_node_temp");
	node->attachObject(boxEntity);
	node->setScale(0, 0, 0);
	node->_setDerivedPosition(MainWindow::mainCamera->getDerivedPosition() + MainWindow::mainCamera->getRealDirection() * 5);

	//GetEngine->RenderOneFrameWithoutGUI();
	//Root::getSingleton().renderOneFrame();
	GetEngine->UpdateMaterials();
	GetEngine->UpdateMaterials();

	material = boxEntity->getSubEntity(0)->getMaterial();
	//originalMaterial = boxEntity->getSubEntity(0)->getOriginalMaterial();

	GetEngine->GetSceneManager()->destroySceneNode(node);
	GetEngine->GetSceneManager()->destroyEntity(boxEntity);
	MeshManager::getSingleton().remove(boxMesh);
	//end
}

void MaterialEditor2::setMaterials(MaterialPtr mat, MaterialPtr originalMat)
{
	material = mat;
	originalMaterial = originalMat;
}

void MaterialEditor2::init(MaterialPtr mat, MaterialPtr originalMat, bool updateMat)
{
	__super::init();

	material = mat;
	originalMaterial = originalMat;

	if (updateMat)
		updateMaterialProps();

	string scheme = material->getTechnique(0)->getName();
	string sys_type = scheme == "Forward" ? "Forward" : "Deferred";

	PropComboBox* renderType = new PropComboBox(this, "Rendering type", { "Forward", "Deferred" });
	renderType->setCurrentItem(sys_type);
	renderType->setUserData(voidCast(new MaterialPropData(material, originalMaterial, nullptr)));
	renderType->setOnChangeCallback([=](Property* prop, string val) { onChangeRenderType(prop, val); });

	addProperty(renderType);

	Property* passRoot = new Property(this, "Passes");
	passRoot->setHasButton(true);
	passRoot->setButtonText("+");
	passRoot->setUserData(voidCast(new MaterialPropData(material, originalMaterial, nullptr)));
	passRoot->setOnButtonClickCallback([=](TreeNode* prop) { onAddPass(prop); });
	passRoot->icon = TextureManager::getSingleton().load("Icons/Inspector/passes.png", "Editor");

	addProperty(passRoot);

	Technique * tech = material->getTechnique(0);

	for (int currentPass = 0; currentPass < tech->getNumPasses(); ++currentPass)
	{
		Pass* _pass = originalMaterial->getTechnique(0)->getPass(currentPass);
		Pass* _ipass = material->getTechnique(0)->getPass(currentPass);

		Property* passGroup = new Property(this, "Pass " + to_string(currentPass));
		passGroup->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		passGroup->setPopupMenu({ "Move Up", "Move Down", "-", "Remove" }, [=](TreeNode* prop, int val) { onPopupPass(prop, val); });
		passGroup->icon = TextureManager::getSingleton().load("Icons/Inspector/pass.png", "Editor");

		std::string blend_type = "Transparent alpha";
		SceneBlendFactor sf = _pass->getSourceBlendFactor();
		SceneBlendFactor df = _pass->getDestBlendFactor();

		if (sf == SBF_SOURCE_ALPHA && df == SBF_ONE_MINUS_SOURCE_ALPHA)
			blend_type = "Transparent alpha";
		if (sf == SBF_SOURCE_COLOUR && df == SBF_ONE_MINUS_SOURCE_COLOUR)
			blend_type = "Transparent color";
		if (sf == SBF_DEST_COLOUR && df == SBF_ZERO)
			blend_type = "Modulate";
		if (sf == SBF_ONE && df == SBF_ONE)
			blend_type = "Add";
		if (sf == SBF_ONE && df == SBF_ZERO)
			blend_type = "Replace";

		PropComboBox* blendingMode = new PropComboBox(this, "Blending mode", { "Transparent alpha", "Transparent color", "Add", "Modulate", "Replace" });
		blendingMode->setCurrentItem(blend_type);
		blendingMode->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		blendingMode->setOnChangeCallback([=](Property* prop, string val) { onChangeBlendingMode(prop, val); });

		string iteration = "Once";
		if (_pass->getIteratePerLight())
			iteration = "Once per light";

		PropComboBox* iterationType = new PropComboBox(this, "Iteration", { "Once", "Once per light" });
		iterationType->setCurrentItem(iteration);
		iterationType->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		iterationType->setOnChangeCallback([=](Property* prop, string val) { onChangeIteration(prop, val); });

		CullingMode mode = _pass->getCullingMode();
		bool culling = !(mode == CullingMode::CULL_NONE);
		bool depth_check = _pass->getDepthCheckEnabled();
		bool depth_write = _pass->getDepthWriteEnabled();
		bool _lighting = _pass->getLightingEnabled();

		PropBool* backfaceCulling = new PropBool(this, "Backface culling", culling);
		PropBool* depthWrite = new PropBool(this, "Depth write", depth_write);
		PropBool* depthCheck = new PropBool(this, "Depth check", depth_check);
		PropBool* lighting = new PropBool(this, "Lighting", _lighting);
		
		backfaceCulling->setOnChangeCallback([=](Property* prop, bool val) { onChangeBackfaceCulling(prop, val); });
		depthWrite->setOnChangeCallback([=](Property* prop, bool val) { onChangeDepthWrite(prop, val); });
		depthCheck->setOnChangeCallback([=](Property* prop, bool val) { onChangeDepthCheck(prop, val); });
		lighting->setOnChangeCallback([=](Property* prop, bool val) { onChangeLighting(prop, val); });

		backfaceCulling->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		depthWrite->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		depthCheck->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		lighting->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));

		passGroup->addChild(blendingMode);
		passGroup->addChild(iterationType);
		passGroup->addChild(backfaceCulling);
		passGroup->addChild(depthWrite);
		passGroup->addChild(depthCheck);
		passGroup->addChild(lighting);

		std::string vertName = "";
		std::string fragName = "";

		if (_pass->hasVertexProgram())
		{
			vertName = _pass->getVertexProgramName();
		}

		if (_pass->hasFragmentProgram())
		{
			fragName = _pass->getFragmentProgramName();
		}

		if (HighLevelGpuProgramManager::getSingleton().resourceExists(vertName, "Assets"))
		{
			HighLevelGpuProgramPtr prog = HighLevelGpuProgramManager::getSingleton().getByName(vertName, "Assets");
			Ogre::GLSL::GLSLProgram* p = static_cast<Ogre::GLSL::GLSLProgram*>(prog.getPointer());

			if (p != nullptr && p->getGLHandle() == 0)
				vertName = "";
		}

		if (HighLevelGpuProgramManager::getSingleton().resourceExists(fragName, "Assets"))
		{
			HighLevelGpuProgramPtr prog = HighLevelGpuProgramManager::getSingleton().getByName(fragName, "Assets");
			Ogre::GLSL::GLSLProgram* p = static_cast<Ogre::GLSL::GLSLProgram*>(prog.getPointer());

			if (p != nullptr && p->getGLHandle() == 0)
				fragName = "";
		}

		if (vertName == "Shaders/DeferredPBR.glslv")
			vertName = "Standard (Deferred)";

		if (vertName == "Shaders/Diffuse.glslv")
			vertName = "Standard Diffuse (Forward)";

		if (vertName == "Shaders/BumpedSpecular.glslv")
			vertName = "Standard Bumped Specular (Forward)";

		if (vertName == "Shaders/Skybox.glslv")
			vertName = "Standard Skybox (Forward)";

		if (fragName == "Shaders/DeferredPBR.glslf")
			fragName = "Standard (Deferred)";

		if (fragName == "Shaders/Diffuse.glslf")
			fragName = "Standard Diffuse (Forward)";

		if (fragName == "Shaders/BumpedSpecular.glslf")
			fragName = "Standard Bumped Specular (Forward)";

		if (fragName == "Shaders/Skybox.glslf")
			fragName = "Standard Skybox (Forward)";

		Property* shadersProp = new Property(this, "Shaders");
		shadersProp->icon = TextureManager::getSingleton().load("Icons/Inspector/shaders.png", "Editor");

		PropString* shaderVert = new PropString(this, "Vertex shader", vertName);
		shaderVert->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		shaderVert->setSupportedFormats({ "glslv" });
		shaderVert->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropVertexShader(prop, from); });
		shaderVert->setPopupMenu({ "Standard (Deferred)", "-", "Standard Diffuse (Forward)", "Standard Bumped Specular (Forward)", "Standard Skybox (Forward)", "-", "Clear" }, [=](TreeNode* prop, int val) { onPopupSelectedVertexShader(prop, val); });
		shaderVert->setReadonly(true);

		PropString* shaderFrag = new PropString(this, "Fragment shader", fragName);
		shaderFrag->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		shaderFrag->setSupportedFormats({ "glslf" });
		shaderFrag->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropFragmentShader(prop, from); });
		shaderFrag->setPopupMenu({ "Standard (Deferred)", "-", "Standard Diffuse (Forward)", "Standard Bumped Specular (Forward)", "Standard Skybox (Forward)", "-", "Clear" }, [=](TreeNode* prop, int val) { onPopupSelectedFragmentShader(prop, val); });
		shaderFrag->setReadonly(true);

		shadersProp->addChild(shaderVert);
		shadersProp->addChild(shaderFrag);

		passGroup->addChild(shadersProp);

		Property* proc_defines = new Property(this, "Preprocessor defines");
		proc_defines->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		proc_defines->setHasButton(true);
		proc_defines->setButtonText("+");
		proc_defines->icon = TextureManager::getSingleton().load("Icons/Inspector/preprocessor_defines.png", "Editor");
		//proc_defines->setOnButtonClickCallback([=](TreeNode* prop) { onAddPPD(prop); });
		proc_defines->setPopupMenu({ "HAS_DIFFUSE", "HAS_NORMAL", "HAS_METALLIC", "HAS_OCCLUSION", "HAS_EMISSION", "USE_IBL", "-", "CUTOUT", "RECEIVE_SHADOWS", "-", "Custom" }, [=](TreeNode* node, int val) { onPopupAddPPD(node, val); });

		std::vector<std::pair<std::string, bool>> & defs = _pass->GetPreprocessorDefines();

		int _pj = 0;
		for (auto it = defs.begin(); it != defs.end(); ++it, ++_pj)
		{
			PropBool* ppd = new PropBool(this, it->first, it->second);
			ppd->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass, it->first)));
			ppd->setOnChangeCallback([=](Property* prop, bool val) { onChangePreprocessorDefine(prop, val); });
			ppd->setPopupMenu({ "Remove" }, [=](TreeNode* prop, int val) { onPopupPPD(prop, val); });

			proc_defines->addChild(ppd);
		}

		passGroup->addChild(proc_defines);

		Property* shader_params = new Property(this, "Parameters");
		shader_params->icon = TextureManager::getSingleton().load("Icons/Inspector/shader_params.png", "Editor");

		passGroup->addChild(shader_params);

		Material::Techniques techs;

		if (material != nullptr)
			techs = material->getTechniques();
		else
			techs = originalMaterial->getTechniques();

		//Shader parameters
		//Check if we have vertex shader attached to the current pass
		if (_ipass->hasVertexProgram())
		{
			try
			{
				//Retrieve the shader's vertex properties 
				GpuProgramParametersSharedPtr paramsVert = _ipass->getVertexProgramParameters();
				GpuNamedConstants varsVert = paramsVert.getPointer()->getConstantDefinitions(); //Vertex parameters

				//Iterate shader parameters
				iterateParameters(paramsVert, varsVert, shader_params, _ipass, _pass, material, originalMaterial); //Vertex parameters
			}
			catch (...)
			{
				MainWindow::GetConsoleWindow()->log("Shader is missing: " + _pass->getVertexProgramName(), LogMessageType::LMT_ERROR);
			}
		}

		//Check if we have fragment shader attached to the current pass
		if (_ipass->hasFragmentProgram())
		{
			try
			{
				//Retrieve the shader's fragment properties 
				GpuProgramParametersSharedPtr paramsFrag = _ipass->getFragmentProgramParameters();
				GpuNamedConstants varsFrag = paramsFrag.getPointer()->getConstantDefinitions(); //Fragment parameters

				//Iterate shader parameters
				iterateParameters(paramsFrag, varsFrag, shader_params, _ipass, _pass, material, originalMaterial); //Fragment parameters
			}
			catch (...)
			{
				MainWindow::GetConsoleWindow()->log("Shader is missing: " + _pass->getFragmentProgramName(), LogMessageType::LMT_ERROR);
			}
		}

		Property* texture_units = new Property(this, "Texture units");
		texture_units->setHasButton(true);
		texture_units->setButtonText("+");
		texture_units->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass)));
		texture_units->setOnButtonClickCallback([=](TreeNode* prop) { onAddTextureUnit(prop); });

		passGroup->addChild(texture_units);

		//Texture units
		VectorIterator tu = _pass->getTextureUnitStateIterator();

		while (tu.hasMoreElements())
		{
			TextureUnitState* tex_state = tu.getNext();

			string tex_name;
			if (tex_state != NULL)
			{
				if (tex_state->_getTexturePtr() != NULL)
					tex_name = tex_state->_getTexturePtr()->getName();
				else if (tex_state->getCubemap() != NULL)
					tex_name = tex_state->getCubemap()->getName();

				//tex_name = IO::GetFileNameWithExt(tex_name);
			}

			Property* texUnit = new Property(this, tex_state->getName());
			texUnit->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass, tex_state, _pass->getTextureUnitStateIndex(tex_state))));
			texUnit->setPopupMenu({ "Rename", "Remove" }, [=](TreeNode* prop, int val) { onPopupSelectedTextureUnit(prop, val); });

			if (tex_state->getContentType() == TextureUnitState::ContentType::CONTENT_NAMED)
			{
				auto formats = MainWindow::GetImagesFileFormats();
				formats.push_back("cubemap");

				//Sampler2D property
				PropButton* m_param = new PropButton(this, "File", tex_name);
				m_param->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass, tex_state, _pass->getTextureUnitStateIndex(tex_state))));
				m_param->setSupportedFormats(formats);
				m_param->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropTexture(prop, from); });
				m_param->setOnClickCallback([=](Property* prop) { onClickTextureUnitFile(prop); });
				m_param->setImage(TextureManager::getSingleton().load(tex_name, "Assets"));

				PropInt* maxAnisotropy = new PropInt(this, "Max anisotropy", tex_state->getTextureAnisotropy());
				maxAnisotropy->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass, tex_state)));
				maxAnisotropy->setOnChangeCallback([=](Property* prop, int val) { onChangeMaxAnisotropy(prop, val); });

				std::string minFilterName = "";
				if (tex_state->getTextureFiltering(FilterType::FT_MIN) == FilterOptions::FO_ANISOTROPIC)
					minFilterName = "Anisotropic";
				if (tex_state->getTextureFiltering(FilterType::FT_MIN) == FilterOptions::FO_LINEAR)
					minFilterName = "Linear";
				if (tex_state->getTextureFiltering(FilterType::FT_MIN) == FilterOptions::FO_POINT)
					minFilterName = "Point";
				if (tex_state->getTextureFiltering(FilterType::FT_MIN) == FilterOptions::FO_NONE)
					minFilterName = "None";

				std::string magFilterName = "";
				if (tex_state->getTextureFiltering(FilterType::FT_MAG) == FilterOptions::FO_ANISOTROPIC)
					magFilterName = "Anisotropic";
				if (tex_state->getTextureFiltering(FilterType::FT_MAG) == FilterOptions::FO_LINEAR)
					magFilterName = "Linear";
				if (tex_state->getTextureFiltering(FilterType::FT_MAG) == FilterOptions::FO_POINT)
					magFilterName = "Point";
				if (tex_state->getTextureFiltering(FilterType::FT_MAG) == FilterOptions::FO_NONE)
					magFilterName = "None";

				std::string mipFilterName = "";
				if (tex_state->getTextureFiltering(FilterType::FT_MIP) == FilterOptions::FO_ANISOTROPIC)
					mipFilterName = "Anisotropic";
				if (tex_state->getTextureFiltering(FilterType::FT_MIP) == FilterOptions::FO_LINEAR)
					mipFilterName = "Linear";
				if (tex_state->getTextureFiltering(FilterType::FT_MIP) == FilterOptions::FO_POINT)
					mipFilterName = "Point";
				if (tex_state->getTextureFiltering(FilterType::FT_MIP) == FilterOptions::FO_NONE)
					mipFilterName = "None";

				PropComboBox* minFiltering = new PropComboBox(this, "Min filter", { "Anisotropic", "Linear", "Point", "None" });
				minFiltering->setCurrentItem(minFilterName);
				minFiltering->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass, tex_state)));
				minFiltering->setOnChangeCallback([=](Property* prop, string val) { onChangeMinFilter(prop, val); });

				PropComboBox* magFiltering = new PropComboBox(this, "Mag filter", { "Anisotropic", "Linear", "Point", "None" });
				magFiltering->setCurrentItem(magFilterName);
				magFiltering->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass, tex_state)));
				magFiltering->setOnChangeCallback([=](Property* prop, string val) { onChangeMagFilter(prop, val); });

				PropComboBox* mipFiltering = new PropComboBox(this, "Mip filter", { "Anisotropic", "Linear", "Point", "None" });
				mipFiltering->setCurrentItem(mipFilterName);
				mipFiltering->setUserData(voidCast(new MaterialPropData(material, originalMaterial, _pass, tex_state)));
				mipFiltering->setOnChangeCallback([=](Property* prop, string val) { onChangeMipFilter(prop, val); });

				texUnit->addChild(m_param);
				texUnit->addChild(maxAnisotropy);
				texUnit->addChild(minFiltering);
				texUnit->addChild(magFiltering);
				texUnit->addChild(mipFiltering);
			}

			texture_units->addChild(texUnit);
		}

		passRoot->addChild(passGroup);
	}
}

void MaterialEditor2::update()
{
	__super::update();

	if (dialogWindow != nullptr)
		dialogWindow->update();
}

void MaterialEditor2::updateEditor()
{
	updateMaterialProps();

	MainWindow::AddOnEndUpdateCallback([=]() {
		float sp = MainWindow::GetInspectorWindow()->getScrollPos();
		MainWindow::GetInspectorWindow()->saveCollapsedProperties();

		auto props = getTreeView()->getRootNode()->children;

		for (auto it = props.begin(); it != props.end(); ++it)
			removeProperty((Property*)*it);

		init(material, originalMaterial, false);

		MainWindow::GetInspectorWindow()->loadCollapsedProperties();
		MainWindow::GetInspectorWindow()->setScrollPos(sp);

		if (onUpdateCallback != nullptr)
			onUpdateCallback();
	});
}

void MaterialEditor2::onTreeViewEndUpdate()
{

}

void MaterialEditor2::serializeMaterial(MaterialPtr mat)
{
	MaterialSerializer serializer;

	while (mat->getNumTechniques() > 1)
	{
		mat->removeTechnique(1);
	}

	if (IO::FileExists(MainWindow::GetProjectAssetsDir() + mat->getOrigin()))
		serializer.exportMaterial(mat, MainWindow::GetProjectAssetsDir() + mat->getOrigin(), false, false, "", mat->getName());
}

void MaterialEditor2::updateMaterial(MaterialPtr mat, MaterialPtr origMat)
{
	if (mat != nullptr)
	{
		if (mat->getGroup() != "Assets")
			return;
	}

	MapIterator it = GetEngine->GetSceneManager()->getMovableObjectIterator(EntityFactory::FACTORY_TYPE_NAME);

	while (it.hasMoreElements())
	{
		Entity* _entity = (Entity*)it.getNext();
		if (_entity->getParentSceneNode() == nullptr)
			continue;

		for (int i = 0; i < _entity->getNumSubEntities(); ++i)
		{
			SubEntity* ent = _entity->getSubEntity(i);
			if (ent->getOriginalMaterial() == origMat)
			{
				if (ent->getMaterial()->getName() != "BaseWhite")
				{
					if (mat != nullptr)
					{
						if (MaterialManager::getSingleton().resourceExists(ent->getMaterial()->getName(), "Assets"))
						{
							MaterialManager::getSingleton().unload(ent->getMaterial()->getName(), "Assets");
							MaterialManager::getSingleton().remove(ent->getMaterial()->getName(), "Assets");
						}
					}
				}

				ent->setMaterial(MaterialPtr());
			}
		}
	}

	TerrainManager* terrMgr = GetEngine->GetTerrainManager();
	PagedGeometry* detail = terrMgr->GetDetailPagedGeometry();
	GrassLoader * grass = terrMgr->GetGrassLoader();

	if (grass != nullptr)
	{
		auto layers = grass->getLayerList();
		for (auto it = layers.begin(); it != layers.end(); ++it)
		{
			GrassLayer* layer = *it;
			if (layer->getOriginalMaterial() == origMat)
			{
				MaterialPtr _mat = layer->getMaterial();
				if (MaterialManager::getSingleton().resourceExists(_mat->getName(), _mat->getGroup()))
					MaterialManager::getSingleton().remove(_mat->getName(), _mat->getGroup());

				layer->setMaterialName(origMat->getName());
			}
		}
	}

	if (detail != nullptr)
	{
		auto lst = detail->getDetailLevels();

		for (auto it = lst.begin(); it != lst.end(); ++it)
		{
			GeometryPageManager* mgr = *it;
			auto pages = mgr->getLoadedPages();

			for (auto _t = pages.begin(); _t != pages.end(); ++_t)
			{
				GeometryPage* page = *_t;

				if (dynamic_cast<BatchPage*>(page))
				{
					BatchPage* bp = (BatchPage*)page;
					BatchedGeometry* bg = bp->getBatchedGeometry();

					auto _i = bg->getSubBatchIterator();
					while (_i.hasMoreElements())
					{
						BatchedGeometry::SubBatch* sb = _i.getNext();
						MaterialPtr sbMat = sb->getMaterial();
						if (sbMat != nullptr)
						{
							SubEntity* ent = sb->getSubEntity();
							if (ent != nullptr)
							{
								if (ent->getOriginalMaterial() == origMat)
									sb->updateMaterial();
							}
						}
					}
				}
			}
		}
	}

	if (mat != nullptr)
	{
		if (mat->getName() != "BaseWhite")
		{
			if (MaterialManager::getSingleton().resourceExists(mat->getName(), "Assets"))
			{
				MaterialManager::getSingleton().unload(mat->getName(), "Assets");
				MaterialManager::getSingleton().remove(mat->getName(), "Assets");
			}
		}
	}

	SceneManager::SkyBoxGenParameters params = GetEngine->GetSceneManager()->getSkyBoxGenParameters();
	MaterialPtr _mat = GetEngine->GetSceneManager()->getSkyBoxMaterial();

	if (_mat != nullptr)
		GetEngine->GetSceneManager()->setSkyBox(true, _mat->getName(), params.skyBoxDistance, true, Quaternion::IDENTITY, "Assets");
	else
		GetEngine->GetSceneManager()->setSkyBox(false, "", params.skyBoxDistance, true, Quaternion::IDENTITY, "Assets");

	updateMaterialProps();
	MainWindow::AddOnEndUpdateCallback([=]() {
		if (onUpdateCallback != nullptr)
			onUpdateCallback();
	});
}

void MaterialEditor2::iterateParameters(GpuProgramParametersSharedPtr params, GpuNamedConstants vars, Property* mat_prop, Pass* pass, Pass* opass, MaterialPtr mat, MaterialPtr origMat)
{
	GpuConstantDefinitionMap::iterator v_it;

	for (v_it = vars.map.begin(); v_it != vars.map.end(); ++v_it)
	{
		string var_name = (*v_it).first;

		if ((*v_it).second.arraySize == 1)
		{
			if ((*v_it).first.find("[") != -1 && (*v_it).first.find("]") != -1)
			{
				bool exists = false;

				string _vname = var_name;
				_vname = _vname.replace(_vname.find("["), _vname.find("]"), "");

				for (auto it = vars.map.begin(); it != vars.map.end(); ++it)
				{
					if (it->first == _vname && it->second.arraySize > 1)
					{
						exists = true;
						break;
					}
				}

				if (!exists)
					continue;
			}
		}
		else if ((*v_it).second.arraySize > 1)
		{
			if ((*v_it).first.find("[") == string::npos && (*v_it).first.find("]") == string::npos)
			{
				continue;
			}
		}

		std::vector<string> excludedVars = {
			"lightMap",
			"useLightMap",
			"shadowBias",
			"castShadows",
			"lightPosition",
			"lightDiffuse",
			"lightSpecular",
			"lightAttenuation",
			"spotlightParams",
			"lightDirection",
			"lightPower",
			"lightType",
			"lightMode",
			"lightEnabled",
			"shadowMap0",
			"shadowMap1",
			"shadowMap2",
			"shadowMap3",
			"shadowMapPoint",
			"texMatrixScaleBias1",
			"texMatrixScaleBias2",
			"texMatrixScaleBias3",
			"shadowAASamples",
			"shadowCascadesBlending",
			"shadowCascadesCount",
			"shadowViewProj",
			"ambientColor",
			"cameraPosition",
			"nearClipDistance",
			"farClipDistance",
			"worldViewProj",
			"worldView",
			"world",
		};

		if (find(excludedVars.begin(), excludedVars.end(), var_name) != excludedVars.end())
			continue;

		//Check if the parameter already exists in the property grid.
		int sc = mat_prop->children.size();
		bool skip = false;

		for (int k = 0; k < sc; k++)
		{
			Property* cp = (Property*)mat_prop->children.at(k);
			if (cp->alias == var_name)
			{
				skip = true;
			}
		}
		if (skip) continue;  //Skip this parameter

		const GpuProgramParameters::AutoConstantEntry* autoEntry = params->findAutoConstantEntry((*v_it).first);

		if (autoEntry != nullptr)
		{
			GpuProgramParameters* p = new GpuProgramParameters();
			size_t sz = p->getNumAutoConstantDefinitions();
			delete p;

			std::vector<string> consts;
			for (int i = 0; i < sz; ++i)
			{
				std::string _n = GpuProgramParameters::AutoConstantDictionary[i].name;
				consts.push_back(_n);
			}

			PropComboBox* m_autoParam = new PropComboBox(this, var_name, consts);
			m_autoParam->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name, MaterialParamType::PT_AUTO)));
			m_autoParam->setOnChangeCallback([=](Property* prop, string val) { onChangeAutoParam(prop, val); });
			m_autoParam->setPopupMenu({ "Manual", "Auto" }, [=](TreeNode* node, int val) { onPopupSelectedParameterType(node, val); });
			m_autoParam->popupCheckedItem = 1;

			for (int i = 0; i < sz; ++i)
			{
				std::string _n = GpuProgramParameters::AutoConstantDictionary[i].name;
				if (GpuProgramParameters::AutoConstantDictionary[i].acType == autoEntry->paramType)
					m_autoParam->setCurrentItem(_n);
			}

			mat_prop->addChild(m_autoParam);
		}
		else
		{
			//Check the Sampler2D type
			if ((*v_it).second.constType == GpuConstantType::GCT_SAMPLER2D)
			{
				int* val = params->getIntPointer((*v_it).second.physicalIndex);

				std::vector<string> samplers;
				for (int i = 0; i < opass->getNumTextureUnitStates(); ++i)
					samplers.push_back(opass->getTextureUnitState(i)->getName());

				//Sampler2D property
				PropComboBox* m_param = new PropComboBox(this, var_name, samplers);
				m_param->setCurrentItem(*val);
				m_param->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name, *val)));
				m_param->setOnChangeCallback([=](Property* prop, int val) { onChangeSampler2D(prop, val); });

				mat_prop->addChild(m_param);
			}

			//Check the SamplerCube type
			if ((*v_it).second.constType == GpuConstantType::GCT_SAMPLERCUBE)
			{
				int* val = params->getIntPointer((*v_it).second.physicalIndex);

				std::vector<string> samplers;
				for (int i = 0; i < opass->getNumTextureUnitStates(); ++i)
					samplers.push_back(opass->getTextureUnitState(i)->getName());

				//SamplerCube property
				PropComboBox* m_param = new PropComboBox(this, var_name, samplers);
				m_param->setCurrentItem(*val);
				m_param->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name, *val)));
				m_param->setOnChangeCallback([=](Property* prop, int val) { onChangeSampler2D(prop, val); });

				mat_prop->addChild(m_param);
			}

			//Check the Vector4/Color type
			if ((*v_it).second.constType == GpuConstantType::GCT_FLOAT4)
			{
				float* val = params->getFloatPointer((*v_it).second.physicalIndex);

				Vector4 vec = Vector4(val[0], val[1], val[2], val[3]);

				PropVector4* group = new PropVector4(this, var_name, vec);
				group->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name)));
				group->setOnChangeCallback([=](Property* prop, Vector4 val) { onChangeVector4(prop, val); });
				group->setPopupMenu({ "Manual", "Auto" }, [=](TreeNode* node, int val) { onPopupSelectedParameterType(node, val); });
				group->popupCheckedItem = 0;

				mat_prop->addChild(group);
			}

			//Check the Vector3/Color type
			if ((*v_it).second.constType == GpuConstantType::GCT_FLOAT3)
			{
				float* val = params->getFloatPointer((*v_it).second.physicalIndex);

				Vector3 vec = Vector3(val[0], val[1], val[2]);

				PropVector3* group = new PropVector3(this, var_name, vec);
				group->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name)));
				group->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeVector3(prop, val); });
				group->setPopupMenu({ "Manual", "Auto" }, [=](TreeNode* node, int val) { onPopupSelectedParameterType(node, val); });
				group->popupCheckedItem = 0;

				mat_prop->addChild(group);
			}

			//Check the Vector2 type
			if ((*v_it).second.constType == GpuConstantType::GCT_FLOAT2)
			{
				float* val = params->getFloatPointer((*v_it).second.physicalIndex);

				Vector2 vec = Vector2(val[0], val[1]);

				PropVector2* group = new PropVector2(this, var_name, vec);
				group->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name)));
				group->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeVector2(prop, val); });
				group->setPopupMenu({ "Manual", "Auto" }, [=](TreeNode* node, int val) { onPopupSelectedParameterType(node, val); });
				group->popupCheckedItem = 0;

				mat_prop->addChild(group);
			}

			//Check the simple float type
			if ((*v_it).second.constType == GpuConstantType::GCT_FLOAT1)
			{
				float* val = params->getFloatPointer((*v_it).second.physicalIndex);
				float real_val = *val;

				PropFloat* prop = new PropFloat(this, var_name, real_val);
				prop->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name)));
				prop->setOnChangeCallback([=](Property* prop, float val) { onChangeFloat(prop, val); });
				prop->setPopupMenu({ "Manual", "Auto" }, [=](TreeNode* node, int val) { onPopupSelectedParameterType(node, val); });
				prop->popupCheckedItem = 0;

				mat_prop->addChild(prop);
			}

			//Check the simple int type
			if ((*v_it).second.constType == GpuConstantType::GCT_INT1)
			{
				int* val = params->getIntPointer((*v_it).second.physicalIndex);
				int int_val = *val;

				PropInt* prop = new PropInt(this, var_name, int_val);
				prop->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name)));
				prop->setOnChangeCallback([=](Property* prop, int val) { onChangeInteger(prop, val); });

				mat_prop->addChild(prop);
			}

			//Check the Matrix3 type
			if ((*v_it).second.constType == GpuConstantType::GCT_MATRIX_3X3)
			{
				float* val = params->getFloatPointer((*v_it).second.physicalIndex);

				Matrix3 matrix = Matrix3(val[0], val[1], val[2],
					val[3], val[4], val[5],
					val[6], val[7], val[8]);

				PropMatrix3* prop = new PropMatrix3(this, var_name, matrix);
				prop->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name)));
				prop->setOnChangeCallback([=](Property* prop, Matrix3 val) { onChangeMatrix3(prop, val); });
				prop->setPopupMenu({ "Manual", "Auto" }, [=](TreeNode* node, int val) { onPopupSelectedParameterType(node, val); });
				prop->popupCheckedItem = 0;

				mat_prop->addChild(prop);
			}

			//Check the Matrix4 type
			if ((*v_it).second.constType == GpuConstantType::GCT_MATRIX_4X4)
			{
				float* val = params->getFloatPointer((*v_it).second.physicalIndex);

				Matrix4 matrix = Matrix4(val[0], val[1], val[2], val[3],
					val[4], val[5], val[6], val[7],
					val[8], val[9], val[10], val[11],
					val[12], val[13], val[14], val[15]);

				PropMatrix4* prop = new PropMatrix4(this, var_name, matrix);
				prop->setUserData(voidCast(new MaterialPropData(material, originalMaterial, pass, var_name)));
				prop->setOnChangeCallback([=](Property* prop, Matrix4 val) { onChangeMatrix4(prop, val); });
				prop->setPopupMenu({ "Manual", "Auto" }, [=](TreeNode* node, int val) { onPopupSelectedParameterType(node, val); });
				prop->popupCheckedItem = 0;

				mat_prop->addChild(prop);
			}
		}
	}
}

void MaterialEditor2::onAddPass(TreeNode* prop)
{
	MaterialPropData* data = dataCast(prop->getUserData());
	MaterialPtr mat = data->originalMaterial;
	originalMaterial->getTechnique(0)->createPass();

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onAddPPD(TreeNode* prop)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	DialogTextInput* dialog = new DialogTextInput();
	dialog->show("Add preprocessor define", "New_PPD", [=](string val, bool okPressed)
	{
		if (okPressed)
		{
			if (!val.empty())
			{
				Pass* pass = data->pass;
				pass->SetPreprocessorDefine(val, true);

				updateMaterial(data->material, data->originalMaterial);
				serializeMaterial(data->originalMaterial);
				updateEditor();
			}
		}

		delete dialogWindow;
		dialogWindow = nullptr;
	});

	dialogWindow = dialog;
}

void MaterialEditor2::onAddTextureUnit(TreeNode* prop)
{
	if (dialogWindow != nullptr)
		return;

	MaterialPropData* data = dataCast(prop->getUserData());

	DialogTextInput * dialog = new DialogTextInput();
	dialog->show("Add texture unit", "NewTextureUnit", [=](string val, bool okPressed)
	{
		if (okPressed)
		{
			if (!val.empty())
			{
				std::string name = val;

				Pass* pass = data->pass;
				TextureUnitState* tex = pass->createTextureUnitState();
				tex->setName(name);

				updateMaterial(data->material, data->originalMaterial);
				serializeMaterial(data->originalMaterial);
				updateEditor();
			}
		}

		delete dialogWindow;
		dialogWindow = nullptr;
	});

	dialogWindow = dialog;
}

std::vector<Pass*> deleteProgram(string parentName, string _name)
{
	std::vector<Pass*> updatePasses;

	if (HighLevelGpuProgramManager::getSingleton().resourceExists(_name, "Assets"))
	{
		HighLevelGpuProgramPtr ptr = HighLevelGpuProgramManager::getSingleton().getByName(_name, "Assets");

		if (ptr->getInstanced())
		{
			MapIterator materials = MaterialManager::getSingleton().getResourceIterator();
			while (materials.hasMoreElements())
			{
				MaterialPtr mat = static_pointer_cast<Material>(materials.getNext());

				for (int i = 0; i < mat->getNumTechniques(); ++i)
				{
					for (int j = 0; j < mat->getTechnique(i)->getNumPasses(); ++j)
					{
						Pass* _pass = mat->getTechnique(i)->getPass(j);

						if (_pass->hasVertexProgram())
						{
							if (_pass->getVertexProgramName() == _name)
							{
								_pass->setVertexProgram("", false);
								_pass->setVertexProgram(parentName, false);
								updatePasses.push_back(_pass);
							}
						}

						if (_pass->hasFragmentProgram())
						{
							if (_pass->getFragmentProgramName() == _name)
							{
								_pass->setFragmentProgram("", false);
								_pass->setFragmentProgram(parentName, false);
								updatePasses.push_back(_pass);
							}
						}
					}
				}
			}

			HighLevelGpuProgramManager::getSingleton().remove(_name, "Assets");
		}
	}

	return updatePasses;
}

std::string getPPDString(Pass* pass)
{
	std::string _defines = "";
	int jj = 0;
	int cnt = 0;

	for (auto it1 = pass->GetPreprocessorDefines().begin(); it1 != pass->GetPreprocessorDefines().end(); ++it1)
	{
		if (it1->second)
		{
			++cnt;
		}
	}

	if (cnt > 0)
	{
		for (auto it1 = pass->GetPreprocessorDefines().begin(); it1 != pass->GetPreprocessorDefines().end(); ++it1)
		{
			if (it1->second)
			{
				_defines += it1->first;

				++jj;

				if (jj < cnt)
				{
					_defines += "_";
				}
			}
		}
	}

	return _defines;
}

void MaterialEditor2::onPopupAddPPD(TreeNode* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());
	Pass* pass = data->pass;

	if (val == 0) pass->SetPreprocessorDefine("HAS_DIFFUSE", true);
	if (val == 1) pass->SetPreprocessorDefine("HAS_NORMAL", true);
	if (val == 2) pass->SetPreprocessorDefine("HAS_METALLIC", true);
	if (val == 3) pass->SetPreprocessorDefine("HAS_OCCLUSION", true);
	if (val == 4) pass->SetPreprocessorDefine("HAS_EMISSION", true);
	if (val == 5) pass->SetPreprocessorDefine("USE_IBL", true);
	if (val == 6) pass->SetPreprocessorDefine("CUTOUT", true);
	if (val == 7) pass->SetPreprocessorDefine("RECEIVE_SHADOWS", true);
	if (val == 8) onAddPPD(prop);

	if (val != 8)
	{
		updateMaterial(data->material, data->originalMaterial);
		serializeMaterial(data->originalMaterial);
		updateEditor();
	}
}

void MaterialEditor2::onPopupPPD(TreeNode* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	if (val == 0)
	{
		Pass* pass = data->pass;

		std::vector<Pass*> updatePasses;

		if (pass->GetPreprocessorDefines().size() > 0)
		{
			std::string _defines = getPPDString(pass);

			if (!_defines.empty())
			{
				if (pass->hasVertexProgram())
				{
					string _name = pass->getVertexProgramName() + "_" + _defines;
					auto del = deleteProgram(pass->getVertexProgramName(), _name);
					updatePasses.insert(updatePasses.end(), del.begin(), del.end());
				}

				if (pass->hasFragmentProgram())
				{
					string _name = pass->getFragmentProgramName() + "_" + _defines;
					auto del = deleteProgram(pass->getFragmentProgramName(), _name);
					updatePasses.insert(updatePasses.end(), del.begin(), del.end());
				}
			}
		}

		pass->RemovePreprocessorDefine(data->varName);

		for (auto it = updatePasses.begin(); it != updatePasses.end(); ++it)
			GetEngine->UpdateShadersInPass(*it);

		updatePasses.clear();

		updateMaterial(data->material, data->originalMaterial);
		serializeMaterial(data->originalMaterial);
		updateEditor();
	}
}

void MaterialEditor2::onPopupPass(TreeNode* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	//Move Up
	if (val == 0)
	{
		Technique* tech = data->originalMaterial->getTechnique(0);
		Pass* pass = data->pass;
		auto passes = tech->getPasses();
		auto it = find(passes.begin(), passes.end(), pass);
		int i = distance(passes.begin(), it);

		if (i - 1 >= 0)
			tech->movePass(i, i - 1);
	}

	//Move Down
	if (val == 1)
	{
		Technique* tech = data->originalMaterial->getTechnique(0);
		Pass* pass = data->pass;
		auto passes = tech->getPasses();
		auto it = find(passes.begin(), passes.end(), pass);
		int i = distance(passes.begin(), it);

		if (i + 1 < tech->getNumPasses())
			tech->movePass(i, i + 1);
	}

	//Delete
	if (val == 2)
	{
		Pass* pass = data->pass;
		Technique* tech = data->originalMaterial->getTechnique(0);
		auto passes = tech->getPasses();
		auto it = find(passes.begin(), passes.end(), pass);
		int i = distance(passes.begin(), it);

		tech->removePass(i);
	}

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onPopupSelectedTextureUnit(TreeNode* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	if (val == 0)
	{
		DialogTextInput* dialog = new DialogTextInput();
		dialog->show("Rename texture unit", data->textureUnitState->getName(), [=](string val, bool okPressed)
		{
			if (okPressed)
			{
				if (!val.empty())
				{
					std::string name = val;

					Pass* pass = data->pass;
					TextureUnitState* tex = pass->getTextureUnitState(data->tag);
					tex->setName(name);

					updateMaterial(data->material, data->originalMaterial);
					serializeMaterial(data->originalMaterial);
					updateEditor();
				}
			}

			delete dialogWindow;
			dialogWindow = nullptr;
		});

		dialogWindow = dialog;
	}

	if (val == 1)
	{
		Pass* pass = data->pass;
		pass->removeTextureUnitState(data->tag);

		updateMaterial(data->material, data->originalMaterial);
		serializeMaterial(data->originalMaterial);
		updateEditor();
	}
}

void MaterialEditor2::onPopupSelectedVertexShader(TreeNode* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	if (data->originalMaterial->getName() == "BaseWhite")
		return;
	
	if (val == 0)
	{
		data->pass->setVertexProgram("Shaders/DeferredPBR.glslv", false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}

	if (val == 1)
	{
		data->pass->setVertexProgram("Shaders/Diffuse.glslv", false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}

	if (val == 2)
	{
		data->pass->setVertexProgram("Shaders/BumpedSpecular.glslv", false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}

	if (val == 3)
	{
		data->pass->setVertexProgram("Shaders/Skybox.glslv", false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}

	if (val == 4)
	{
		data->pass->setGpuProgram(GpuProgramType::GPT_VERTEX_PROGRAM, GpuProgramPtr(), false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}
}

void MaterialEditor2::onPopupSelectedFragmentShader(TreeNode* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	if (val == 0)
	{
		data->pass->setFragmentProgram("Shaders/DeferredPBR.glslf", false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}

	if (val == 1)
	{
		data->pass->setFragmentProgram("Shaders/Diffuse.glslf", false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}

	if (val == 2)
	{
		data->pass->setFragmentProgram("Shaders/BumpedSpecular.glslf", false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}

	if (val == 3)
	{
		data->pass->setFragmentProgram("Shaders/Skybox.glslf", false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}

	if (val == 4)
	{
		data->pass->setGpuProgram(GpuProgramType::GPT_FRAGMENT_PROGRAM, GpuProgramPtr(), false);

		serializeMaterial(data->originalMaterial);
		updateMaterial(data->material, data->originalMaterial);
		updateEditor();
	}
}

void MaterialEditor2::onPopupSelectedParameterType(TreeNode* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	if (val == 0)
	{
		data->originalMaterial->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_FLOAT, Material::ValueVariant((int)0));

		updateMaterial(data->material, data->originalMaterial);
		serializeMaterial(data->originalMaterial);
		updateEditor();
	}
	else
	{
		if (data->originalMaterial->IsProgramSharedParameterExists(data->varName))
		{
			Material::ParameterType tp = data->originalMaterial->GetProgramSharedParameter(data->varName).type;

			if (tp != Material::ParameterType::PT_SAMPLER2D && tp != Material::ParameterType::PT_SAMPLER_CUBE && tp != Material::ParameterType::PT_INT && tp != Material::ParameterType::PT_BOOL)
			{
				data->originalMaterial->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_AUTO, Material::ValueVariant((int)0));

				updateMaterial(data->material, data->originalMaterial);
				serializeMaterial(data->originalMaterial);
				updateEditor();
			}
		}
		else
		{
			data->originalMaterial->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_AUTO, Material::ValueVariant((int)0));

			updateMaterial(data->material, data->originalMaterial);
			serializeMaterial(data->originalMaterial);
			updateEditor();
		}
	}
}

void MaterialEditor2::onDropTexture(TreeNode* prop, TreeNode* from)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	Ogre::TexturePtr tex;
	CubemapPtr cube;

	if (IO::GetFileExtension(fullPath) == "cubemap")
		cube = CubemapManager::getSingleton().load(fullPath, "Assets");
	else
		tex = TextureManager::getSingleton().load(fullPath, "Assets");

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	if (pass->getNumTextureUnitStates() > data->tag)
	{
		TextureUnitState* state = pass->getTextureUnitState(data->tag);

		if (state == NULL)
		{
			state = pass->createTextureUnitState();
			state->setName(data->textureUnitState->getName());
		}

		if (IO::GetFileExtension(fullPath) == "cubemap")
			state->setCubemap(cube);
		else
		{
			state->setCubemap(CubemapPtr());
			state->setTexture(tex);
		}
	}

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onDropVertexShader(TreeNode* prop, TreeNode* from)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	data->pass->setVertexProgram("");
	data->pass->setVertexProgram(fullPath);

	serializeMaterial(data->originalMaterial);
	updateMaterial(data->material, data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onDropFragmentShader(TreeNode* prop, TreeNode* from)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	data->pass->setFragmentProgram("");
	data->pass->setFragmentProgram(fullPath);

	serializeMaterial(data->originalMaterial);
	updateMaterial(data->material, data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onChangeRenderType(Property* prop, std::string val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Technique* tech = mat->getTechnique(0);
	tech->setName(val);
	if (val == "Forward")
		tech->setSchemeName(MaterialManager::DEFAULT_SCHEME_NAME);
	else
		tech->setSchemeName(val);

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onChangeBlendingMode(Property* prop, std::string val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	if (val == "Add")
		pass->setSceneBlending(SceneBlendType::SBT_ADD);

	if (val == "Modulate")
		pass->setSceneBlending(SceneBlendType::SBT_MODULATE);

	if (val == "Replace")
		pass->setSceneBlending(SceneBlendType::SBT_REPLACE);

	if (val == "Transparent alpha")
		pass->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_ALPHA);

	if (val == "Transparent color")
		pass->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_COLOUR);

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onChangeIteration(Property* prop, std::string val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	if (val == "Once")
		pass->setIteratePerLight(false, false);

	if (val == "Once per light")
		pass->setIteratePerLight(true, false);

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onChangeBackfaceCulling(Property* prop, bool val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	pass->setManualCullingMode(ManualCullingMode::MANUAL_CULL_NONE);

	if (val)
		pass->setCullingMode(CullingMode::CULL_CLOCKWISE);
	else
		pass->setCullingMode(CullingMode::CULL_NONE);

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onChangeDepthWrite(Property* prop, bool val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	pass->setDepthWriteEnabled(val);

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onChangeDepthCheck(Property* prop, bool val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	pass->setDepthCheckEnabled(val);

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onChangeLighting(Property* prop, bool val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	pass->setLightingEnabled(val);

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onChangePreprocessorDefine(Property* prop, bool val)
{
	MaterialPropData* data = dataCast(prop->getUserData());
	Pass* pass = data->pass;

	std::vector<Pass*> updatePasses;

	if (pass->GetPreprocessorDefines().size() > 0)
	{
		std::string _defines = getPPDString(pass);

		if (!_defines.empty())
		{
			if (pass->hasVertexProgram())
			{
				string _name = pass->getVertexProgramName() + "_" + _defines;
				auto del = deleteProgram(pass->getVertexProgramName(), _name);
				updatePasses.insert(updatePasses.end(), del.begin(), del.end());
			}

			if (pass->hasFragmentProgram())
			{
				string _name = pass->getFragmentProgramName() + "_" + _defines;
				auto del = deleteProgram(pass->getFragmentProgramName(), _name);
				updatePasses.insert(updatePasses.end(), del.begin(), del.end());
			}
		}
	}

	pass->SetPreprocessorDefine(prop->alias, val);

	for (auto it = updatePasses.begin(); it != updatePasses.end(); ++it)
		GetEngine->UpdateShadersInPass(*it);

	updatePasses.clear();

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(data->originalMaterial);
	updateEditor();
}

void MaterialEditor2::onClickTextureUnitFile(Property* prop)
{
	MaterialPropData* data = dataCast(prop->getUserData());
	TextureUnitState* state = data->textureUnitState;

	TexturePtr tex = state->_getTexturePtr();
	if (tex != nullptr)
	{
		MainWindow::GetAssetsWindow()->focusOnFile(tex->getName());
	}
}

void MaterialEditor2::onChangeSampler2D(Property* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());
	MaterialPtr mat = data->originalMaterial;

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_SAMPLER2D, Material::ValueVariant(val));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
	updateEditor();
}

void MaterialEditor2::onChangeSamplerCube(Property* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_SAMPLER_CUBE, Material::ValueVariant(val));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
	updateEditor();
}

void MaterialEditor2::onChangeVector4(Property* prop, Vector4 val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	Pass* pass = data->pass;
	MaterialPtr mat = data->originalMaterial;

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_VECTOR4, Material::ValueVariant(val));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
}

void MaterialEditor2::onChangeVector3(Property* prop, Vector3 val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	Pass* pass = data->pass;
	MaterialPtr mat = data->originalMaterial;

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_VECTOR3, Material::ValueVariant(val));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
}

void MaterialEditor2::onChangeVector2(Property* prop, Vector2 val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	Pass* pass = data->pass;
	MaterialPtr mat = data->originalMaterial;

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_VECTOR2, Material::ValueVariant(val));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
}

void MaterialEditor2::onChangeFloat(Property* prop, float val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_FLOAT, Material::ValueVariant(val));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
}

void MaterialEditor2::onChangeInteger(Property* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_INT, Material::ValueVariant(val));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
}

void MaterialEditor2::onChangeMatrix3(Property* prop, Matrix3 val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	Pass* pass = data->pass;
	MaterialPtr mat = data->originalMaterial;

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_MATRIX3, Material::ValueVariant(val));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
}

void MaterialEditor2::onChangeMatrix4(Property* prop, Matrix4 val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	Pass* pass = data->pass;
	MaterialPtr mat = data->originalMaterial;

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_MATRIX4, Material::ValueVariant(val));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
}

void MaterialEditor2::onChangeAutoParam(Property* prop, std::string val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;

	GpuProgramParameters* p = new GpuProgramParameters();
	size_t sz = p->getNumAutoConstantDefinitions();
	delete p;

	int index = 0;

	for (int i = 0; i < sz; ++i)
	{
		std::string _n = GpuProgramParameters::AutoConstantDictionary[i].name;
		if (_n == val)
		{
			index = i;
			break;
		}
	}

	mat->SetProgramSharedParameter(data->varName, Material::ParameterType::PT_AUTO, Material::ValueVariant(index));

	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(mat);
	updateEditor();
}

void MaterialEditor2::onChangeContentType(Property* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	if (pass->getNumTextureUnitStates() > data->tag)
	{
		TextureUnitState* state = pass->getTextureUnitState(data->tag);

		if (state == NULL)
		{
			state = pass->createTextureUnitState();
			state->setName(data->varName);
		}

		if (val == 0)
			state->setContentType(TextureUnitState::ContentType::CONTENT_NAMED);
		if (val == 1)
			state->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);
	}

	MaterialPtr ptr = data->originalMaterial;
	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(ptr);
	updateEditor();
}

void MaterialEditor2::onChangeMaxAnisotropy(Property* prop, int val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	if (pass->getNumTextureUnitStates() > data->tag)
	{
		TextureUnitState* state = pass->getTextureUnitState(data->tag);

		if (state == NULL)
		{
			state = pass->createTextureUnitState();
			state->setName(data->varName);
		}

		state->setTextureAnisotropy(val);
	}

	MaterialPtr ptr = data->originalMaterial;
	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(ptr);
}

void MaterialEditor2::onChangeMinFilter(Property* prop, std::string val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	if (pass->getNumTextureUnitStates() > data->tag)
	{
		TextureUnitState* state = pass->getTextureUnitState(data->tag);

		if (state == NULL)
		{
			state = pass->createTextureUnitState();
			state->setName(prop->name);
		}

		if (val == "Anisotropic")
			state->setTextureFiltering(FilterType::FT_MIN, FilterOptions::FO_ANISOTROPIC);
		if (val == "Linear")
			state->setTextureFiltering(FilterType::FT_MIN, FilterOptions::FO_LINEAR);
		if (val == "Point")
			state->setTextureFiltering(FilterType::FT_MIN, FilterOptions::FO_POINT);
		if (val == "None")
			state->setTextureFiltering(FilterType::FT_MIN, FilterOptions::FO_NONE);
	}

	MaterialPtr ptr = data->originalMaterial;
	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(ptr);
	updateEditor();
}

void MaterialEditor2::onChangeMagFilter(Property* prop, std::string val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	if (pass->getNumTextureUnitStates() > data->tag)
	{
		TextureUnitState* state = pass->getTextureUnitState(data->tag);

		if (state == NULL)
		{
			state = pass->createTextureUnitState();
			state->setName(prop->name);
		}

		if (val == "Anisotropic")
			state->setTextureFiltering(FilterType::FT_MAG, FilterOptions::FO_ANISOTROPIC);
		if (val == "Linear")
			state->setTextureFiltering(FilterType::FT_MAG, FilterOptions::FO_LINEAR);
		if (val == "Point")
			state->setTextureFiltering(FilterType::FT_MAG, FilterOptions::FO_POINT);
		if (val == "None")
			state->setTextureFiltering(FilterType::FT_MAG, FilterOptions::FO_NONE);
	}

	MaterialPtr ptr = data->originalMaterial;
	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(ptr);
	updateEditor();
}

void MaterialEditor2::onChangeMipFilter(Property* prop, std::string val)
{
	MaterialPropData* data = dataCast(prop->getUserData());

	MaterialPtr mat = data->originalMaterial;
	Pass* pass = data->pass;

	if (pass->getNumTextureUnitStates() > data->tag)
	{
		TextureUnitState* state = pass->getTextureUnitState(data->tag);

		if (state == NULL)
		{
			state = pass->createTextureUnitState();
			state->setName(prop->name);
		}

		if (val == "Anisotropic")
			state->setTextureFiltering(FilterType::FT_MIP, FilterOptions::FO_ANISOTROPIC);
		if (val == "Linear")
			state->setTextureFiltering(FilterType::FT_MIP, FilterOptions::FO_LINEAR);
		if (val == "Point")
			state->setTextureFiltering(FilterType::FT_MIP, FilterOptions::FO_POINT);
		if (val == "None")
			state->setTextureFiltering(FilterType::FT_MIP, FilterOptions::FO_NONE);
	}

	MaterialPtr ptr = data->originalMaterial;
	updateMaterial(data->material, data->originalMaterial);
	serializeMaterial(ptr);
	updateEditor();
}
