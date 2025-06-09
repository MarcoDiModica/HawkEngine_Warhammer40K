using HawkEngine;

public class TimerToMenu : MonoBehaviour
{
    public float timer;
    public float timeToMenu = 15f; // Time in seconds before switching to the main menu
    bool once = false;

    public override void Awake()
    {

    }

    public override void Start()
    {

    }

    public override void Update(float deltaTime)
    {
        timer += deltaTime * 1;

        if (timer > timeToMenu && once == false)
        {
            SceneManager.LoadScene("MainMenu");
            once = true;
        }
    }
}