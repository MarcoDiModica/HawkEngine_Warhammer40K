using HawkEngine;

public class Scene1ToScene2 : MonoBehaviour
{
    private Collider nextLvlCollider;
    bool changeScene = false;

    public override void Start()
    {
        nextLvlCollider = gameObject.GetComponent<Collider>();


        if (nextLvlCollider == null)
        {
            Engineson.print("ERROR: No NextLvl object found");
        }
    }

    public override void Update(float deltaTime)
    {
        if(changeScene)
        {
            SceneManager.LoadScene("Level2");
        }
    }

    override public void OnTriggerEnter(GameObject other)
    {
        if (other != null)
        {
            if (other.name == "Player" && !changeScene)
            {
                changeScene = true;
            }
        }
    }
}