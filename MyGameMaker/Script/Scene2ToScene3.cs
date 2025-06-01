using HawkEngine;
public class Scene2ToScene3 : MonoBehaviour
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
        if (changeScene)
        {
            changeScene = false;
            SceneManager.isLevel2 = false;
            SceneManager.isBossFight = true;
            SceneManager.LoadSceneWithFade("BetaRelease_Week1_Bossfight", 0.5f);
        }
    }
    public override void OnTriggerEnter(GameObject other)
    {
        if (other != null)
        {

            if (other.name == "Player" && !changeScene)
            {
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