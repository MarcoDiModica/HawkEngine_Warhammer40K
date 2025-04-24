using HawkEngine;
using System.Numerics;

public class Test1 : MonoBehaviour, IInteractable 
{
    public float speed = 5f;

    public Prefab prefab;
    public GameObject prefab2;

    //Mathf functions
    private float t = 0;
    private float angle = 0;
    private Vector3 origin = new Vector3(0, 0, 0);

    public bool hasInteracted { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }

    public void Interact()
    {
        throw new System.NotImplementedException();
    }

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
        
        if (Input.GetKeyDown(KeyCode.C))
        {
            Instantiate(prefab);
        }

        //----------- MATHF FUNCTIONS -------------------//
        //t += deltaTime;
        //var transform = gameObject.GetComponent<Transform>();

        //if (Input.GetKey(KeyCode.A))
        //{
        //    var start = transform.position;
        //    var end = new Vector3(-5, 0, 0);
        //    transform.position = Mathf.LerpVector3(start, end, deltaTime);
        //}

        //if (Input.GetKey(KeyCode.D))
        //{
        //    float raw = transform.position.X + 5 * deltaTime;
        //    float clamped = Mathf.Clamp(raw, -5, 5);
        //    transform.position = new Vector3(clamped, transform.position.Y, transform.position.Z);
        //}
        //if (Input.GetKey(KeyCode.W))
        //{
        //    float y = Mathf.MoveTowards(transform.position.Y, 3f, 1f * deltaTime);
        //    transform.position = new Vector3(transform.position.X, y, transform.position.Z);
        //}
        //if (Input.GetKey(KeyCode.S))
        //{
        //    float y = Mathf.SmoothStep(transform.position.Y, -3f, deltaTime);
        //    transform.position = new Vector3(transform.position.X, y, transform.position.Z);
        //}
        //if (Input.GetKey(KeyCode.Q))
        //{
        //    float z = Mathf.PingPong(t * 2f, 2f);
        //    transform.position = new Vector3(transform.position.X, transform.position.Y, z);
        //}

        //-----------GetComponent----------------//     
        if (Input.GetKey(KeyCode.A))
        {
            if (gameObject.GetComponent<IInteractable>() != null)
            {
                Engineson.print("Has IInteractable");
                //gameObject.RemoveComponent<IInteractable>();
            }
        }     

        //-------------- TRANSFORM TEST -------------------//
        //var tf = gameObject.transform;
        //if (Input.GetKey(KeyCode.F))
        //{
        //    tf.position += tf.forward * speed * deltaTime;
        //}
        //if (Input.GetKey(KeyCode.R))
        //{
        //    tf.position += tf.right * speed * deltaTime;
        //}
        //if (Input.GetKey(KeyCode.U))
        //{
        //    tf.position += tf.up * speed * deltaTime;
        //}
        //if (Input.GetKey(KeyCode.A))
        //{
        //    tf.RotateLocal(1f * deltaTime, new Vector3(0, 1, 0));
        //}
        //if (Input.GetKey(KeyCode.D))
        //{
        //    tf.RotateLocal(-1f * deltaTime, new Vector3(0, 1, 0));
        //}
        //if (Input.GetKeyDown(KeyCode.L))
        //{
        //    tf.LookAt(new Vector3(0, 0, 0));
        //    Engineson.print("LookAt origin");
        //}
        //if (Input.GetKey(KeyCode.W))
        //{
        //    tf.localScale += new Vector3(1f, 1f, 1f) * deltaTime;
        //}
        //if (Input.GetKey(KeyCode.Z))
        //{
        //    tf.localScale = new Vector3(1, 1, 1);
        //}
    }



}
