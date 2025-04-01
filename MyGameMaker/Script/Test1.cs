using HawkEngine;
using System.Numerics;

public class Test1 : MonoBehaviour
{
    public override void Start()
    {
        Engineson.print("Test1 Staart");
    }

    public override void Update(float deltaTime)
    {
        Engineson.print("Im a CUBEEeE");

        if (Input.GetKeyDown(KeyCode.C))
        {
            gameObject.SelfDestroy();
            Engineson.print("Test1 Destroyed");
        }
    }
}
