using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;


namespace FalcoEngine
{
    public class AudioListener : Component
    {
        internal AudioListener()
        {

        }

        public float volume { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public bool paused { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
    }
}
