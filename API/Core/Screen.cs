using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public static class Screen
    {
        public static int width { [MethodImpl(MethodImplOptions.InternalCall)] get; }
        public static int height { [MethodImpl(MethodImplOptions.InternalCall)] get; }
    }
}
