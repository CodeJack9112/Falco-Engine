using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class PostEffectPass
    {
        private IntPtr pass_ptr = (IntPtr)0;

        internal PostEffectPass() { }

        public enum PassType
        {
            Clear,           /// Clear target to one colour
            Stencil,         /// Set stencil operation
            RenderScene,     /// Render the scene or part of it
            RenderQuad,      /// Render a full screen quad
            RenderCustom     /// Render a custom sequence
        }

        public string materialName { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public int identifier { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public PassType passType { get { return (PassType)INTERNAL_getPassType(); } set { INTERNAL_setPassType((int)value); } }

        public void SetInput(int index, string textureName, int mrtIndex = 0)
        {
            INTERNAL_setInput(index, textureName, mrtIndex);
        }

        public void SetFirstRenderQueue(int queue)
        {
            INTERNAL_setFirstRenderQueue(queue);
        }

        public void SetLastRenderQueue(int queue)
        {
            INTERNAL_setLastRenderQueue(queue);
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setInput(int index, string textureName, int mrtIndex);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setPassType(int type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_getPassType();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_setFirstRenderQueue(int queue);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_setLastRenderQueue(int queue);
    }
}
