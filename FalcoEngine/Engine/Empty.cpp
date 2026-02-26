#include "stdafx.h"
#include "Empty.h"

std::string EmptyObjectFactory::FACTORY_TYPE_NAME = "Empty";

Empty::Empty()
{
}

Empty::Empty(std::string name)
{
	mName = name;
}


Empty::~Empty()
{
	
}

Real Empty::getBoundingRadius(void) const
{
	return 1.0f;
}

const AxisAlignedBox & Empty::getBoundingBox(void) const
{
	AxisAlignedBox box;
	box.setMinimum(-1, -1, -1);
	box.setMaximum(1, 1, 1);

	return box;
}

const String & Empty::getMovableType(void) const
{
	return EmptyObjectFactory::FACTORY_TYPE_NAME;
}

void Empty::_updateRenderQueue(RenderQueue * queue)
{
}

void Empty::visitRenderables(Renderable::Visitor * visitor, bool debugRenderables)
{
}
