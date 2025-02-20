using HawkEngine;

public class ElMasterController : MonoBehaviour
{
    public float timer = 0;
    public float timeToChange = 1;

    public override void Start()
    {
        timer = 0;
    }

    public override void Update(float deltaTime)
    {
        timer += deltaTime;
        if (timer >= timeToChange)
        {
            timer = 0;
            Engineson.print("MUCHO MACHU PICHU");
        }
    }
}