using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public class GameObject
    {
        private IntPtr native_ptr = (IntPtr)0;

        /*----------- PUBLIC ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern GameObject();

        public string name { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public string uniqueName { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public bool enabled { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public string tag { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public string layer { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

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

        public AttachedObject attachedObject { [MethodImpl(MethodImplOptions.InternalCall)] get; }

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

        public static GameObject Find(string name)
        {
            return INTERNAL_find(name);
        }

        public static GameObject GetByUniqueName(string name)
        {
            return INTERNAL_getByUniqueName(name);
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GameObject INTERNAL_find(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GameObject INTERNAL_getByUniqueName(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern MonoBehaviour INTERNAL_get_mono_behaviour(string className);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern T INTERNAL_get_mono_behaviour_t<T>(Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern T INTERNAL_get_component_t<T>(Type type);
    }
}
