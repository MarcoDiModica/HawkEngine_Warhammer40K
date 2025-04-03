using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class UITransform : Component
    {
        //funciones
        [MethodImpl(MethodImplOptions.InternalCall)]

        public extern void DOMoveUI(Vector3 position,float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMoveXUI(float positionX, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMoveYUI(float positionY, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMoveZUI(float positionZ, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScaleUI(Vector3 scale, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScaleXUI(float scaleX, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScaleYUI(float scaleY, float duration, Modes modes);

        [MethodImpl (MethodImplOptions.InternalCall)]
        public extern void DOScaleZUI(float scaleZ, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetScaleUI(Vector3 scale, Modes modes);

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
