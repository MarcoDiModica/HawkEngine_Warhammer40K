using HawkEngine;
using System.Numerics;

public class Test1 : MonoBehaviour
{
    public override void Start()
    {
        Engineson.print("Test1 Start");
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetKeyDown(KeyCode.C))
        {
            Engineson.print("C key ss pressed");
            Engineson.print("Position");
        }
    }
}
