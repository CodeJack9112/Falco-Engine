
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Animation : Component
    {
        internal Animation() { }

        /*----------- PUBLIC ------------*/

        public delegate void AnimationEvent(Animation sender, string animName);
        public event AnimationEvent onEnded;

        public enum PlayMode
        {
            Prev = 0,
            Current = 1,
            Both = 2,
            None = 3
        }

        public enum PositionMode
        {
            Start = 0,
            End = 1,
            Current = 2
        }

        public bool isPlaying { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        public void Play(string name)
        {
            INTERNAL_play(name);
        }

        public void CrossFade(string name, float duration = 1.0f, PlayMode playMode = PlayMode.Both, PositionMode prevStatePosition = PositionMode.Current, PositionMode currentStatePosition = PositionMode.Current)
        {
            INTERNAL_crossFade(name, duration, (int)playMode, (int)prevStatePosition, (int)currentStatePosition);
        }

        public void Stop()
        {
            INTERNAL_stop();
        }

        public void SetLoop(string name, bool loop)
        {
            INTERNAL_setLoop(name, loop);
        }

        public bool GetLoop(string name)
        {
            return INTERNAL_getLoop(name);
        }

        public bool IsPlaying(string name)
        {
            return INTERNAL_isPlaying(name);
        }

        private void CallOnEnded(Animation sender, string animName)
        {
            onEnded?.Invoke(sender, animName);
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_play(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_crossFade(string name, float duration, int playMode, int prevStatePosition, int currentStatePosition);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_stop();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setLoop(string name, bool loop);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool INTERNAL_getLoop(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool INTERNAL_isPlaying(string name);
    }
}
