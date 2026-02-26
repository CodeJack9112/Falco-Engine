#pragma once

#include "OgreIncludes.h"
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include "../Ogre/source/OgreMain/custom/MonoScript.h"

#include "../Mono/include/mono/jit/jit.h"
#include "../Mono/include/mono/metadata/assembly.h"
#include "../Mono/include/mono/metadata/debug-helpers.h"
#include "../Mono/include/mono/metadata/object.h"
#include "../Mono/include/mono/metadata/threads.h"
#include "../Mono/include/mono/metadata/mono-config.h"
#include "../Mono/include/mono/metadata/class.h"
#include "../Mono/include/mono/metadata/mono-gc.h"
#include "../Mono/include/mono/metadata/attrdefs.h"

#include <string>
#include <list>

using namespace std;

class MonoRuntime
{
public:
	enum CompileConfiguration { Debug, Release };

	class ObjectInstance
	{
	public:
		SceneNode* node = nullptr;
		MonoObject* instance = nullptr;
		MonoScript* script = nullptr;
		bool startExecuted = false;
	};

private:
	MonoDomain * domain = nullptr;
	MonoAssembly * assembly;
	MonoImage * image;

	MonoAssembly * base_assembly = nullptr;
	MonoImage * base_image = nullptr;

	SceneManager * mSceneMgr;

	bool startExecuted = false;
	string monoDirectory;
	string libDirectory;
	string solutionPath;

	std::list<MonoClass*> behaviourClasses;

	std::list<MonoClass*> GetAssemblyClassList(MonoImage * image);

	std::string GetStringProperty(const char *propertyName, MonoClass *classType, MonoObject *classObject);
	void RetrieveClassesAndFields();

	std::function<void(std::string)> buildCallback = nullptr;
	std::function<void(std::string)> logCallback = nullptr;

	std::string lastLog = "";
	std::string sceneToLoad = "";
	std::vector<SceneNode*> objectsToDestroy;
	std::vector<SceneNode*> destroyedObjects;

	void DestroyNodes();
	void CreateObjects(SceneNode* root);
	void RebuildClassList(SceneNode* root);

	std::map<MonoScript*, std::string> classNames;

public:
	typedef std::vector<std::pair<std::string, std::string>> FunctionList; //Store class name and function name
	typedef std::vector<MonoScript::MonoFieldInfo> FieldList; //Store field info
	typedef std::vector<MonoScript*> ScriptList;

	MonoRuntime();
	MonoRuntime(string monoDir, string libDir);
	~MonoRuntime();

	void SetMonoDirectory(string dir) { monoDirectory = dir; }
	void SetSolutionPath(string path) { solutionPath = path; }

	void CompileLibrary(CompileConfiguration config = CompileConfiguration::Debug, bool callCallbacks = true);
	void OpenLibrary();
	void CreateObjects();
	void CloseLibrary(bool cleanup = true);
	void Execute(string methodName, void ** params = nullptr, std::string stringParams = "");
	void Execute(SceneNode * root, string methodName, void ** params = nullptr, std::string stringParams = "");
	void ExecuteForNode(SceneNode * node, string methodName, string className = "", void** params = nullptr, std::string stringParams = "");
	void Execute(MonoObject* object, string methodName, void** params = nullptr, std::string stringParams = "");
	void SetSceneManager(SceneManager * manager) { mSceneMgr = manager; }
	void SetBuildCallback(std::function<void(std::string)> callback);
	void SetLogCallback(std::function<void(std::string)> callback);
	void CleanUp();
	void CleanUp(SceneNode * root);
	std::string GetLastLog() { return lastLog; }

	void MonoScriptSetFields(MonoScript * monoScript);
	void MonoScriptSetFieldsAllNodes(SceneNode * root);
	void GetSceneNodeFromMonoObject(SceneNode * root, MonoObject * obj, SceneNode *& out_node);
	void AddDestroyObject(SceneNode* node) { objectsToDestroy.push_back(node); }
	void LoadScene(std::string scene) { sceneToLoad = scene; }
	void CheckSceneToLoad();
	void RebuildClassList();

	MonoDomain * GetDomain() { return domain; }
	MonoImage * GetBaseImage() { return image; }
	FunctionList GetFunctions(SceneNode * node);
	FieldList GetFields(MonoScript * script);
	std::list<MonoClass*>& GetBehaviourClasses() { return behaviourClasses; }
	MonoClass * FindClass(std::string name);

	//std::list<ObjectInstance>& GetMonoObjects() { return monoObjects; }

public:
	//Classes
	MonoClass * behaviour_class;
	MonoClass * transform_class;
	MonoClass * rigidbody_class;
	MonoClass * vector2_class;
	MonoClass * vector3_class;
	MonoClass * vector4_class;
	MonoClass * color_class;
	MonoClass * quaternion_class;
	MonoClass * input_class;
	MonoClass * gameobject_class;
	MonoClass * animation_class;
	MonoClass * audiosource_class;
	MonoClass * audiolistener_class;
	MonoClass * navmeshagent_class;
	MonoClass * navmeshobstacle_class;
	MonoClass * component_class;
	MonoClass * font_class;
	MonoClass * posteffect_class;
	MonoClass * posteffect_technique_class;
	MonoClass * posteffect_texture_class;
	MonoClass * posteffect_pass_class;
	MonoClass * posteffect_target_pass_class;
	MonoClass * asset_class;
	MonoClass * material_class;
	MonoClass * pass_class;
	MonoClass * shader_class;
	MonoClass * attached_object_class;
	MonoClass * camera_class;
	MonoClass * entity_class;
	MonoClass * subentity_class;
	MonoClass * uielement_class;
	MonoClass * text_class;
	MonoClass * image_class;
	MonoClass * button_class;
	MonoClass * texture2d_class;
	MonoClass * collision_class;
	MonoClass * collider_class;
	MonoClass * meshcollider_class;
	MonoClass * boxcollider_class;
	MonoClass * spherecollider_class;
	MonoClass * capsulecollider_class;
	MonoClass * vehicle_class;

	MonoClassField * native_ptr_field;
	MonoClassField * script_ptr_field;
	//MonoClassField * enabled_field;
	MonoClassField * transform_native_ptr_field;
	MonoClassField * gameobject_native_ptr_field;
	MonoClassField * component_this_ptr_field;
	MonoClassField * component_native_ptr_field;
	MonoClassField * posteffect_this_ptr_field;
	MonoClassField * posteffect_instance_this_ptr_field;
	MonoClassField * posteffect_technique_ptr_field;
	MonoClassField * posteffect_technique_output_pass_field;
	MonoClassField * posteffect_texture_ptr_field;
	MonoClassField * posteffect_pass_ptr_field;
	MonoClassField * posteffect_target_pass_ptr_field;
	MonoClassField * posteffect_listener_ptr;
	MonoClassField * asset_ptr_field;
	MonoClassField * attached_object_ptr_field;
	MonoClassField * subentity_ptr_field;
	MonoClassField * pass_ptr_field;

	MonoClassField* vector2_x;
	MonoClassField* vector2_y;

	MonoClassField * vector3_x;
	MonoClassField * vector3_y;
	MonoClassField * vector3_z;

	MonoClassField* vector4_x;
	MonoClassField* vector4_y;
	MonoClassField* vector4_z;
	MonoClassField* vector4_w;

	MonoClassField * quaternion_x;
	MonoClassField * quaternion_y;
	MonoClassField * quaternion_z;
	MonoClassField * quaternion_w;

	MonoClassField* color_r;
	MonoClassField* color_g;
	MonoClassField* color_b;
	MonoClassField* color_a;

	MonoClassField* collision_other;
	MonoClassField* collision_point;
	MonoClassField* collision_normal;
};