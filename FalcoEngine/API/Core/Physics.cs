using System.Runtime.CompilerServices;

namespace FalcoEngine
{
    public static class Physics
    {
        public static RaycastHit Raycast(Vector3 from, Vector3 to)
        {
            RaycastHit hit;

            INTERNAL_raycast(ref from, ref to, out hit);

            return hit;
        }

        public static Collider[] OverlapSphere(Vector3 center, float radius)
        {
            return INTERNAL_overlapSphere(ref center, radius);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_raycast(ref Vector3 from, ref Vector3 to, out RaycastHit hit);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Collider[] INTERNAL_overlapSphere(ref Vector3 center, float radius);
    }
}
