using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Camera : AttachedObject
    {
        internal Camera()
        {

        }

        public static Camera main { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public Vector3 WorldToScreenPoint(Vector3 world)
        {
            INTERNAL_worldToScreenPoint(ref world, out Vector3 screen);

            return screen;
        }

        public Vector3 ScreenToWorldPoint(Vector3 screen)
        {
            INTERNAL_screenToWorldPoint(ref screen, out Vector3 world);

            return world;
        }

        public Matrix4 GetViewMatrix(bool ownFrustumOnly = false)
        {
            INTERNAL_getViewMatrix(ownFrustumOnly, out Matrix4 matrix);
            return matrix;
        }

        public Matrix4 GetProjectionMatrixWithRSDepth()
        {
            INTERNAL_getProjectionMatrixWithRSDepth(out Matrix4 matrix);
            return matrix;
        }

        public bool IsObjectVisible(GameObject @object)
        {
            return INTERNAL_isObjectVisible(@object);
        }

        public float nearClipDistance { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
        public float farClipDistance { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        /// <summary>
        /// 8 points of corners. First 4 are near, second 4 are far. Top right, top left, bottom left, bottom right
        /// </summary>
        /// <returns>Vector3[8]</returns>
        public Vector3[] GetWorldSpaceCorners()
        {
            Vector3 nTopRight, nTopLeft, nBottomLeft, nBottomRight;
            Vector3 fTopRight, fTopLeft, fBottomLeft, fBottomRight;

            INTERNAL_getWorldSpaceCorners(out nTopRight, out nTopLeft, out nBottomLeft, out nBottomRight,
                out fTopRight, out fTopLeft, out fBottomLeft, out fBottomRight);

            return new Vector3[8] { nTopRight, nTopLeft, nBottomLeft, nBottomRight, fTopRight, fTopLeft, fBottomLeft, fBottomRight };
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getViewMatrix(bool ownFrustumOnly, out Matrix4 matrix);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getProjectionMatrixWithRSDepth(out Matrix4 matrix);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_worldToScreenPoint(ref Vector3 world, out Vector3 screen);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_screenToWorldPoint(ref Vector3 screen, out Vector3 world);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getWorldSpaceCorners(out Vector3 nTopRight, out Vector3 nTopLeft, out Vector3 nBottomLeft, out Vector3 nBottomRight,
                out Vector3 fTopRight, out Vector3 fTopLeft, out Vector3 fBottomLeft, out Vector3 fBottomRight);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool INTERNAL_isObjectVisible(GameObject @object);
    }
}
