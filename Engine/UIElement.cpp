#include "stdafx.h"
#include "UIElement.h"
#include "Engine.h"
#include "Screen.h"
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include "InputManager.h"
#include "GUIDGenerator.h"

std::string UIElementFactory::FACTORY_TYPE_NAME = "UIElement";

UIElement::UIElement(string name) : ManualObject(name)
{
	//setDynamic(true);
	setCastShadows(false);

	realSize = Vector2(1, 1);

	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	setBoundingBox(aabInf);

	setRenderQueueGroup(RENDER_QUEUE_6 - 1);
	setQueryFlags(1 << 1);

	// Create material
	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		string guid = GUIDGenerator::genGuid();
		edMaterial = Ogre::MaterialManager::getSingleton().create(getName() + "_boundsMaterial_" + guid, "Assets", true);
		edMaterial->removeAllTechniques();
		Ogre::Technique* mTech = edMaterial->createTechnique();
		Ogre::Pass* mPass = mTech->createPass();
		mPass = edMaterial->getTechnique(0)->getPass(0);
		edMaterial->setReceiveShadows(false);
		edMaterial->setLightingEnabled(false);
		edMaterial->setDepthWriteEnabled(false);
		edMaterial->setCullingMode(CullingMode::CULL_NONE);
		edMaterial->setManualCullingMode(ManualCullingMode::MANUAL_CULL_NONE);
		edMaterial->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_ALPHA);
	}

	setCastShadows(false);

	redrawBounds();

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		//Subscribe mouse move event
		mouseMoveEventID = GetEngine->GetInputManager()->SubscribeMouseMoveEvent([=](int x, int y)
		{
			mousex = x;
			mousey = y;

			if (active && getVisible())
			{
				if (IsMouseHover())
				{
					if (!isMouseHover)
					{
						//Call delegate
						MonoMethodDesc* methodDesc = nullptr;
						MonoMethod* method = nullptr;

						MonoClass* mclass = GetEngine->GetMonoRuntime()->uielement_class;

						std::string methodDescStr = std::string("UIElement:CallOnMouseEnter(UIElement)").c_str();
						methodDesc = mono_method_desc_new(methodDescStr.c_str(), false);

						if (methodDesc)
						{
							method = mono_method_desc_search_in_class(methodDesc, mclass);
							mono_method_desc_free(methodDesc);

							if (method)
							{
								void* args[1] = { objectMono };

								mono_runtime_invoke(method, objectMono, args, nullptr);
							}
						}
					}

					isMouseHover = true;

					//Call delegate
					MonoMethodDesc* methodDesc = nullptr;
					MonoMethod* method = nullptr;

					MonoClass* mclass = GetEngine->GetMonoRuntime()->uielement_class;

					std::string methodDescStr = std::string("UIElement:CallOnMouseMove(UIElement)").c_str();
					methodDesc = mono_method_desc_new(methodDescStr.c_str(), false);

					if (methodDesc)
					{
						method = mono_method_desc_search_in_class(methodDesc, mclass);
						mono_method_desc_free(methodDesc);

						if (method)
						{
							void* args[1] = { objectMono };

							mono_runtime_invoke(method, objectMono, args, nullptr);
						}
					}
				}
				else
				{
					if (isMouseHover)
					{
						//Call delegate
						MonoMethodDesc* methodDesc = nullptr;
						MonoMethod* method = nullptr;

						MonoClass* mclass = GetEngine->GetMonoRuntime()->uielement_class;

						std::string methodDescStr = std::string("UIElement:CallOnMouseLeave(UIElement)").c_str();
						methodDesc = mono_method_desc_new(methodDescStr.c_str(), false);

						if (methodDesc)
						{
							method = mono_method_desc_search_in_class(methodDesc, mclass);
							mono_method_desc_free(methodDesc);

							if (method)
							{
								void* args[1] = { objectMono };

								mono_runtime_invoke(method, objectMono, args, nullptr);
							}
						}
					}

					isMouseHover = false;
				}
			}
		});

		//Subscribe mouse down event
		mouseDownEventID = GetEngine->GetInputManager()->SubscribeMouseDownEvent([=](InputManager::MouseButton mb, int x, int y)
		{
			if (active && getVisible())
			{
				if (mb == InputManager::MouseButton::MBE_LEFT)
				{
					if (IsMouseHover())
					{
						isMousePressed = true;

						//Call delegate
						MonoMethodDesc* methodDesc = nullptr;
						MonoMethod* method = nullptr;

						MonoClass* mclass = GetEngine->GetMonoRuntime()->uielement_class;

						std::string methodDescStr = std::string("UIElement:CallOnMouseDown(UIElement)").c_str();
						methodDesc = mono_method_desc_new(methodDescStr.c_str(), false);

						if (methodDesc)
						{
							method = mono_method_desc_search_in_class(methodDesc, mclass);
							mono_method_desc_free(methodDesc);

							if (method)
							{
								void* args[1] = { objectMono };

								mono_runtime_invoke(method, objectMono, args, nullptr);
							}
						}
					}
				}
			}
		});

		//Subscribe mouse up event
		mouseUpEventID = GetEngine->GetInputManager()->SubscribeMouseUpEvent([=](InputManager::MouseButton mb, int x, int y)
		{
			if (active && getVisible())
			{
				if (mb == InputManager::MouseButton::MBE_LEFT)
				{
					if (isMousePressed)
					{
						isMousePressed = false;

						//Call delegate
						MonoMethodDesc* methodDesc = nullptr;
						MonoMethod* method = nullptr;

						MonoClass* mclass = GetEngine->GetMonoRuntime()->uielement_class;

						std::string methodDescStr = std::string("UIElement:CallOnMouseUp(UIElement)").c_str();
						methodDesc = mono_method_desc_new(methodDescStr.c_str(), false);

						if (methodDesc)
						{
							method = mono_method_desc_search_in_class(methodDesc, mclass);
							mono_method_desc_free(methodDesc);

							if (method)
							{
								void* args[1] = { objectMono };

								mono_runtime_invoke(method, objectMono, args, nullptr);
							}
						}
					}
				}
			}
		});
	}
}

UIElement::~UIElement()
{
	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		edMaterial->unload();
		MaterialManager::getSingleton().remove(edMaterial->getHandle());
	}

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		//Unsubscribe mouse move event
		GetEngine->GetInputManager()->UnsubscribeMouseMoveEvent(mouseMoveEventID);
		GetEngine->GetInputManager()->UnsubscribeMouseDownEvent(mouseDownEventID);
		GetEngine->GetInputManager()->UnsubscribeMouseUpEvent(mouseUpEventID);
	}
}

void UIElement::SetAnchor(Vector2 anchor)
{
	this->anchor = anchor;
}

void UIElement::SetSize(Vector2 sz)
{
	realSize = sz;
}

void UIElement::setColor(ColourValue color)
{
	mColor = color;
}

RealRect UIElement::getScissorsRect()
{
	Vector2 screenPos = GetScreenPosition();
	Vector2 screenSize = Screen::GetScreenSize();

	float r = GetRealSize().x * UICanvas::UNIT_TO_PIXELS * Screen::GetScreenScaleFactor();
	float b = GetRealSize().y * UICanvas::UNIT_TO_PIXELS * Screen::GetScreenScaleFactor();

	return RealRect(screenPos.x, screenSize.y - screenPos.y, screenPos.x + r, screenSize.y - screenPos.y + b);
}

Vector2 UIElement::GetScreenPosition()
{
	Vector2 pos = Vector2::ZERO;

	if (getParentSceneNode() != nullptr)
	{
		pos = Vector2(getParentSceneNode()->_getDerivedPosition().x, getParentSceneNode()->_getDerivedPosition().y);
	}

	pos *= UICanvas::UNIT_TO_PIXELS;

	if (Ogre::Root::getSingleton().getRenderSystem()->_getViewport() == nullptr)
		return Vector2::ZERO;

	int w = Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getActualWidth();
	int h = Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getActualHeight();
	double fw = w;
	double fh = h;
	int refW = w;
	int refH = h;
	double i = 0;
	double k = 0;
	Vector3 cPos = Vector3::ZERO;

	Vector3 scale = Vector3(1.0, 1.0, 1.0);

	if (getParentSceneNode() != nullptr)
		scale = getParentSceneNode()->_getDerivedScale();

	SceneManager* mgr = GetEngine->GetSceneManager();
	if (UICanvasFactory::uiCanvas[mgr] != nullptr)
	{
		refW = UICanvasFactory::uiCanvas[mgr]->GetReferenceScreenWidth();
		refH = UICanvasFactory::uiCanvas[mgr]->GetReferenceScreenHeight();
		cPos = UICanvasFactory::uiCanvas[mgr]->getParentSceneNode()->_getDerivedPosition();
		pos += Vector2(-cPos.x, -cPos.y) * UICanvas::UNIT_TO_PIXELS;
	}

	double offsetW = w - refW;
	double offsetH = h - refH;

	if (refW > 0)
		fw = refW;
	if (refH > 0)
		fh = refH;

	double sclW = w / fw;
	double sclH = h / fh;

	bool adjustWithScreen = false;

	if (UICanvasFactory::uiCanvas[mgr] != nullptr)
	{
		if (UICanvasFactory::uiCanvas[mgr]->GetScaleMode() == UICanvas::ScaleMode::AdjustWithScreenSize)
		{
			sclW = 1;
			sclH = 1;
			adjustWithScreen = true;
		}
	}

	UIElement::Alignment align = GetCanvasAlignment();

	SceneNode * parent = nullptr;
	
	if (getParentSceneNode() != nullptr)
	{
		parent = getParentSceneNode()->getParentSceneNode();
	}

	/*if (parent != nullptr && parent->getName() != "Ogre/SceneRoot")
	{
		if (parent->getAttachedObject(0)->getName() == "UI_Canvas_Object_1")
		{
			realSize += Vector2(0.0000025, 0.0000025);
		}
	}*/

	if (parent != nullptr && parent->getAttachedObjects().size() > 0)
	{
		if (parent->getAttachedObject(0)->getMovableType() == UICanvasFactory::FACTORY_TYPE_NAME)
		{
			//Canvas alignment setup
			if (align == UIElement::Alignment::TopLeft)
			{
				if (!adjustWithScreen)
				{
					i = (pos.x + fw / 2) * sclH;
					k = (pos.y + fh / 2) * sclH;
				}
				else
				{
					i = (pos.x + fw / 2) * sclH;
					k = (pos.y + fh / 2) * sclH + offsetH;
				}
			}

			if (align == UIElement::Alignment::MiddleLeft)
			{
				i = (pos.x + fw / 2) * sclH;
				k = (pos.y + h / 2);
			}

			if (align == UIElement::Alignment::BottomLeft)
			{
				i = (pos.x + fw / 2) * sclH;
				k = (pos.y + fh / 2) * sclH;
			}

			if (align == UIElement::Alignment::TopMiddle)
			{
				if (!adjustWithScreen)
				{
					i = (pos.x + w / 2);
					k = ((pos.y + fh / 2) * sclH);
				}
				else
				{
					i = (pos.x + w / 2);
					k = ((pos.y + fh / 2) * sclH) + offsetH;
				}
			}

			if (align == UIElement::Alignment::TopRight)
			{
				if (oldPos != pos)
				{
					oldPos = pos;
					startX = fw - (pos.x + fw / 2);
				}

				if (!adjustWithScreen)
				{
					i = (w - startX * sclH);// (pos.x + fw / 2) + offsetW;
					k = (pos.y + fh / 2) * sclH;// + offsetH;
				}
				else
				{
					i = (w - startX * sclH);// (pos.x + fw / 2) + offsetW;
					k = (pos.y + fh / 2) * sclH + offsetH;
				}
			}

			if (align == UIElement::Alignment::MiddleRight)
			{
				if (oldPos != pos)
				{
					oldPos = pos;
					startX = fw - (pos.x + fw / 2);
				}

				i = (w - startX * sclH);//(pos.x + fw / 2) + offsetW;
				k = (pos.y + h / 2);
			}

			if (align == UIElement::Alignment::BottomRight)
			{
				if (oldPos != pos)
				{
					oldPos = pos;
					startX = fw - (pos.x + fw / 2);
				}

				i = (w - startX * sclH);//(pos.x + fw / 2) + offsetW;
				k = (pos.y + fh / 2) * sclH;
			}

			if (align == UIElement::Alignment::BottomMiddle)
			{
				i = (pos.x + w / 2);
				k = (pos.y + fh / 2) * sclH;
			}

			if (align == UIElement::Alignment::Center)
			{
				i = (pos.x + w / 2);
				k = (pos.y + h / 2);
			}
		}

		if (dynamic_cast<UIElement*>(parent->getAttachedObject(0)))
		{
			UIElement * el = ((UIElement*)parent->getAttachedObject(0));
			UIElement::Alignment palign = ((UIElement*)parent->getAttachedObject(0))->GetCanvasAlignment();

			if (align == UIElement::Alignment::TopLeft)
			{
				float pposx = el->GetScreenPosition().x;
				float pposy = el->GetScreenPosition().y;

				if (oldPos != pos)
				{
					oldPos = pos;

					float p_posx = (el->getParentSceneNode()->_getDerivedPosition().x - cPos.x - (el->GetRealSize().x * el->GetAnchor().x)) * UICanvas::UNIT_TO_PIXELS;
					float p_posy = (el->getParentSceneNode()->_getDerivedPosition().y - cPos.y + (el->GetRealSize().y * el->GetAnchor().y)) * UICanvas::UNIT_TO_PIXELS;

					startX = (p_posx - pos.x);
					startY = (p_posy - pos.y);
				}

				i = pposx - startX * sclH;
				k = pposy - startY * sclH;
			}

			if (align == UIElement::Alignment::MiddleLeft)
			{
				float pposx = el->GetScreenPosition().x;
				float pposy = el->GetScreenPosition().y - ((el->GetRealSize().y / 2) * UICanvas::UNIT_TO_PIXELS * sclH);

				if (oldPos != pos)
				{
					oldPos = pos;

					float p_posx = (el->getParentSceneNode()->_getDerivedPosition().x - cPos.x - (el->GetRealSize().x * el->GetAnchor().x)) * UICanvas::UNIT_TO_PIXELS;
					float p_posy = (el->getParentSceneNode()->_getDerivedPosition().y - cPos.y - (el->GetRealSize().y / 2 - (el->GetRealSize().y * el->GetAnchor().y))) * UICanvas::UNIT_TO_PIXELS;

					startX = (p_posx - pos.x);
					startY = (p_posy - pos.y);
				}

				i = pposx - startX * sclH;
				k = pposy - startY * sclH;
			}

			if (align == UIElement::Alignment::BottomLeft)
			{
				float pposx = el->GetScreenPosition().x;
				float pposy = el->GetScreenPosition().y - (el->GetRealSize().y * UICanvas::UNIT_TO_PIXELS * sclH);

				if (oldPos != pos)
				{
					oldPos = pos;

					float p_posx = (el->getParentSceneNode()->_getDerivedPosition().x - cPos.x - (el->GetRealSize().x * el->GetAnchor().x)) * UICanvas::UNIT_TO_PIXELS;
					float p_posy = (el->getParentSceneNode()->_getDerivedPosition().y - cPos.y - (el->GetRealSize().y - (el->GetRealSize().y * el->GetAnchor().y))) * UICanvas::UNIT_TO_PIXELS;

					startX = (p_posx - pos.x);
					startY = (p_posy - pos.y);
				}

				i = pposx - startX * sclH;
				k = pposy - startY * sclH;
			}

			if (align == UIElement::Alignment::TopMiddle)
			{
				float pposx = el->GetScreenPosition().x + ((el->GetRealSize().x / 2) * UICanvas::UNIT_TO_PIXELS * sclH);
				float pposy = el->GetScreenPosition().y;

				if (oldPos != pos)
				{
					oldPos = pos;

					float p_posx = (el->getParentSceneNode()->_getDerivedPosition().x - cPos.x + (el->GetRealSize().x / 2 - (el->GetRealSize().x * el->GetAnchor().x))) * UICanvas::UNIT_TO_PIXELS;
					float p_posy = (el->getParentSceneNode()->_getDerivedPosition().y - cPos.y + (el->GetRealSize().y * el->GetAnchor().y)) * UICanvas::UNIT_TO_PIXELS;

					startX = (p_posx - pos.x);
					startY = (p_posy - pos.y);
				}

				i = pposx - startX * sclH;
				k = pposy - startY * sclH;
			}

			if (align == UIElement::Alignment::TopRight)
			{
				float pposx = el->GetScreenPosition().x + (el->GetRealSize().x * UICanvas::UNIT_TO_PIXELS * sclH);
				float pposy = el->GetScreenPosition().y;

				if (oldPos != pos)
				{
					oldPos = pos;

					float p_posx = (el->getParentSceneNode()->_getDerivedPosition().x - cPos.x + (el->GetRealSize().x - (el->GetRealSize().x * el->GetAnchor().x))) * UICanvas::UNIT_TO_PIXELS;
					float p_posy = (el->getParentSceneNode()->_getDerivedPosition().y - cPos.y + (el->GetRealSize().y * el->GetAnchor().y)) * UICanvas::UNIT_TO_PIXELS;

					startX = (p_posx - pos.x);
					startY = (p_posy - pos.y);
				}

				i = pposx - startX * sclH;
				k = pposy - startY * sclH;
			}

			if (align == UIElement::Alignment::MiddleRight)
			{
				float pposx = el->GetScreenPosition().x + (el->GetRealSize().x * UICanvas::UNIT_TO_PIXELS * sclH);
				float pposy = el->GetScreenPosition().y - ((el->GetRealSize().y / 2) * UICanvas::UNIT_TO_PIXELS * sclH);

				if (oldPos != pos)
				{
					oldPos = pos;

					float p_posx = (el->getParentSceneNode()->_getDerivedPosition().x - cPos.x + (el->GetRealSize().x - (el->GetRealSize().x * el->GetAnchor().x))) * UICanvas::UNIT_TO_PIXELS;
					float p_posy = (el->getParentSceneNode()->_getDerivedPosition().y - cPos.y - (el->GetRealSize().y / 2 - (el->GetRealSize().y * el->GetAnchor().y))) * UICanvas::UNIT_TO_PIXELS;

					startX = (p_posx - pos.x);
					startY = (p_posy - pos.y);
				}

				i = pposx - startX * sclH;
				k = pposy - startY * sclH;
			}

			if (align == UIElement::Alignment::BottomRight)
			{
				float pposx = el->GetScreenPosition().x + (el->GetRealSize().x * UICanvas::UNIT_TO_PIXELS * sclH);
				float pposy = el->GetScreenPosition().y - (el->GetRealSize().y * UICanvas::UNIT_TO_PIXELS * sclH);

				if (oldPos != pos)
				{
					oldPos = pos;

					float p_posx = (el->getParentSceneNode()->_getDerivedPosition().x - cPos.x + (el->GetRealSize().x - (el->GetRealSize().x * el->GetAnchor().x))) * UICanvas::UNIT_TO_PIXELS;
					float p_posy = (el->getParentSceneNode()->_getDerivedPosition().y - cPos.y - (el->GetRealSize().y - (el->GetRealSize().y * el->GetAnchor().y))) * UICanvas::UNIT_TO_PIXELS;

					startX = (p_posx - pos.x);
					startY = (p_posy - pos.y);
				}

				i = pposx - startX * sclH;
				k = pposy - startY * sclH;
			}

			if (align == UIElement::Alignment::BottomMiddle)
			{
				float pposx = el->GetScreenPosition().x + ((el->GetRealSize().x / 2) * UICanvas::UNIT_TO_PIXELS * sclH);
				float pposy = el->GetScreenPosition().y - (el->GetRealSize().y * UICanvas::UNIT_TO_PIXELS * sclH);

				if (oldPos != pos)
				{
					oldPos = pos;

					float p_posx = (el->getParentSceneNode()->_getDerivedPosition().x - cPos.x + (el->GetRealSize().x / 2 - (el->GetRealSize().x * el->GetAnchor().x))) * UICanvas::UNIT_TO_PIXELS;
					float p_posy = (el->getParentSceneNode()->_getDerivedPosition().y - cPos.y - (el->GetRealSize().y - (el->GetRealSize().y * el->GetAnchor().y))) * UICanvas::UNIT_TO_PIXELS;

					startX = (p_posx - pos.x);
					startY = (p_posy - pos.y);
				}

				i = pposx - startX * sclH;
				k = pposy - startY * sclH;
			}

			if (align == UIElement::Alignment::Center)
			{
				float pposx = el->GetScreenPosition().x + (el->GetRealSize().x * el->GetAnchor().x * UICanvas::UNIT_TO_PIXELS * sclH);
				float pposy = el->GetScreenPosition().y - (el->GetRealSize().y * el->GetAnchor().y * UICanvas::UNIT_TO_PIXELS * sclH);

				if (oldPos != pos)
				{
					oldPos = pos;

					float p_posx = (el->getParentSceneNode()->_getDerivedPosition().x - cPos.x) * UICanvas::UNIT_TO_PIXELS;
					float p_posy = (el->getParentSceneNode()->_getDerivedPosition().y - cPos.y) * UICanvas::UNIT_TO_PIXELS;

					startX = (p_posx - pos.x);
					startY = (p_posy - pos.y);
				}

				i = (pposx - startX * sclH);
				k = (pposy - startY * sclH);
			}
		}
	}

	i -= GetRealSize().x * GetAnchor().x * UICanvas::UNIT_TO_PIXELS * sclH;
	k += GetRealSize().y * GetAnchor().y * UICanvas::UNIT_TO_PIXELS * sclH;

	return Vector2(i, k);
}

Vector2 UIElement::GetSize()
{
	return realSize;
}

Vector2 UIElement::GetRealSize()
{
	if (getParentSceneNode() != nullptr)
		return Vector2(realSize.x * getParentSceneNode()->_getDerivedScale().x, realSize.y * getParentSceneNode()->_getDerivedScale().y);
	else
		return Vector2(1.0, 1.0);
}

Vector2 UIElement::GetPixelSize()
{
	return GetRealSize() * UICanvas::UNIT_TO_PIXELS;
}

Vector2 UIElement::GetPixelSizeScaled()
{
	return GetRealSize() * UICanvas::UNIT_TO_PIXELS * Screen::GetScreenScaleFactor();
}

void UIElement::SetCanvasAlignment(string alignment)
{
	if (alignment == "BottomLeft") canvasAlignment = Alignment::BottomLeft;
	if (alignment == "BottomMiddle") canvasAlignment = Alignment::BottomMiddle;
	if (alignment == "BottomRight") canvasAlignment = Alignment::BottomRight;
	if (alignment == "Center") canvasAlignment = Alignment::Center;
	if (alignment == "MiddleLeft") canvasAlignment = Alignment::MiddleLeft;
	if (alignment == "MiddleRight") canvasAlignment = Alignment::MiddleRight;
	if (alignment == "TopLeft") canvasAlignment = Alignment::TopLeft;
	if (alignment == "TopMiddle") canvasAlignment = Alignment::TopMiddle;
	if (alignment == "TopRight") canvasAlignment = Alignment::TopRight;
}

string UIElement::GetCanvasAlignmentString()
{
	switch (canvasAlignment)
	{
	case Alignment::BottomLeft: return "BottomLeft";
	case Alignment::BottomMiddle: return "BottomMiddle";
	case Alignment::BottomRight: return "BottomRight";
	case Alignment::Center: return "Center";
	case Alignment::MiddleLeft: return "MiddleLeft";
	case Alignment::MiddleRight: return "MiddleRight";
	case Alignment::TopLeft: return "TopLeft";
	case Alignment::TopMiddle: return "TopMiddle";
	case Alignment::TopRight: return "TopRight";
	}
}

const String & UIElement::getMovableType(void) const
{
	return UIElementFactory::FACTORY_TYPE_NAME;
}

uint32 UIElement::getTypeFlags(void) const
{
	return SceneManager::ENTITY_TYPE_MASK;
}

void UIElement::redrawBounds()
{
	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		Vector2 anchor = GetAnchor() * GetSize();

		begin(edMaterial->getName(), RenderOperation::OT_LINE_STRIP, "Assets");

		float width = GetSize().x;
		float height = -GetSize().y;

		position(width - anchor.x, height + anchor.y, 0);
		position(width - anchor.x, anchor.y, 0);
		position(-anchor.x, anchor.y, 0);
		position(-anchor.x, height + anchor.y, 0);
		position(width - anchor.x, height + anchor.y, 0);

		position(-anchor.x, height + anchor.y, 0);
		position(-anchor.x, anchor.y, 0);
		position(width - anchor.x, anchor.y, 0);
		position(width - anchor.x, height + anchor.y, 0);
		position(-anchor.x, height + anchor.y, 0);

		/*index(0);
		index(1);
		index(2);
		index(3);
		index(0);

		index(3);
		index(2);
		index(1);
		index(0);
		index(3);*/

		end();
	}
}

bool UIElement::IsMouseHover()
{
	return (
		mousex > GetScreenPosition().x &&
		mousex < GetScreenPosition().x + GetPixelSizeScaled().x &&
		mousey > (Screen::GetScreenSize().y - GetScreenPosition().y) &&
		mousey < (Screen::GetScreenSize().y - GetScreenPosition().y) + GetPixelSizeScaled().y
		);
}