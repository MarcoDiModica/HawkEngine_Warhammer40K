using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    public abstract class Component
    {
        public UIntPtr CplusplusInstance;

        public string name { get; private set; }
        public bool enabled { get; private set; }

        public Component()
        {
            EngineCalls.print("component in C#");
            enabled = true;
        }

        public Component(UIntPtr instance)
        {
            CplusplusInstance = instance;
            enabled = true;
        }

        public abstract void Start();
        
        public abstract void Update(float deltaTime);

        public abstract void Destroy();

        [System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        private extern void SetEnabled(UIntPtr instance, bool enabled);

        [System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
        private extern void SetName(UIntPtr instance, string name);
    }
}
