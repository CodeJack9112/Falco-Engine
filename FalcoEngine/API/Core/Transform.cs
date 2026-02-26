using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Transform
    {
        internal Transform()
        {

        }

        private IntPtr native_ptr = (IntPtr)0;

        /*----------- PUBLIC ------------*/

        public GameObject gameObject { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public Vector3 position
        {
            get
            {
                INTERNAL_get_position(out Vector3 value);
                return value;
            }
            set
            {
                INTERNAL_set_position(ref value);
            }
        }

        public Vector3 scale
        {
            get
            {
                INTERNAL_get_scale(out Vector3 value);
                return value;
            }
            set
            {
                INTERNAL_set_scale(ref value);
            }
        }

        public Quaternion rotation
        {
            get
            {
                INTERNAL_get_rotation(out Quaternion value);
                return value;
            }
            set
            {
                INTERNAL_set_rotation(ref value);
            }
        }

        public Vector3 localPosition
        {
            get
            {
                INTERNAL_get_local_position(out Vector3 value);
                return value;
            }
            set
            {
                INTERNAL_set_local_position(ref value);
            }
        }

        public Quaternion localRotation
        {
            get
            {
                INTERNAL_get_local_rotation(out Quaternion value);
                return value;
            }
            set
            {
                INTERNAL_set_local_rotation(ref value);
            }
        }

        public int childCount { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public Transform parent { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public Transform GetChild(int index)
        {
            return INTERNAL_get_child(index);
        }

        public Transform FindChild(string name)
        {
            return INTERNAL_find_child(name);
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_get_position(out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_position(ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_get_scale(out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_scale(ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_get_rotation(out Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_rotation(ref Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_get_local_position(out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_local_position(ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_get_local_rotation(out Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_local_rotation(ref Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Transform INTERNAL_get_child(int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Transform INTERNAL_find_child(string name);
    }
}