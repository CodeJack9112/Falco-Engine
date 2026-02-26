using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FalcoEngine
{
    public struct RaycastHit
    {
        public bool hasHit;
        public Rigidbody rigidbody;
        public Vector3 hitPoint;
        public Vector3 worldNormal;
    }
}
