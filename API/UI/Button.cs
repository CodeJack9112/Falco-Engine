using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class Button : UIElement
    {
        internal Button()
        {

        }

        public Texture2D normalStateImage { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
        public Texture2D hoverStateImage { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
        public Texture2D pressedStateImage { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }
        public Texture2D disabledStateImage { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        public Color normalStateColor
        {
            get
            {
                INTERNAL_getNormalStateColor(out Color value);
                return value;
            }
            set
            {
                INTERNAL_setNormalStateColor(ref value);
            }
        }

        public Color hoverStateColor
        {
            get
            {
                INTERNAL_getHoverStateColor(out Color value);
                return value;
            }
            set
            {
                INTERNAL_setHoverStateColor(ref value);
            }
        }

        public Color pressedStateColor
        {
            get
            {
                INTERNAL_getPressedStateColor(out Color value);
                return value;
            }
            set
            {
                INTERNAL_setPressedStateColor(ref value);
            }
        }

        public Color disabledStateColor
        {
            get
            {
                INTERNAL_getDisabledStateColor(out Color value);
                return value;
            }
            set
            {
                INTERNAL_setDisabledStateColor(ref value);
            }
        }

        public bool interactable { [MethodImpl(MethodImplOptions.InternalCall)] get; [MethodImpl(MethodImplOptions.InternalCall)] set; }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getNormalStateColor(out Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setNormalStateColor(ref Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getHoverStateColor(out Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setHoverStateColor(ref Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getPressedStateColor(out Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setPressedStateColor(ref Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getDisabledStateColor(out Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setDisabledStateColor(ref Color value);
    }
}
