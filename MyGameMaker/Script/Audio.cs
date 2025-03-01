using System;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace HawkEngine
{
    public class Audio : Component
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public extern void InitSharedAudioEngine();
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public extern void ShutDownSharedAudioEngine();
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public extern bool LoadAudio(string filePath, bool isMusic = false);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public extern void Play(bool loop = false);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public extern void Stop();
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public extern void Pause();
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public extern bool Resume();

        private GameObject owner;
        public override void Start() { }
        public override void Update(float deltaTime) { }
        public override void Destroy() { }
    }
}