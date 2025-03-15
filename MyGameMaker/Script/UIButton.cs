using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class UIButton : Component
    {
        //funciones
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern ButtonState GetState();

        //contructor
        private GameObject owner;

        public UIButton(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            Engineson.print("UIButton created");
        }

        public UIButton()
        {
            Engineson.print("UIButton default constructor");
        }

        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }

    public enum ButtonState
    {
        DISABLED = 0,
        DEFAULT = 1,
        HOVERED = 2,
        PRESSED = 3,
        CLICKED = 4
    }

}
