using HawkEngine;
using System;
using System.Numerics;
using System.Runtime.InteropServices;

public class MusicManagerLvl1 : MonoBehaviour
{
    
    private string lvl1music = "Assets/Audio/Music/Warhammer_Level1.ogg";
    private string lvl1townmusic = "Assets/Audio/Music/Warhammer_Level1Church.ogg";
    private string lvl1pathmusic = "Assets/Audio/Music/Warhammer_Level1Boss.ogg";
    private string lvl1crashsmusic = "Assets/Audio/Music/Warhammer_Level1Boss.ogg";

    private GameObject townGO;
    private GameObject pathGO;
    private GameObject crashGO;
    public override void Start()
    {
        Audio.Play(lvl1music, true);
        townGO = GameObject.Find("churchCollider");
        crashGO = GameObject.Find("hallwayBossCollider");
        pathGO = GameObject.Find("pathCollider");

    }
    public override void OnTriggerEnter(GameObject other)
    {
        if (other == townGO)
        {
            Audio.Stop(lvl1music);
            Audio.Play(lvl1townmusic, true);

        }
        if (other == pathGO)
        {
            Audio.Stop(lvl1townmusic);
            Audio.Play(lvl1pathmusic, true);

        }

        if (other == crashGO)
        {
            Audio.Stop(lvl1pathmusic);
            Audio.Play(lvl1crashsmusic, true);
        }
    }


}
