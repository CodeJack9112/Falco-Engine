using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public static class Prefab
    {
        public static GameObject Instantiate(string prefabFileName)
        {
            Vector3 pos = new Vector3(0, 0, 0);
            Quaternion rot = new Quaternion(0, 0, 0, 1);

            return INTERNAL_instantiate(prefabFileName, ref pos, ref rot);
        }

        public static GameObject Instantiate(string prefabFileName, Vector3 position)
        {
            Quaternion rot = new Quaternion(0, 0, 0, 1);

            return INTERNAL_instantiate(prefabFileName, ref position, ref rot);
        }

        public static GameObject Instantiate(string prefabFileName, Vector3 position, Quaternion rotation)
        {
            return INTERNAL_instantiate(prefabFileName, ref position, ref rotation);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GameObject INTERNAL_instantiate(string prefabFileName, ref Vector3 position, ref Quaternion rotation);
    }
}
