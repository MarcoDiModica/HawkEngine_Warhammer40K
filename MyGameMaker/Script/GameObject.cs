using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    public class GameObject
    {
        UIntPtr CplusplusInstance; /* ptr to C++ instance */

        // most of theese constructors are for testing
        public GameObject(string name)
        {
            this.name = name;
            EngineCalls.print("Ive been called with the name " + name);
            //CplusplusInstance = ptr;
        }

        public GameObject(string name, UIntPtr C_doppleganger)
        {
            this.name = name;

            CplusplusInstance = C_doppleganger;

            EngineCalls.print("Ive gotten the name " + GetName());

            if (name == "Samson") {
                AddChild(EngineCalls.CreateGameObject("joe"));
             }

           // EngineCalls.Destroy(this);
        }

        public GameObject()
        {
            EngineCalls.print("I am a go created in C#");
            //CplusplusInstance = ptr;
        }

        public GameObject(int num)
        {
            EngineCalls.print("Ive been called with number " + num);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        /* This attribute tells the compiler that the method's implementation is external 
         * and typically defined in a low-level or platform-specific library */
        public extern T AddComponent<T>();
        /* extern defines a method outside of the current Assembly */

        public  T GetComponent<T>()
        {
            return TryGetComponent<T>(typeof(T).ToString());

        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern T RemoveComponent<T>();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool HasComponent();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        //  there is an extra monoComponent, this ptr to the object instance
        extern internal T TryGetComponent<T>( /* monoObject ,*/ string type, int inputType = 0);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern string GetName();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void AddChild(GameObject child);


        //----LifeCycleMethods----// 
        //these will be called from C++ editor
        public virtual void Start() { }

        public virtual void Update() { }

        public virtual void FixedUpdate() { }



        //--------Fields-----------//

        private string name;
        //public extern string name
        //{
        //    [MethodImplAttribute(MethodImplOptions.InternalCall)]
        //    get; // Get from C++

        //    set; // Set in C#
        //}
        public extern string tag
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get; // Get from C++

            set; // Set in C#
        }

    }
}
