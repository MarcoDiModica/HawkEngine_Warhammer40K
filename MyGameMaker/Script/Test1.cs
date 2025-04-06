using HawkEngine;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Numerics;

public class Test1 : MonoBehaviour
{
    public GameObject testPrefab;
    public Vector2 testVector2;
    public Vector3 testVector3;
    //public Vector4 testVector4;

    public List<GameObject> testList = new List<GameObject>();
    public List<Vector2> testListVector2 = new List<Vector2>();
    public List<Vector3> testListVector3 = new List<Vector3>();
    public List<float> testListFloat = new List<float>();

    public enum TestEnum
    {
        Test1,
        Test2,
        Test3
    }

    public TestEnum testEnum;

    public Transform testTransform;
    public Collider testCollider;

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
            //Engineson.print("Test1 Vector4: " + testVector4);
        }

        if (Input.GetKeyDown(KeyCode.C))
        {
            gameObject.SelfDestroy();
            Engineson.print("Test1 Destroyed");
        }
    }
}
