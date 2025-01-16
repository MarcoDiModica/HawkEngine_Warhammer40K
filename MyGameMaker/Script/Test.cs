using System;

public class Test : MonoBehaviour
{
    private float timer = 0.0f;
    public override void Start()
    {
        Console.WriteLine("Test Start Console.WriteLine");
        HawkEngine.EngineCalls.print("Hello from C# PrintEngineCall");
    }

    public override void Update(float deltaTime)
    {
        timer += deltaTime;
        if (timer >= 1.0f)
        {
            HawkEngine.EngineCalls.print("¡Ha pasado un segundo!");
            Console.WriteLine("Test Update Console.WriteLine");
            timer = 0.0f;
        }

        if (HawkEngine.Input.GetKeyDown(Convert.ToInt32(HawkEngine.KeyCode.A)))
        {
            HawkEngine.EngineCalls.print("A is pressed");
        }

        if (HawkEngine.Input.GetMouseButtonDown(1))
        {
            HawkEngine.EngineCalls.print("Left Click is pressed");
        }

    }
}

