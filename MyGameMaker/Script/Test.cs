using System;

public class Test : MonoBehaviour
{
    private float timer = 0.0f;
    public override void Start()
    {
        Console.WriteLine("Test Start Console.WriteLine");
        HawkEngine.Engineson.print("Hello from C# PrintEngineCall");
    }

    public override void Update(float deltaTime)
    {
        timer += deltaTime;
        if (timer >= 1.0f)
        {
            HawkEngine.Engineson.print("¡Ha pasado un segundo!");
            Console.WriteLine("Test Update Console.WriteLine");
            timer = 0.0f;
        }

        //if (HawkEngine.Input.GetKeyDown(HawkEngine.KeyCode.A))
        //{
        //    HawkEngine.Engineson.print("A is pressed");
        //}

        if (HawkEngine.Input.GetMouseButtonDown(1))
        {
            HawkEngine.Engineson.print("Left Click is pressed");
        }

        int testAxis = HawkEngine.Input.GetAxis("Horizontal");

        if (testAxis != 0)
            HawkEngine.Engineson.print("Axis Horizontal: " + testAxis);

        if (HawkEngine.Input.anyKeyDown)
            HawkEngine.Engineson.print(HawkEngine.Input.inputString);

        
        

    }
}

