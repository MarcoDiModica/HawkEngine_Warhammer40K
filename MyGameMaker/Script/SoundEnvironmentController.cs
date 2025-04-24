using HawkEngine;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;

public class SoundEnvironmentController : MonoBehaviour
{
    public int index = 0;

    private string[] envSounds = new string[]
    {
            "Assets/Audio/SFX/Environment/bushesMoving_ready.wav",
            "Assets/Audio/SFX/Environment/cableFailing_ready.wav",
            "Assets/Audio/SFX/Environment/drippingWater_ready.wav",
            "Assets/Audio/SFX/Environment/insect1_ready.wav",
            "Assets/Audio/SFX/Environment/windWhistling_ready.wav",
    };
    private AudioSource sound;
    private bool isPlaying = false;
    private List<AudioClip> sounds = new List<AudioClip>();

    public override void Awake()
    {
        sound = gameObject.AddComponent<AudioSource>();
    }

    public override void Start()
    {
        sound = gameObject.GetComponent<AudioSource>();
        if (sound == null)
        {
            Engineson.print("EnvironmentSound: Audio component not found");
        }

        for (int i = 0; i < envSounds.Length; i++)
        {
            AudioClip audioClip = new AudioClip(envSounds[i], "EnvSound" + i, true);
            sounds.Add(audioClip);
        }

    }

    public override void Update(float deltaTime) { }

    public override void OnTriggerEnter(GameObject other)
    {
        if (!isPlaying && index >= 0 && index < envSounds.Length)
        {
            sound.Play(sounds[index]);
            isPlaying = true;
        }
    }

    public override void OnTriggerExit(GameObject other)
    {
        if (isPlaying)
        {
            for (int i = 0; i < sounds.Count; i++)
            {
                sound.Stop(sounds[i]);
            }
            
            isPlaying = false;
        }
    }
}
