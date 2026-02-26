using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public abstract class Asset
    {
        internal Asset() { }

        private IntPtr asset_ptr = (IntPtr)0;

        public static bool Exists(string filename)
        {
            return INTERNAL_exists(filename);
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_exists(string filename);
    }
}
