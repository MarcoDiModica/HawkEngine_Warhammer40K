using HawkEngine;
using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class Rigidbody : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetVelocity(Vector3 velocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetVelocity();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void AddForce(Vector3 force);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetMass(float mass);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetMass();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetFriction(float friction);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetFriction();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetGravity(Vector3 gravity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetGravity();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetDamping(float linearDamping, float angularDamping);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector2 GetDamping();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetKinematic(bool isKinematic);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsKinematic();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void EnableContinuousCollision();

        private GameObject owner;

        public Rigidbody(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            Engineson.print("Rigidbody created");
        }

        public Rigidbody()
        {
            Engineson.print("Rigidbody default constructor");
        }

        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }
}
