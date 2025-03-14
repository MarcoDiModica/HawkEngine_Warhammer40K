using System;
using System.Numerics;
using System.Runtime.CompilerServices;
using HawkEngine;


namespace HawkEngine
{
    public class SkeletalAnimation : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAnimationSpeed(float speed);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetAnimationSpeed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAnimation(int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern int GetAnimationIndex();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetAnimationLength();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetAnimationTime();

        [MethodImpl(MethodImplOptions.InternalCall)]

        public extern void SetAnimationPlayTime(float time);

        [MethodImpl(MethodImplOptions.InternalCall)]

        public extern void SetAnimationPlayState(bool play);

        [MethodImpl(MethodImplOptions.InternalCall)]

        public extern bool GetAnimationPlayState();





        private GameObject owner;

        public SkeletalAnimation(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            Engineson.print("Rigidbody created");
        }

        public SkeletalAnimation()
        {
            Engineson.print("Rigidbody default constructor");
        }

        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }
}
