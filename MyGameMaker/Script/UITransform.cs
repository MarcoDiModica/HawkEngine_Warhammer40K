using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class UITransform : Component
    {
        //funciones
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetUIScale(Vector3 scale);

        //contructor
        private GameObject owner;

        public UITransform(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            Engineson.print("UITransform created");
        }

        public UITransform()
        {
            Engineson.print("UITransform default constructor");
        }

        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }
}
