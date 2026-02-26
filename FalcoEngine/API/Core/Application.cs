using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public static class Application
    {
        public static void Quit()
        {
            INTERNAL_quit();
        }

        public static string assetsPath { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_quit();
    }
}
