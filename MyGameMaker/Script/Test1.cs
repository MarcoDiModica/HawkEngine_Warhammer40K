using HawkEngine;
using System.Numerics;

public class Test1 : MonoBehaviour
{
    public float value = 0.0f;
    private Transform transform;
    
    public override void Start()
    {
        Engineson.print("Test1 Start");
        transform = GetComponent<Transform>();
        if (transform == null)
        {
            Engineson.print("Transform is null");
        }
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            Engineson.print("B key was pressed");
            value += 1.0f;
            transform.position = new Vector3(value, 0.0f, 0.0f);
        }
    }
}
