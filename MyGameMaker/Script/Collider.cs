using System;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace HawkEngine
{
    public class Collider : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsTrigger();

        private UIntPtr _nativeCamera;
        private GameObject owner;

        public Collider(UIntPtr nativeTransform, GameObject owner)
        {
            CplusplusInstance = nativeTransform;
            this.owner = owner;

            Engineson.print("collider aqui");
        }

        public Collider()
        {
            Engineson.print("collider created");
        }

        public override void Start() { }
        public override void Update(float deltaTime) { }
        public override void Destroy() { }

        //-------------Fields ----------------//
        
    }
}