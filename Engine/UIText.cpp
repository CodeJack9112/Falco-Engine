#include "stdafx.h"
#include "UICanvas.h"
#include "UIText.h"
#include "Screen.h"
#include "Engine.h"
#include "UIManager.h"
#include <OgreMaterialManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgreHardwareBufferManager.h>
#include <Engine\ResourceMap.h>

#include "GUIDGenerator.h"

#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1

std::string UITextFactory::FACTORY_TYPE_NAME = "UIText";

UIText::UIText(string name) : UIElement(name)
{
	GetEngine->GetSceneManager()->addRenderQueueListener(this);

	//mName = name;
	mpFont = NULL;
	mText = "New Text";
	mCharHeight = 0.24;
	mSpaceWidth = 0;

	mRenderOp.vertexData = NULL;
	_begin = false;

	if (SceneManager::createMonoObjects)
	{
		if (SceneManager::monoDomain != nullptr)
		{
			UIText* uiElement = this;

			if (objectMono == nullptr)
			{
				objectMono = mono_object_new(SceneManager::monoDomain, SceneManager::monoTextClass);
				mono_runtime_object_init(objectMono);
				mono_field_set_value(objectMono, SceneManager::monoAttachedObjectPtrField, reinterpret_cast<void*>(&uiElement));
				gcHandle = mono_gchandle_new(objectMono, true);
			}
		}
	}
}

UIText::~UIText()
{
	GetEngine->GetSceneManager()->removeRenderQueueListener(this);

	if (mRenderOp.vertexData)
		delete mRenderOp.vertexData;

	if (!mpMaterial.isNull())
	{
		mpMaterial->unload();
		MaterialManager::getSingletonPtr()->remove(mpMaterial->getHandle());
	}
}

const String & UIText::getMovableType(void) const
{
	return UITextFactory::FACTORY_TYPE_NAME;
}

Real UIText::getSize() const
{
	return mCharHeight * UICanvas::UNIT_TO_PIXELS;
}

void UIText::setFontName(const String &fontName)
{
	if (fontName.empty())
		return;

	if ((Ogre::MaterialManager::getSingletonPtr()->resourceExists(mName + "Material")))
	{
		Ogre::MaterialManager::getSingleton().remove(mName + "Material");
	}

	if (mFontName != fontName || mpMaterial.isNull() || !mpFont)
	{
		this->clear();
		_begin = true;

		mFontName = fontName;

		mpFont = (Font*)FontManager::getSingleton().getByName(mFontName, "Assets").getPointer();

		if (mpFont)
		{
			mFontGuid = ResourceMap::guidMap[mpFont];

			if (!mpMaterial.isNull())
			{
				mpMaterial->unload();
				MaterialManager::getSingletonPtr()->remove(mpMaterial->getHandle());

			}
			mpFont->load();

			mpMaterial = mpFont->getMaterial()->clone(mName + "_" + GUIDGenerator::genGuid() + "_Material");
			if (!mpMaterial->isLoaded())
				mpMaterial->load();

			//mpMaterial->setDepthCheckEnabled(!mOnTop);
			mpMaterial->setDepthBias(1.0, 1.0);
			mpMaterial->setDepthWriteEnabled(false);
			mpMaterial->setLightingEnabled(false);
			mpMaterial->setReceiveShadows(false);
			mpMaterial->setCullingMode(CullingMode::CULL_NONE);
			mpMaterial->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_ALPHA);
			mpMaterial->setManualCullingMode(ManualCullingMode::MANUAL_CULL_NONE);
			mpMaterial->getTechnique(0)->setName("Forward");
			mpMaterial->getTechnique(0)->getPass(0)->setVertexProgram("UICanvas_vertex", "Assets");
			mpMaterial->getTechnique(0)->getPass(0)->setFragmentProgram("UICanvas_fragment", "Assets");
			mpMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("tex", 0);
			mpMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("color", getColor());

			//if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
				this->_setupGeometry();
		}
	}
}

void UIText::setText(const String &caption)
{
	if (caption != mText)
	{
		this->clear();
		_begin = true;

		mText = caption;
		//if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			if (!mText.empty())
				this->_setupGeometry();
		}
	}
}

void UIText::setColor(ColourValue color)
{
	if (color != getColor())
	{
		__super::setColor(color);

		for (int i = 0; i < mpMaterial->getNumTechniques(); ++i)
		{
			for (int j = 0; j < mpMaterial->getTechnique(i)->getNumPasses(); ++j)
			{
				if (mpMaterial->getTechnique(i)->getPass(j)->hasFragmentProgram())
				{
					if (mpMaterial->getTechnique(i)->getPass(j)->getFragmentProgramParameters()->_findNamedConstantDefinition("color") != nullptr)
					{
						mpMaterial->getTechnique(i)->getPass(j)->getFragmentProgramParameters()->setNamedConstant("color", getColor());
					}
				}
			}
		}

		this->_updateColors();
	}
}

void UIText::setFontSize(Real height)
{
	if (height != getSize())
	{
		this->clear();
		_begin = true;

		mCharHeight = height / UICanvas::UNIT_TO_PIXELS;
		//if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
			this->_setupGeometry();
	}
}

void UIText::setSpaceWidth(Real width)
{
	if (width != mSpaceWidth)
	{
		mSpaceWidth = width;
		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
			this->_setupGeometry();
	}
}

void UIText::setVerticalAlignment(VerticalAlignment alignment)
{
	vAlign = alignment;
	Redraw();
}

void UIText::setHorizontalAlignment(HorizontalAlignment alignment)
{
	hAlign = alignment;
	Redraw();
}

void UIText::SetAnchor(Vector2 anchor)
{
	__super::SetAnchor(anchor);

	Redraw();
}

void UIText::SetSize(Vector2 sz)
{
	__super::SetSize(sz);

	Redraw();
}

void UIText::Redraw()
{
	this->clear();
	_begin = true;

	//if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		this->_setupGeometry();
}

MeshPtr UIText::convertToMesh(const String & meshName, const String & groupName)
{
	if (this->getNumSections() > 0)
		return __super::convertToMesh(meshName, groupName);
	else
		return MeshPtr();
}

void UIText::_setupGeometry()
{
	if (mFontName.empty())
		return;

	if (!mpFont)
		return;

	//GetEngine->GetUIManager()->Cleanup();
	//GetEngine->GetUIManager()->RemoveRenderQueue(rq);

	unsigned int vertexCount = static_cast<unsigned int>(mText.size() * 6);

	//if (mRenderOp.vertexData != nullptr)
	//{
	//	delete mRenderOp.vertexData;
	//	mRenderOp.vertexData = nullptr;
	//	//this->_updateColors();
	//}
	
	if (mRenderOp.vertexData == nullptr)
		mRenderOp.vertexData = new VertexData();

	mRenderOp.indexData = 0;
	mRenderOp.vertexData->vertexStart = 0;
	mRenderOp.vertexData->vertexCount = vertexCount;
	mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST;
	mRenderOp.useIndexes = false;

	VertexDeclaration *decl = mRenderOp.vertexData->vertexDeclaration;
	VertexBufferBinding *bind = mRenderOp.vertexData->vertexBufferBinding;
	size_t offset = 0;

	// create/bind positions/tex.ccord. buffer
	if (!decl->findElementBySemantic(VES_POSITION))
		decl->addElement(POS_TEX_BINDING, offset, VET_FLOAT3, VES_POSITION);

	offset += VertexElement::getTypeSize(VET_FLOAT3);

	if (!decl->findElementBySemantic(VES_TEXTURE_COORDINATES))
		decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

	HardwareVertexBufferSharedPtr ptbuf = HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(POS_TEX_BINDING),
		mRenderOp.vertexData->vertexCount,
		HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	bind->setBinding(POS_TEX_BINDING, ptbuf);

	// Colours - store these in a separate buffer because they change less often
	if (!decl->findElementBySemantic(VES_DIFFUSE))
		decl->addElement(COLOUR_BINDING, 0, VET_COLOUR, VES_DIFFUSE);

	HardwareVertexBufferSharedPtr cbuf = HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(COLOUR_BINDING),
		mRenderOp.vertexData->vertexCount,
		HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	bind->setBinding(COLOUR_BINDING, cbuf);

	size_t charlen = mText.size();
	float *pPCBuff = static_cast<float*>(ptbuf->lock(HardwareBuffer::HBL_DISCARD));

	Vector2 pos = GetScreenPosition();

	float w = Screen::GetScreenSize().x;
	float h = Screen::GetScreenSize().y;

	double x = (-1.0f + pos.x * (2.0f / w) + (1.0f / w) * Screen::GetScreenScaleFactor());
	double y = (-1.0f + pos.y * (2.0f / h) + (1.0f / h) * Screen::GetScreenScaleFactor());

	float largestWidth = 0;
	float left = 0.0;
	float top = 0.0;

	Vector2 anchor = GetAnchor();

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		anchor = Vector2(0, 0);
	}

	Real spaceWidth = mSpaceWidth;
	// Derive space width from a capital A
	if (spaceWidth == 0)
		spaceWidth = mpFont->getGlyphAspectRatio('a') * mCharHeight;

	Vector3 currPos;

	if (vAlign == VerticalAlignment::V_MIDDLE)
	{
		top = mCharHeight * 2;

		for (auto i = mText.begin(); i != mText.end(); ++i)
		{
			if (*i == '\n')
				top += mCharHeight * 2;
		}

		top = (top * 0.5f) - (GetSize().y * 0.5f) + (GetSize().y * anchor.y);
	}

	if (vAlign == VerticalAlignment::V_TOP)
	{
		top = GetSize().y * anchor.y;
	}

	if (vAlign == VerticalAlignment::V_BOTTOM)
	{
		float textHeight = mCharHeight * 2;

		for (auto i = mText.begin(); i != mText.end(); ++i)
		{
			if (*i == '\n')
				textHeight += mCharHeight * 2;
		}

		top = (textHeight - GetSize().y) + (GetSize().y * anchor.y);
	}

	std::vector<float> linePos;
	float len = 0.0f;
	int k = 0;
	for (auto j = mText.begin(); j != mText.end(); ++j, ++k)
	{
		if (*j == ' ')
			len += spaceWidth;
		else if (*j != '\n')
			len += mpFont->getGlyphAspectRatio((unsigned char)* j) * mCharHeight * 2.0;
		if (*j == '\n' || k == mText.size() - 1)
		{
			//if (*j == '\n')
			//	len -= mpFont->getGlyphAspectRatio('\n') * mCharHeight * 2.0;

			if (hAlign == HorizontalAlignment::H_CENTER)
				linePos.push_back((len * 0.5) - (GetSize().x * 0.5) + (GetSize().x * anchor.x));

			if (hAlign == HorizontalAlignment::H_LEFT)
				linePos.push_back(GetSize().x * anchor.x);

			if (hAlign == HorizontalAlignment::H_RIGHT)
				linePos.push_back(len - GetSize().x + (GetSize().x * anchor.x));

			len = 0;
		}
	}

	if (linePos.size() > 0)
		left = -linePos[0];

	k = 0;
	int l = 0;
	for (auto i = mText.begin(); i != mText.end(); ++i, ++k)
	{
		if (*i == '\n')
		{
			top -= mCharHeight * 2.0;
			++l;

			if (linePos.size() > l)
				left = -linePos[l];
			
			mRenderOp.vertexData->vertexCount -= 6;
			continue;
		}

		if (*i == ' ')
		{
			left += spaceWidth;
			mRenderOp.vertexData->vertexCount -= 6;
			continue;
		}

		Real horiz_height = mpFont->getGlyphAspectRatio((unsigned char)*i);
		Real u1, u2, v1, v2;
		Ogre::Font::UVRect utmp;
		utmp = mpFont->getGlyphTexCoords((unsigned char)*i);
		u1 = utmp.left;
		u2 = utmp.right;
		v1 = utmp.top;
		v2 = utmp.bottom;

		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			if (_begin)
			{
				begin(mpMaterial->getName(), RenderOperation::OT_TRIANGLE_LIST, "Assets");
			}
			else
			{
				beginUpdate(k);
			}
		}

		//float z = getParentSceneNode()->index * 0.1f;
		float z = 0;

		// each vert is (x, y, z, u, v)
		//-------------------------------------------------------------------------------------
		// First tri
		//
		// Upper left
		*pPCBuff++ = x + ((left * UICanvas::UNIT_TO_PIXELS) * (2.0f / w)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = y + ((top * UICanvas::UNIT_TO_PIXELS) * (2.0f / h)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = z;
		*pPCBuff++ = u1;
		*pPCBuff++ = v1;

		// Deal with bounds
		currPos = Ogre::Vector3(left, top, 0.0);

		//Vertex
		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			position(currPos);
			textureCoord(u1, v1);
		}

		top -= mCharHeight * 2.0;

		*pPCBuff++ = x + ((left * UICanvas::UNIT_TO_PIXELS) * (2.0f / w)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = y + ((top * UICanvas::UNIT_TO_PIXELS) * (2.0f / h)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = z;
		*pPCBuff++ = u1;
		*pPCBuff++ = v2;

		// Deal with bounds
		currPos = Ogre::Vector3(left, top, 0.0);
		//Vertex
		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			position(currPos);
			textureCoord(u1, v2);
		}

		top += mCharHeight * 2.0;
		left += horiz_height * mCharHeight * 2.0;

		*pPCBuff++ = x + ((left * UICanvas::UNIT_TO_PIXELS) * (2.0f / w)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = y + ((top * UICanvas::UNIT_TO_PIXELS) * (2.0f / h)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = z;
		*pPCBuff++ = u2;
		*pPCBuff++ = v1;
		//-------------------------------------------------------------------------------------

		// Deal with bounds
		currPos = Ogre::Vector3(left, top, 0.0);

		//Vertex
		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			position(currPos);
			textureCoord(u2, v1);
		}

		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			index(0);
			index(1);
			index(2);
		}

		//-------------------------------------------------------------------------------------
		// Second tri
		//
		*pPCBuff++ = x + ((left * UICanvas::UNIT_TO_PIXELS) * (2.0f / w)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = y + ((top * UICanvas::UNIT_TO_PIXELS) * (2.0f / h)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = z;
		*pPCBuff++ = u2;
		*pPCBuff++ = v1;

		currPos = Ogre::Vector3(left, top, 0.0);

		//Vertex
		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			position(currPos);
			textureCoord(u2, v1);
		}

		top -= mCharHeight * 2.0;
		left -= horiz_height * mCharHeight * 2.0;

		*pPCBuff++ = x + ((left * UICanvas::UNIT_TO_PIXELS) * (2.0f / w)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = y + ((top * UICanvas::UNIT_TO_PIXELS) * (2.0f / h)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = z;
		*pPCBuff++ = u1;
		*pPCBuff++ = v2;

		currPos = Ogre::Vector3(left, top, 0.0);

		//Vertex
		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			position(currPos);
			textureCoord(u1, v2);
		}

		left += horiz_height * mCharHeight * 2.0;

		*pPCBuff++ = x + ((left * UICanvas::UNIT_TO_PIXELS) * (2.0f / w)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = y + ((top * UICanvas::UNIT_TO_PIXELS) * (2.0f / h)) * Screen::GetScreenScaleFactor();
		*pPCBuff++ = z;
		*pPCBuff++ = u2;
		*pPCBuff++ = v2;
		//-------------------------------------------------------------------------------------

		currPos = Ogre::Vector3(left, top, 0.0);

		//Vertex
		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			position(currPos);
			textureCoord(u2, v2);
		}

		// Go back up with top
		top += mCharHeight * 2.0;

		float currentWidth = (left + 1) / 2 - 0;
		if (currentWidth > largestWidth)
			largestWidth = currentWidth;

		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			index(3);
			index(4);
			index(5);
		}

		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
			end();
	}

	// Unlock vertex buffer
	ptbuf->unlock();

	_updateColors();

	redrawBounds();
}

void UIText::_updateColors(void)
{
	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		if (mRenderOp.vertexData == nullptr)
			return;

		// Convert to system-specific
		Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
		RGBA color;
		rs->convertColourValue(getColor(), &color);
		HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);
		RGBA* pDest = static_cast<RGBA*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
		for (int i = 0; i < (int)mRenderOp.vertexData->vertexCount; ++i)
			*pDest++ = color;
		vbuf->unlock();
		//mUpdateColors = false;
	}
}

void UIText::renderQueueStarted(uint8 queueGroupId, const String & invocation, bool & skipThisInvocation)
{
	
}

void UIText::renderQueueEnded(uint8 queueGroupId, const String & invocation, bool & repeatThisInvocation)
{
	if (!getVisible())
		return;

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		if (GetEngine->loadedScene.empty())
			return;

		if (mpMaterial == nullptr)
			return;

		if (queueGroupId == RENDER_QUEUE_8)
		{
			//_setupGeometry();

			UIManager::UIRenderQueue* rq = new UIManager::UIRenderQueue();
			rq->index = getParentNode()->index;
			rq->renderOp = mRenderOp;
			rq->texture = mpMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->_getTexturePtr();
			rq->rect = getScissorsRect();

			GetEngine->GetUIManager()->AddRenderQueue(rq);
		}
	}
}