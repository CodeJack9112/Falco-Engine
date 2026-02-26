using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public class UIElement : AttachedObject
    {
        internal UIElement()
        {

        }

        public enum CanvasAlignment { TopLeft, MiddleLeft, BottomLeft, TopRight, MiddleRight, BottomRight, TopMiddle, BottomMiddle, Center };

        public delegate void MouseEvent(UIElement sender);
        public event MouseEvent onMouseDown;
        public event MouseEvent onMouseUp;
        public event MouseEvent onMouseMove;
        public event MouseEvent onMouseEnter;
        public event MouseEvent onMouseLeave;

        public CanvasAlignment canvasAlignment
        {
            get
            {
                return (CanvasAlignment)INTERNAL_getAlignment();
            }
            set
            {
                INTERNAL_setAlignment((int)value);
            }
        }

        public Color color
        {
            get
            {
                INTERNAL_getColor(out Color value);
                return value;
            }
            set
            {
                INTERNAL_setColor(ref value);
            }
        }

        public Vector2 anchor
        {
            get
            {
                INTERNAL_getAnchor(out Vector2 value);
                return value;
            }
            set
            {
                INTERNAL_setAnchor(ref value);
            }
        }

        public Vector2 size
        {
            get
            {
                INTERNAL_getSize(out Vector2 value);
                return value;
            }
            set
            {
                INTERNAL_setSize(ref value);
            }
        }

        public Vector2 canvasPosition
        {
            get
            {
                INTERNAL_getPosition(out Vector2 value);
                return value;
            }
            set
            {
                INTERNAL_setPosition(ref value);
            }
        }

        /*----------- INTERNAL CALLS ------------*/

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getColor(out Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setColor(ref Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getAnchor(out Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setAnchor(ref Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int INTERNAL_getAlignment();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setAlignment(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getSize(out Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setSize(ref Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_getPosition(out Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void INTERNAL_setPosition(ref Vector2 value);

        private void CallOnMouseDown(UIElement sender)
        {
            onMouseDown?.Invoke(sender);
        }

        private void CallOnMouseUp(UIElement sender)
        {
            onMouseUp?.Invoke(sender);
        }

        private void CallOnMouseMove(UIElement sender)
        {
            onMouseMove?.Invoke(sender);
        }

        private void CallOnMouseEnter(UIElement sender)
        {
            onMouseEnter?.Invoke(sender);
        }

        private void CallOnMouseLeave(UIElement sender)
        {
            onMouseLeave?.Invoke(sender);
        }
    }
}
