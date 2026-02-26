#pragma once

#include "OgreIncludes.h"
#include <OgreSceneManager.h>
#include <string>

using namespace Ogre;
using namespace std;

class UIElement : public ManualObject
{
public:
	enum Alignment { TopLeft, MiddleLeft, BottomLeft, TopRight, MiddleRight, BottomRight, TopMiddle, BottomMiddle, Center };

private:
	Vector2 realSize;
	Vector2 anchor = Vector2(0.5, 0.5);

	Alignment canvasAlignment = Alignment::Center;

	Vector2 oldPos = Vector2(INFINITY, INFINITY);
	float startX = 0;
	float startY = 0;

	MaterialPtr edMaterial;
	ColourValue	mColor = ColourValue::White;

	int mousex = 0;
	int mousey = 0;

	size_t mouseMoveEventID = -1;
	size_t mouseDownEventID = -1;
	size_t mouseUpEventID = -1;

	bool isMousePressed = false;
	bool isMouseHover = false;

	bool active = true;

public:
	UIElement(string name);
	virtual ~UIElement();

	Vector2 GetAnchor() { return anchor; }
	Vector2 GetScreenPosition();
	Vector2 GetSize();
	Vector2 GetRealSize();
	Vector2 GetPixelSize();
	Vector2 GetPixelSizeScaled();

	void SetCanvasAlignment(string alignment);
	void SetCanvasAlignment(Alignment alignment) { canvasAlignment = alignment; }
	string GetCanvasAlignmentString();
	Alignment GetCanvasAlignment() { return canvasAlignment; }

	virtual void SetAnchor(Vector2 anchor);
	virtual void SetSize(Vector2 sz);
	virtual void Redraw() {}
	virtual MaterialPtr GetMaterial() { return MaterialPtr(); }
	virtual void setColor(ColourValue color);
	ColourValue getColor() { return mColor; }
	RealRect getScissorsRect();

	//Overrides
	const String& getMovableType(void) const;
	uint32 getTypeFlags(void) const;

	void redrawBounds();

	bool IsMouseHover();
	bool getActive() { return active; }
	void setActive(bool value) { active = value; }
};

class UIElementFactory : public MovableObjectFactory
{
public:
	UIElementFactory() {}
	~UIElementFactory() {}

	static std::string FACTORY_TYPE_NAME;

	const String& getType(void) const { return FACTORY_TYPE_NAME; }
	uint32 getTypeFlags(void) const { return SceneManager::ENTITY_TYPE_MASK; }

	ManualObject * createInstanceImpl(const String& name, const NameValuePairList* params)
	{
		return (ManualObject*)(OGRE_NEW UIElement(name));
	}

	void destroyInstance(MovableObject * obj) override
	{
		OGRE_DELETE obj;
	}
};