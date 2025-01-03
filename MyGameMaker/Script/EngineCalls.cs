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
        /* The implementation of theese methods lies in C++ */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void print(string message);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject CreateGameObject(string name);
    }
}
