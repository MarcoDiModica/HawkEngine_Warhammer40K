using System.Numerics;
using HawkEngine;

public class EnemyController : MonoBehaviour
{
    private Transform player;
    private float distToChase = 10.0f;
    private float speedMovement = 15.0f;
    private Rigidbody rb;
    public float acceleration = 15.0f;

    public override void Start()
    {
        
        player = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();

        if (player == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }
    }

    public override void Update(float deltaTime)
    {
        Vector3 playerPos = player.position;

        if (Vector3.Distance(gameObject.GetComponent<Transform>().position, playerPos) < distToChase)
        {
            Vector3 currentVelocity = rb.GetVelocity();
            Vector3 moveDirection = Vector3.Normalize(playerPos - gameObject.GetComponent<Transform>().position);
            Vector3 desiredVelocity = moveDirection * speedMovement;

            if (desiredVelocity.LengthSquared() > 0)
            {
                desiredVelocity = Vector3.Normalize(desiredVelocity) * speedMovement;
            }

            Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
            rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
        }
        else
        {
            rb.SetVelocity(Vector3.Zero);
        }
    }
}