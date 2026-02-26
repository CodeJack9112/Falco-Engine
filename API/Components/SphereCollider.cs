using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class SphereCollider : Collider
    {
        internal SphereCollider()
        {

        }

        /*----------- PUBLIC ------------*/

        public float radius { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
    }
}
