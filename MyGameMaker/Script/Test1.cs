using HawkEngine;
using System;
using System.Collections.Generic;
using System.Numerics;

public class Test1 : MonoBehaviour 
{
    private int backgroundMusicId;
    private int ambienceId;
    private int walkingSoundId;
    private Dictionary<string, int> soundEffects = new Dictionary<string, int>();

    private const string MUSIC_BACKGROUND = "Assets/Audio/testMusic.ogg";
    private const string MUSIC_BOSS = "Assets/Audio/testMusic2.wav";
    private const string AMBIENCE_FOREST = "Assets/Audio/testMusic.wav";
    private const string SFX_FOOTSTEP = "Assets/Audio/test1.wav";
    private const string SFX_JUMP = "Assets/Audio/test2.wav";
    private const string SFX_ATTACK = "Assets/Audio/test1.wav";
    private const string SFX_COLLECT = "Assets/Audio/test2.wav";

    public override void Start()
    {
        Audio.MasterVolume = 0.8f;
        Audio.MusicVolume = 0.6f;
        Audio.SfxVolume = 1.0f;

        //backgroundMusicId = Audio.PlayMusic(MUSIC_BACKGROUND);
        Audio.MusicVolume = 0.5f;

        ambienceId = Audio.Play(MUSIC_BACKGROUND, true);
        Audio.MasterVolume = 0.5f;

        Audio.SchedulePlay(SFX_COLLECT, 5.0f);
    }

    public override void Update(float deltaTime)
    {
        HandleInput();

        if (Input.GetKeyDown(KeyCode.M))
        {
            ToggleMusic();
        }

        if (Input.GetKeyDown(KeyCode.V))
        {
            CycleVolume();
        }

        if (Input.GetKeyDown(KeyCode.B))
        {
            SwitchBackgroundMusic();
        }
    }

    private bool isWalkingSoundPlaying = false;

    private void HandleInput()
    {
        if (Input.GetKeyDown(KeyCode.SPACE))
        {
            Audio.PlayOneShot(SFX_JUMP);
        }

        if (Input.GetKeyDown(KeyCode.E))
        {
            int attackId = Audio.PlayOneShot(SFX_ATTACK);
            soundEffects["attack"] = attackId;
        }

        if (Input.GetKeyDown(KeyCode.F))
        {
            int audioo = Audio.PlayOneShot(SFX_ATTACK);
            //soundEffects["collect"] = collectId;
        }

        bool isMoving = Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.A) ||
                    Input.GetKey(KeyCode.S) || Input.GetKey(KeyCode.D);

        if (isMoving && !isWalkingSoundPlaying)
        {
            walkingSoundId = Audio.Play(SFX_FOOTSTEP, loop: true);
            isWalkingSoundPlaying = true;
        }
        else if (!isMoving && isWalkingSoundPlaying)
        {
            Audio.Stop(walkingSoundId);
            isWalkingSoundPlaying = false;
        }
    }

    private bool IsPlaying(int audioId)
    {
        return audioId != 0;
    }

    private void ToggleMusic()
    {
        if (IsPlaying(backgroundMusicId))
        {
            Audio.Pause(backgroundMusicId);
            Engineson.print("Music paused");
        }
        else
        {
            Audio.Resume(backgroundMusicId);
            Engineson.print("Music resumed");
        }
    }

    private void CycleVolume()
    {
        if (Audio.MasterVolume > 0.7f)
        {
            Audio.MasterVolume = 0.5f;
            Engineson.print("Volume: Medium");
        }
        else if (Audio.MasterVolume > 0.3f)
        {
            Audio.MasterVolume = 0.2f;
            Engineson.print("Volume: Low");
        }
        else
        {
            Audio.MasterVolume = 1.0f;
            Engineson.print("Volume: High");
        }
    }

    private void SwitchBackgroundMusic()
    {
        Audio.StopMusic(MUSIC_BACKGROUND);
        backgroundMusicId = Audio.PlayMusic(MUSIC_BOSS);
        Engineson.print("Switched to boss music!");

        Audio.SchedulePlay(MUSIC_BACKGROUND, 10.0f, loop: true);
    }
}