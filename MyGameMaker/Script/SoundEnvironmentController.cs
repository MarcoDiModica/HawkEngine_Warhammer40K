using HawkEngine;
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
    private Audio sound;
    private bool isPlaying = false;

    public override void Awake()
    {
        sound = gameObject.AddComponent<Audio>();
    }

    public override void Start()
    {
        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("EnvironmentSound: Audio component not found");
        }
    }

    public override void Update(float deltaTime) { }

    public override void OnTriggerEnter(GameObject other)
    {
        if (!isPlaying && index >= 0 && index < envSounds.Length)
        {
            sound.LoadAudio(envSounds[index]);
            sound.Play(true);
            isPlaying = true;
        }
    }

    public override void OnTriggerExit(GameObject other)
    {
        if (isPlaying)
        {
            sound.Stop();
            isPlaying = false;
        }
    }
}
