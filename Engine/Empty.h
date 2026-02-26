#pragma once

#include "OgreIncludes.h"
#include <OgreMovableObject.h>

using namespace Ogre;

class Empty : public MovableObject
{
public:
	Empty();
	Empty(std::string name);
	~Empty();

	Real getBoundingRadius(void) const;
	const AxisAlignedBox& getBoundingBox(void) const;
	const String& getMovableType(void) const;
	void _updateRenderQueue(RenderQueue* queue);
	void visitRenderables(Renderable::Visitor* visitor, bool debugRenderables = false);
};

class EmptyObjectFactory : public MovableObjectFactory
{
public:
	EmptyObjectFactory() {}
	~EmptyObjectFactory() {}

	static std::string FACTORY_TYPE_NAME;

	const String& getType(void) const { return FACTORY_TYPE_NAME; }

	Empty * createInstanceImpl(const String& name, const NameValuePairList* params)
	{
		return OGRE_NEW Empty(name);
	}

	void destroyInstance(MovableObject * obj) override
	{
		OGRE_DELETE obj;
	}
};