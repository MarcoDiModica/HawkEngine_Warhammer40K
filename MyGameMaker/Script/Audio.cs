using System;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public class Audio : Component
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Play(bool loop = false);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        public extern void Stop();
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Pause();
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool Resume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetVolume(float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float GetVolume();

        private GameObject owner;
        public Audio(UIntPtr nativeAudio, GameObject owner)
        {
            CplusplusInstance = nativeAudio;
            this.owner = owner;
            Engineson.print("Audio created");
        }

        public Audio()
        {
            Engineson.print("Audio default constructor");
        }

        public override void Start() { }
        public override void Update(float deltaTime) { }
        public override void Destroy() { }
    }
}