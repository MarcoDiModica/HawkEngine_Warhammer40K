using HawkEngine;
using System.ComponentModel.Design;
using System.Numerics;

public class Test1 : MonoBehaviour
{
    public GameObject testPrefab;
    public Vector2 testVector2;
    public Vector3 testVector3;
    public Vector4 testVector4;
    public override void Start()
    {
        Engineson.print("Test1 Staart");
    }

    public override void Update(float deltaTime)
    {
        Engineson.print("Im a CUBEEE"); 

        if (testPrefab != null)
        {
            
        }

        if (Input.GetKeyDown(KeyCode.A))
        {
            //testPrefab.GetComponent<Transform>().position += new Vector3(0, 1, 0);
            //Vector values print
            Engineson.print("Test1 Vector2: " + testVector2);
            Engineson.print("Test1 Vector3: " + testVector3);
            Engineson.print("Test1 Vector4: " + testVector4);
        }

        if (Input.GetKeyDown(KeyCode.C))
        {
            gameObject.SelfDestroy();
            Engineson.print("Test1 Destroyed");
        }
    }
}
