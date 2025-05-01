using System;
using System.Runtime.CompilerServices;

namespace HawkEngine
{
    public static class Audio
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int Play(string path, bool loop = false);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int PlayOneShot(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Stop(int audioId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Stop(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Pause(int audioId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Pause(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Resume(int audioId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Resume(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int PlayMusic(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StopMusic(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StopAllMusic();

        public static float MasterVolume
        {
            get { return GetMasterVolume(); }
            set { SetMasterVolume(value); }
        }

        public static float MusicVolume
        {
            get { return GetMusicVolume(); }
            set { SetMusicVolume(value); }
        }

        public static float SfxVolume
        {
            get { return GetSfxVolume(); }
            set { SetSfxVolume(value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetMasterVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetMasterVolume(float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetMusicVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetMusicVolume(float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetSfxVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetSfxVolume(float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetVolume(int audioId, float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetVolume(string path, float volume);

        // Group controls
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StopAll();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void PauseAll();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ResumeAll();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SchedulePlay(string path, float delay, bool loop = false);
    }
}