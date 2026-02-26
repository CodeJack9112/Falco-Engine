#pragma once

#include "OgreIncludes.h"
#include "UIElement.h"
#include "UIEvent.h"
#include <OgreRenderQueueListener.h>
#include <string>

using namespace std;
using namespace Ogre;

class UIButton : public UIElement, public RenderQueueListener
{
private:
	struct UITexture
	{
		double x1, y1, x2, y2;// sprite coordinates
		double tx1, ty1, tx2, ty2;// texture coordinates
		TexturePtr texHandle;// texture handle
	};

	UITexture tex;

	virtual void renderQueueStarted(uint8 queueGroupId, const String& invocation, bool& skipThisInvocation);
	virtual void renderQueueEnded(uint8 queueGroupId, const String &invocation, bool &repeatThisInvocation);

	void renderBuffer();
	void createHardwareBuffer(unsigned int size);
	void destroyHardwareBuffer();

	// ogre specifics

	RenderOperation renderOp;
	HardwareVertexBufferSharedPtr hardwareBuffer;
	HardwareVertexBufferSharedPtr colorBuffer;

private:
	TexturePtr imageNormal;
	TexturePtr imageHover;
	TexturePtr imagePressed;
	TexturePtr imageDisabled;
	TexturePtr image;

	ColourValue colorNormal = ColourValue::White;
	ColourValue colorHover = ColourValue::White;
	ColourValue colorPressed = ColourValue::White;
	ColourValue colorDisabled = ColourValue::White;
	ColourValue color = ColourValue::White;

	MaterialPtr material;
	std::string imageName = "";

	size_t mouseMoveEventID = -1;
	size_t mouseDownEventID = -1;
	size_t mouseUpEventID = -1;

	UIEvent pointerDownEvent;
	UIEvent pointerUpEvent;

	bool isMousePressed = false;
	bool interactable = true;

	TexturePtr GetImage() { return image; }
	string GetImageName() { return imageName; }

public:
	UIButton(string name);
	virtual ~UIButton();

	void SetImage(std::string imageName);
	void SetImageNormal(std::string imageName);
	void SetImageHover(std::string imageName);
	void SetImagePressed(std::string imageName);
	void SetImageDisabled(std::string imageName);

	TexturePtr GetImageNormal() { return imageNormal; }
	TexturePtr GetImageHover() { return imageHover; }
	TexturePtr GetImagePressed() { return imagePressed; }
	TexturePtr GetImageDisabled() { return imageDisabled; }

	std::string GetImageNormalName() { return imageNormal != nullptr ? imageNormal->getName() : ""; }
	std::string GetImageHoverName() { return imageHover != nullptr ? imageHover->getName() : ""; }
	std::string GetImagePressedName() { return imagePressed != nullptr ? imagePressed->getName() : ""; }
	std::string GetImageDisabledName() { return imageDisabled != nullptr ? imageDisabled->getName() : ""; }

	void SetColor(ColourValue value) { color = value; }
	void SetColorNormal(ColourValue value) { colorNormal = value; setColor(getColor()); }
	void SetColorHover(ColourValue value) { colorHover = value; }
	void SetColorPressed(ColourValue value) { colorPressed = value; }
	void SetColorDisabled(ColourValue value) { colorDisabled = value; setColor(getColor()); }

	ColourValue GetColor() { return color; }
	ColourValue GetColorNormal() { return colorNormal; }
	ColourValue GetColorHover() { return colorHover; }
	ColourValue GetColorPressed() { return colorPressed; }
	ColourValue GetColorDisabled() { return colorDisabled; }

	void SetInteractable(bool value);
	bool GetInteractable() { return interactable; }

	UIEvent GetPointerDownEvent() { return pointerDownEvent; }
	UIEvent GetPointerUpEvent() { return pointerUpEvent; }

	void SetPointerDownEvent(UIEvent eventName) { pointerDownEvent = eventName; }
	void SetPointerUpEvent(UIEvent eventName) { pointerUpEvent = eventName; }

	virtual MaterialPtr GetMaterial() override { return material; }

	virtual void SetAnchor(Vector2 anchor);
	virtual void SetSize(Vector2 sz);
	virtual void Redraw();
	const String& getMovableType(void) const;

	virtual void setColor(ColourValue color);
};

class UIButtonFactory : public MovableObjectFactory
{
public:
	UIButtonFactory() {}
	~UIButtonFactory() {}

	static std::string FACTORY_TYPE_NAME;

	const String& getType(void) const { return FACTORY_TYPE_NAME; }
	uint32 getTypeFlags(void) const { return SceneManager::ENTITY_TYPE_MASK; }

	ManualObject * createInstanceImpl(const String& name, const NameValuePairList* params)
	{
		return (ManualObject*)(OGRE_NEW UIButton(name));
	}

	void destroyInstance(MovableObject * obj) override
	{
		OGRE_DELETE obj;
	}
};