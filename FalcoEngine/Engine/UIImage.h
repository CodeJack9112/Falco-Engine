#pragma once

#include "OgreIncludes.h"
#include "UIElement.h"
#include <OgreRenderQueueListener.h>

#include <string>

using namespace std;
using namespace Ogre;

class UIImage : public UIElement, public RenderQueueListener
{
public:
	UIImage(string name);
	~UIImage();

private:
	struct UITexture
	{
		double x1, y1, x2, y2;
		double tx1, ty1, tx2, ty2;
		TexturePtr texHandle;
	};

	UITexture tex;

	virtual void renderQueueStarted(uint8 queueGroupId, const String& invocation, bool& skipThisInvocation);
	virtual void renderQueueEnded(uint8 queueGroupId, const String &invocation, bool &repeatThisInvocation);

	void renderBuffer();
	void createHardwareBuffer(unsigned int size);
	void destroyHardwareBuffer();

	Ogre::RenderOperation renderOp;
	Ogre::HardwareVertexBufferSharedPtr hardwareBuffer;
	HardwareVertexBufferSharedPtr colorBuffer;

private:
	TexturePtr image;

	MaterialPtr material;
	std::string imageName = "";

public:
	void SetImage(std::string imageName);	

	const String& getMovableType(void) const;
	TexturePtr GetImage() { return image; }
	string GetImageName() { return imageName; }

	virtual void SetAnchor(Vector2 anchor);
	virtual void SetSize(Vector2 sz);
	virtual void Redraw();
	virtual MaterialPtr GetMaterial() override { return material; }

	virtual void setColor(ColourValue color);
};

class UIImageFactory : public MovableObjectFactory
{
public:
	UIImageFactory() {}
	~UIImageFactory() {}

	static std::string FACTORY_TYPE_NAME;

	const String& getType(void) const { return FACTORY_TYPE_NAME; }
	uint32 getTypeFlags(void) const { return SceneManager::ENTITY_TYPE_MASK; }

	ManualObject * createInstanceImpl(const String& name, const NameValuePairList* params)
	{
		return (ManualObject*)(OGRE_NEW UIImage(name));
	}

	void destroyInstance(MovableObject * obj) override
	{
		OGRE_DELETE obj;
	}
};