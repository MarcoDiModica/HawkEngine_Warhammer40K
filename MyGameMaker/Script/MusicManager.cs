using HawkEngine;
using System;
using System.Numerics;
using System.Runtime.InteropServices;

public class MusicManager : MonoBehaviour
{
    private string lvl2music = "Assets/Audio/Music/Warhammer_Level2_Livingquarter.ogg";
    private string lvl2churchmusic = "Assets/Audio/Music/Warhammer_Level2_Church.ogg";
    private string lvl2bossmusic = "Assets/Audio/Music/Warhammer_Level2_TelecommunicationsHub.ogg";

    private bool churchMusicPlayed = false;
    private bool bossMusicPlayed = false;

    public override void Start()
    {
        // Only play initial music if this is the first collider or a master music manager
        if (gameObject.name == "churchCollider" || gameObject.name == "MusicManager")
        {
            Audio.Play(lvl2music, true);
        }
    }
    
    public override void OnTriggerEnter(GameObject other)
    {
        // Check if the entering object is the Player
        if (other.name == "Player")
        {
            // Check which collider this script is attached to by the GameObject name
            if (gameObject.name == "churchCollider" && !churchMusicPlayed)
            {
                Audio.Stop(lvl2music);
                Audio.Play(lvl2churchmusic, true);
                churchMusicPlayed = true;
                Engineson.print("Switched to church music");
            }
            else if (gameObject.name == "hallwayBossCollider" && !bossMusicPlayed)
            {
                Audio.Stop(lvl2churchmusic);
                Audio.Play(lvl2bossmusic, true);
                bossMusicPlayed = true;
                Engineson.print("Switched to boss area music");
            }
        }
    }


}
