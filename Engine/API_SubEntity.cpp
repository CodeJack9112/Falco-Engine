#include "API_SubEntity.h"
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreRoot.h>
#include "Engine.h"

MonoObject* API_SubEntity::getMaterial(MonoObject* this_ptr)
{
	SubEntity* ent;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->subentity_ptr_field, reinterpret_cast<void*>(&ent));

	if (ent->getOriginalMaterial() == nullptr)
		GetEngine->UpdateMaterials(ent->getParent());

	MaterialPtr m = ent->getMaterial();
	if (!m->isLoaded())
		m->load();

	Material* mat = m.getPointer();

	if (mat != nullptr)
	{
		MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->material_class);
		mono_runtime_object_init(obj);
		mono_field_set_value(obj, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

		return obj;
	}

	return nullptr;
}

void API_SubEntity::setMaterial(MonoObject* this_ptr, MonoObject* material)
{
	SubEntity* ent;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->subentity_ptr_field, reinterpret_cast<void*>(&ent));

	Material* mat;
	mono_field_get_value(material, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	ent->setMaterial(MaterialPtr(mat));
}

MonoObject* API_SubEntity::getSharedMaterial(MonoObject* this_ptr)
{
	SubEntity* ent;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->subentity_ptr_field, reinterpret_cast<void*>(&ent));

	MaterialPtr m = ent->getOriginalMaterial();
	Material* mat = m.getPointer();

	if (mat == nullptr)
		GetEngine->UpdateMaterials(ent->getParent());

	m = ent->getOriginalMaterial();
	mat = m.getPointer();

	if (!m->isLoaded())
		m->load();

	MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->material_class);
	mono_runtime_object_init(obj);
	mono_field_set_value(obj, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	return obj;
}

void API_SubEntity::setSharedMaterial(MonoObject* this_ptr, MonoObject* material)
{
	SubEntity* ent;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->subentity_ptr_field, reinterpret_cast<void*>(&ent));

	Material* mat;
	mono_field_get_value(material, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	ent->setOriginalMaterial(MaterialPtr(mat));
}
