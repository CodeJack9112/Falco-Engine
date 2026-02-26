#pragma once

#include "OgreIncludes.h"
#include "UIElement.h"
#include "../Ogre/source/Components/Overlay/include/OgreFontManager.h"
#include <OgreRenderQueueListener.h>

#include <string>

using namespace std;
using namespace Ogre;

class UIText : public UIElement, public RenderQueueListener
{
public:
	enum VerticalAlignment { V_TOP, V_MIDDLE, V_BOTTOM };
	enum HorizontalAlignment { H_LEFT, H_CENTER, H_RIGHT };

	UIText(string name);
	~UIText();

	const String& getMovableType(void) const;

protected:
	String mFontName = "";
	String mFontGuid = "";
	String mText;

	RenderOperation mRenderOp;

	Real mCharHeight;
	Real mSpaceWidth;

	bool _begin = true;

	Ogre::Font * mpFont;
	MaterialPtr mpMaterial;

	VerticalAlignment vAlign = VerticalAlignment::V_MIDDLE;
	HorizontalAlignment hAlign = HorizontalAlignment::H_CENTER;

public:
	// Set settings
	void setFontName(const String &fontName);
	void setText(const String &caption);
	virtual void setColor(ColourValue color);
	void setFontSize(Real height);
	void setSpaceWidth(Real width);
	VerticalAlignment getVerticalAlignment() { return vAlign; }
	HorizontalAlignment getHorizontalAlignment() { return hAlign; }
	void setVerticalAlignment(VerticalAlignment alignment);
	void setHorizontalAlignment(HorizontalAlignment alignment);

	// Get settings
	const String &getFontName()	const { return mFontName; }
	const String &getFontGuid()	const { return mFontGuid; }
	const String &getText()	const { return mText; }

	Real getSize() const;
	Real getSpaceWidth() const { return mSpaceWidth; }
	virtual void SetAnchor(Vector2 anchor);
	virtual void SetSize(Vector2 sz);
	virtual void Redraw();
	virtual MaterialPtr GetMaterial() override { return mpMaterial; }

	virtual MeshPtr convertToMesh(const String& meshName, const String& groupName = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	void update() { _setupGeometry(); }

protected:

	void _setupGeometry();
	void _updateColors();

	virtual void renderQueueStarted(uint8 queueGroupId, const String& invocation, bool& skipThisInvocation);
	virtual void renderQueueEnded(uint8 queueGroupId, const String &invocation, bool &repeatThisInvocation);
};

class UITextFactory : public MovableObjectFactory
{
public:
	UITextFactory() {}
	~UITextFactory() {}

	static std::string FACTORY_TYPE_NAME;

	const String& getType(void) const { return FACTORY_TYPE_NAME; }
	uint32 getTypeFlags(void) const { return SceneManager::ENTITY_TYPE_MASK; }

	ManualObject * createInstanceImpl(const String& name, const NameValuePairList* params)
	{
		return (ManualObject*)(OGRE_NEW UIText(name));
	}

	void destroyInstance(MovableObject * obj) override
	{
		OGRE_DELETE obj;
	}
};