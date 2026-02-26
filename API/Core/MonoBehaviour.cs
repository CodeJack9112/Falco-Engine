
/*-------------------------------*/
/*-------------------------------*/
/*------Falco Engine C# API------*/
/*-------------------------------*/
/*-------------------------------*/
/*-------Author: tracer0707------*/
/*------------2018-2019----------*/
/*-------------------------------*/
/*-------------------------------*/

using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public abstract class MonoBehaviour
    {
        /*-CALLING C++ METHODS-*/
        /*---------------------*/

        //Native pointer to unmanaged object
        private IntPtr native_ptr = (IntPtr)0;
        private IntPtr script_ptr = (IntPtr)0;
        //private bool active = true; // Dont't delete this! needed for internal use

        /*------------PUBLIC FIELDS------------*/
        /*-------------------------------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern MonoBehaviour();

        //Managed name string
        public string name { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public string uniqueName { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        //Enabled
        public bool enabled { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        //GameObject component
        public GameObject gameObject { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        //Transform component
        public Transform transform { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        //RigidBody component
        public Rigidbody rigidbody { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        //Animation component
        public Animation animation { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        //Audio source component
        public AudioSource audioSource { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        //NavMesh Agent component
        public NavMeshAgent navMeshAgent { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        //Get another attached script
        public MonoBehaviour GetMonoBehaviour(string className)
        {
            return INTERNAL_get_mono_behaviour(className);
        }

        //Get another attached script
        public T GetMonoBehaviour<T>()
        {
            return INTERNAL_get_mono_behaviour_t<T>(typeof(T));
        }

        //Get component
        public T GetComponent<T>()
        {
            return INTERNAL_get_component_t<T>(typeof(T));
        }

        /// <summary>
        /// Find object of type
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        public static T FindObjectOfType<T>(bool findInactive = true)
        {
            return INTERNAL_find_object_of_type_t<T>(typeof(T), findInactive);
        }

        /// <summary>
        /// Find objects of type
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        public static T[] FindObjectsOfType<T>(bool findInactive = true)
        {
            return INTERNAL_find_objects_of_type_t<T>(typeof(T), findInactive);
        }

        public void Destroy(GameObject gameObject)
        {
            INTERNAL_destroy_game_object(gameObject);
        }

        public void Destroy(Component component)
        {
            INTERNAL_destroy_component(component);
        }

        private void Destroy()
        {
            INTERNAL_destroy();
        }

        /*-----------INTERNAL CALLS------------*/
        /*-------------------------------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern MonoBehaviour INTERNAL_get_mono_behaviour(string className);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern T INTERNAL_get_mono_behaviour_t<T>(Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern T INTERNAL_get_component_t<T>(Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern T INTERNAL_find_object_of_type_t<T>(Type type, bool findInactive);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern T[] INTERNAL_find_objects_of_type_t<T>(Type type, bool findInactive);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_destroy_game_object(GameObject gameObject);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_destroy_component(Component component);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_destroy();
    }
}
