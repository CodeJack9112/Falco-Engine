#pragma once

#include "OgrePrerequisites.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreVector4.h"

#include <string>

#include "../../../../Mono/include/mono/metadata/object.h"

using namespace std;
using namespace Ogre;

class _OgreExport MonoScript
{
public:
	struct ValueVariant
	{
	public:
		ValueVariant() {};
		ValueVariant(std::string strVal) { stringVal = strVal; }
		ValueVariant(bool bVal) { boolVal = bVal; }
		ValueVariant(int iVal) { intVal = iVal; }
		ValueVariant(float fVal) { floatVal = fVal; }
		ValueVariant(Vector2 vecVal) { vec2Val = vecVal; }
		ValueVariant(Vector3 vecVal) { vec3Val = vecVal; }
		ValueVariant(Vector4 vecVal) { vec4Val = vecVal; }

		std::string stringVal = "";
		bool boolVal = false;
		int intVal = 0;
		float floatVal = 0;
		std::string objectVal = "Game Object [None]";
		Vector2 vec2Val = Vector2(0, 0);
		Vector3 vec3Val = Vector3(0, 0, 0);
		Vector4 vec4Val = Vector4(0, 0, 0, 0);
	};

	struct MonoFieldInfo
	{
	public:
		std::string fieldName = "";
		std::string fieldType = "";
		ValueVariant fieldValue;
	};

	MonoScript(Ogre::SceneNode * node);
	~MonoScript();

	Ogre::SceneNode* sceneNode = nullptr;
	//string className;
	MonoClass* _class = nullptr;
	MonoObject * object = nullptr;
	uint32_t gcHandle = 0;
	std::map<std::string, MonoFieldInfo> fieldSerializeList;
	bool enabled = true;
	bool startExecuted = false;

	void CreateInstance();
	void DestroyInstance();
};

