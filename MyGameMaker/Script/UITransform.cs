using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public enum Modes
    {
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT,
        LINEAR
    }
    public class UITransform : Component
    {
        //funciones
        [MethodImpl(MethodImplOptions.InternalCall)]

        public extern void DOMove(Vector3 position,float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMoveX(float positionX, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMoveY(float positionY, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOMoveZ(float positionZ, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScale(Vector3 scale, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScaleX(float scaleX, float duration, Modes modes);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DOScaleY(float scaleY, float duration, Modes modes);

        [MethodImpl (MethodImplOptions.InternalCall)]
        public extern void DOScaleZ(float scaleZ, float duration, Modes modes);

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
