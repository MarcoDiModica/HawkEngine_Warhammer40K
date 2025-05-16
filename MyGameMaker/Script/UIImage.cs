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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetImageEnabled(bool enabled);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetImageHasAnimation(bool hasAnimation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetImageAnimationSpeed(float speed);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetImageAnimationIndexLimit(int indexLimit);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetImageAnimation(int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetImageSpriteSize(float width, float height);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetImageAnimIndex(int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void PlayStopAnimation(bool play);


        //contructor
        private GameObject owner;

        public UIImage(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            //Engineson.print("UIImage created");
        }

        public UIImage()
        {
            //Engineson.print("UIImage default constructor");
        }

        public override void Awake() { }
        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }
}
