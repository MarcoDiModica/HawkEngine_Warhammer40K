using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class UICanvas : Component
    {
        //funciones

        //contructor
        private GameObject owner;

        public UICanvas(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            Engineson.print("UICanvas created");
        }

        public UICanvas()
        {
            Engineson.print("UICanvas default constructor");
        }

        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }
}
