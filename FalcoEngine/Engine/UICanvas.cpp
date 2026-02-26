#include "stdafx.h"
#include "UICanvas.h"
#include "Engine.h"
#include "GUIDGenerator.h"
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

std::string UICanvasFactory::FACTORY_TYPE_NAME = "UICanvas";
std::map<SceneManager*, UICanvas*> UICanvasFactory::uiCanvas;
SceneManager* UICanvasFactory::manager = nullptr;
const float UICanvas::UNIT_TO_PIXELS = 50.0f;
HighLevelGpuProgramPtr UICanvas::guiShaderV;
HighLevelGpuProgramPtr UICanvas::guiShaderF;

UICanvas::UICanvas(string name) : ManualObject(name)
{
	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		// Create material
		string guid = GUIDGenerator::genGuid();
		material = Ogre::MaterialManager::getSingleton().create(name + "_material_" + guid, "Assets", true);
		material->removeAllTechniques();
		Ogre::Technique* mTech = material->createTechnique();
		Ogre::Pass* mPass = mTech->createPass();
		mPass = material->getTechnique(0)->getPass(0);
		material->setReceiveShadows(false);
		material->setLightingEnabled(false);
		material->setDepthWriteEnabled(false);
		material->setCullingMode(CullingMode::CULL_NONE);
		material->setManualCullingMode(ManualCullingMode::MANUAL_CULL_NONE);
		material->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_ALPHA);

		setCastShadows(false);

		begin(material->getName(), RenderOperation::OT_LINE_STRIP, "Assets");

		float x = -5.5;
		float y = 5.5;
		float x2 = 5.5;
		float y2 = -5.5;

		position(x2, y2, 0);
		position(x2, y, 0);
		position(x, y, 0);
		position(x, y2, 0);

		index(0);
		index(1);
		index(2);
		index(3);
		index(0);

		index(3);
		index(2);
		index(1);
		index(0);
		index(3);

		end();

		setRenderQueueGroup(RENDER_QUEUE_6 - 2);
	}

	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	setBoundingBox(aabInf);

	realSize = Vector2(11, 11);

	SetReferenceScreenSize(refScreenWidth, refScreenHeight);
}

UICanvas::~UICanvas()
{
	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		if (material != nullptr)
		{
			material->unload();
			MaterialManager::getSingleton().remove(material->getHandle());
		}
	}
}

void UICanvas::SetReferenceScreenSize(int w, int h)
{
	refScreenWidth = w;
	refScreenHeight = h;

	realSize = Vector2((float)w / UNIT_TO_PIXELS, (float)h / UNIT_TO_PIXELS);

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		beginUpdate(0);

		float x = -(realSize.x / 2.0);
		float y = (realSize.y / 2.0);
		float x2 = (realSize.x / 2.0);
		float y2 = -(realSize.y / 2.0);

		position(x2, y2, 0);
		position(x2, y, 0);
		position(x, y, 0);
		position(x, y2, 0);

		index(0);
		index(1);
		index(2);
		index(3);
		index(0);

		index(3);
		index(2);
		index(1);
		index(0);
		index(3);

		end();
	}
}

Vector2 UICanvas::GetRealSize()
{
	return realSize;
}

void UICanvas::SetScaleMode(string mode)
{
	if (mode == "Scale with screen size") scaleMode = ScaleMode::ScaleWithScreenSize;
	if (mode == "Adjust with screen size") scaleMode = ScaleMode::AdjustWithScreenSize;
}

string UICanvas::GetScaleModeString()
{
	switch (scaleMode)
	{
	case ScaleMode::ScaleWithScreenSize: return "Scale with screen size";
	case ScaleMode::AdjustWithScreenSize: return "Adjust with screen size";
	}
}

const String & UICanvas::getMovableType(void) const
{
	return UICanvasFactory::FACTORY_TYPE_NAME;
}

SceneNode * UICanvas::getParentSceneNode()
{
	return MovableObject::getParentSceneNode();
}
