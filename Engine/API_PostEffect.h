#pragma once

#include "API.h"

class API_PostEffect
{

public:
	//Register methods
	static void Register()
	{
		//Compositor
		mono_add_internal_call("FalcoEngine.PostEffect::.ctor", (void*)ctor);
		mono_add_internal_call("FalcoEngine.PostEffect::get_enabled", (void*)getEnabled);
		mono_add_internal_call("FalcoEngine.PostEffect::set_enabled", (void*)setEnabled);
		mono_add_internal_call("FalcoEngine.PostEffect::INTERNAL_create", (void*)create);
		mono_add_internal_call("FalcoEngine.PostEffect::INTERNAL_createTechnique", (void*)createTechnique);
		mono_add_internal_call("FalcoEngine.PostEffect::get_camera", (void*)getCamera);

		//Compositor technique
		mono_add_internal_call("FalcoEngine.PostEffectTechnique::INTERNAL_createTexture", (void*)createTexture);
		mono_add_internal_call("FalcoEngine.PostEffectTechnique::INTERNAL_createTargetPass", (void*)createTargetPass);

		//Compositor target pass
		mono_add_internal_call("FalcoEngine.PostEffectTargetPass::INTERNAL_getInputMode", (void*)getInputMode);
		mono_add_internal_call("FalcoEngine.PostEffectTargetPass::INTERNAL_setInputMode", (void*)setInputMode);
		mono_add_internal_call("FalcoEngine.PostEffectTargetPass::get_output", (void*)getOutput);
		mono_add_internal_call("FalcoEngine.PostEffectTargetPass::set_output", (void*)setOutput);
		mono_add_internal_call("FalcoEngine.PostEffectTargetPass::get_onlyInitial", (void*)getOnlyInitial);
		mono_add_internal_call("FalcoEngine.PostEffectTargetPass::set_onlyInitial", (void*)setOnlyInitial);
		mono_add_internal_call("FalcoEngine.PostEffectTargetPass::INTERNAL_createPass", (void*)createPass);

		//Compositor pass
		mono_add_internal_call("FalcoEngine.PostEffectPass::get_materialName", (void*)getMaterialName);
		mono_add_internal_call("FalcoEngine.PostEffectPass::set_materialName", (void*)setMaterialName);
		mono_add_internal_call("FalcoEngine.PostEffectPass::INTERNAL_setInput", (void*)setInput);
		mono_add_internal_call("FalcoEngine.PostEffectPass::get_identifier", (void*)getIdentifier);
		mono_add_internal_call("FalcoEngine.PostEffectPass::set_identifier", (void*)setIdentifier);
		mono_add_internal_call("FalcoEngine.PostEffectPass::INTERNAL_getPassType", (void*)getPassType);
		mono_add_internal_call("FalcoEngine.PostEffectPass::INTERNAL_setPassType", (void*)setPassType);
		mono_add_internal_call("FalcoEngine.PostEffectPass::INTERNAL_setFirstRenderQueue", (void*)setFirstRenderQueue);
		mono_add_internal_call("FalcoEngine.PostEffectPass::INTERNAL_setLastRenderQueue", (void*)setLastRenderQueue);

		//Compositor texture
		mono_add_internal_call("FalcoEngine.PostEffectTexture::get_width", (void*)getTextureWidth);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::set_width", (void*)setTextureWidth);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::get_height", (void*)getTextureHeight);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::set_height", (void*)setTextureHeight);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::get_targetWidthScaled", (void*)getTextureTargetWidthScaled);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::set_targetWidthScaled", (void*)setTextureTargetWidthScaled);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::get_targetHeightScaled", (void*)getTextureTargetHeightScaled);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::set_targetHeightScaled", (void*)setTextureTargetHeightScaled);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::get_referenceTarget", (void*)getTextureReferenceTarget);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::set_referenceTarget", (void*)setTextureReferenceTarget);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::get_referenceName", (void*)getTextureReferenceName);
		mono_add_internal_call("FalcoEngine.PostEffectTexture::set_referenceName", (void*)setTextureReferenceName);
	}

private:
	//Constructor
	static void ctor(MonoObject* this_ptr);

	//Create
	static void create(MonoObject* this_ptr, MonoString* name);

	//Get enabled
	static bool getEnabled(MonoObject* this_ptr);

	//Set enabled
	static void setEnabled(MonoObject* this_ptr, bool enabled);

	//Get identifier
	static int getIdentifier(MonoObject* this_ptr);

	//Set identifier
	static void setIdentifier(MonoObject* this_ptr, int identifier);

	//Create technique
	static MonoObject * createTechnique(MonoObject* this_ptr);

	//Create texture
	static MonoObject* createTexture(MonoObject* this_ptr, MonoString* name);

	//Get camera
	static MonoObject* getCamera(MonoObject* this_ptr);

	//Create target pass
	static MonoObject* createTargetPass(MonoObject* this_ptr);

	//Get input mode
	static int getInputMode(MonoObject* this_ptr);

	//Set input mode
	static void setInputMode(MonoObject* this_ptr, int mode);

	//Get output name
	static MonoString* getOutput(MonoObject* this_ptr);

	//Set output
	static void setOutput(MonoObject* this_ptr, MonoString* name);

	//Get only initial
	static bool getOnlyInitial(MonoObject* this_ptr);

	//Set only initial
	static void setOnlyInitial(MonoObject* this_ptr, bool initial);

	//Create pass
	static MonoObject* createPass(MonoObject* this_ptr);

	//Get material name
	static MonoString* getMaterialName(MonoObject* this_ptr);

	//Set material name
	static void setMaterialName(MonoObject* this_ptr, MonoString* name);

	//Set input texture
	static void setInput(MonoObject* this_ptr, int index, MonoString * textureName, int mrtIndex);

	//Get pass type
	static int getPassType(MonoObject* this_ptr);

	//Set pass type
	static void setPassType(MonoObject* this_ptr, int type);

	//Get texture width
	static int getTextureWidth(MonoObject* this_ptr);

	//Set texture width
	static void setTextureWidth(MonoObject* this_ptr, int width);

	//Get texture height
	static int getTextureHeight(MonoObject* this_ptr);

	//Set texture height
	static void setTextureHeight(MonoObject* this_ptr, int height);

	//Get texture target width scaled
	static float getTextureTargetWidthScaled(MonoObject* this_ptr);

	//Set texture target width scaled
	static void setTextureTargetWidthScaled(MonoObject* this_ptr, float width);

	//Get texture target height scaled
	static float getTextureTargetHeightScaled(MonoObject* this_ptr);

	//Set texture target height scaled
	static void setTextureTargetHeightScaled(MonoObject* this_ptr, float height);

	//Get texture reference target
	static MonoString* getTextureReferenceTarget(MonoObject* this_ptr);

	//Set texture reference target
	static void setTextureReferenceTarget(MonoObject* this_ptr, MonoString* target);

	//Get texture reference name
	static MonoString* getTextureReferenceName(MonoObject* this_ptr);

	//Set texture reference name
	static void setTextureReferenceName(MonoObject* this_ptr, MonoString* name);

	//Set first render queue
	static void setFirstRenderQueue(MonoObject* this_ptr, int queue);

	//Set last render queue
	static void setLastRenderQueue(MonoObject* this_ptr, int queue);
};

