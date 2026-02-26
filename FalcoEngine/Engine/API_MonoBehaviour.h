#pragma once

//API class for access all game objects in scene


#include "API.h"

class API_MonoBehaviour
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.MonoBehaviour::.ctor", (void*)ctor);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::get_name", (void*)getName);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::get_uniqueName", (void*)getUniqueName);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::get_enabled", (void*)getEnabled);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::set_enabled", (void*)setEnabled);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::get_transform", (void*)getTransform);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::get_rigidbody", (void*)getRigidbody);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::get_gameObject", (void*)getGameObject);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::get_animation", (void*)getAnimation);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::get_audioSource", (void*)getAudioSource);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::get_navMeshAgent", (void*)getNavMeshAgent);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::INTERNAL_get_mono_behaviour", (void*)getMonoBehaviour);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::INTERNAL_get_mono_behaviour_t", (void*)getMonoBehaviourT);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::INTERNAL_get_component_t", (void*)getComponentT);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::INTERNAL_destroy_game_object", (void*)destroyGameObject);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::INTERNAL_destroy_component", (void*)destroyComponent);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::INTERNAL_destroy", (void*)dctor);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::INTERNAL_find_object_of_type_t", (void*)findObjectOfTypeT);
		mono_add_internal_call("FalcoEngine.MonoBehaviour::INTERNAL_find_objects_of_type_t", (void*)findObjectsOfTypeT);
	}

private:
	//Constructor
	static void ctor(MonoObject * this_ptr)
	{
		
	}

	//Destructor
	static void dctor(MonoObject* this_ptr);

	//Get alias name
	static MonoString * getName(MonoObject * this_ptr);

	//Get unique name
	static MonoString * getUniqueName(MonoObject* this_ptr);

	//Get enabled
	static bool getEnabled(MonoObject * this_ptr);

	//Set enabled
	static void setEnabled(MonoObject * this_ptr, bool enabled);

	//Get transform
	static MonoObject * getTransform(MonoObject * this_ptr);

	//Get rigidbody
	static MonoObject * getRigidbody(MonoObject * this_ptr);

	//Get GameObject
	static MonoObject * getGameObject(MonoObject * this_ptr);

	//Get Animation
	static MonoObject * getAnimation(MonoObject * this_ptr);

	//Get audio source
	static MonoObject * getAudioSource(MonoObject * this_ptr);

	//Get navmesh agent
	static MonoObject * getNavMeshAgent(MonoObject * this_ptr);

	//Get mono behaviour by class name
	static MonoObject * getMonoBehaviour(MonoObject * this_ptr, MonoObject * className);

	//Get mono behaviour by class type
	static MonoObject * getMonoBehaviourT(MonoObject * this_ptr, MonoReflectionType * type);

	//Get component by class type
	static MonoObject * getComponentT(MonoObject * this_ptr, MonoReflectionType * type);
	
	//Find object of type
	static MonoObject* findObjectOfTypeT(MonoReflectionType* type, bool findInactive);

	//Find objects of type
	static MonoArray* findObjectsOfTypeT(MonoReflectionType* type, bool findInactive);

	//Destroy game object
	static void destroyGameObject(MonoObject * this_ptr, MonoObject * gameObject);
	
	//Destroy component
	static void destroyComponent(MonoObject * this_ptr, MonoObject * component);
};