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
    private bool churchMusicPlayed = false;
    private bool bossMusicPlayed = false;

    public override void Start()
    {
        Audio.Play(lvl2music, true);
        churchGO = GameObject.Find("churchCollider");
        bossGO = GameObject.Find("hallwayBossCollider");

    }
    public override void OnTriggerEnter(GameObject other)
    {
        // Check if the entering object is the Player
        if (other.name == "Player")
        {
            // Check which collider this component is attached to
            if (gameObject == churchGO && !churchMusicPlayed)
            {
                Audio.Stop(lvl2music);
                Audio.Play(lvl2churchmusic, true);
                churchMusicPlayed = true;
            }
            else if (gameObject == bossGO && !bossMusicPlayed)
            {
                Audio.Stop(lvl2churchmusic);
                Audio.Play(lvl2bossmusic, true);
                bossMusicPlayed = true;
            }
        }
    }


}
