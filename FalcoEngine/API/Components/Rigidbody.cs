using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Rigidbody : Component
    {
        internal Rigidbody()
        {

        }

        /*----------- PUBLIC ------------*/

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

        public Vector3 linearVelocity
        {
            get
            {
                INTERNAL_get_linear_velocity(out Vector3 value);
                return value;
            }
            set
            {
                INTERNAL_set_linear_velocity(ref value);
            }
        }

        public Vector3 angularVelocity
        {
            get
            {
                INTERNAL_get_angular_velocity(out Vector3 value);
                return value;
            }
            set
            {
                INTERNAL_set_angular_velocity(ref value);
            }
        }

        public bool isKinematic
        {
            get
            {
                return INTERNAL_get_is_kinematic();
            }
            set
            {
                INTERNAL_set_is_kinematic(value);
            }
        }

        public float mass
        {
            get
            {
                return INTERNAL_get_mass();
            }
            set
            {
                INTERNAL_set_mass(value);
            }
        }

        public void AddForce(Vector3 force)
        {
            AddForce(force, Vector3.zero);
        }

        public void AddForce(Vector3 force, Vector3 rel_pos)
        {
            INTERNAL_add_force(ref force, ref rel_pos);
        }

        public void AddTorque(Vector3 torque)
        {
            INTERNAL_add_torque(ref torque);
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_get_position(out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_position(ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_get_rotation(out Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_rotation(ref Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_get_linear_velocity(out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_linear_velocity(ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_get_angular_velocity(out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_angular_velocity(ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_add_force(ref Vector3 force, ref Vector3 rel_pos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_add_torque(ref Vector3 torque);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool INTERNAL_get_is_kinematic();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_is_kinematic(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern float INTERNAL_get_mass();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_mass(float value);
    }
}
