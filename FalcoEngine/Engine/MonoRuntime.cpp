#include "stdafx.h"
#include "MonoRuntime.h"
#include <Windows.h>

#include <iostream>
#include <stdexcept>
#include <stdio.h>

#include "../boost/process.hpp"
#include "IO.h"
#include "StringConverter.h"

#include "Engine.h"
#include "OgreCompositorManager.h"

#include "API_MonoBehaviour.h"
#include "API_Transform.h"
#include "API_RigidBody.h"
#include "API_Quaternion.h"
#include "API_Input.h"
#include "API_Physics.h"
#include "API_GameObject.h"
#include "API_Animation.h"
#include "API_AudioSource.h"
#include "API_NavMeshAgent.h"
#include "API_Component.h"
#include "API_Prefab.h"
#include "API_GUI.h"
#include "API_Screen.h"
#include "API_Cursor.h"
#include "API_Time.h"
#include "API_SceneManager.h"
#include "API_Texture.h"
#include "API_Debug.h"
#include "API_PostEffect.h"
#include "API_Material.h"
#include "API_AttachedObject.h"
#include "API_Camera.h"
#include "API_Application.h"
#include "API_Entity.h"
#include "API_SubEntity.h"
#include "API_Text.h"
#include "API_Button.h"
#include "API_Image.h"
#include "API_UIElement.h"
#include "API_AudioListener.h"
#include "API_Shader.h"
#include "API_Collider.h"
#include "API_MeshCollider.h"
#include "API_BoxCollider.h"
#include "API_SphereCollider.h"
#include "API_CapsuleCollider.h"
#include "API_Asset.h"
#include "API_PlayerPrefs.h"
#include "API_Random.h"
#include "API_Vehicle.h"

#include "../Engine/SceneSerializer.h"

#define BUFSIZE 4096 

MonoRuntime::MonoRuntime()
{
	srand(static_cast<unsigned>(time(0)));
}

MonoRuntime::MonoRuntime(string monoDir, string libDir)
{
	monoDirectory = monoDir;
	mono_set_dirs(CP_UNI((monoDir + "lib/")).c_str(), CP_UNI((monoDir + "etc/")).c_str());
	libDirectory = libDir;

	mono_config_parse(NULL);

	domain = mono_jit_init("Falco_engine");
	mono_domain_set(domain, FALSE);

	if (!domain)
	{
		MessageBoxA(0, "Failed to initialize Mono!", "Error", MB_ICONERROR | MB_OK);
	}
}

MonoRuntime::~MonoRuntime()
{
}

void MonoRuntime::SetBuildCallback(std::function<void(std::string)> callback)
{
	buildCallback = callback;
}

void MonoRuntime::SetLogCallback(std::function<void(std::string)> callback)
{
	logCallback = callback;
}

void MonoRuntime::CompileLibrary(CompileConfiguration config, bool callCallbacks)
{
	//mono_thread_attach(domain);

	//Compile the scripts
	if (callCallbacks)
	{
		if (buildCallback != nullptr)
			buildCallback("Compiling scripts...");
	}

	string conf = "Debug/";
	string debug = "-debug+";

	if (config == CompileConfiguration::Release)
	{
		conf = "Release/";
		debug = "-debug-";
	}

	IO::CreateDir(solutionPath + "bin/");
	IO::CreateDir(solutionPath + "bin/Debug/");
	IO::CreateDir(solutionPath + "bin/Release/");
	IO::FileCopy(solutionPath + "FalcoEngine.dll", solutionPath + "bin/" + conf + "FalcoEngine.dll");

	int nCodePage = GetACP();

	std::string command = monoDirectory + "bin/mono.exe \"" + monoDirectory + "lib/mono/4.5/mcs.exe\" -codepage:65001 --stacktrace " + debug + " -lib:\"" + solutionPath + "FalcoEngine.dll\" -r:\"" + solutionPath + "FalcoEngine.dll\" -target:library -out:\"" + solutionPath + "bin/" + conf + "MainAssembly.dll\" -recurse:\"" + IO::ReplaceFrontSlashes(GetEngine->GetAssetsPath()) + "\"*.cs \"" + solutionPath + "Main.cs\"";
	
	std::wstring ws = ::StringConvert::s2ws(command, nCodePage);
	ws.push_back(0);

	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if (CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
	{
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFO siStartInfo;
		BOOL bSuccess = FALSE;

		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

		siStartInfo.cb = sizeof(STARTUPINFO);
		siStartInfo.hStdError = g_hChildStd_OUT_Wr;
		siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
		siStartInfo.hStdInput = NULL;
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

		// Create the child process.
		bSuccess = CreateProcessW(
			NULL,
			&ws[0],        // command line 
			NULL,          // process security attributes 
			NULL,          // primary thread security attributes 
			TRUE,          // handles are inherited 
			NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, // | CREATE_UNICODE_ENVIRONMENT, // creation flags 
			NULL,          // use parent's environment 
			NULL,          // use parent's current directory 
			&siStartInfo,  // STARTUPINFO pointer 
			&piProcInfo);  // receives PROCESS_INFORMATION 

		DWORD dwRead;
		HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

		CloseHandle(g_hChildStd_OUT_Wr);
		lastLog = "";

		for (;;)
		{
			CHAR chBuf[BUFSIZE] = {0};
			bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
			
			if (!bSuccess || dwRead <= 0)
			{
				break;
			}

			string str = string(chBuf).substr(0, dwRead);
			str = StringConvert::cp_convert(str, 866, GetACP());
			
			if (callCallbacks)
			{
				if (logCallback != nullptr)
					logCallback(str);
			}

			lastLog += str + "\n";
		}

		//WaitForSingleObject(piProcInfo.hProcess, INFINITE);

		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}

	Sleep(1000);

	if (callCallbacks)
	{
		if (buildCallback != nullptr)
			buildCallback("--Done--");
	}
}

void MonoRuntime::OpenLibrary()
{
	//mono_thread_attach(domain);
	domain = mono_domain_create_appdomain("Falco_engine", NULL);
	mono_domain_set(domain, 0);

	//Register API
	API_Input::Register();
	API_GameObject::Register();
	API_MonoBehaviour::Register();
	API_Quaternion::Register();
	API_Transform::Register();
	API_Physics::Register();
	API_RigidBody::Register();
	API_Animation::Register();
	API_AudioSource::Register();
	API_NavMeshAgent::Register();
	API_Component::Register();
	API_Prefab::Register();
	API_GUI::Register();
	API_Screen::Register();
	API_Cursor::Register();
	API_Time::Register();
	API_SceneManager::Register();
	API_Texture::Register();
	API_Debug::Register();
	API_PostEffect::Register();
	API_Material::Register();
	API_AttachedObject::Register();
	API_Camera::Register();
	API_Application::Register();
	API_Entity::Register();
	API_SubEntity::Register();
	API_UIElement::Register();
	API_Text::Register();
	API_Button::Register();
	API_Image::Register();
	API_AudioListener::Register();
	API_Shader::Register();
	API_Collider::Register();
	API_MeshCollider::Register();
	API_BoxCollider::Register();
	API_SphereCollider::Register();
	API_CapsuleCollider::Register();
	API_Asset::Register();
	API_PlayerPrefs::Register();
	API_Random::Register();
	API_Vehicle::Register();

	//Base lib
	string fp = IO::GetFilePath(libDirectory);

	int nCodePage = GetACP();

	if (base_assembly == nullptr)
	{
		std::string _path = fp + "FalcoEngine.dll";
		base_assembly = mono_domain_assembly_open(domain, ::StringConvert::cp_convert(_path.c_str(), nCodePage, 65001).c_str());

		if (!base_assembly)
		{
			MessageBoxA(0, "base assembly open failed", "Error", MB_ICONERROR | MB_OK);
		}

		base_image = mono_assembly_get_image(base_assembly);
		if (!base_image)
		{
			MessageBoxA(0, "base assembly get image failed", "Error", MB_ICONERROR | MB_OK);
		}
	}

	//App lib
	assembly = mono_domain_assembly_open(domain, ::StringConvert::cp_convert(libDirectory.c_str(), nCodePage, 65001).c_str());

	if (!assembly)
	{
		MessageBoxA(0, "mono_domain_assembly_open failed", "Error", MB_ICONERROR | MB_OK);
	}

	image = mono_assembly_get_image(assembly);
	if (!image)
	{
		MessageBoxA(0, "mono_assembly_get_image failed", "Error", MB_ICONERROR | MB_OK);
	}

	MonoMethodDesc * des = mono_method_desc_new("FalcoEngine.EntryPoint:Main", true);
	MonoMethod * main = mono_method_desc_search_in_image(des, base_image);

	MonoArray* array = mono_array_new(domain, mono_get_string_class(), 1);
	MonoString * str = mono_string_new(domain, CP_UNI((fp + "FalcoEngine.dll")).c_str());
	mono_array_set(array, MonoString*, 0, str);

	int retval = mono_runtime_exec_main(main, array, NULL);
	mono_method_desc_free(des);

	RetrieveClassesAndFields();
	behaviourClasses = GetAssemblyClassList(image);

	SceneManager::monoDomain = domain;
	SceneManager::monoGameObjectClass = gameobject_class;
	SceneManager::monoBehaviourClass = behaviour_class;
	SceneManager::monoTransformClass = transform_class;
	SceneManager::monoCameraClass = camera_class;
	SceneManager::monoEntityClass = entity_class;
	SceneManager::monoSubEntityClass = subentity_class;
	SceneManager::monoUIElementClass = uielement_class;
	SceneManager::monoTextClass = text_class;
	SceneManager::monoImageClass = image_class;
	SceneManager::monoButtonClass = button_class;

	SceneManager::monoComponentThisPtrField = component_this_ptr_field;
	SceneManager::monoNativePtrField = native_ptr_field;
	SceneManager::monoScriptPtrField = script_ptr_field;
	SceneManager::monoComponentNativePtrField = component_native_ptr_field;
	SceneManager::monoAttachedObjectPtrField = attached_object_ptr_field;
	SceneManager::monoSubEntityPtrField = subentity_ptr_field;
}

void MonoRuntime::CloseLibrary(bool cleanup)
{
	mono_thread_attach(domain);

	behaviourClasses.clear();
	
	CleanUp();

	mono_assembly_close(assembly);
	assembly = nullptr;
	
	mono_assembly_close(base_assembly);
	base_assembly = nullptr;

	domain = nullptr;
	image = nullptr;
	base_image = nullptr;

	if (cleanup)
		mono_jit_cleanup(domain);
}

void MonoRuntime::CleanUp()
{
	//DestroyInstances(GetEngine->GetSceneManager()->getRootSceneNode());
	CleanUp(GetEngine->GetSceneManager()->getRootSceneNode());
}

void MonoRuntime::CleanUp(SceneNode* root)
{
	for (auto it = root->monoScripts.begin(); it != root->monoScripts.end(); ++it)
	{
		(*it)->DestroyInstance();
	}

	VectorIterator c = root->getChildIterator();
	while (c.hasMoreElements())
	{
		SceneNode* child = (SceneNode*)c.getNext();
		CleanUp(child);
	}
}

MonoRuntime::FunctionList MonoRuntime::GetFunctions(SceneNode * node)
{
	std::vector<std::pair<std::string, std::string>> list;

	if (node != nullptr)
	{
		for (std::vector<MonoScript*>::iterator it = node->monoScripts.begin(); it != node->monoScripts.end(); ++it)
		{
			MonoObject * obj = (MonoObject*)(*it)->object;
			MonoClass * cls = mono_object_get_class(obj);
			string class_name = mono_class_get_name(cls);
			class_name = CP_SYS(class_name);

			void * iter = nullptr;
			MonoMethod * method = nullptr;

			while (method = mono_class_get_methods(cls, &iter))
			{
				string method_name = mono_method_get_name(method);

				if (method_name != ".ctor" && method_name != "BeginFrame" && method_name != "EndFrame" && method_name != "Start" && method_name != "OnGUI" && method_name != "Update" && method_name != "FixedUpdate")
				{
					list.push_back(make_pair(class_name, method_name));
				}
			}
		}
	}

	return list;
}

MonoRuntime::FieldList MonoRuntime::GetFields(MonoScript * script)
{
	FieldList list;

	if (script != nullptr)
	{
		MonoObject * obj = (MonoObject*)script->object;
		MonoClass * baseClass = mono_object_get_class(obj);
		//string class_name = mono_class_get_name(baseclass);
		//class_name = CP_SYS(class_name);

		void * it = nullptr;
		MonoClassField * field = nullptr;

		MonoClass* parent = nullptr;

		std::vector<MonoClass*> classes = { baseClass };

		parent = mono_class_get_parent(baseClass);

		while (parent != nullptr) {
			classes.push_back(parent);
			parent = mono_class_get_parent(parent);
		}

		for (auto _it = classes.begin(); _it != classes.end(); ++_it)
		{
			MonoClass* cls = *_it;

			it = nullptr;
			field = nullptr;

			while (field = mono_class_get_fields(cls, &it))
			{
				uint32_t flags = mono_field_get_flags(field);
				bool _static = flags & MONO_FIELD_ATTR_STATIC;

				/*
					{ Mono attributes info: }

					MONO_FIELD_ATTR_PRIVATE :: PRIVATE;
					MONO_FIELD_ATTR_FAM_AND_ASSEM :: PROTECTED_AND_INTERNAL;
					MONO_FIELD_ATTR_ASSEMBLY :: INTERNAL;
					MONO_FIELD_ATTR_FAMILY :: PROTECTED;
					MONO_FIELD_ATTR_PUBLIC :: PUBLIC;
				*/

				if ((flags & MONO_FIELD_ATTR_FIELD_ACCESS_MASK) == MONO_FIELD_ATTR_PUBLIC && !_static)
				{
					MonoType* type = mono_field_get_type(field);
					std::string field_name = mono_field_get_name(field);
					std::string field_type = mono_type_get_name(type);

					field_name = CP_SYS(field_name);
					field_type = CP_SYS(field_type);

					MonoScript::MonoFieldInfo inf;

					inf.fieldName = field_name;
					inf.fieldType = field_type;

					if (field_type == "System.Int32")
					{
						int val;
						mono_field_get_value(obj, field, &val);
						inf.fieldValue = MonoScript::ValueVariant(val);
					}
					if (field_type == "System.Single")
					{
						float val;
						mono_field_get_value(obj, field, &val);
						inf.fieldValue = MonoScript::ValueVariant(val);
					}
					if (field_type == "System.String")
					{
						//MonoString * val = nullptr;
						MonoObject* val = mono_field_get_value_object(domain, field, obj);
						const char* _str = (const char*)mono_string_to_utf8((MonoString*)val);
						if (_str != nullptr)
						{
							std::string str = CP_SYS(std::string(_str));
							inf.fieldValue = MonoScript::ValueVariant(std::string(str));
						}
						else
							inf.fieldValue = MonoScript::ValueVariant(std::string(""));
					}
					if (field_type == "System.Boolean")
					{
						bool val = nullptr;
						mono_field_get_value(obj, field, &val);
						inf.fieldValue = MonoScript::ValueVariant(val);
					}
					if (field_type == "FalcoEngine.GameObject")
					{
						MonoObject* val = mono_field_get_value_object(domain, field, obj);
						if (val != nullptr)
						{
							SceneNode* objNode = nullptr;
							GetSceneNodeFromMonoObject(GetEngine->GetSceneManager()->getRootSceneNode(), val, objNode);
							if (objNode != nullptr)
							{
								MonoScript::ValueVariant _v;
								_v.objectVal = objNode->getName();
								inf.fieldValue = _v;
							}
						}
					}
					if (field_type == "FalcoEngine.Vector2")
					{
						MonoObject* val = mono_field_get_value_object(domain, field, obj);
						if (val != nullptr)
						{
							float xval = 0;
							float yval = 0;

							mono_field_get_value(val, vector2_x, &xval);
							mono_field_get_value(val, vector2_y, &yval);

							inf.fieldValue = MonoScript::ValueVariant(Vector2(xval, yval));
						}
					}
					if (field_type == "FalcoEngine.Vector3")
					{
						MonoObject* val = mono_field_get_value_object(domain, field, obj);
						if (val != nullptr)
						{
							float xval = 0;
							float yval = 0;
							float zval = 0;

							mono_field_get_value(val, vector3_x, &xval);
							mono_field_get_value(val, vector3_y, &yval);
							mono_field_get_value(val, vector3_z, &zval);

							inf.fieldValue = MonoScript::ValueVariant(Vector3(xval, yval, zval));
						}
					}
					if (field_type == "FalcoEngine.Vector4")
					{
						MonoObject* val = mono_field_get_value_object(domain, field, obj);
						if (val != nullptr)
						{
							float xval = 0;
							float yval = 0;
							float zval = 0;
							float wval = 0;

							mono_field_get_value(val, vector4_x, &xval);
							mono_field_get_value(val, vector4_y, &yval);
							mono_field_get_value(val, vector4_z, &zval);
							mono_field_get_value(val, vector4_w, &wval);

							inf.fieldValue = MonoScript::ValueVariant(Vector4(xval, yval, zval, wval));
						}
					}

					list.push_back(inf);
				}
			}
		}
	}

	return list;
}

MonoClass * MonoRuntime::FindClass(std::string name)
{
	auto it = find_if(behaviourClasses.begin(), behaviourClasses.end(), [name](MonoClass* __class) -> bool { return mono_class_get_name(__class) == CP_UNI(name); });

	if (it != behaviourClasses.end())
		return *it;

	return nullptr;
}

void MonoRuntime::MonoScriptSetFields(MonoScript * monoScript)
{
	MonoRuntime::FieldList lst = GetFields(monoScript);

	//Remove incorrect fields
	std::vector<std::string> toRemove;

	for (MonoRuntime::FieldList::iterator it = lst.begin(); it != lst.end(); ++it)
	{
		for (std::map<std::string, MonoScript::MonoFieldInfo>::iterator it2 = monoScript->fieldSerializeList.begin(); it2 != monoScript->fieldSerializeList.end(); ++it2)
		{
			if (it->fieldName == it2->first)
			{
				if (it->fieldType != it2->second.fieldType)
					toRemove.push_back(it->fieldName);
			}
		}
	}

	for (std::map<std::string, MonoScript::MonoFieldInfo>::iterator it2 = monoScript->fieldSerializeList.begin(); it2 != monoScript->fieldSerializeList.end(); ++it2)
	{
		MonoRuntime::FieldList::iterator it = find_if(lst.begin(), lst.end(), [it2](const MonoScript::MonoFieldInfo& s) { return s.fieldName == it2->first; });

		if (it == lst.end())
			toRemove.push_back(it2->first);
	}

	for (std::vector<std::string>::iterator it = toRemove.begin(); it != toRemove.end(); ++it)
	{
		monoScript->fieldSerializeList.erase(*it);
	}

	toRemove.clear();

	for (std::map<std::string, MonoScript::MonoFieldInfo>::iterator it = monoScript->fieldSerializeList.begin(); it != monoScript->fieldSerializeList.end(); ++it)
	{
		MonoScript::MonoFieldInfo inf = it->second;
		MonoScript::ValueVariant val = inf.fieldValue;

		MonoClass * _class = mono_object_get_class((MonoObject*)monoScript->object);

		if (_class != nullptr)
		{
			MonoClassField * _fld = mono_class_get_field_from_name(_class, CP_UNI(inf.fieldName).c_str());

			uint32_t flags = mono_field_get_flags(_fld);
			bool _static = flags & MONO_FIELD_ATTR_STATIC;

			if ((flags & MONO_FIELD_ATTR_FIELD_ACCESS_MASK) == MONO_FIELD_ATTR_PUBLIC && !_static)
			{
				if (inf.fieldType == "System.Int32")
				{
					mono_field_set_value((MonoObject*)monoScript->object, _fld, &(int)val.intVal);
				}
				if (inf.fieldType == "System.Single")
				{
					mono_field_set_value((MonoObject*)monoScript->object, _fld, &(float)val.floatVal);
				}
				if (inf.fieldType == "System.Boolean")
				{
					bool b = (bool)val.boolVal;
					mono_field_set_value((MonoObject*)monoScript->object, _fld, &b);
				}
				if (inf.fieldType == "System.String")
				{
					MonoString * _str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(val.stringVal).c_str());
					mono_field_set_value((MonoObject*)monoScript->object, _fld, _str);
				}
				if (inf.fieldType == "FalcoEngine.GameObject")
				{
					if (val.objectVal != "Game Object [None]")
					{
						if (GetEngine->GetSceneManager()->hasSceneNode(val.objectVal))
						{
							SceneNode * node = GetEngine->GetSceneManager()->getSceneNode(val.objectVal);
							if (node->gameObjectMono != nullptr)
							{
								mono_field_set_value((MonoObject*)monoScript->object, _fld, node->gameObjectMono);
							}
						}
					}
				}
				if (inf.fieldType == "FalcoEngine.Vector2")
				{
					MonoObject * vec2 = mono_object_new(domain, vector2_class);
					mono_field_set_value(vec2, vector2_x, &(float)inf.fieldValue.vec2Val.x);
					mono_field_set_value(vec2, vector2_y, &(float)inf.fieldValue.vec2Val.y);

					void* _vec2 = mono_object_unbox(vec2);
					mono_field_set_value((MonoObject*)monoScript->object, _fld, _vec2);
				}
				if (inf.fieldType == "FalcoEngine.Vector3")
				{
					MonoObject* vec3 = mono_object_new(domain, vector3_class);
					mono_field_set_value(vec3, vector3_x, &(float)inf.fieldValue.vec3Val.x);
					mono_field_set_value(vec3, vector3_y, &(float)inf.fieldValue.vec3Val.y);
					mono_field_set_value(vec3, vector3_z, &(float)inf.fieldValue.vec3Val.z);

					void* _vec3 = mono_object_unbox(vec3);
					mono_field_set_value((MonoObject*)monoScript->object, _fld, _vec3);
				}
				if (inf.fieldType == "FalcoEngine.Vector4")
				{
					MonoObject* vec4 = mono_object_new(domain, vector4_class);
					mono_field_set_value(vec4, vector4_x, &(float)inf.fieldValue.vec4Val.x);
					mono_field_set_value(vec4, vector4_y, &(float)inf.fieldValue.vec4Val.y);
					mono_field_set_value(vec4, vector4_z, &(float)inf.fieldValue.vec4Val.z);
					mono_field_set_value(vec4, vector4_w, &(float)inf.fieldValue.vec4Val.w);

					void* _vec4 = mono_object_unbox(vec4);
					mono_field_set_value((MonoObject*)monoScript->object, _fld, _vec4);
				}
			}
		}
	}
}

void MonoRuntime::MonoScriptSetFieldsAllNodes(SceneNode * root)
{
	for (std::vector<MonoScript*>::iterator sit = root->monoScripts.begin(); sit != root->monoScripts.end(); ++sit)
	{
		MonoScriptSetFields(*sit);
	}

	VectorIterator it = root->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode * node = (SceneNode*)it.getNext();

		MonoScriptSetFieldsAllNodes(node);
	}
}

void MonoRuntime::RetrieveClassesAndFields()
{
	//Get mono classes
	behaviour_class = mono_class_from_name(base_image, "FalcoEngine", "MonoBehaviour");
	transform_class = mono_class_from_name(base_image, "FalcoEngine", "Transform");
	rigidbody_class = mono_class_from_name(base_image, "FalcoEngine", "Rigidbody");
	vector2_class = mono_class_from_name(base_image, "FalcoEngine", "Vector2");
	vector3_class = mono_class_from_name(base_image, "FalcoEngine", "Vector3");
	vector4_class = mono_class_from_name(base_image, "FalcoEngine", "Vector4");
	color_class = mono_class_from_name(base_image, "FalcoEngine", "Color");
	quaternion_class = mono_class_from_name(base_image, "FalcoEngine", "Quaternion");
	input_class = mono_class_from_name(base_image, "FalcoEngine", "Input");
	gameobject_class = mono_class_from_name(base_image, "FalcoEngine", "GameObject");
	animation_class = mono_class_from_name(base_image, "FalcoEngine", "Animation");
	audiosource_class = mono_class_from_name(base_image, "FalcoEngine", "AudioSource");
	audiolistener_class = mono_class_from_name(base_image, "FalcoEngine", "AudioListener");
	navmeshagent_class = mono_class_from_name(base_image, "FalcoEngine", "NavMeshAgent");
	navmeshobstacle_class = mono_class_from_name(base_image, "FalcoEngine", "NavMeshObstacle");
	component_class = mono_class_from_name(base_image, "FalcoEngine", "Component");
	font_class = mono_class_from_name(base_image, "FalcoEngine", "Font");
	posteffect_class = mono_class_from_name(base_image, "FalcoEngine", "PostEffect");
	posteffect_technique_class = mono_class_from_name(base_image, "FalcoEngine", "PostEffectTechnique");
	posteffect_texture_class = mono_class_from_name(base_image, "FalcoEngine", "PostEffectTexture");
	posteffect_pass_class = mono_class_from_name(base_image, "FalcoEngine", "PostEffectPass");
	posteffect_target_pass_class = mono_class_from_name(base_image, "FalcoEngine", "PostEffectTargetPass");
	asset_class = mono_class_from_name(base_image, "FalcoEngine", "Asset");
	material_class = mono_class_from_name(base_image, "FalcoEngine", "Material");
	pass_class = mono_class_from_name(base_image, "FalcoEngine", "Pass");
	shader_class = mono_class_from_name(base_image, "FalcoEngine", "Shader");
	attached_object_class = mono_class_from_name(base_image, "FalcoEngine", "AttachedObject");
	camera_class = mono_class_from_name(base_image, "FalcoEngine", "Camera");
	entity_class = mono_class_from_name(base_image, "FalcoEngine", "Entity");
	subentity_class = mono_class_from_name(base_image, "FalcoEngine", "SubEntity");
	uielement_class = mono_class_from_name(base_image, "FalcoEngine", "UIElement");
	text_class = mono_class_from_name(base_image, "FalcoEngine", "Text");
	image_class = mono_class_from_name(base_image, "FalcoEngine", "Image");
	button_class = mono_class_from_name(base_image, "FalcoEngine", "Button");
	texture2d_class = mono_class_from_name(base_image, "FalcoEngine", "Texture2D");
	collision_class = mono_class_from_name(base_image, "FalcoEngine", "Collision");
	collider_class = mono_class_from_name(base_image, "FalcoEngine", "Collider");
	meshcollider_class = mono_class_from_name(base_image, "FalcoEngine", "MeshCollider");
	boxcollider_class = mono_class_from_name(base_image, "FalcoEngine", "BoxCollider");
	spherecollider_class = mono_class_from_name(base_image, "FalcoEngine", "SphereCollider");
	capsulecollider_class = mono_class_from_name(base_image, "FalcoEngine", "CapsuleCollider");
	vehicle_class = mono_class_from_name(base_image, "FalcoEngine", "Vehicle");

	//Get mono fields
	native_ptr_field = mono_class_get_field_from_name(behaviour_class, "native_ptr");
	//enabled_field = mono_class_get_field_from_name(behaviour_class, "active");
	script_ptr_field = mono_class_get_field_from_name(behaviour_class, "script_ptr");
	transform_native_ptr_field = mono_class_get_field_from_name(transform_class, "native_ptr");
	gameobject_native_ptr_field = mono_class_get_field_from_name(gameobject_class, "native_ptr");
	
	component_this_ptr_field = mono_class_get_field_from_name(component_class, "this_ptr");
	component_native_ptr_field = mono_class_get_field_from_name(component_class, "native_ptr");
	
	posteffect_this_ptr_field = mono_class_get_field_from_name(posteffect_class, "posteffect_ptr");
	posteffect_instance_this_ptr_field = mono_class_get_field_from_name(posteffect_class, "posteffect_instance_ptr");
	posteffect_listener_ptr = mono_class_get_field_from_name(posteffect_class, "listener_ptr");
	posteffect_technique_ptr_field = mono_class_get_field_from_name(posteffect_technique_class, "technique_ptr");
	posteffect_technique_output_pass_field = mono_class_get_field_from_name(posteffect_technique_class, "finalPass");
	posteffect_texture_ptr_field = mono_class_get_field_from_name(posteffect_texture_class, "texture_ptr");
	posteffect_pass_ptr_field = mono_class_get_field_from_name(posteffect_pass_class, "pass_ptr");
	posteffect_target_pass_ptr_field = mono_class_get_field_from_name(posteffect_target_pass_class, "pass_ptr");
	
	asset_ptr_field = mono_class_get_field_from_name(asset_class, "asset_ptr");
	attached_object_ptr_field = mono_class_get_field_from_name(attached_object_class, "object_ptr");
	subentity_ptr_field = mono_class_get_field_from_name(subentity_class, "subentity_ptr");
	pass_ptr_field = mono_class_get_field_from_name(pass_class, "pass_ptr");

	vector2_x = mono_class_get_field_from_name(vector2_class, "x");
	vector2_y = mono_class_get_field_from_name(vector2_class, "y");

	vector3_x = mono_class_get_field_from_name(vector3_class, "x");
	vector3_y = mono_class_get_field_from_name(vector3_class, "y");
	vector3_z = mono_class_get_field_from_name(vector3_class, "z");

	vector4_x = mono_class_get_field_from_name(vector4_class, "x");
	vector4_y = mono_class_get_field_from_name(vector4_class, "y");
	vector4_z = mono_class_get_field_from_name(vector4_class, "z");
	vector4_w = mono_class_get_field_from_name(vector4_class, "w");

	quaternion_x = mono_class_get_field_from_name(quaternion_class, "x");
	quaternion_y = mono_class_get_field_from_name(quaternion_class, "y");
	quaternion_z = mono_class_get_field_from_name(quaternion_class, "z");
	quaternion_w = mono_class_get_field_from_name(quaternion_class, "w");

	color_r = mono_class_get_field_from_name(color_class, "r");
	color_g = mono_class_get_field_from_name(color_class, "g");
	color_b = mono_class_get_field_from_name(color_class, "b");
	color_a = mono_class_get_field_from_name(color_class, "a");

	collision_other = mono_class_get_field_from_name(collision_class, "other");
	collision_point = mono_class_get_field_from_name(collision_class, "point");
	collision_normal = mono_class_get_field_from_name(collision_class, "normal");
}

std::list<MonoClass*> MonoRuntime::GetAssemblyClassList(MonoImage * image)
{
	std::list<MonoClass*> class_list;

	const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);

	int rows = mono_table_info_get_rows(table_info);

	/* For each row, get some of its values */
	for (int i = 0; i < rows; i++)
	{
		MonoClass* _class = nullptr;
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
		const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
		_class = mono_class_from_name(image, name_space, name);

		if (_class != nullptr)
		{
			if (mono_class_is_subclass_of(_class, behaviour_class, false))
			{
				if (string(name) != string("<Module>") && string(name) != string("MonoBehaviour"))
					class_list.push_back(_class);
			}
		}
	}

	return class_list;
}

std::string MonoRuntime::GetStringProperty(const char *propertyName, MonoClass *classType, MonoObject *classObject)
{
	MonoProperty *messageProperty;
	MonoMethod *messageGetter;
	MonoString *messageString;

	messageProperty = mono_class_get_property_from_name(classType, propertyName);
	if (messageProperty != nullptr)
	{
		messageGetter = mono_property_get_get_method(messageProperty);
		messageString = (MonoString*)mono_runtime_invoke(messageGetter, classObject, NULL, NULL);
		std::string str = mono_string_to_utf8(messageString);
		str = CP_SYS(str);

		return str;
	}

	return "";
}

void MonoRuntime::Execute(string methodName, void ** params, std::string stringParams)
{
	if (GetEngine->loadedScene.empty())
		return;

	Execute(GetEngine->GetSceneManager()->getRootSceneNode(), methodName, params, stringParams);

	destroyedObjects.clear();
}

void MonoRuntime::Execute(SceneNode* root, string methodName, void** params, std::string stringParams)
{
	bool destroyed = false;

	for (auto it = root->monoScripts.begin(); it != root->monoScripts.end(); ++it)
	{
		if (!root->getVisible())
			continue;

		bool _enabled = (*it)->enabled;

		if (!_enabled)
			continue;

		if (methodName == "Start")
		{
			if (!(*it)->startExecuted)
			{
				(*it)->startExecuted = true;
				Execute((*it)->object, methodName, params, stringParams);
			}
		}
		else
		{
			Execute((*it)->object, methodName, params, stringParams);
		}

		auto _it = find(objectsToDestroy.begin(), objectsToDestroy.end(), root);
		if (_it != objectsToDestroy.end())
			destroyed = true;

		DestroyNodes();

		if (destroyed)
			break;
	}

	if (!destroyed)
	{
		std::vector<Node*> c = root->getChildrenList();
		for (auto it = c.begin(); it != c.end(); ++it)
		{
			SceneNode* node = (SceneNode*)*it;
			auto del = find(destroyedObjects.begin(), destroyedObjects.end(), node);

			if (del == destroyedObjects.end())
				Execute(node, methodName, params, stringParams);
		}
	}
}

void MonoRuntime::ExecuteForNode(SceneNode * node, string methodName, string className, void** params, std::string stringParams)
{
	if (GetEngine->loadedScene.empty())
		return;

	if (node == nullptr)
		return;

	if (!node->getVisible())
		return;

	MonoMethodDesc * methodDesc = nullptr;
	MonoMethod* method = nullptr;

	if (className.empty())
	{
		for (auto it = node->monoScripts.begin(); it != node->monoScripts.end(); ++it)
		{
			MonoObject* obj = (MonoObject*)(*it)->object;

			bool _enabled = (*it)->enabled;

			if (!_enabled)
				continue;

			Execute(obj, methodName, params, stringParams);

			bool _destroyed = false;
			auto _it = find(objectsToDestroy.begin(), objectsToDestroy.end(), node);
			if (_it != objectsToDestroy.end())
				_destroyed = true;

			DestroyNodes();

			if (_destroyed)
				break;
		}
	}
	else
	{
		auto it = find_if(node->monoScripts.begin(), node->monoScripts.end(), [className](MonoScript* script) -> bool { return mono_class_get_name(script->_class) == CP_UNI(className); });

		if (it != node->monoScripts.end())
		{
			MonoObject* obj = (MonoObject*)(*it)->object;

			bool _enabled = (*it)->enabled;

			if (!_enabled)
				return;

			Execute(obj, methodName, params, stringParams);

			DestroyNodes();
		}
	}
}

void MonoRuntime::Execute(MonoObject* object, string methodName, void** params, std::string stringParams)
{
	//Build a method description object
	MonoClass* mclass = mono_object_get_class(object);
	string class_name = mono_class_get_name(mclass);
	class_name = CP_SYS(class_name);

	string methodDescStr = string(class_name + ":" + methodName + "(" + stringParams + ")").c_str();
	MonoMethodDesc*  methodDesc = mono_method_desc_new(methodDescStr.c_str(), false);

	if (methodDesc)
	{
		//Search the method in the image
		MonoMethod* method = mono_method_desc_search_in_class(methodDesc, mclass);
		mono_method_desc_free(methodDesc);

		if (method)
		{
			MonoObject* except = nullptr;
			mono_runtime_invoke(method, object, params, (MonoObject**)&except);

			if (except != nullptr)
			{
				MonoClass* exceptionClass;
				MonoType* exceptionType;
				std::string _typeName, _message, _source, _stackTrace;

				exceptionClass = mono_object_get_class(except);
				exceptionType = mono_class_get_type(exceptionClass);
				_typeName = mono_type_get_name(exceptionType);
				_message = GetStringProperty("Message", exceptionClass, except);
				_source = GetStringProperty("Source", exceptionClass, except);
				_stackTrace = GetStringProperty("StackTrace", exceptionClass, except);

				std::cout << "Exception: " << _typeName << endl;
				if (_message != "")
					std::cout << _message << endl;
				if (_source != "")
					std::cout << _source << endl;
				if (_stackTrace != "")
					std::cout << _stackTrace << endl;
				std::cout << endl;
			}
		}
	}
}

void MonoRuntime::DestroyNodes()
{
	for (auto it = objectsToDestroy.begin(); it != objectsToDestroy.end(); ++it)
	{
		SceneNode* node = *it;
		Engine::Destroy(node);

		destroyedObjects.push_back(node);
	}

	objectsToDestroy.clear();
}

void MonoRuntime::CreateObjects()
{
	CreateObjects(GetEngine->GetSceneManager()->getRootSceneNode());
}

void MonoRuntime::CreateObjects(SceneNode* root)
{
	for (auto it = root->monoScripts.begin(); it != root->monoScripts.end(); ++it)
	{
		(*it)->_class = FindClass(classNames[*it]);
		(*it)->CreateInstance();
	}

	VectorIterator c = root->getChildIterator();
	while (c.hasMoreElements())
	{
		SceneNode* node = (SceneNode*)c.getNext();
		CreateObjects(node);
	}
}

void MonoRuntime::GetSceneNodeFromMonoObject(SceneNode * root, MonoObject * obj, SceneNode *& out_node)
{
	VectorIterator it = root->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode * node = (SceneNode*)it.getNext();

		if (node->gameObjectMono == obj)
		{
			out_node = node;
			break;
		}

		GetSceneNodeFromMonoObject(node, obj, out_node);
	}
}

void MonoRuntime::RebuildClassList()
{
	classNames.clear();
	RebuildClassList(GetEngine->GetSceneManager()->getRootSceneNode());
}

void MonoRuntime::RebuildClassList(SceneNode* root)
{
	for (auto it = root->monoScripts.begin(); it != root->monoScripts.end(); ++it)
	{
		classNames[*it] = CP_SYS(mono_class_get_name((*it)->_class));
	}

	VectorIterator c = root->getChildIterator();
	while (c.hasMoreElements())
	{
		SceneNode* node = (SceneNode*)c.getNext();
		RebuildClassList(node);
	}
}

void MonoRuntime::CheckSceneToLoad()
{
	if (!sceneToLoad.empty())
	{
		string _scene = sceneToLoad;
		sceneToLoad = "";

		SceneSerializer sceneSerializer;
		sceneSerializer.Deserialize(GetEngine->GetSceneManager(), _scene);
	}
}
