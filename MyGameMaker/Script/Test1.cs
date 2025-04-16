using HawkEngine;
using System.Numerics;

public class Test1 : MonoBehaviour
{
    public float speed = 5f;
    public int index = 0;
    public override void Awake()
    {
        Engineson.print("Test1 Awake");
    }
    public override void Start()
    {
        Engineson.print("Test1 Start");
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetKeyDown(KeyCode.H))
        {
            gameObject.GetComponent<SkeletalAnimation>().PlayAnimOnce(index,0.1f);
        }

        if (Input.GetKeyDown(KeyCode.K) && gameObject.name == "playerMesh2")
        {
            index++;
        }
        if (Input.GetKeyDown(KeyCode.L))
        {
            GetComponent<SkeletalAnimation>().SetLoop(false);
        }

    }
}
