using System;
using HawkEngine;
public class Test : MonoBehaviour
{
    private float timer = 0.0f;
    public override void Start()
    {
        Console.WriteLine("Test Start Console.WriteLine");
        Engineson.print("Hello from C# PrintEngineCall");
    }

    public override void Update(float deltaTime)
    {
        timer += deltaTime;
        if (timer >= 1.0f)
        {
            Engineson.print("¡Ha pasado un segundo!");
            Console.WriteLine("Test Update Console.WriteLine");
            timer = 0.0f;
        }

        //if (HawkEngine.Input.GetKeyDown(HawkEngine.KeyCode.A))
        //{
        //    HawkEngine.Engineson.print("A is pressed");
        //}

        if (Input.GetMouseButtonDown(1))
        {
            Engineson.print("Left Click is pressed");
        }

        int testAxis = Input.GetAxis("Horizontal");

        if (testAxis != 0)
            Engineson.print("Axis Horizontal: " + testAxis);

        if (Input.anyKeyDown)
            Engineson.print(Input.inputString);

        
        

    }
}

