using System;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public abstract class Component
    {
        public UIntPtr CplusplusInstance;

        public string name { get; private set; }
        public bool enabled { get; private set; }

        public Component()
        {
            //Engineson.print("component in C#");
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

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void SetEnabled(UIntPtr instance, bool enabled);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void SetName(UIntPtr instance, string name);
    }
}
