using HawkEngine;
public class Scene2ToMenu : MonoBehaviour
{
    private Collider TriggerLevel;
    bool changeScene = false;
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
            SceneManager.LoadScene("MainMenu");
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