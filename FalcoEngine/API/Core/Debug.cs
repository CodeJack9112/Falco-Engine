using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;


namespace FalcoEngine
{
    public class Debug
    {
        public static void Log(string message)
        {
            INTERNAL_log(message);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_log(string message);
    }
}
