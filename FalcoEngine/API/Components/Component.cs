using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Component
    {
        internal Component()
        {

        }

        private IntPtr this_ptr = (IntPtr)0;
        private IntPtr native_ptr = (IntPtr)0;

        /*----------- PUBLIC ------------*/

        public bool enabled { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public GameObject gameObject { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        /*----------- INTERNAL CALLS ------------*/


    }
}
