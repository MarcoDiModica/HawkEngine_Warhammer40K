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
            SceneManager.isLevel2 = true;
            SceneManager.isBossFight = false;
            SceneManager.LoadScene("BetaRelease_Week1_Lvl2");
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