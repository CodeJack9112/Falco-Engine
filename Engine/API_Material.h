#pragma once

#include "API.h"

class API_Material
{
public:
	//Register methods
	static void Register()
	{
		//Material
		mono_add_internal_call("FalcoEngine.Material::.ctor", (void*)ctor);
		mono_add_internal_call("FalcoEngine.Pass::.ctor", (void*)passCtor);
		mono_add_internal_call("FalcoEngine.Material::get_name", (void*)getName);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getRenderingType", (void*)getRenderingType);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_setRenderingType", (void*)setRenderingType);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_find", (void*)find);

		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getParameterMat3", (void*)getParameterMatrix3);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getParameterMat4", (void*)getParameterMatrix4);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getParameterVec2", (void*)getParameterVector2);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getParameterVec3", (void*)getParameterVector3);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getParameterVec4", (void*)getParameterVector4);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getParameterFloat", (void*)getParameterFloat);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getParameterInt", (void*)getParameterInt);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getParameterAuto", (void*)getParameterAuto);

		mono_add_internal_call("FalcoEngine.Material::INTERNAL_setParameterMat3", (void*)setParameterMatrix3);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_setParameterMat4", (void*)setParameterMatrix4);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_setParameterVec2", (void*)setParameterVector2);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_setParameterVec3", (void*)setParameterVector3);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_setParameterVec4", (void*)setParameterVector4);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_setParameterFloat", (void*)setParameterFloat);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_setParameterInt", (void*)setParameterInt);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_setParameterAuto", (void*)setParameterAuto);

		mono_add_internal_call("FalcoEngine.Material::get_passCount", (void*)getPassCount);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_getPass", (void*)getPass);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_addPass", (void*)addPass);
		mono_add_internal_call("FalcoEngine.Material::INTERNAL_removePass", (void*)removePass);

		//Pass
		mono_add_internal_call("FalcoEngine.Pass::INTERNAL_getBlendingMode", (void*)getBlendingMode);
		mono_add_internal_call("FalcoEngine.Pass::INTERNAL_setBlendingMode", (void*)setBlendingMode);
		mono_add_internal_call("FalcoEngine.Pass::get_vertexShader", (void*)getVertexShader);
		mono_add_internal_call("FalcoEngine.Pass::set_vertexShader", (void*)setVertexShader);
		mono_add_internal_call("FalcoEngine.Pass::get_fragmentShader", (void*)getFragmentShader);
		mono_add_internal_call("FalcoEngine.Pass::set_fragmentShader", (void*)setFragmentShader);
		mono_add_internal_call("FalcoEngine.Pass::INTERNAL_getIterationType", (void*)getIterationType);
		mono_add_internal_call("FalcoEngine.Pass::INTERNAL_setIterationType", (void*)setIterationType);
	}

private:
	//Constructor
	static void ctor(MonoObject* this_ptr);
	static void passCtor(MonoObject* this_ptr);

	//Get name
	static MonoString * getName(MonoObject* this_ptr);

	//getRenderingType
	static int getRenderingType(MonoObject* this_ptr);

	//setRenderingType
	static void setRenderingType(MonoObject* this_ptr, int val);

	//getIterationType
	static int getIterationType(MonoObject* this_ptr);

	//setIterationType
	static void setIterationType(MonoObject* this_ptr, int val);

	//Find
	static MonoObject* find(MonoString * name);

	//Get parameter Matrix3
	static void getParameterMatrix3(MonoObject* this_ptr, MonoString* name, API::Matrix3* matrix);

	//Get parameter Matrix4
	static void getParameterMatrix4(MonoObject* this_ptr, MonoString* name, API::Matrix4* matrix);

	//Get parameter Vector2
	static void getParameterVector2(MonoObject* this_ptr, MonoString* name, API::Vector2* vector2);

	//Get parameter Vector3
	static void getParameterVector3(MonoObject* this_ptr, MonoString* name, API::Vector3* vector3);

	//Get parameter Vector4
	static void getParameterVector4(MonoObject* this_ptr, MonoString* name, API::Vector4* vector4);

	//Get parameter float
	static float getParameterFloat(MonoObject* this_ptr, MonoString* name);

	//Get parameter int
	static int getParameterInt(MonoObject* this_ptr, MonoString* name);

	//Get parameter int
	static int getParameterAuto(MonoObject* this_ptr, MonoString* name);

	//Set parameter Matrix3
	static void setParameterMatrix3(MonoObject* this_ptr, MonoString* name, API::Matrix3* matrix);

	//Set parameter Matrix4
	static void setParameterMatrix4(MonoObject* this_ptr, MonoString* name, API::Matrix4 * matrix);

	//Set parameter Vector2
	static void setParameterVector2(MonoObject* this_ptr, MonoString* name, API::Vector2* vector2);

	//Set parameter Vector3
	static void setParameterVector3(MonoObject* this_ptr, MonoString* name, API::Vector3* vector3);

	//Set parameter Vector4
	static void setParameterVector4(MonoObject* this_ptr, MonoString* name, API::Vector4* vector4);

	//Set parameter float
	static void setParameterFloat(MonoObject* this_ptr, MonoString* name, float flt);

	//Set parameter int
	static void setParameterInt(MonoObject* this_ptr, MonoString* name, int val);

	//Set parameter int
	static void setParameterAuto(MonoObject* this_ptr, MonoString* name, int val);

	//Get pass count
	static int getPassCount(MonoObject* this_ptr);

	//Get pass
	static MonoObject* getPass(MonoObject* this_ptr, int index);

	//getBlendingMode
	static int getBlendingMode(MonoObject* this_ptr);

	//setBlendingMode
	static void setBlendingMode(MonoObject* this_ptr, int val);

	//getVertexShader
	static MonoObject* getVertexShader(MonoObject* this_ptr);

	//setVertexShader
	static void setVertexShader(MonoObject* this_ptr, MonoObject* val);

	//getFragmentShader
	static MonoObject* getFragmentShader(MonoObject* this_ptr);

	//setFragmentShader
	static void setFragmentShader(MonoObject* this_ptr, MonoObject* val);
	
	//addPass
	static MonoObject* addPass(MonoObject* this_ptr);

	//removePass
	static void removePass(MonoObject* this_ptr, int index);
};

