using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Text : UIElement
    {
        internal Text()
        {

        }

        public enum VerticalAlignment { Top, Middle, Bottom };
        public enum HorizontalAlignment { Left, Center, Right };

        public string text { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public string font { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public float fontSize { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public HorizontalAlignment horizontalAlignment
        {
            get
            {
                return (HorizontalAlignment)INTERNAL_get_horizontalAlignment();
            }
            set
            {
                INTERNAL_set_horizontalAlignment((int)value);
            }
        }

        public VerticalAlignment verticalAlignment
        {
            get
            {
                return (VerticalAlignment)INTERNAL_get_verticalAlignment();
            }
            set
            {
                INTERNAL_set_verticalAlignment((int)value);
            }
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_get_horizontalAlignment();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_horizontalAlignment(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_get_verticalAlignment();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_set_verticalAlignment(int value);
    }
}
