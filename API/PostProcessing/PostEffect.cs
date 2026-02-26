using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public abstract class PostEffect : MonoBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern PostEffect();

        //Link with engine internal post effect object
        private IntPtr posteffect_ptr = (IntPtr)0;
        private IntPtr posteffect_instance_ptr = (IntPtr)0;
        private IntPtr listener_ptr = (IntPtr)0;

        public new bool enabled { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
        public Camera camera { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public delegate void MaterialRenderEvent(int pass_id, Material material);
        public event MaterialRenderEvent onMaterialRender;

        /*----------- PUBLIC ------------*/

        public void Create(string name)
        {
            INTERNAL_create(name);
        }

        public PostEffectTechnique CreateTechnique()
        {
            return INTERNAL_createTechnique();
        }

        /*----------- INTERNAL CALLS ------------*/

        private void CallOnMaterialRender(int pass_id, Material material)
        {
            onMaterialRender?.Invoke(pass_id, material);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_create(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern PostEffectTechnique INTERNAL_createTechnique();
    }
}
