using HawkEngine;
using System;
using System.Numerics;
using System.Runtime.InteropServices;

public class MusicManagerLvl1 : MonoBehaviour
{
    
    private string lvl1music = "Assets/Audio/Music/Warhammer_Level1_RuinedCity.ogg";
    private string lvl1townmusic = "Assets/Audio/Music/Warhammer_Level1_RuinedTown.ogg";
    private string lvl1pathmusic = "Assets/Audio/Music/Warhammer_Level1_Pathway.ogg";
    private string lvl1crashsmusic = "Assets/Audio/Music/Warhammer_Level1_CrashedShip.ogg";

    private GameObject townGO;
    private GameObject pathGO;
    private GameObject crashGO;
    private bool townMusicPlayed = false;
    private bool pathMusicPlayed = false;
    private bool crashMusicPlayed = false;

    public override void Start()
    {
        Audio.Play(lvl1music, true);
        townGO = GameObject.Find("townCollider");
        crashGO = GameObject.Find("crashCollider");
        pathGO = GameObject.Find("pathCollider");

    }
    public override void OnTriggerEnter(GameObject other)
    {
        // Check if the entering object is the Player
        if (other.name == "Player")
        {
            // Check which collider this component is attached to
            if (gameObject == townGO && !townMusicPlayed)
            {
                Audio.Stop(lvl1music);
                Audio.Play(lvl1townmusic, true);
                townMusicPlayed = true;
            }
            else if (gameObject == pathGO && !pathMusicPlayed)
            {
                Audio.Stop(lvl1townmusic);
                Audio.Play(lvl1pathmusic, true);
                pathMusicPlayed = true;
            }
            else if (gameObject == crashGO && !crashMusicPlayed)
            {
                Audio.Stop(lvl1pathmusic);
                Audio.Play(lvl1crashsmusic, true);
                crashMusicPlayed = true;
            }
        }
    }


}
