using HawkEngine;
using System.Numerics;


public class Test2 : MonoBehaviour
{
    private float t = 0;
    private Transform transform;

    public override void Awake()
    {
        transform = gameObject.GetComponent<Transform>();
    } 
    
    public override void Start()
    {
       
    }

    public override void Update(float deltaTime)
    {
        t += deltaTime;

        if (Input.GetKey(KeyCode.A))
        {
            var start = transform.position;
            var end = new Vector3(-5, 0, 0);
            transform.position = Mathf.LerpVector3(start, end, deltaTime);
        }

        if (Input.GetKey(KeyCode.D))
        {
            float raw = transform.position.X + 5 * deltaTime;
            float clamped = Mathf.Clamp(raw, -5, 5);
            transform.position = new Vector3(clamped, transform.position.Y, transform.position.Z);
        }

        if (Input.GetKey(KeyCode.W))
        {
            float y = Mathf.MoveTowards(transform.position.Y, 3f, 1f * deltaTime);
            transform.position = new Vector3(transform.position.X, y, transform.position.Z);
        }

        if (Input.GetKey(KeyCode.S))
        {
            float y = Mathf.SmoothStep(transform.position.Y, -3f, deltaTime);
            transform.position = new Vector3(transform.position.X, y, transform.position.Z);
        }
    }
}
