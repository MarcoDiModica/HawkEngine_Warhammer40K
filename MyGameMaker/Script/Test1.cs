using HawkEngine;
using System.Numerics;

public class Test1 : MonoBehaviour
{
    public float speed = 5f;
   
    public override void Start()
    {
        Engineson.print("Test1 Start");
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            Engineson.print("B key was pressed");
            SceneManager.LoadScene("Scene2");
        }
    }
}
