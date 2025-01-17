using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    public class Component
    {
        UIntPtr CplusplusInstance;

        public Component() { EngineCalls.print("component in C#"); }

        public Component(UIntPtr instance)
        {
            CplusplusInstance = instance;
        }
    }
}
