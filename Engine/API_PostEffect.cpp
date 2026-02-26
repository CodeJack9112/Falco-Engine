#include "API_PostEffect.h"
#include "Engine.h"
#include "OgreCompositorInstance.h"
#include "OgreCompositorManager.h"
#include "OgreCompositor.h"
#include "OgreCompositionPass.h"
#include "OgreCompositionTargetPass.h"
#include "OgreCompositorChain.h"
#include "OgreCamera.h"
#include "PostEffectListener.h"
#include "StringConverter.h"
#include <iostream>

using namespace Ogre;

void API_PostEffect::ctor(MonoObject* this_ptr)
{
	
}

void API_PostEffect::create(MonoObject* this_ptr, MonoString* name)
{
	SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));

	std::string compName = (const char*)mono_string_to_utf8((MonoString*)name);
	compName = CP_SYS(compName);

	try
	{
		CompositorPtr compositor = CompositorManager::getSingleton().create(compName, "Assets");

		Compositor* comp = compositor.getPointer();
		CompositorInstance* compInstance = nullptr;

		mono_field_set_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_this_ptr_field, reinterpret_cast<void*>(&comp));
		mono_field_set_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_instance_this_ptr_field, reinterpret_cast<void*>(&compInstance));
	}
	catch(Exception e)
	{
		std::cout << "Post effect with name \"" << compName << "\" already exists in the scene" << endl;
		std::cout << e.getDescription() << endl;
	}
}

bool API_PostEffect::getEnabled(MonoObject* this_ptr)
{
	MonoScript* script = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->script_ptr_field, reinterpret_cast<void*>(&script));

	if (script != nullptr)
	{
		return script->enabled;
	}

	return false;
}

void API_PostEffect::setEnabled(MonoObject* this_ptr, bool enabled)
{
	API::APIFuncs::setEnabled(this_ptr, enabled);

	CompositorInstance* compInstance;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_instance_this_ptr_field, reinterpret_cast<void*>(&compInstance));

	if (compInstance != nullptr)
	{
		compInstance->setEnabled(enabled);
	}
	else
	{
		SceneNode* node;
		Compositor* comp;
		mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->native_ptr_field, reinterpret_cast<void*>(&node));
		mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_this_ptr_field, reinterpret_cast<void*>(&comp));

		comp->load();

		if (node->getAttachedObjects().size() > 0)
		{
			if (node->getAttachedObject(0)->getMovableType() == "Camera")
			{
				Camera* camera = (Camera*)node->getAttachedObject(0);

				CompositorInstance* compInstance = CompositorManager::getSingleton().addCompositor(camera->getViewport(), comp->getName());

				if (compInstance != nullptr)
				{
					PostEffectListener* listener = new PostEffectListener();
					listener->setPostEffectMono(this_ptr);

					compInstance->addListener(listener);

					if (compInstance != nullptr)
					{
						mono_field_set_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_instance_this_ptr_field, reinterpret_cast<void*>(&compInstance));
						compInstance->setEnabled(enabled);
					}
				}
			}
		}
	}
}

MonoObject* API_PostEffect::createTechnique(MonoObject* this_ptr)
{
	Compositor* comp;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_this_ptr_field, reinterpret_cast<void*>(&comp));

	if (comp != nullptr)
	{
		CompositionTechnique* tech = comp->createTechnique();
		CompositionTargetPass* tp = tech->getOutputTargetPass();

		MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->posteffect_technique_class);
		mono_runtime_object_init(obj);
		mono_field_set_value(obj, GetEngine->GetMonoRuntime()->posteffect_technique_ptr_field, reinterpret_cast<void*>(&tech));

		MonoObject* obj2 = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->posteffect_target_pass_class);
		mono_runtime_object_init(obj2);
		mono_field_set_value(obj2, GetEngine->GetMonoRuntime()->posteffect_target_pass_ptr_field, reinterpret_cast<void*>(&tp));

		mono_field_set_value(obj, GetEngine->GetMonoRuntime()->posteffect_technique_output_pass_field, obj2);

		return obj;
	}

	return nullptr;
}

MonoObject* API_PostEffect::createTexture(MonoObject* this_ptr, MonoString* name)
{
	CompositionTechnique* tech;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_technique_ptr_field, reinterpret_cast<void*>(&tech));

	std::string texName = (const char*)mono_string_to_utf8((MonoString*)name);
	texName = CP_SYS(texName);

	CompositionTechnique::TextureDefinition* def = tech->createTextureDefinition(texName);
	def->width = 0;
	def->height = 0;
	def->formatList.push_back(Ogre::PF_R8G8B8);

	MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->posteffect_texture_class);
	mono_runtime_object_init(obj);
	mono_field_set_value(obj, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	return obj;
}

MonoObject* API_PostEffect::getCamera(MonoObject* this_ptr)
{
	CompositorInstance* comp;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_instance_this_ptr_field, reinterpret_cast<void*>(&comp));
	Camera* cam = comp->getChain()->getViewport()->getCamera();

	return cam->objectMono;
}

MonoObject* API_PostEffect::createTargetPass(MonoObject* this_ptr)
{
	CompositionTechnique* tech;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_technique_ptr_field, reinterpret_cast<void*>(&tech));

	CompositionTargetPass* tp = tech->createTargetPass();
	tp->setShadowsEnabled(false);
	
	MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->posteffect_target_pass_class);
	mono_runtime_object_init(obj);
	mono_field_set_value(obj, GetEngine->GetMonoRuntime()->posteffect_target_pass_ptr_field, reinterpret_cast<void*>(&tp));

	return obj;
}

int API_PostEffect::getInputMode(MonoObject* this_ptr)
{
	CompositionTargetPass* tp;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_target_pass_ptr_field, reinterpret_cast<void*>(&tp));

	return static_cast<int>(tp->getInputMode());
}

void API_PostEffect::setInputMode(MonoObject* this_ptr, int mode)
{
	CompositionTargetPass* tp;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_target_pass_ptr_field, reinterpret_cast<void*>(&tp));

	return tp->setInputMode(static_cast<CompositionTargetPass::InputMode>(mode));
}

MonoString* API_PostEffect::getOutput(MonoObject* this_ptr)
{
	CompositionTargetPass* tp;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_target_pass_ptr_field, reinterpret_cast<void*>(&tp));

	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(tp->getOutputName()).c_str());

	return str;
}

void API_PostEffect::setOutput(MonoObject* this_ptr, MonoString* name)
{
	CompositionTargetPass* tp;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_target_pass_ptr_field, reinterpret_cast<void*>(&tp));

	std::string texName = (const char*)mono_string_to_utf8((MonoString*)name);
	texName = CP_SYS(texName);

	tp->setOutputName(texName);
}

bool API_PostEffect::getOnlyInitial(MonoObject* this_ptr)
{
	CompositionTargetPass* tp;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_target_pass_ptr_field, reinterpret_cast<void*>(&tp));

	return tp->getOnlyInitial();
}

void API_PostEffect::setOnlyInitial(MonoObject* this_ptr, bool initial)
{
	CompositionTargetPass* tp;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_target_pass_ptr_field, reinterpret_cast<void*>(&tp));

	tp->setOnlyInitial(initial);
}

MonoObject* API_PostEffect::createPass(MonoObject* this_ptr)
{
	CompositionTargetPass* tp;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_target_pass_ptr_field, reinterpret_cast<void*>(&tp));

	CompositionPass* p = tp->createPass();
	p->setType(CompositionPass::PassType::PT_RENDERQUAD);
	p->setMaterialScheme(MaterialManager::DEFAULT_SCHEME_NAME);
	
	MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->posteffect_pass_class);
	mono_runtime_object_init(obj);
	mono_field_set_value(obj, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	return obj;
}

int API_PostEffect::getIdentifier(MonoObject* this_ptr)
{
	CompositionPass* p;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	return p->getIdentifier();
}

void API_PostEffect::setIdentifier(MonoObject* this_ptr, int identifier)
{
	CompositionPass* p;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	p->setIdentifier(identifier);
}

MonoString* API_PostEffect::getMaterialName(MonoObject* this_ptr)
{
	CompositionPass* p;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	MonoString* str = nullptr;
	
	if (p->getMaterial() != nullptr)
		str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(p->getMaterial()->getName()).c_str());

	return str;
}

void API_PostEffect::setMaterialName(MonoObject* this_ptr, MonoString* name)
{
	CompositionPass* p;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	std::string matName = (const char*)mono_string_to_utf8((MonoString*)name);
	matName = CP_SYS(matName);

	if (MaterialManager::getSingleton().resourceExists(matName, "Assets"))
	{
		MaterialPtr mat = MaterialManager::getSingleton().getByName(matName, "Assets");
		mat->load();

		p->setMaterial(mat);
	}
	else
		std::cout << "Material \"" << matName << "\" not found" << endl;
}

void API_PostEffect::setInput(MonoObject* this_ptr, int index, MonoString* textureName, int mrtIndex)
{
	CompositionPass* p;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	std::string texName = (const char*)mono_string_to_utf8((MonoString*)textureName);
	texName = CP_SYS(texName);

	p->setInput(index, texName, mrtIndex);
}

int API_PostEffect::getPassType(MonoObject* this_ptr)
{
	CompositionPass* p;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	return static_cast<int>(p->getType());
}

void API_PostEffect::setPassType(MonoObject* this_ptr, int type)
{
	CompositionPass* p;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	p->setType(static_cast<CompositionPass::PassType>(type));
}

int API_PostEffect::getTextureWidth(MonoObject* this_ptr)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	return def->width;
}

void API_PostEffect::setTextureWidth(MonoObject* this_ptr, int width)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	def->width = width;
}

int API_PostEffect::getTextureHeight(MonoObject* this_ptr)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	return def->height;
}

void API_PostEffect::setTextureHeight(MonoObject* this_ptr, int height)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	def->height = height;
}

float API_PostEffect::getTextureTargetWidthScaled(MonoObject* this_ptr)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	return def->widthFactor;
}

void API_PostEffect::setTextureTargetWidthScaled(MonoObject* this_ptr, float width)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	def->widthFactor = width;
}

float API_PostEffect::getTextureTargetHeightScaled(MonoObject* this_ptr)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	return def->heightFactor;
}

void API_PostEffect::setTextureTargetHeightScaled(MonoObject* this_ptr, float height)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	def->heightFactor = height;
}

MonoString* API_PostEffect::getTextureReferenceTarget(MonoObject* this_ptr)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	MonoString * str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(def->refCompName).c_str());

	return str;
}

void API_PostEffect::setTextureReferenceTarget(MonoObject* this_ptr, MonoString* target)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	std::string str = (const char*)mono_string_to_utf8((MonoString*)target);
	str = CP_SYS(str);

	def->refCompName = str;
}

MonoString* API_PostEffect::getTextureReferenceName(MonoObject* this_ptr)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(def->refTexName).c_str());

	return str;
}

void API_PostEffect::setTextureReferenceName(MonoObject* this_ptr, MonoString* name)
{
	CompositionTechnique::TextureDefinition* def;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_texture_ptr_field, reinterpret_cast<void*>(&def));

	std::string str = (const char*)mono_string_to_utf8((MonoString*)name);
	str = CP_SYS(str);

	def->refTexName = str;
}

void API_PostEffect::setFirstRenderQueue(MonoObject* this_ptr, int queue)
{
	CompositionPass* p;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	p->setFirstRenderQueue(queue);
}

void API_PostEffect::setLastRenderQueue(MonoObject* this_ptr, int queue)
{
	CompositionPass* p;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->posteffect_pass_ptr_field, reinterpret_cast<void*>(&p));

	p->setLastRenderQueue(queue);
}
