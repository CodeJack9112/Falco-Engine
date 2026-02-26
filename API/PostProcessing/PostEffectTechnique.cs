using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class PostEffectTechnique
    {
        private IntPtr technique_ptr = (IntPtr)0;
        private PostEffectTargetPass finalPass = null;

        internal PostEffectTechnique() { }

        public PostEffectTargetPass outputPass { get { return finalPass; } }

        public PostEffectTexture CreateTexture(string name)
        {
            return INTERNAL_createTexture(name);
        }

        public PostEffectTargetPass CreateTargetPass()
        {
            return INTERNAL_createTargetPass();
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern PostEffectTexture INTERNAL_createTexture(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern PostEffectTargetPass INTERNAL_createTargetPass();
    }
}
