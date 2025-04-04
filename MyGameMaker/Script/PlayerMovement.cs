using HawkEngine;
using System;
using System.Numerics;

public class PlayerMovement : MonoBehaviour
{
    public float moveSpeed = 10.0f;
    public float walkSpeed = 5; 
    public float runSpeed = 10.0f; 
    public float rotationSpeed = 30.0f;
    public float acceleration = 40.0f;
    public float deceleration = 15.0f;

    private Rigidbody rb;
    private Collider collider;

    private float currentRotationAngle;
    private Vector3 moveDirection = Vector3.Zero;
    private Vector3 rotationDirection = Vector3.Zero;
    private PlayerDash playerDash;
    private PlayerInput playerInput;

    private GameObject playerCamera;
    private Transform cameraTransform;
    private PlayerController playerController;

    public PlayerData playerData;

    public override void Awake()
    {
        
    }

    public override void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
        if (rb == null)
        {
           // Engineson.print("ERROR: PlayerMovement requires a Rigidbody component!");
            return;
        }

        rb.SetMass(1.0f);

        collider = gameObject.GetComponent<Collider>();
        if (collider == null)
        {
           // Engineson.print("ERROR: PlayerMovement requires a Collider component!");
            return;
        }

        playerDash = gameObject.GetComponent<PlayerDash>();

        Transform transform = gameObject.GetComponent<Transform>();
        if (transform != null)
        {
            currentRotationAngle = transform.eulerAngles.Y;
        }

        playerInput = gameObject.GetComponent<PlayerInput>();
        if (playerInput == null)
        {
           // Engineson.print("ERROR: PlayerMovement requires a PlayerInput component!");
            return;
        }

        playerCamera = GameObject.Find("MainCamera");
        if (playerCamera == null)
        {
            //Engineson.print("ERROR: playerCamera is NULL!");
            return;
        }

        cameraTransform = playerCamera.GetComponent<Transform>();
        if (cameraTransform == null) 
        {
           // Engineson.print("ERROR: cameraTransform is NULL!");
            return;
        }

        playerController = gameObject.GetComponent<PlayerController>();
        if (playerController == null)
        {
           // Engineson.print("ERROR: playerController is NULL!");
            return;
        }

        playerData = playerController.playerData;
    }

    public override void Update(float deltaTime)
    {
        //moveSpeed = playerData.movSpeed;
        //Engineson.print("Player Movement Update: " + moveSpeed);

        if (playerDash == null || !playerDash.IsDashing)
        {
            UpdateMovement(moveDirection, deltaTime);
            
            if (rotationDirection != Vector3.Zero)
            {
                UpdateRotation(rotationDirection, deltaTime);
            }
            else
            {
                UpdateRotation(moveDirection, deltaTime);
            }
        }
    }

    override public void OnCollisionEnter(GameObject other)
    {
        if (other != null)
        {
           // Engineson.print("OnCollisionEnter con: " + other.name);
            Engineson.print(other.GetComponent<Collider>().name);
        }
        else
        {
           // Engineson.print("Error: GameObject other es nulo");

        }
    }

    public void SetMoveDirection(Vector3 direction)
    {
        moveDirection = direction;
    }

    public void SetLookDirection(Vector3 direction)
    {
        rotationDirection = direction;
    }

    private void UpdateMovement(Vector3 moveDirection, float deltaTime)
    {
        Vector3 currentVelocity = rb.GetVelocity();
        Vector2 leftStick = Input.GetLeftStick();
        float magnitude = leftStick.Length();

        
        if(playerData.GodMode == true)
        {
            moveSpeed = runSpeed*3;
        } else if (magnitude > 0.1f)
        {
            if (magnitude > 0.7f)
            {
                moveSpeed = runSpeed;
            }
            else
            {
                moveSpeed = walkSpeed;
            }

        }

        Vector3 desiredVelocity = moveDirection * moveSpeed;
        if (playerInput?.IsShooting() == true)
        {
            desiredVelocity /= 2f;
        }
        Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
        rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
    }
    private void UpdateRotation(Vector3 moveDirection, float deltaTime)
    {

        if (moveDirection != Vector3.Zero)
        {
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