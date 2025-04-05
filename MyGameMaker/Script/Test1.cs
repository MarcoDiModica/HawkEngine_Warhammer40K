using HawkEngine;
using System.ComponentModel.Design;
using System.Numerics;

public class Test1 : MonoBehaviour
{
    public GameObject testPrefab;
    public override void Start()
    {
        Engineson.print("Test1 Staart");
    }

    public override void Update(float deltaTime)
    {
        Engineson.print("Im a CUBEEE"); 

        if (testPrefab != null)
        {
            if (Input.GetKeyDown(KeyCode.A))
            {
                testPrefab.GetComponent<Transform>().position += new Vector3(0, 1, 0);
            }
        }

        if (Input.GetKeyDown(KeyCode.C))
        {
            gameObject.SelfDestroy();
            Engineson.print("Test1 Destroyed");
        }
    }
}
