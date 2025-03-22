using HawkEngine;
using System;
using System.Numerics;

public class PlayerMovement : MonoBehaviour
{
    public float moveSpeed = 41.0f;
    public float rotationSpeed = 30.0f;
    public float acceleration = 20.0f;
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

    public override void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
        if (rb == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Rigidbody component!");
            return;
        }

        rb.SetMass(1.0f);

        collider = gameObject.GetComponent<Collider>();
        if (collider == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Collider component!");
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
            Engineson.print("ERROR: PlayerMovement requires a PlayerInput component!");
            return;
        }

        playerCamera = GameObject.Find("MainCamera");
        if (playerCamera == null)
        {
            Engineson.print("ERROR: playerCamera is NULL!");
            return;
        }

        cameraTransform = playerCamera.GetComponent<Transform>();
        if (cameraTransform == null) 
        {
            Engineson.print("ERROR: cameraTransform is NULL!");
            return;
        }
    }

    public override void Update(float deltaTime)
    {
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

    //override public void OnCollisionEnter(Collider other)
    //{
    //    Engineson.print("Player Collision Enter:");
    //}

    //override public void OnCollisionStay(Collider other)
    //{
    //    Engineson.print("Player Collision Stay: ");
    //}

    //override public void OnCollisionExit(Collider other)
    //{
    //    Engineson.print("Player Collision Exit: ");
    //}

    //override public void OnTriggerEnter(Collider other)
    //{
    //    Engineson.print("Player Trigger Enter: ");
    //}

    //override public void OnTriggerStay(Collider other)
    //{
    //    Engineson.print("Player Trigger Stay: ");
    //}

    //override public void OnTriggerExit(Collider other)
    //{
    //    Engineson.print("Player Trigger Exit: ");
    //}

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
        Vector3 desiredVelocity = moveDirection * moveSpeed;
        
        if (playerInput?.IsShooting() == true)
        {
            desiredVelocity /= 1.3f;
        }

        if (desiredVelocity.LengthSquared() > 0)
        {
            desiredVelocity = Vector3.Normalize(desiredVelocity) * moveSpeed;
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