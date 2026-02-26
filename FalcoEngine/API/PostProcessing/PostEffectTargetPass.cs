using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class PostEffectTargetPass
    {
        public enum InputMode { None, Previous }

        private IntPtr pass_ptr = (IntPtr)0;

        internal PostEffectTargetPass() { }

        public InputMode inputMode { get { return (InputMode)INTERNAL_getInputMode(); } set { INTERNAL_setInputMode((int)value); } }

        public string output { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public bool onlyInitial { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public PostEffectPass CreatePass()
        {
            return INTERNAL_createPass();
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_getInputMode();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setInputMode(int mode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern PostEffectPass INTERNAL_createPass();
    }
}
