#include "API_UIElement.h"
#include "UIElement.h"
#include "Engine.h"
#include "Screen.h"

void API_UIElement::getColor(MonoObject* this_ptr, API::Color* out_color)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	ColourValue _color = uiElement->getColor();

	out_color->r = _color.r;
	out_color->g = _color.g;
	out_color->b = _color.b;
	out_color->a = _color.a;
}

void API_UIElement::setColor(MonoObject* this_ptr, API::Color* ref_color)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	uiElement->setColor(ColourValue(ref_color->r, ref_color->g, ref_color->b, ref_color->a));
}

void API_UIElement::getAnchor(MonoObject* this_ptr, API::Vector2* out_value)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	Vector2 _anchor = uiElement->GetAnchor();

	out_value->x = _anchor.x;
	out_value->y = _anchor.y;
}

void API_UIElement::setAnchor(MonoObject* this_ptr, API::Vector2* ref_value)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	uiElement->SetAnchor(Vector2(ref_value->x, ref_value->y));
}

int API_UIElement::getAlignment(MonoObject* this_ptr)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	return static_cast<int>(uiElement->GetCanvasAlignment());
}

void API_UIElement::setAlignment(MonoObject* this_ptr, int value)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	uiElement->SetCanvasAlignment(static_cast<UIElement::Alignment>(value));
}

void API_UIElement::getSize(MonoObject* this_ptr, API::Vector2* out_value)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	Vector2 _value = uiElement->GetSize();

	out_value->x = _value.x;
	out_value->y = _value.y;
}

void API_UIElement::setSize(MonoObject* this_ptr, API::Vector2* ref_value)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	uiElement->SetSize(Vector2(ref_value->x, ref_value->y));
}

void API_UIElement::getPosition(MonoObject* this_ptr, API::Vector2* out_value)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	Vector3 _value = (uiElement->getParentSceneNode()->_getDerivedPosition()) * UICanvas::UNIT_TO_PIXELS;

	SceneManager* mgr = GetEngine->GetSceneManager();

	if (UICanvasFactory::uiCanvas[mgr] != nullptr)
	{
		Vector3 cPos = UICanvasFactory::uiCanvas[mgr]->getParentSceneNode()->_getDerivedPosition();
		_value += Vector3(-cPos.x, -cPos.y, 0.0f) * UICanvas::UNIT_TO_PIXELS;

		_value += Vector3(UICanvasFactory::uiCanvas[mgr]->GetReferenceScreenWidth() * 0.5f, -UICanvasFactory::uiCanvas[mgr]->GetReferenceScreenHeight() * 0.5f, 0.0f);
	}

	out_value->x = _value.x;
	out_value->y = _value.y;
}

void API_UIElement::setPosition(MonoObject* this_ptr, API::Vector2* ref_value)
{
	UIElement* uiElement;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&uiElement));

	float z = uiElement->getParentSceneNode()->_getDerivedPosition().z;
	Vector3 pos = Vector3(ref_value->x / UICanvas::UNIT_TO_PIXELS, ref_value->y / UICanvas::UNIT_TO_PIXELS, z);

	SceneManager* mgr = GetEngine->GetSceneManager();

	if (UICanvasFactory::uiCanvas[mgr] != nullptr)
	{
		Vector3 cPos = UICanvasFactory::uiCanvas[mgr]->getParentSceneNode()->_getDerivedPosition();
		pos += Vector3(cPos.x, cPos.y, 0.0f);

		pos += Vector3(-UICanvasFactory::uiCanvas[mgr]->GetReferenceScreenWidth() * 0.5f, UICanvasFactory::uiCanvas[mgr]->GetReferenceScreenHeight() * 0.5f, 0.0f) / UICanvas::UNIT_TO_PIXELS;
	}

	uiElement->getParentSceneNode()->_setDerivedPosition(pos);
}