using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FalcoEngine
{
    public struct Collision
    {
        //The other rigidbody that this object collided with
        public Rigidbody other;

        //Collision point in world space
        public Vector3 point;

        //Collision normal in world space
        public Vector3 normal;
    }
}
