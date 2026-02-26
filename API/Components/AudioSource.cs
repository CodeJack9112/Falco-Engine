using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class AudioSource : Component
    {
        internal AudioSource()
        {

        }

        /*----------- PUBLIC ------------*/

        public bool isPlaying { [MethodImpl(MethodImplOptions.InternalCall)] get; }
        public bool isPaused { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public bool loop { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public float volume { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public float minDistance { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public float maxDistance { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public bool is2D { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public string fileName { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public void Play()
        {
            INTERNAL_play();
        }

        public void Pause()
        {
            INTERNAL_pause();
        }

        public void Resume()
        {
            INTERNAL_resume();
        }

        public void Stop()
        {
            INTERNAL_stop();
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_play();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_pause();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_resume();


        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_stop();
    }
}
