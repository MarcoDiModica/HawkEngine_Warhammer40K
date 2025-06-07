using System;
using System.Runtime.CompilerServices;
using HawkEngine;

public class AudioLevel2 : MonoBehaviour
{
    private string Lvl2Music = "Assets/Audio/Music/Level2AmbientBeta_Build2.ogg";
    private bool hasStartedMusic = false;

    public override void Update(float deltaTime)
    {
        // Disabled - MusicManager handles level 2 music switching
        // if (!hasStartedMusic)
        // {
        //     Audio.Play(Lvl2Music, true);
        //     hasStartedMusic = true;
        // }

        
    }

    public override void OnTriggerEnter(GameObject other)
    {
        // Disabled - MusicManager handles level 2 music switching
        // if (other != null)
        // {
        //     if (other.name == "Player")
        //     {
        //         Audio.Stop(Lvl2Music);
        //     }
        // }
    }




}
