using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public static class Time
    {
        public static int frameRate { [MethodImpl(MethodImplOptions.InternalCall)] get; }
        public static float deltaTime { [MethodImpl(MethodImplOptions.InternalCall)] get; }
        public static float timeScale { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
    }
}
