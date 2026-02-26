using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Texture2D
    {
        private string _file_path = "";
        private IntPtr _texPtr = (IntPtr)0;

        public string fileName { get { return _file_path; } }
        public int width { [MethodImpl(MethodImplOptions.InternalCall)] get; }
        public int height { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public static Texture2D LoadFromFile(string path)
        {
            return INTERNAL_loadFromFile(path);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Texture2D INTERNAL_loadFromFile(string path);
    }
}
