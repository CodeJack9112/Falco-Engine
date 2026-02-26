#pragma once

#include "OgreIncludes.h"
#include <OgreManualObject.h>
#include <string>

using namespace Ogre;
using namespace std;

//UI Canvas
//1 Unit = 50 pixels

class UICanvas : public ManualObject
{
public:
	enum ScaleMode { ScaleWithScreenSize, AdjustWithScreenSize };
	static HighLevelGpuProgramPtr guiShaderV;
	static HighLevelGpuProgramPtr guiShaderF;

private:
	MaterialPtr material;
	int refScreenWidth = 1280;
	int refScreenHeight = 800;
	Vector2 realSize;
	ScaleMode scaleMode = ScaleMode::ScaleWithScreenSize;

public:
	UICanvas(std::string name);
	~UICanvas();

	static const float UNIT_TO_PIXELS;

	void SetReferenceScreenSize(int w, int h);
	int GetReferenceScreenWidth() { return refScreenWidth; }
	int GetReferenceScreenHeight() { return refScreenHeight; }
	Vector2 GetReferenceScreenSize() { return Vector2(refScreenWidth, refScreenHeight); }
	Vector2 GetRealSize();
	string GetScaleModeString();
	ScaleMode GetScaleMode() { return scaleMode; }
	void SetScaleMode(string mode);
	void SetScaleMode(ScaleMode mode) { scaleMode = mode; }

	//Overrides
	const String& getMovableType(void) const;
	SceneNode * getParentSceneNode();
	virtual MaterialPtr GetMaterial() { return material; }
};

class UICanvasFactory : public ManualObjectFactory
{
public:
	UICanvasFactory() {}
	~UICanvasFactory() {}

	static std::string FACTORY_TYPE_NAME;
	static std::map<SceneManager*, UICanvas*> uiCanvas;
	static SceneManager* manager;

	const String& getType(void) const { return FACTORY_TYPE_NAME; }

	ManualObject * createInstanceImpl(const String& name, const NameValuePairList* params)
	{
		if (uiCanvas[manager] == nullptr)
		{
			uiCanvas[manager] = (OGRE_NEW UICanvas(name));
			return (ManualObject*)uiCanvas[manager];
		}
		else
			return (ManualObject*)uiCanvas[manager];
	}

	void destroyInstance(MovableObject * obj) override
	{
		if (obj == (MovableObject*)uiCanvas[manager])
			uiCanvas[manager] = nullptr;

		OGRE_DELETE obj;
	}
};