using HawkEngine;
using System;
using System.Numerics;
using System.Runtime.InteropServices;

public class MusicManager : MonoBehaviour
{
    private string lvl2music = "Assets/Audio/Music/Level2AmbientBeta_Build2.ogg";
    private string lvl2churchmusic = "Assets/Audio/Music/Warhammer_Level2Church.ogg";
    private string lvl2bossmusic = "Assets/Audio/Music/Warhammer_Level2BossThemeHallway.ogg";

    public override void Start()
    {
        Audio.Play(lvl2music, true);
    }
    public override void OnTriggerEnter(GameObject other)
    {
        if (other.tag == "churchCollider")
        {
            Audio.Stop(lvl2music);
            Audio.Play(lvl2churchmusic, true);
        }
        if (other.tag == "hallwayBossCollider")
        {
            Audio.Stop(lvl2churchmusic);
            Audio.Play(lvl2bossmusic, true);
        }
    }


}
