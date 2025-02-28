using System;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace HawkEngine
{
    public class Collider : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetTrigger(bool trigger);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsTrigger();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetPosition(Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Quaternion GetRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRotation(Quaternion rotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern Vector3 GetSize();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetSize(Vector3 size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool active);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SnapToPosition();

        private GameObject owner;

        public Collider(UIntPtr nativeCollider, GameObject owner)
        {
            CplusplusInstance = nativeCollider;
            this.owner = owner;
            Engineson.print("Collider created");
        }

        public Collider()
        {
            Engineson.print("Collider default constructor");
        }

        public override void Start() { }
        public override void Update(float deltaTime) { }
        public override void Destroy() { }
    

    //-------------Fields ----------------//
    }
}