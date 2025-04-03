using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace HawkEngine
{
    public class GameObject
    {
        UIntPtr CplusplusInstance; /* ptr to C++ instance */

        public GameObject(string name, UIntPtr C_doppleganger)
        {
            CplusplusInstance = C_doppleganger;
            this.name = name;
            Engineson.print("Ive gotten the name " + GetName());

            if (name == "Samson") {
                AddChild(Engineson.CreateGameObject("joe", null));
             }

           // EngineCalls.Destroy(this);
        }

        public T GetComponent<T>()
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
        extern internal T TryAddComponent<T>(int type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool active);
        public T AddComponent<T>()
        {
            return TryAddComponent<T>( Engineson.MapComponent(typeof(T)) );
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void AddChild(GameObject child);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject Find(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void AddScript(string scriptName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool active);

        //----LifeCycleMethods----// 
        //these will be called from C++ editor
        public virtual void Awake() { }

        public virtual void Start() { }

        public virtual void Update() { }

        public virtual void FixedUpdate() { }



        //--------Fields-----------//

       // private string name;
        //public extern string name
        //{
        //    [MethodImplAttribute(MethodImplOptions.InternalCall)]
        //    get; // Get from C++

        //    set; // Set in C#
        //}
        public string tag
        {
            get
            {
                return GetTag();
            }
            set
            {
                SetTag(value);
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern string GetTag();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void SetTag(string newTag);

        public string name
        {
            get
            {
                return GetName();
            }
            set
            {
                SetName(value);
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private  extern string GetName();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private  extern void SetName(string newTag);
        

        internal Vector3 SetGravity(Vector3 vector3)
        {
            throw new NotImplementedException();
        }


    }
}
