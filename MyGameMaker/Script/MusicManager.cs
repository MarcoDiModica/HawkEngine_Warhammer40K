using HawkEngine;
using System;
using System.Numerics;
using System.Runtime.InteropServices;

public class MusicManager : MonoBehaviour
{
    private string lvl2music = "Assets/Audio/Music/Warhammer_Level2_Livingquarter.ogg";
    private string lvl2churchmusic = "Assets/Audio/Music/Warhammer_Level2_Church.ogg";
    private string lvl2bossmusic = "Assets/Audio/Music/Warhammer_Level2_TelecommunicationsHub.ogg";

    private GameObject churchGO;
    private GameObject bossGO;

    public override void Start()
    {
        Audio.Play(lvl2music, true);
        churchGO = GameObject.Find("churchCollider");
        bossGO = GameObject.Find("hallwayBossCollider");

    }
    public override void OnTriggerEnter(GameObject other)
    {
        if (other == churchGO)
        {
            Audio.Stop(lvl2music);
            Audio.Play(lvl2churchmusic, true);
           
        }
        if (other == bossGO)
        {
            Audio.Stop(lvl2churchmusic);
            Audio.Play(lvl2bossmusic, true);
            
        }
    }


}
