using System;
using System.Numerics;
using System.Runtime.CompilerServices;
using HawkEngine;

namespace HawkEngine
{
    public class ParticleFX : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ApplyPreset(int presetID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float SetOneShot();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Play();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Stop();
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void EmitBurst(int count);

        private GameObject owner;

        public ParticleFX(UIntPtr nativeRigidbody, GameObject owner)
        {
            CplusplusInstance = nativeRigidbody;
            this.owner = owner;
            //Engineson.print("ParticleFX created");
        }

        public ParticleFX()
        {
            //Engineson.print("ParticleFX default constructor");
        }

        public override void Awake() { }

        public override void Start() { }

        public override void Update(float deltaTime) { }

        public override void Destroy() { }
    }
}
