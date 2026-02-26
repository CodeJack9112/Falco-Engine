#include "API_Material.h"
#include "Engine.h"
#include "OgreMaterialManager.h"
#include "OgreMaterial.h"
#include "OgreTechnique.h"
#include "StringConverter.h"

#include <OgreHighLevelGpuProgram.h>
#include <OgreHighLevelGpuProgramManager.h>

using namespace Ogre;

void API_Material::ctor(MonoObject* this_ptr)
{
}

void API_Material::passCtor(MonoObject* this_ptr)
{
}

MonoString* API_Material::getName(MonoObject* this_ptr)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));
	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(mat->getName()).c_str());

	return str;
}

int API_Material::getRenderingType(MonoObject* this_ptr)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	string scheme = mat->getTechnique(0)->getName();
	int type = scheme == "Forward" ? 0 : 1;

	return type;
}

void API_Material::setRenderingType(MonoObject* this_ptr, int val)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	for each (Technique * tech in mat->getTechniques())
	{
		tech->setName(val == 0 ? "Forward" : "Deferred");
		tech->setSchemeName(val == 0 ? MaterialManager::DEFAULT_SCHEME_NAME : "Deferred");
	}
}

int API_Material::getIterationType(MonoObject* this_ptr)
{
	Pass* pass;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

	int iter_type = 0;

	if (!pass->getIteratePerLight())
		return 0;
	else
		return 1;
}

void API_Material::setIterationType(MonoObject* this_ptr, int val)
{
	Pass* pass;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

	if (val == 0)
		pass->setIteratePerLight(false);
	if (val == 1)
		pass->setIteratePerLight(true);
}

MonoObject* API_Material::find(MonoString* name)
{
	std::string matName = (const char*)mono_string_to_utf8((MonoString*)name);
	matName = CP_SYS(matName);
	
	if (MaterialManager::getSingleton().resourceExists(matName, "Assets"))
	{
		MaterialPtr m = MaterialManager::getSingleton().getByName(matName, "Assets");
		Material* mat = m.getPointer();

		MonoObject * obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->material_class);
		mono_runtime_object_init(obj);
		mono_field_set_value(obj, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

		return obj;
	}

	return nullptr;
}

void API_Material::setParameterMatrix3(MonoObject* this_ptr, MonoString* name, API::Matrix3* matrix)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	Matrix3 mat3 = Matrix3(matrix->m00, matrix->m01, matrix->m02,
		matrix->m10, matrix->m11, matrix->m12,
		matrix->m20, matrix->m21, matrix->m22);

	mat->SetProgramSharedParameter(paramName, Material::ParameterType::PT_MATRIX3, Material::ValueVariant(mat3));
}

void API_Material::setParameterMatrix4(MonoObject* this_ptr, MonoString* name, API::Matrix4* matrix)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);
	
	Matrix4 mat4 = Matrix4(matrix->m00, matrix->m01, matrix->m02, matrix->m03,
		matrix->m10, matrix->m11, matrix->m12, matrix->m13,
		matrix->m20, matrix->m21, matrix->m22, matrix->m23,
		matrix->m30, matrix->m31, matrix->m32, matrix->m33);

	mat->SetProgramSharedParameter(paramName, Material::ParameterType::PT_MATRIX4, Material::ValueVariant(mat4));
}

void API_Material::setParameterVector2(MonoObject* this_ptr, MonoString* name, API::Vector2* vector2)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	Vector2 vec2 = Vector2(vector2->x, vector2->y);

	mat->SetProgramSharedParameter(paramName, Material::ParameterType::PT_VECTOR2, Material::ValueVariant(vec2));
}

void API_Material::setParameterVector3(MonoObject* this_ptr, MonoString* name, API::Vector3* vector3)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	Vector3 vec3 = Vector3(vector3->x, vector3->y, vector3->z);

	mat->SetProgramSharedParameter(paramName, Material::ParameterType::PT_VECTOR3, Material::ValueVariant(vec3));
}

void API_Material::setParameterVector4(MonoObject* this_ptr, MonoString* name, API::Vector4* vector4)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	Vector4 vec4 = Vector4(vector4->x, vector4->y, vector4->z, vector4->w);

	mat->SetProgramSharedParameter(paramName, Material::ParameterType::PT_VECTOR4, Material::ValueVariant(vec4));
}

void API_Material::setParameterFloat(MonoObject* this_ptr, MonoString* name, float flt)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	mat->SetProgramSharedParameter(paramName, Material::ParameterType::PT_FLOAT, Material::ValueVariant(flt));
}

void API_Material::setParameterInt(MonoObject* this_ptr, MonoString* name, int val)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	mat->SetProgramSharedParameter(paramName, Material::ParameterType::PT_INT, Material::ValueVariant(val));
}

void API_Material::setParameterAuto(MonoObject* this_ptr, MonoString* name, int val)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	GpuProgramParameters::AutoConstantType type = static_cast<GpuProgramParameters::AutoConstantType>(val);

	GpuProgramParameters* p = new GpuProgramParameters();
	size_t sz = p->getNumAutoConstantDefinitions();
	delete p;

	int index = 0;

	for (int i = 0; i < sz; ++i)
	{
		GpuProgramParameters::AutoConstantType _n = GpuProgramParameters::AutoConstantDictionary[i].acType;
		if (_n == type)
		{
			index = i;
			break;
		}
	}

	mat->SetProgramSharedParameter(paramName, Material::ParameterType::PT_AUTO, Material::ValueVariant(index));
}

void API_Material::getParameterMatrix3(MonoObject* this_ptr, MonoString* name, API::Matrix3* matrix)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	Matrix3 m = mat->GetProgramSharedParameter(paramName).value.mat3Val;

	matrix->m00 = m[0][0]; matrix->m01 = m[0][1]; matrix->m02 = m[0][2];
	matrix->m10 = m[1][0]; matrix->m11 = m[1][1]; matrix->m12 = m[1][2];
	matrix->m20 = m[2][0]; matrix->m21 = m[2][1]; matrix->m22 = m[2][2];
}

void API_Material::getParameterMatrix4(MonoObject* this_ptr, MonoString* name, API::Matrix4* matrix)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	Matrix4 m = mat->GetProgramSharedParameter(paramName).value.mat4Val;

	matrix->m00 = m[0][0]; matrix->m01 = m[0][1]; matrix->m02 = m[0][2]; matrix->m03 = m[0][3];
	matrix->m10 = m[1][0]; matrix->m11 = m[1][1]; matrix->m12 = m[1][2]; matrix->m13 = m[1][3];
	matrix->m20 = m[2][0]; matrix->m21 = m[2][1]; matrix->m22 = m[2][2]; matrix->m23 = m[2][3];
	matrix->m30 = m[3][0]; matrix->m31 = m[3][1]; matrix->m32 = m[3][2]; matrix->m33 = m[3][3];
}

void API_Material::getParameterVector2(MonoObject* this_ptr, MonoString* name, API::Vector2* vector2)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	Vector2 v = mat->GetProgramSharedParameter(paramName).value.vec2Val;

	vector2->x = v.x;
	vector2->y = v.y;
}

void API_Material::getParameterVector3(MonoObject* this_ptr, MonoString* name, API::Vector3* vector3)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	Vector3 v = mat->GetProgramSharedParameter(paramName).value.vec3Val;

	vector3->x = v.x;
	vector3->y = v.y;
	vector3->z = v.z;
}

void API_Material::getParameterVector4(MonoObject* this_ptr, MonoString* name, API::Vector4* vector4)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	Vector4 v = mat->GetProgramSharedParameter(paramName).value.vec4Val;

	vector4->x = v.x;
	vector4->y = v.y;
	vector4->z = v.z;
	vector4->w = v.w;
}

float API_Material::getParameterFloat(MonoObject* this_ptr, MonoString* name)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	return mat->GetProgramSharedParameter(paramName).value.floatVal;
}

int API_Material::getParameterInt(MonoObject* this_ptr, MonoString* name)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	return mat->GetProgramSharedParameter(paramName).value.intVal;
}

int API_Material::getParameterAuto(MonoObject* this_ptr, MonoString* name)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	std::string paramName = (const char*)mono_string_to_utf8((MonoString*)name);
	paramName = CP_SYS(paramName);

	int val = mat->GetProgramSharedParameter(paramName).value.intVal;

	GpuProgramParameters* p = new GpuProgramParameters();
	size_t sz = p->getNumAutoConstantDefinitions();
	delete p;

	int index = 0;

	for (int i = 0; i < sz; ++i)
	{
		int _n = static_cast<int>(GpuProgramParameters::AutoConstantDictionary[i].acType);
		if (_n == val)
		{
			index = i;
			break;
		}
	}

	return index;
}

int API_Material::getPassCount(MonoObject* this_ptr)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	Technique* tech = mat->getTechnique(0);

	return tech->getNumPasses();
}

MonoObject* API_Material::getPass(MonoObject* this_ptr, int index)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	Technique* tech = mat->getTechnique(0);

	if (tech->getNumPasses() > index)
	{
		Pass* pass = tech->getPass(index);

		MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->pass_class);
		mono_runtime_object_init(obj);
		mono_field_set_value(obj, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

		return obj;
	}

	return nullptr;
}

MonoObject* API_Material::addPass(MonoObject* this_ptr)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	Technique* tech = mat->getTechnique(0);

	Pass* pass = tech->createPass();

	MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->pass_class);
	mono_runtime_object_init(obj);
	mono_field_set_value(obj, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

	return obj;
}

void API_Material::removePass(MonoObject* this_ptr, int index)
{
	Material* mat;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&mat));

	Technique* tech = mat->getTechnique(0);

	if (tech->getNumPasses() > index)
		tech->removePass(index);
}

int API_Material::getBlendingMode(MonoObject* this_ptr)
{
	Pass* pass;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

	int blend_type = 0;

	SceneBlendFactor sf = pass->getSourceBlendFactor();
	SceneBlendFactor df = pass->getDestBlendFactor();

	if (sf == SBF_SOURCE_ALPHA && df == SBF_ONE_MINUS_SOURCE_ALPHA)
		blend_type = 0;
	if (sf == SBF_SOURCE_COLOUR && df == SBF_ONE_MINUS_SOURCE_COLOUR)
		blend_type = 1;
	if (sf == SBF_ONE && df == SBF_ONE)
		blend_type = 2;
	if (sf == SBF_DEST_COLOUR && df == SBF_ZERO)
		blend_type = 3;
	if (sf == SBF_ONE && df == SBF_ZERO)
		blend_type = 4;

	return blend_type;
}

void API_Material::setBlendingMode(MonoObject* this_ptr, int val)
{
	Pass* pass;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

	if (val == 0)
		pass->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_ALPHA);

	if (val == 1)
		pass->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_COLOUR);

	if (val == 2)
		pass->setSceneBlending(SceneBlendType::SBT_ADD);

	if (val == 3)
		pass->setSceneBlending(SceneBlendType::SBT_MODULATE);

	if (val == 4)
		pass->setSceneBlending(SceneBlendType::SBT_REPLACE);
}

MonoObject* API_Material::getVertexShader(MonoObject* this_ptr)
{
	Pass* pass;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

	if (pass->hasVertexProgram())
	{
		GpuProgramPtr prog = GpuProgramPtr(pass->getVertexProgram());
		HighLevelGpuProgram* p = (HighLevelGpuProgram*)prog.getPointer();

		if (p != nullptr)
		{
			MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->shader_class);
			mono_runtime_object_init(obj);
			mono_field_set_value(obj, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&p));

			return obj;
		}
	}

	return nullptr;
}

void API_Material::setVertexShader(MonoObject* this_ptr, MonoObject* val)
{
	Pass* pass;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

	if (val != nullptr)
	{
		HighLevelGpuProgram* prog = nullptr;
		mono_field_get_value(val, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&prog));

		if (prog != nullptr)
		{
			pass->setVertexProgram(prog->getName(), false);
		}
	}
	else
	{
		pass->setVertexProgram("", false);
	}
}

MonoObject* API_Material::getFragmentShader(MonoObject* this_ptr)
{
	Pass* pass;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

	if (pass->hasFragmentProgram())
	{
		GpuProgramPtr prog = GpuProgramPtr(pass->getFragmentProgram());
		HighLevelGpuProgram* p = (HighLevelGpuProgram*)prog.getPointer();

		if (p != nullptr)
		{
			MonoObject* obj = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->shader_class);
			mono_runtime_object_init(obj);
			mono_field_set_value(obj, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&p));

			return obj;
		}
	}

	return nullptr;
}

void API_Material::setFragmentShader(MonoObject* this_ptr, MonoObject* val)
{
	Pass* pass;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->pass_ptr_field, reinterpret_cast<void*>(&pass));

	if (val != nullptr)
	{
		HighLevelGpuProgram* prog = nullptr;
		mono_field_get_value(val, GetEngine->GetMonoRuntime()->asset_ptr_field, reinterpret_cast<void*>(&prog));

		if (prog != nullptr)
		{
			pass->setFragmentProgram(prog->getName(), false);
		}
	}
	else
	{
		pass->setFragmentProgram("", false);
	}
}
