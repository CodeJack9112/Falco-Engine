#include "SelectionBox.h"
#include <OgreRenderQueue.h>
#include <OgreSceneNode.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreHighLevelGpuProgram.h>

SelectionBox::SelectionBox(const Ogre::String& name) : ManualObject(name)
{
	setRenderQueueGroup(Ogre::RenderQueueGroupID::RENDER_QUEUE_OVERLAY);
	setUseIdentityProjection(true);
	setUseIdentityView(true);
	setQueryFlags(0);
	setCastShadows(false);

	Ogre::StringStream v_src;
	v_src << "uniform mat4 cWorldViewProj;";
	v_src << "attribute vec4 vertex;";
	v_src << "void main(){";
	v_src << "gl_Position = cWorldViewProj * vertex;";
	v_src << "}";

	Ogre::StringStream f_src;
	f_src << "uniform vec4 color;";
	f_src << "void main(){";
	f_src << "gl_FragColor = color;";
	f_src << "}";

	HighLevelGpuProgramPtr ptrProgramV = HighLevelGpuProgramManager::getSingleton().createProgram("transparent_vs", "Editor", "glsl", GPT_VERTEX_PROGRAM);
	ptrProgramV->setParameter("profiles", "glsl120");
	ptrProgramV->setSource(v_src.str());
	ptrProgramV->getDefaultParameters()->setNamedAutoConstant("cWorldViewProj", GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

	HighLevelGpuProgramPtr ptrProgramF = HighLevelGpuProgramManager::getSingleton().createProgram("transparent_fs", "Editor", "glsl", GPT_FRAGMENT_PROGRAM);
	ptrProgramF->setParameter("profiles", "glsl120");
	ptrProgramF->setSource(f_src.str());
	ptrProgramF->getDefaultParameters()->setNamedConstant("color", Ogre::ColourValue(1, 1, 1, 1));

	MaterialPtr mat = MaterialManager::getSingleton().create("selection_box_border", "Editor");
	mat->setLightingEnabled(false);
	mat->setReceiveShadows(false);
	mat->setCullingMode(CullingMode::CULL_NONE);
	mat->setDepthWriteEnabled(false);
	mat->setDepthCheckEnabled(false);
	mat->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	mat->getTechnique(0)->getPass(0)->setVertexProgram("transparent_vs");
	mat->getTechnique(0)->getPass(0)->setFragmentProgram("transparent_fs");
	mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(0.9, 0.9, 1.0, 0.8));

	MaterialPtr mat1 = MaterialManager::getSingleton().create("selection_box", "Editor");
	mat1->setLightingEnabled(false);
	mat1->setReceiveShadows(false);
	mat1->setCullingMode(CullingMode::CULL_NONE);
	mat1->setDepthWriteEnabled(false);
	mat1->setDepthCheckEnabled(false);
	mat1->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	mat1->getTechnique(0)->getPass(0)->setVertexProgram("transparent_vs");
	mat1->getTechnique(0)->getPass(0)->setFragmentProgram("transparent_fs");
	mat1->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(0.7, 0.7, 0.8, 0.5));
}

SelectionBox::~SelectionBox()
{
}

void SelectionBox::setCorners(float left, float top, float right, float bottom)
{
	left = 2 * left - 1;
	right = 2 * right - 1;
	top = 1 - 2 * top;
	bottom = 1 - 2 * bottom;
	
	clear();
	setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);

	getParentSceneNode()->_setDerivedPosition(Vector3(0, 0, 0));
	getParentSceneNode()->_setDerivedOrientation(Quaternion::IDENTITY);

	setUseIdentityProjection(true);
	setUseIdentityView(true);

	begin("selection_box", Ogre::RenderOperation::OT_TRIANGLE_LIST, "Editor");
	position(left, top, 0);
	position(right, top, 0);
	position(right, bottom, 0);
	position(left, top, 0);
	position(right, bottom, 0);
	position(left, bottom, 0);
	end();

	begin("selection_box_border", Ogre::RenderOperation::OT_LINE_LIST, "Editor");
	position(left, top, 0);
	position(right, top, 0);
	position(right, top, 0);
	position(right, bottom, 0);
	position(right, bottom, 0);
	position(left, bottom, 0);
	position(left, bottom, 0);
	position(left, top, 0);
	end();
}

void SelectionBox::setCorners(const Ogre::Vector2& topLeft, const Ogre::Vector2& bottomRight)
{
	setCorners(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}
