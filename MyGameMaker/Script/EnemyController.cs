using System;
using System.Numerics;
using HawkEngine;

public class EnemyController : MonoBehaviour
{
    private Transform player;
    private float distToChase = 10.0f;
    private float speedMovement = 15.0f;
    private Rigidbody rb;
    public float acceleration = 15.0f;
    Vector3 moveDirection;
    private float currentRotationAngle;
    public float rotationSpeed = 30.0f;
    private Collider collider;

    public override void Start()
    {
        
        player = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();

        if (player == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }

        collider = gameObject.GetComponent<Collider>();
        if (collider == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Collider component!");
            return;
        }
    }

    public override void Update(float deltaTime)
    {
        Vector3 playerPos = player.position;

        if (Vector3.Distance(gameObject.GetComponent<Transform>().position, playerPos) < distToChase)
        {
            Vector3 currentVelocity = rb.GetVelocity();
            moveDirection = Vector3.Normalize(playerPos - gameObject.GetComponent<Transform>().position);
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

        if (moveDirection != Vector3.Zero)
        {
            currentRotationAngle = GetComponent<Transform>().eulerAngles.Y;
            float targetAngle = (float)Math.Atan2(moveDirection.X, moveDirection.Z);
            float targetAngleDegrees = targetAngle * (180.0f / (float)Math.PI);

            while (targetAngleDegrees - currentRotationAngle > 180.0f) targetAngleDegrees -= 360.0f;
            while (targetAngleDegrees - currentRotationAngle < -180.0f) targetAngleDegrees += 360.0f;

            currentRotationAngle = Lerp(currentRotationAngle, targetAngleDegrees, rotationSpeed * deltaTime);

            Vector3 eulerRotation = new Vector3(0, currentRotationAngle, 0);
            Quaternion newRotation = Quaternion.CreateFromYawPitchRoll(
                eulerRotation.Y * ((float)Math.PI / 180.0f),
                eulerRotation.X * ((float)Math.PI / 180.0f),
                eulerRotation.Z * ((float)Math.PI / 180.0f)
            );

            collider.SetRotation(newRotation);
        }
    }
    private float Lerp(float start, float end, float t)
    {
        return start + (end - start) * Math.Min(1, Math.Max(0, t));
    }

}