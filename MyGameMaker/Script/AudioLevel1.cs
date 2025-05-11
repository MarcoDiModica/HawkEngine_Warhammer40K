using System;
using System.Runtime.CompilerServices;
using HawkEngine;

public class AudioLevel1 : MonoBehaviour
{
        private string Lvl1Music = "Assets/Audio/Music/Level1_AmbientMusic_No_1_BetaBuild2.ogg";
        private bool hasStartedMusic = false;

    public override void Update(float deltaTime)
    {
        if (!hasStartedMusic)
        {
            Audio.Play(Lvl1Music, true);
            hasStartedMusic = true;
        }
    }




}


