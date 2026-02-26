#include "stdafx.h"
#include "UIImage.h"
#include "Engine.h"
#include "InputManager.h"
#include "Screen.h"
#include "UIManager.h"
#include "GUIDGenerator.h"
#include <OgreMaterialManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgreHardwareBufferManager.h>

std::string UIImageFactory::FACTORY_TYPE_NAME = "UIImage";
#define MINIMAL_HARDWARE_BUFFER_SIZE 120

UIImage::UIImage(string name) : UIElement(name)
{
	GetEngine->GetSceneManager()->addRenderQueueListener(this);
	hardwareBuffer.setNull();

	// Create material
	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		string guid = GUIDGenerator::genGuid();
		material = Ogre::MaterialManager::getSingleton().create(name + "_material_" + guid, "Assets", false);
		material->removeAllTechniques();
		Ogre::Technique* mTech = material->createTechnique();
		mTech->setName("Forward");
		Ogre::Pass* mPass = mTech->createPass();
		mPass = material->getTechnique(0)->getPass(0);
		material->setReceiveShadows(false);
		material->setLightingEnabled(false);
		material->setDepthWriteEnabled(false);
		material->setCullingMode(CullingMode::CULL_NONE);
		material->setManualCullingMode(ManualCullingMode::MANUAL_CULL_NONE);
		material->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_ALPHA);
		mPass->setVertexProgram("UICanvas_vertex", "Assets");
		mPass->setFragmentProgram("UICanvas_fragment", "Assets");
		mPass->getFragmentProgramParameters()->setNamedConstant("tex", 0);
		mPass->getFragmentProgramParameters()->setNamedConstant("color", getColor());

		Redraw();
	}

	if (SceneManager::createMonoObjects)
	{
		if (SceneManager::monoDomain != nullptr)
		{
			UIImage* uiElement = this;

			if (objectMono == nullptr)
			{
				objectMono = mono_object_new(SceneManager::monoDomain, SceneManager::monoImageClass);
				mono_runtime_object_init(objectMono);
				mono_field_set_value(objectMono, SceneManager::monoAttachedObjectPtrField, reinterpret_cast<void*>(&uiElement));
				gcHandle = mono_gchandle_new(objectMono, true);
			}
		}
	}
}

UIImage::~UIImage()
{
	if (!hardwareBuffer.isNull())
		destroyHardwareBuffer();

	GetEngine->GetSceneManager()->removeRenderQueueListener(this);

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		material->unload();
		MaterialManager::getSingleton().remove(material->getHandle());
	}
}

void UIImage::SetImage(std::string imageName)
{
	if (imageName.empty())
		return;

	this->imageName = imageName;

	image = Ogre::TextureManager::getSingleton().load(imageName, "Assets");

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		for (int i = 0; i < material->getNumTechniques(); ++i)
		{
			for (int j = 0; j < material->getTechnique(i)->getNumPasses(); ++j)
			{
				TextureUnitState* state = nullptr;

				if (material->getTechnique(i)->getPass(j)->getTextureUnitStates().size() > 0)
					state = material->getTechnique(i)->getPass(j)->getTextureUnitState(0);
				else
				{
					state = material->getTechnique(i)->getPass(j)->createTextureUnitState();
				}

				state->setTexture(image);
			}
		}
	}
}

void UIImage::SetAnchor(Vector2 anchor)
{
	__super::SetAnchor(anchor);

	Redraw();
}

void UIImage::SetSize(Vector2 sz)
{
	__super::SetSize(sz);

	Redraw();
}

void UIImage::Redraw()
{
	__super::Redraw();

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		clear();

		Vector2 anchor = GetAnchor() * GetSize();

		begin(material->getName(), RenderOperation::OT_TRIANGLE_LIST, "Assets");

		float width = GetSize().x;
		float height = -GetSize().y;

		position(width - anchor.x, height + anchor.y, 0);
		textureCoord(1, 1);
		position(width - anchor.x, anchor.y, 0);
		textureCoord(1, 0);
		position(-anchor.x, anchor.y, 0);
		textureCoord(0, 0);
		position(-anchor.x, height + anchor.y, 0);
		textureCoord(0, 1);

		index(0);
		index(1);
		index(2);
		index(2);
		index(1);
		index(0);

		index(0);
		index(3);
		index(2);
		index(2);
		index(3);
		index(0);

		end();
	}

	redrawBounds();
}

void UIImage::setColor(ColourValue color)
{
	__super::setColor(color);

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
	{
		for (int i = 0; i < material->getNumTechniques(); ++i)
		{
			for (int j = 0; j < material->getTechnique(i)->getNumPasses(); ++j)
			{
				if (material->getTechnique(i)->getPass(j)->hasFragmentProgram())
				{
					if (material->getTechnique(i)->getPass(j)->getFragmentProgramParameters()->_findNamedConstantDefinition("color") != nullptr)
					{
						material->getTechnique(i)->getPass(j)->getFragmentProgramParameters()->setNamedConstant("color", getColor());
					}
				}
			}
		}
	}

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		if (renderOp.vertexData == nullptr)
			return;

		RGBA _color;

		// Convert to system-specific
		Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
		rs->convertColourValue(getColor(), &_color);
		HardwareVertexBufferSharedPtr vbuf = renderOp.vertexData->vertexBufferBinding->getBuffer(1);
		RGBA* pDest = static_cast<RGBA*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
		for (int i = 0; i < (int)renderOp.vertexData->vertexCount; ++i)
			*pDest++ = _color;
		vbuf->unlock();
	}
}

const String& UIImage::getMovableType(void) const
{
	return UIImageFactory::FACTORY_TYPE_NAME;
}

void UIImage::renderQueueStarted(uint8 queueGroupId, const String & invocation, bool & skipThisInvocation)
{
	
}

void UIImage::renderQueueEnded(uint8 queueGroupId, const String & invocation, bool & repeatThisInvocation)
{
	if (!getVisible())
		return;

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		if (GetEngine->loadedScene.empty())
			return;

		if (queueGroupId == RENDER_QUEUE_8)
		{
			SceneManager* mgr = GetEngine->GetSceneManager();
			UICanvas* canvas = UICanvasFactory::uiCanvas[mgr];

			int w = Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getActualWidth();
			int h = Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getActualHeight();

			Vector2 pos = GetScreenPosition();

			double x = -1.0f + pos.x * (2.0f / w) + (1.0f / w) * Screen::GetScreenScaleFactor();
			double y = -1.0f + pos.y * (2.0f / h) + (1.0f / h) * Screen::GetScreenScaleFactor();

			double x2 = x + ((GetRealSize().x * UICanvas::UNIT_TO_PIXELS) * (2.0f / w)) * Screen::GetScreenScaleFactor();
			double y2 = y - ((GetRealSize().y * UICanvas::UNIT_TO_PIXELS) * (2.0f / h)) * Screen::GetScreenScaleFactor();

			TexturePtr img = GetImage();
			tex.texHandle = img;
			tex.x1 = x;
			tex.y1 = y;
			tex.x2 = x2;
			tex.y2 = y2;
			tex.tx1 = 0;
			tex.ty1 = 0;
			tex.tx2 = 1;
			tex.ty2 = 1;

			renderBuffer();
			setColor(getColor());
		}
	}
}

void UIImage::createHardwareBuffer(unsigned int size)
{
	Ogre::VertexDeclaration* vd;

	renderOp.vertexData = new Ogre::VertexData;
	renderOp.vertexData->vertexStart = 0;

	vd = renderOp.vertexData->vertexDeclaration;
	vd->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	vd->addElement(0, Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3), Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

	hardwareBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(vd->getVertexSize(0), size, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE, false);// use shadow buffer? no

	renderOp.vertexData->vertexBufferBinding->setBinding(0, hardwareBuffer);

	vd->addElement(1, 0, VET_COLOUR, VES_DIFFUSE);
	colorBuffer = HardwareBufferManager::getSingleton().createVertexBuffer(vd->getVertexSize(1), size, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE, false);
	renderOp.vertexData->vertexBufferBinding->setBinding(1, colorBuffer);

	renderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
	renderOp.useIndexes = false;
}

void UIImage::destroyHardwareBuffer()
{
	delete renderOp.vertexData;
	renderOp.vertexData = 0;
	hardwareBuffer.setNull();
	colorBuffer.setNull();
}

void UIImage::renderBuffer()
{
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	unsigned int newSize;

	newSize = 6;

	if (newSize<MINIMAL_HARDWARE_BUFFER_SIZE)
		newSize = MINIMAL_HARDWARE_BUFFER_SIZE;

	// grow hardware buffer if needed
	if (hardwareBuffer.isNull() || hardwareBuffer->getNumVertices()<newSize)
	{
		if (!hardwareBuffer.isNull())
			destroyHardwareBuffer();

		createHardwareBuffer(newSize);
	}

	if (tex.texHandle == nullptr) return;

	// write quads to the hardware buffer, and remember chunks
	float* buffer;
	//float z = getParentSceneNode()->index * 0.1f;
	float z = 0;

	buffer = (float*)hardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

	// 1st point (left bottom)
	*buffer = tex.x1; buffer++;
	*buffer = tex.y2; buffer++;
	*buffer = z; buffer++;
	*buffer = tex.tx1; buffer++;
	*buffer = tex.ty2; buffer++;
	// 2st point (right top)
	*buffer = tex.x2; buffer++;
	*buffer = tex.y1; buffer++;
	*buffer = z; buffer++;
	*buffer = tex.tx2; buffer++;
	*buffer = tex.ty1; buffer++;
	// 3rd point (left top)
	*buffer = tex.x1; buffer++;
	*buffer = tex.y1; buffer++;
	*buffer = z; buffer++;
	*buffer = tex.tx1; buffer++;
	*buffer = tex.ty1; buffer++;

	// 4th point (left bottom)
	*buffer = tex.x1; buffer++;
	*buffer = tex.y2; buffer++;
	*buffer = z; buffer++;
	*buffer = tex.tx1; buffer++;
	*buffer = tex.ty2; buffer++;
	// 5th point (right bottom)
	*buffer = tex.x2; buffer++;
	*buffer = tex.y1; buffer++;
	*buffer = z; buffer++;
	*buffer = tex.tx2; buffer++;
	*buffer = tex.ty1; buffer++;
	// 6th point (right top)
	*buffer = tex.x2; buffer++;
	*buffer = tex.y2; buffer++;
	*buffer = z; buffer++;
	*buffer = tex.tx2; buffer++;
	*buffer = tex.ty2; buffer++;

	hardwareBuffer->unlock();

	renderOp.vertexData->vertexCount = 6;
	renderOp.vertexData->vertexStart = 0;

	UIManager::UIRenderQueue * q = new UIManager::UIRenderQueue();
	q->index = getParentNode()->index;
	q->renderOp = renderOp;
	q->texture = tex.texHandle;
	q->rect = getScissorsRect();
	//q->renderType = UIManager::RenderType::TEXTURE;

	GetEngine->GetUIManager()->AddRenderQueue(q);
}