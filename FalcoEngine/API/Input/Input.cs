using System.Runtime.CompilerServices;

namespace FalcoEngine
{
    public class Input
    {
        public static Vector2 cursorDirection
        {
            get
            {
                INTERNAL_get_cursor_direction(out Vector2 value);
                return value;
            }
        }

        public static Vector2 cursorPosition
        {
            get
            {
                INTERNAL_get_cursor_position(out Vector2 value);
                return value;
            }
        }

        public static Vector2 cursorRelativePosition
        {
            get
            {
                INTERNAL_get_cursor_relative_position(out Vector2 value);
                return value;
            }
        }

        public static bool GetKey(ScanCode key)
        {
            return INTERNAL_get_key((int)key);
        }

        public static bool GetKeyDown(ScanCode key)
        {
            return INTERNAL_get_key_down((int)key);
        }

        public static bool GetKeyUp(ScanCode key)
        {
            return INTERNAL_get_key_up((int)key);
        }

        public static bool GetMouseButton(int button)
        {
            return INTERNAL_get_mouse_button(button);
        }

        public static bool GetMouseButtonDown(int button)
        {
            return INTERNAL_get_mouse_button_down(button);
        }

        public static bool GetMouseButtonUp(int button)
        {
            return INTERNAL_get_mouse_button_up(button);
        }

        public static Vector2 GetMouseWheel()
        {
            INTERNAL_get_mouse_wheel(out Vector2 value);
            return value;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_get_cursor_direction(out Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_get_cursor_position(out Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_get_cursor_relative_position(out Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_get_key(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_get_key_down(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_get_key_up(int key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_get_mouse_button(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_get_mouse_button_down(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_get_mouse_button_up(int button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_get_mouse_wheel(out Vector2 value);
    }
}