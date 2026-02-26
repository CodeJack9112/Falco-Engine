using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FalcoEngine
{
    public struct Vector2
    {
        public Vector2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x + b.x, a.y + b.y);
        }

        public float x;
        public float y;

        /// <summary>
        ///   <para>Returns a nicely formatted string for this vector.</para>
        /// </summary>
        /// <param name="format"></param>
        public override string ToString()
        {
            return String.Format("({0:F1}, {1:F1})", x, y);
        }

        /// <summary>
        ///   <para>Returns a nicely formatted string for this vector.</para>
        /// </summary>
        /// <param name="format"></param>
        public string ToString(string format)
        {
            return String.Format("({0}, {1})", x.ToString(format, CultureInfo.InvariantCulture.NumberFormat), y.ToString(format, CultureInfo.InvariantCulture.NumberFormat));
        }
    }
}
