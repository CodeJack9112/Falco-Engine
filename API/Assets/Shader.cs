using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Shader : Asset
    {
        public enum ShaderType
        {
            Vertex = 0,
            Fragment = 1
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Shader();

        public string name { [MethodImpl(MethodImplOptions.InternalCall)] get; }
        public ShaderType type { get { return (ShaderType)INTERNAL_getType(); } }

        public static Shader Find(string name)
        {
            return INTERNAL_find(name);
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Shader INTERNAL_find(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int INTERNAL_getType();
    }
}
