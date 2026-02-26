#include "API_Button.h"
#include "UIButton.h"
#include "Engine.h"
#include <Engine\StringConverter.h>
#include <iostream>

MonoObject* API_Button::getNormalStateImage(MonoObject* this_ptr)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	TexturePtr texture = button->GetImageNormal();

	if (texture != nullptr)
	{
		MonoObject* _tex = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->texture2d_class);
		mono_runtime_object_init(_tex);
		MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");
		MonoClassField* _fileName = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_file_path");

		MonoString* _str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(texture->getName()).c_str());

		Texture* texPtr = texture.getPointer();

		mono_field_set_value(_tex, _texPtr, reinterpret_cast<void*>(&texPtr));
		mono_field_set_value(_tex, _fileName, _str);

		return _tex;
	}

	return nullptr;
}

void API_Button::setNormalStateImage(MonoObject* this_ptr, MonoObject* texture)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");

	Texture* _tex;
	mono_field_get_value(texture, _texPtr, reinterpret_cast<void*>(&_tex));

	button->SetImageNormal(_tex->getName());
}

MonoObject* API_Button::getHoverStateImage(MonoObject* this_ptr)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	TexturePtr texture = button->GetImageHover();

	if (texture != nullptr)
	{
		MonoObject* _tex = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->texture2d_class);
		mono_runtime_object_init(_tex);
		MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");
		MonoClassField* _fileName = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_file_path");

		MonoString* _str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(texture->getName()).c_str());

		Texture* texPtr = texture.getPointer();

		mono_field_set_value(_tex, _texPtr, reinterpret_cast<void*>(&texPtr));
		mono_field_set_value(_tex, _fileName, _str);

		return _tex;
	}

	return nullptr;
}

void API_Button::setHoverStateImage(MonoObject* this_ptr, MonoObject* texture)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");

	Texture* _tex;
	mono_field_get_value(texture, _texPtr, reinterpret_cast<void*>(&_tex));

	button->SetImageHover(_tex->getName());
}

MonoObject* API_Button::getPressedStateImage(MonoObject* this_ptr)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	TexturePtr texture = button->GetImagePressed();

	if (texture != nullptr)
	{
		MonoObject* _tex = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->texture2d_class);
		mono_runtime_object_init(_tex);
		MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");
		MonoClassField* _fileName = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_file_path");

		MonoString* _str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(texture->getName()).c_str());

		Texture* texPtr = texture.getPointer();

		mono_field_set_value(_tex, _texPtr, reinterpret_cast<void*>(&texPtr));
		mono_field_set_value(_tex, _fileName, _str);

		return _tex;
	}

	return nullptr;
}

void API_Button::setPressedStateImage(MonoObject* this_ptr, MonoObject* texture)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");

	Texture* _tex;
	mono_field_get_value(texture, _texPtr, reinterpret_cast<void*>(&_tex));

	button->SetImagePressed(_tex->getName());
}

MonoObject* API_Button::getDisabledStateImage(MonoObject* this_ptr)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	TexturePtr texture = button->GetImageDisabled();

	if (texture != nullptr)
	{
		MonoObject* _tex = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->texture2d_class);
		mono_runtime_object_init(_tex);
		MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");
		MonoClassField* _fileName = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_file_path");

		MonoString* _str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(texture->getName()).c_str());

		Texture* texPtr = texture.getPointer();

		mono_field_set_value(_tex, _texPtr, reinterpret_cast<void*>(&texPtr));
		mono_field_set_value(_tex, _fileName, _str);

		return _tex;
	}

	return nullptr;
}

void API_Button::setDisabledStateImage(MonoObject* this_ptr, MonoObject* texture)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	MonoClassField* _texPtr = mono_class_get_field_from_name(GetEngine->GetMonoRuntime()->texture2d_class, "_texPtr");

	Texture* _tex;
	mono_field_get_value(texture, _texPtr, reinterpret_cast<void*>(&_tex));

	button->SetImageDisabled(_tex->getName());
}

void API_Button::getNormalStateColor(MonoObject* this_ptr, API::Color* value)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	value->r = button->GetColorNormal().r;
	value->g = button->GetColorNormal().g;
	value->b = button->GetColorNormal().b;
	value->a = button->GetColorNormal().a;
}

void API_Button::setNormalStateColor(MonoObject* this_ptr, API::Color* value)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	button->SetColorNormal(ColourValue(value->r, value->g, value->b, value->a));
}

void API_Button::getHoverStateColor(MonoObject* this_ptr, API::Color* value)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	value->r = button->GetColorHover().r;
	value->g = button->GetColorHover().g;
	value->b = button->GetColorHover().b;
	value->a = button->GetColorHover().a;
}

void API_Button::setHoverStateColor(MonoObject* this_ptr, API::Color* value)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	button->SetColorHover(ColourValue(value->r, value->g, value->b, value->a));
}

void API_Button::getPressedStateColor(MonoObject* this_ptr, API::Color* value)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	value->r = button->GetColorPressed().r;
	value->g = button->GetColorPressed().g;
	value->b = button->GetColorPressed().b;
	value->a = button->GetColorPressed().a;
}

void API_Button::setPressedStateColor(MonoObject* this_ptr, API::Color* value)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	button->SetColorPressed(ColourValue(value->r, value->g, value->b, value->a));
}

void API_Button::getDisabledStateColor(MonoObject* this_ptr, API::Color* value)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	value->r = button->GetColorDisabled().r;
	value->g = button->GetColorDisabled().g;
	value->b = button->GetColorDisabled().b;
	value->a = button->GetColorDisabled().a;
}

void API_Button::setDisabledStateColor(MonoObject* this_ptr, API::Color* value)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	button->SetColorDisabled(ColourValue(value->r, value->g, value->b, value->a));
}

bool API_Button::getInteractable(MonoObject* this_ptr)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	return button->GetInteractable();
}

void API_Button::setInteractable(MonoObject* this_ptr, bool value)
{
	UIButton* button;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&button));

	button->SetInteractable(value);
}
