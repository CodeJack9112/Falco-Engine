using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace FalcoEngine
{
    public static class Random
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        //Generate random number in range[min (Inclusive), max (Inclusive)]
        public extern static float Range(float min, float max);
    }
}
