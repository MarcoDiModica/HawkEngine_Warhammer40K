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

    private bool lvl1musicPlayed = false;
    private bool townMusicPlayed = false;
    private bool pathMusicPlayed = false;
    private bool crashMusicPlayed = false;

    public override void Start()
    {
        // Only play initial music if this is the first collider or a master music manager
        if (gameObject.name == "townCollider" || gameObject.name == "MusicManagerLvl1")
        {
            Audio.Play(lvl1music, true);
            lvl1musicPlayed = true;
        }
    }
    
    public override void OnTriggerEnter(GameObject other)
    {
        // Check if the entering object is the Player
        if (other.name == "Player")
        {
            // Check which collider this script is attached to by the GameObject name
            if (gameObject.name == "townCollider" && !townMusicPlayed)
            {
                if (lvl1musicPlayed)
                {
                    Audio.Stop(lvl1music);
                }
                Audio.Play(lvl1townmusic, true);
                townMusicPlayed = true;
                Engineson.print("Switched to town music");
            }
            else if (gameObject.name == "pathCollider" && !pathMusicPlayed)
            {
                Audio.Stop(lvl1townmusic);
                Audio.Play(lvl1pathmusic, true);
                pathMusicPlayed = true;
                Engineson.print("Switched to path music");
            }
            else if (gameObject.name == "crashCollider" && !crashMusicPlayed)
            {
                Audio.Stop(lvl1pathmusic);
                Audio.Play(lvl1crashsmusic, true);
                crashMusicPlayed = true;
                Engineson.print("Switched to crash music");
            }
        }
    }


}
