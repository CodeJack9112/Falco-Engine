using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class NavMeshAgent : Component
    {
        internal NavMeshAgent()
        {

        }

        /*----------- PUBLIC ------------*/

        public Vector3 targetPosition
        {
            get
            {
                INTERNAL_getTargetPosition(out Vector3 position);
                return position;
            }
            set
            {
                INTERNAL_setTargetPosition(ref value);
            }
        }
        public float radius { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
        public float height { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
        public float speed { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
        public float acceleration { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getTargetPosition(out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setTargetPosition(ref Vector3 position);
    }
}
