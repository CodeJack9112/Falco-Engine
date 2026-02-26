using System;

namespace FalcoEngine
{
    public class Font
    {
        private string _file_path = "";
        private IntPtr _nativeFntPtr = (IntPtr)0;
        private IntPtr _fntPtr = (IntPtr)0;

        public string fileName { get { return _file_path; } }
    }
}
