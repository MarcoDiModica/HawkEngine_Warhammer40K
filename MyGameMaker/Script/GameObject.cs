using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    class GameObject
    {
        IntPtr CplusplusInstance; /* ptr to C++ instance */

        public GameObject(string name)
        {
            this.name = name;
            //CplusplusInstance = ptr;
        }

        public GameObject()
        {
            EngineCalls.print("I am a go created in C#");
            //CplusplusInstance = ptr;
        }


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        /* This attribute tells the compiler that the method's implementation is external 
         * and typically defined in a low-level or platform-specific library */
        public extern T AddComponent<T>();
        /* extern defines a method outside of the current Assembly */

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern T GetCompopnent<T>(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern T RemoveComponent<T>();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool HasComponent();



        //----LifeCycleMethods----// 
        //these will be called from C++ editor
        public virtual void Start() { }

        public virtual void Update() { }

        public virtual void FixedUpdate() { }



        //--------Fields-----------//


        public extern string name
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get; // Get from C++

            set; // Set in C#
        }
        public extern string tag
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get; // Get from C++

            set; // Set in C#
        }

    }
}
