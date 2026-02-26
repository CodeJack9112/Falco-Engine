using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Material : Asset
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Material();

        public string name { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public RenderingType renderingType { get { return (RenderingType)INTERNAL_getRenderingType(); } set { INTERNAL_setRenderingType((int)value); } }

        public int passCount { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public Pass GetPass(int index)
        {
            return INTERNAL_getPass(index);
        }

        public Pass AddPass()
        {
            return INTERNAL_addPass();
        }

        public void RemovePass(int index)
        {
            INTERNAL_removePass(index);
        }

        public static Material Find(string name)
        {
            return INTERNAL_find(name);
        }

        //Get parameters

        public void GetParameter(string name, out Matrix3 matrix)
        {
            INTERNAL_getParameterMat3(name, out matrix);
        }

        public void GetParameter(string name, out Matrix4 matrix)
        {
            INTERNAL_getParameterMat4(name, out matrix);
        }

        public void GetParameter(string name, out Vector2 vector2)
        {
            INTERNAL_getParameterVec2(name, out vector2);
        }

        public void GetParameter(string name, out Vector3 vector3)
        {
            INTERNAL_getParameterVec3(name, out vector3);
        }

        public void GetParameter(string name, out Vector4 vector4)
        {
            INTERNAL_getParameterVec4(name, out vector4);
        }

        public void GetParameter(string name, out float flt)
        {
            flt = INTERNAL_getParameterFloat(name);
        }

        public void GetParameter(string name, out int val)
        {
            val = INTERNAL_getParameterInt(name);
        }

        public void GetParameter(string name, out AutoConstantType val)
        {
            val = (AutoConstantType)INTERNAL_getParameterAuto(name);
        }

        //Set parameters

        public void SetParameter(string name, Matrix3 matrix)
        {
            INTERNAL_setParameterMat3(name, ref matrix);
        }

        public void SetParameter(string name, Matrix4 matrix)
        {
            INTERNAL_setParameterMat4(name, ref matrix);
        }

        public void SetParameter(string name, Vector2 vector2)
        {
            INTERNAL_setParameterVec2(name, ref vector2);
        }

        public void SetParameter(string name, Vector3 vector3)
        {
            INTERNAL_setParameterVec3(name, ref vector3);
        }

        public void SetParameter(string name, Vector4 vector4)
        {
            INTERNAL_setParameterVec4(name, ref vector4);
        }

        public void SetParameter(string name, float flt)
        {
            INTERNAL_setParameterFloat(name, flt);
        }

        public void SetParameter(string name, int val)
        {
            INTERNAL_setParameterInt(name, val);
        }

        public void SetParameter(string name, AutoConstantType val)
        {
            INTERNAL_setParameterAuto(name, (int)val);
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Material INTERNAL_find(string name);

        //Get
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getParameterMat3(string name, out Matrix3 matrix);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getParameterMat4(string name, out Matrix4 matrix);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getParameterVec2(string name, out Vector2 vector2);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getParameterVec3(string name, out Vector3 vector3);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getParameterVec4(string name, out Vector4 vector4);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern float INTERNAL_getParameterFloat(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_getParameterInt(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_getParameterAuto(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Pass INTERNAL_getPass(int index);

        //Set
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setParameterMat3(string name, ref Matrix3 matrix);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setParameterMat4(string name, ref Matrix4 matrix);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setParameterVec2(string name, ref Vector2 vector2);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setParameterVec3(string name, ref Vector3 vector3);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setParameterVec4(string name, ref Vector4 vector4);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setParameterFloat(string name, float flt);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setParameterInt(string name, int val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setParameterAuto(string name, int val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_getRenderingType();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setRenderingType(int val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern Pass INTERNAL_addPass();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_removePass(int index);
    }
}
