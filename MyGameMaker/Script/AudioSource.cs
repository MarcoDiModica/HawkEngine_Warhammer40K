using System;
using System.Runtime.CompilerServices;
using System.Numerics;
using HawkEngine;
using System.Collections.Generic;

namespace HawkEngine
{
    public class AudioClip
    {
        public string path;
        public string name;
        public bool loop;

        public AudioClip(string path, string name, bool loop)
        {
            this.path = path;
            this.name = name;
            this.loop = loop;
        }

    }

    public class AudioSource : Component
    {
        public void LoadAudioClip(AudioClip audioClip)
        {
            LoadSound(audioClip.path, audioClip.loop);
        }

        public void Play(AudioClip audioClip)
        {
            PlaySound(audioClip.path);
        }

        public void Stop(AudioClip audioClip)
        {
            StopSound(audioClip.path);
        }

        public void Pause(AudioClip audioClip)
        {
            PauseSound(audioClip.path);
        }

        public void Resume(AudioClip audioClip)
        {
            ResumeSound(audioClip.path);
        }

        public void SetVolume(AudioClip audioClip, float volume)
        {
            SetVolumeSound(audioClip.path, volume);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void LoadSound(string path, bool loop);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void PlaySound(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void StopSound(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void PauseSound(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void ResumeSound(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetVolumeSound(string path, float volume);

        public GameObject owner;
        public AudioSource(UIntPtr nativeAudioSource, GameObject owner)
        {
            CplusplusInstance = nativeAudioSource;
            this.owner = owner;
            Engineson.print("AudioSource created");
        }

        public AudioSource()
        {
            Engineson.print("AudioSource default constructor");
        }

        public override void Awake() { }
        public override void Start() { }
        public override void Update(float deltaTime) { }
        public override void Destroy() { }
    }
}
