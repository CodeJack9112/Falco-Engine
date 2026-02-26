using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Entity : AttachedObject
    {
        public int subEntitiesCount { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public SubEntity GetSubEntity(int index)
        {
            return INTERNAL_getSubEntity(index);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern SubEntity INTERNAL_getSubEntity(int index);
    }
}
