using HawkEngine;
using System;
using System.Numerics;

public class PlayerMovement : MonoBehaviour
{
    public float moveSpeed = 10.0f;
    public float walkSpeed = 5f;
    public float runSpeed = 30.0f;
    public float rotationSpeed = 30.0f;
    public float acceleration = 40.0f;
    public float deceleration = 15.0f;

    private Rigidbody rb;
    private Collider collider;
    private Transform transform;

    private float currentRotationAngle;
    private Vector3 moveDirection = Vector3.Zero;
    private Vector3 rotationDirection = Vector3.Zero;
    private PlayerDash playerDash;
    private PlayerInput playerInput;

    private GameObject playerCamera;
    private Transform cameraTransform;
    private PlayerController playerController;

    public PlayerData playerData;

    // Flag to track successful component initialization
    private bool componentsInitialized = false;

    public override void Awake()
    {
        try
        {
            // Find main camera safely
            playerCamera = GameObject.Find("MainCamera");
            if (playerCamera == null)
            {
                Engineson.print("WARNING: MainCamera not found in PlayerMovement");
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in PlayerMovement.Awake: {e.Message}");
        }
    }

    public override void Start()
    {
        try
        {
            rb = gameObject.GetComponent<Rigidbody>();
            collider = gameObject.GetComponent<Collider>();
            transform = gameObject.GetComponent<Transform>();
            playerDash = gameObject.GetComponent<PlayerDash>();
            playerInput = gameObject.GetComponent<PlayerInput>();
            playerController = gameObject.GetComponent<PlayerController>();

            if (rb == null || collider == null || transform == null)
            {
                Engineson.print("ERROR: PlayerMovement missing critical components (Rigidbody, Collider, or Transform)");
                return;
            }

            rb.SetMass(1.0f);

            currentRotationAngle = transform.eulerAngles.Y;

            if (playerCamera != null)
            {
                cameraTransform = playerCamera.GetComponent<Transform>();
                if (cameraTransform == null)
                {
                    Engineson.print("WARNING: Camera Transform not found");
                }
            }

            if (playerController != null)
            {
                playerData = playerController.playerData;
                if (playerData == null)
                {
                    Engineson.print("WARNING: PlayerData not found on PlayerController");
                }
            }
            else
            {
                Engineson.print("WARNING: PlayerController not found");
            }

            if (playerDash == null)
                Engineson.print("WARNING: PlayerDash component not found");

            if (playerInput == null)
                Engineson.print("WARNING: PlayerInput component not found");

            componentsInitialized = true;
            Engineson.print("PlayerMovement initialized successfully");
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in PlayerMovement.Start: {e.Message}");
        }
    }

    public override void Update(float deltaTime)
    {
        if (!componentsInitialized)
            return;

        try
        {
            bool isDashing = playerDash != null && playerDash.IsDashing;
            if (!isDashing)
            {
                UpdateMovement(moveDirection, deltaTime);

                Vector3 lookDirection = Vector3.Zero;
                if (playerInput != null)
                {
                    lookDirection = playerInput.GetCurrentLookDirection();

                    if (playerInput.IsShooting() && lookDirection != Vector3.Zero)
                    {
                        UpdateRotation(lookDirection, deltaTime);
                    }
                    else
                    {
                        UpdateRotation(moveDirection, deltaTime);
                    }
                }
                else
                {
                    UpdateRotation(moveDirection, deltaTime);
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in PlayerMovement.Update: {e.Message}");
        }
    }

    public override void OnCollisionEnter(GameObject other)
    {
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
        if (!componentsInitialized || rb == null)
            return;

        try
        {
            Vector3 currentVelocity = rb.GetVelocity();

            UpdateMovementSpeed();

            Vector3 desiredVelocity = moveDirection * moveSpeed;

            if (playerInput != null && playerInput.IsShooting())
            {
                desiredVelocity /= 2f;
            }

            Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);

            rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in PlayerMovement.UpdateMovement: {e.Message}");
        }
    }

    private void UpdateMovementSpeed()
    {
        if (playerInput == null || playerData == null)
            return;

        try
        {
            if (!playerInput.IsKeyboardMoving())
            {
                if (playerData.GodMode)
                {
                    moveSpeed = (runSpeed + playerData.blackRageSpeed + playerData.stimmSpeed) * 3;
                }
                else
                {
                    Vector2 leftStick = Input.GetLeftStick();
                    float magnitude = leftStick.Length();

                    if (magnitude > 0.1f)
                    {
                        if (magnitude > 0.7f)
                            moveSpeed = runSpeed + playerData.blackRageSpeed + playerData.stimmSpeed;
                        else
                            moveSpeed = walkSpeed + playerData.blackRageSpeed + playerData.stimmSpeed;
                    }
                }
            }
        }
        catch (Exception e)
        {
            Engineson.print($"ERROR in PlayerMovement.UpdateMovementSpeed: {e.Message}");
        }
    }

    private void UpdateRotation(Vector3 direction, float deltaTime)
    {
        if (!componentsInitialized || collider == null)
            return;

        try
        {
            if (direction != Vector3.Zero)
            {
                float targetAngle = (float)Math.Atan2(direction.X, direction.Z);
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
        catch (Exception e)
        {
            Engineson.print($"ERROR in PlayerMovement.UpdateRotation: {e.Message}");
        }
    }

    public void SetSpeedToRun()
    {
        moveSpeed = runSpeed;
    }

    public void SetSpeedToWalk()
    {
        moveSpeed = walkSpeed;
    }

    private float Lerp(float start, float end, float t)
    {
        float clampedT = Math.Min(1, Math.Max(0, t));
        return start + (end - start) * clampedT;
    }
}