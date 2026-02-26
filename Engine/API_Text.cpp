#include "API_Text.h"
#include "UIText.h"
#include "Engine.h"
#include <Engine\StringConverter.h>
#include <iostream>

MonoString* API_Text::getText(MonoObject* this_ptr)
{
	UIText* text;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&text));

	std::string _str = CP_UNI(text->getText());
	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), _str.c_str());

	return str;
}

void API_Text::setText(MonoObject* this_ptr, MonoString* text)
{
	UIText* uitext;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uitext));

	std::string _text = (const char*)mono_string_to_utf8((MonoString*)text);
	_text = CP_SYS(_text);

	uitext->setText(_text);
}

MonoString* API_Text::getFont(MonoObject* this_ptr)
{
	UIText* text;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&text));

	std::string _str = CP_UNI(text->getFontName());
	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), _str.c_str());

	return str;
}

void API_Text::setFont(MonoObject* this_ptr, MonoString* text)
{
	UIText* uitext;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uitext));

	std::string _text = (const char*)mono_string_to_utf8((MonoString*)text);
	_text = CP_SYS(_text);

	uitext->setFontName(_text);

	if (FontManager::getSingleton().getByName(_text, "Assets") == nullptr)
	{
		std::cout << "No such font: " << _text << std::endl;
	}
}

float API_Text::getFontSize(MonoObject* this_ptr)
{
	UIText* text;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&text));

	return text->getSize();
}

void API_Text::setFontSize(MonoObject* this_ptr, float size)
{
	UIText* text;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&text));

	text->setFontSize(size);
}

int API_Text::getHorizontalAlignment(MonoObject* this_ptr)
{
	UIText* text;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&text));

	return static_cast<int>(text->getHorizontalAlignment());
}

void API_Text::setHorizontalAlignment(MonoObject* this_ptr, int alignment)
{
	UIText* text;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&text));

	text->setHorizontalAlignment(static_cast<UIText::HorizontalAlignment>(alignment));
}

int API_Text::getVerticalAlignment(MonoObject* this_ptr)
{
	UIText* text;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&text));

	return static_cast<int>(text->getVerticalAlignment());
}

void API_Text::setVerticalAlignment(MonoObject* this_ptr, int alignment)
{
	UIText* text;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&text));

	text->setVerticalAlignment(static_cast<UIText::VerticalAlignment>(alignment));
}
