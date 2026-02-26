using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public static class SceneManager
    {
        public static string loadedScene { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public static void LoadScene(string fileName)
        {
            INTERNAL_load_scene(fileName);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_load_scene(string fileName);
    }
}
