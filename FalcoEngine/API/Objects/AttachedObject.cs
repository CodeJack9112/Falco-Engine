using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class AttachedObject
    {
        private IntPtr object_ptr = (IntPtr)0;

        internal AttachedObject()
        {

        }

        public GameObject gameObject { [MethodImpl(MethodImplOptions.InternalCall)] get; }
    }
}
