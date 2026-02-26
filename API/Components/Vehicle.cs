using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace FalcoEngine
{
    public class Vehicle : Component
    {
        public struct Wheel
        {
            public float radius { get; set; }
            public float width { get; set; }
            public float suspensionStiffness { get; set; }
            public float suspensionDamping { get; set; }
            public float suspensionCompression { get; set; }
            public float suspensionRestLength { get; set; }
            public float friction { get; set; }
            public float rollInfluence { get; set; }
            public Vector3 direction { get; set; }
            public Vector3 axle { get; set; }
            public Vector3 connectionPoint { get; set; }
            public bool isFrontWheel { get; set; }
            public GameObject connectedObject { get; set; }
        }

        internal Vehicle()
        {

        }

        public int numWheels { [MethodImpl(MethodImplOptions.InternalCall)] get; }

        //Angle in degrees
        public void SetSteering(float angle, int wheelIndex)
        {
            INTERNAL_setSteering(angle, wheelIndex);
        }

        //Angle in degrees
        public float GetSteering(int wheelIndex)
        {
            return INTERNAL_getSteering(wheelIndex);
        }

        public void SetBreak(float value, int wheelIndex)
        {
            INTERNAL_setBreak(value, wheelIndex);
        }

        public void ApplyEngineForce(float value, int wheelIndex)
        {
            INTERNAL_applyEngineForce(value, wheelIndex);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setSteering(float angle, int wheel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern float INTERNAL_getSteering(int wheel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setBreak(float value, int wheel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_applyEngineForce(float value, int wheel);
    }
}
