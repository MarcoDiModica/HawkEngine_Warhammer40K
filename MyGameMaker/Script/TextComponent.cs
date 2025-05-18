using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class UIText : Component
    {
        //funciones
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetText(string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetTextColor(Vector4 color);

        [MethodImpl(MethodImplOptions.InternalCall)]

        public extern void SetTextSize(int size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetBoxSize(Vector2 size);

        private GameObject owner;

        public UIText(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            Engineson.print("TextComponent created");
        }

        public UIText()
        {
            Engineson.print("TextComponent default constructor");
        }

        public override void Awake() { }
        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }
}