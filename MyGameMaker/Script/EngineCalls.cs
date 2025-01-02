using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    class EngineCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        // --------------------Marshal - treat string as C const char*
        public static extern void print(string message);
    }
}
