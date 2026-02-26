using System;
using System.Runtime.CompilerServices;

namespace FalcoEngine
{
    public class Pass
    {
        private IntPtr pass_ptr = (IntPtr)0;

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Pass();

        public BlendingMode blendingMode { get { return (BlendingMode)INTERNAL_getBlendingMode(); } set { INTERNAL_setBlendingMode((int)value); } }

        public Shader vertexShader { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public Shader fragmentShader { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public IterationType iterationType
        {
            get
            {
                return (IterationType)INTERNAL_getIterationType();
            }

            set
            {
                INTERNAL_setIterationType((int)value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_getBlendingMode();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setBlendingMode(int val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_getIterationType();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setIterationType(int val);
    }
}
