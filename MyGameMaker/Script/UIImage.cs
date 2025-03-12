using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class UIImage : Component
    {
        //funciones
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetImage(string path);

        //contructor
        private GameObject owner;

        public UIImage(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            Engineson.print("UIImage created");
        }

        public UIImage()
        {
            Engineson.print("UIImage default constructor");
        }

        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }
}
