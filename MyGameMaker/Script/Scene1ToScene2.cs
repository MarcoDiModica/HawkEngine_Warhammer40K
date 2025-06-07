using HawkEngine;
public class Scene1ToScene2 : MonoBehaviour
{
    private Collider TriggerLevel;
    bool changeScene = false;

    public override void Awake()
    {

    }

    public override void Start()
    {
        TriggerLevel = gameObject.GetComponent<Collider>();
        if (TriggerLevel == null)
        {
            Engineson.print("Error");
        }
    }
    public override void Update(float deltaTime)
    {
        if (changeScene) {
            changeScene = false;
            // Stop Level 1 music before transitioning to Level 2
            Audio.Stop("Assets/Audio/Music/Warhammer_Level1_RuinedCity.ogg");
            Audio.Stop("Assets/Audio/Music/Warhammer_Level1_RuinedTown.ogg");
            Audio.Stop("Assets/Audio/Music/Warhammer_Level1_Pathway.ogg");
            Audio.Stop("Assets/Audio/Music/Warhammer_Level1_CrashedShip.ogg");
            SceneManager.isLevel2 = true;
            SceneManager.isBossFight = false;
            SceneManager.LoadSceneWithFade("BetaRelease_Week1_Lvl2");
        }
    }
    public override void OnTriggerEnter(GameObject other)
    {
        if (other != null) { 

            if (other.name == "Player" && !changeScene) { 
                changeScene = true;
            }
        }

    }

    public override void OnCollisionStay(GameObject other)
    {
        if (other != null)
        {
            if (other.name == "Player")
            {
                Engineson.print("Enters the colider");
            }
        }
    }
}