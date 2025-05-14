using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class TextComponent : Component
    {
        //funciones
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetText(string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetTextColor(Vector4 color);

        [MethodImpl(MethodImplOptions.InternalCall)]

        public extern void SetTextSize(int size);

        private GameObject owner;

        public TextComponent(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            Engineson.print("TextComponent created");
        }

        public TextComponent()
        {
            Engineson.print("TextComponent default constructor");
        }

        public override void Awake() { }
        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }
}