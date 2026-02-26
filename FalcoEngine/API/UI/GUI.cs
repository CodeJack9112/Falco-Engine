using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace FalcoEngine
{
    public enum WindowFlags
    {
        NoResize = 2,
        NoCollapse = 32,
        NoMove = 4,
        NoBringToFrontOnFocus = 8192,
        NoTitleBar = 1,
        NoScrollbar = 8
    }

    public static class GUI
    {
        public static Font AddFont(string fileName, int size)
        {
            return INTERNAL_add_font(fileName, size);
        }

        public static void SetFont(Font font)
        {
            INTERNAL_set_font(font);
        }

        public static void UnsetFont()
        {
            INTERNAL_unset_font();
        }

        public static void Label(string text, Vector2 position)
        {
            Color color = new Color(1, 1, 1, 1);
            INTERNAL_gui_label(text, ref position, ref color);
        }

        public static void Label(string text, Vector2 position, Color color)
        {
            INTERNAL_gui_label(text, ref position, ref color);
        }

        public static bool Button(string text, Vector2 position, Vector2 size)
        {
            return INTERNAL_gui_button(text, ref position, ref size);
        }

        public static void Image(Texture2D texture, Vector2 position, Vector2 size)
        {
            Color color = new Color(1, 1, 1, 1);
            INTERNAL_gui_image(texture, ref position, ref size, ref color);
        }

        public static void Image(Texture2D texture, Vector2 position, Vector2 size, Color color)
        {
            INTERNAL_gui_image(texture, ref position, ref size, ref color);
        }

        public static bool BeginWindow(string text, Vector2 position, Vector2 size)
        {
            return INTERNAL_gui_begin_window(text, ref position, ref size, 0.8f, true, 0);
        }

        public static bool BeginWindow(string text, Vector2 position, Vector2 size, float alpha)
        {
            return INTERNAL_gui_begin_window(text, ref position, ref size, alpha, true, 0);
        }

        public static bool BeginWindow(string text, Vector2 position, Vector2 size, float alpha, bool closeButton, WindowFlags flags)
        {
            return INTERNAL_gui_begin_window(text, ref position, ref size, alpha, closeButton, (int)flags);
        }

        public static bool BeginWindow(string text, Vector2 position, Vector2 size, WindowFlags flags)
        {
            return INTERNAL_gui_begin_window(text, ref position, ref size, 0.8f, true, (int)flags);
        }

        public static bool BeginWindow(string text, Vector2 position, Vector2 size, bool closeButton, WindowFlags flags)
        {
            return INTERNAL_gui_begin_window(text, ref position, ref size, 0.8f, closeButton, (int)flags);
        }

        public static void EndWindow()
        {
            INTERNAL_gui_end_window();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Font INTERNAL_add_font(string fileName, int size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_set_font(Font font);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_unset_font();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_gui_label(string text, ref Vector2 position, ref Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_gui_button(string text, ref Vector2 position, ref Vector2 size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_gui_begin_window(string text, ref Vector2 position, ref Vector2 size, float alpha, bool closeButton, int flags);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool INTERNAL_gui_end_window();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void INTERNAL_gui_image(Texture2D texture, ref Vector2 position, ref Vector2 size, ref Color color);
    }
}
