using System;
using System.Numerics;
using HawkEngine;

public class PlayerCamera : MonoBehaviour
{
    public GameObject playerRef;
    public Camera cameraRef;
    private Transform cameraTransform;

    public float smoothness = 19.0f;

    public float maxOffsetDistance = 3.6f;
    public float offsetSmoothness = 5.0f;
    public float returnDelay = 0.2f;

    public Vector3 currentOffset = new Vector3(0, 20, -10.5f);
    public Vector3 targetOffset = new Vector3(0, 20, 0);
    private Vector3 offsetVelocity = Vector3.Zero;

    private Vector3 dashOffset = Vector3.Zero;
    private Vector3 dashOffsetVelocity = Vector3.Zero;
    private float dashOffsetReturnDelay = 0.2f;
    private float dashOffsetTimer = 0f;
    private bool isDashingCamera = false;

    public double originalFOV = 45.0;
    private float dashFOV = 40.0f;
    private double currentFOV;
    private double targetFOV;
    private double fovVelocity = 0;
    private float zoomSpeed = 5.0f;

    private bool followPlayer = true;

    private Vector3 originalRotation;

    private float timeSinceInput = 0f;


    private PlayerController playerController;
    private PlayerInput playerInput;
    private ShakeManager shakeManager;


    private bool isPanning = false;
    private Vector3 panStartOffset;
    private Vector3 panTargetOffset;
    private float panTimer = 0f;
    private float panHoldDuration = 0f;
    private float panLerpToSpeed = 1f;
    private float panLerpBackSpeed = 1f;
    private PanState panState = PanState.Inactive;

    private enum PanState { Inactive, MovingToPoint, Holding, Returning }


    Vector3 totalOffset;
    float panLerpDuration = 1.0f;

    public void PanToPoint(Vector3 worldTargetPosition, float holdDuration, float panSpeed, float returnSpeed)
    {
        Vector3 playerPos = playerRef.GetComponent<Transform>().position;
        Vector3 worldOffset = worldTargetPosition - playerPos;

        panStartOffset = currentOffset;
        panTargetOffset = worldOffset;
        panTimer = 0f;
        panHoldDuration = holdDuration;
        panLerpToSpeed = panSpeed;
        panLerpBackSpeed = returnSpeed;
        panState = PanState.MovingToPoint;
        isPanning = true;

        followPlayer = false;
        playerInput.BlockMovement();
    }

    public override void Awake()
    {
        currentFOV = originalFOV;
        targetFOV = originalFOV;
        originalRotation = gameObject.GetComponent<Transform>().GetEulerAngles();
    }

    public override void Start()
    {
        playerRef = GameObject.Find("Player");
        cameraRef = gameObject.GetComponent<Camera>();
        cameraTransform = gameObject.GetComponent<Transform>();

        if (playerRef == null)
        {
            Engineson.print("ERROR: PlayerCamera requires a GameObject named 'Player' in the scene!");
            return;
        }

        playerController = playerRef.GetComponent<PlayerController>();
        if (playerController == null)
        {
            Engineson.print("ERROR: PlayerCamera requires a PlayerController component on the Player GameObject!");
            return;
        }

        playerInput = playerRef.GetComponent<PlayerInput>();
        if (playerInput == null)
        {
            Engineson.print("ERROR: PlayerCamera requires a PlayerInput component on the Player GameObject!");
            return;
        }

        shakeManager = GameObject.Find("ShakeManager")?.GetComponent<ShakeManager>();
        if (shakeManager == null)
        {
            Engineson.print("ERROR: ShakeManager not found");
        }


        cameraRef.SetFollowTarget(playerRef, currentOffset, 0, true, true, true, smoothness);
        cameraRef.SetCameraFieldOfView(originalFOV * (Math.PI / 180.0));

        // originalRotation = cameraTransform.rotation;
    }

    public override void Update(float deltaTime)
    {

        if (Input.GetKeyDown(KeyCode.Q))
        {
            PanToPoint(new Vector3(25, 30, 20), 2.0f, 1.5f, 1.0f);
        }
        Vector2 rightStickInput = Input.GetRightStick();
        Vector2 leftStickInput = Input.GetLeftStick();

        CameraDebugUpdate(deltaTime);

        Vector3 baseOffset = new Vector3(-23.8f, 41.6f, 23.8f);

        if (rightStickInput != Vector2.Zero)
        {
            timeSinceInput = 0f;

            Vector3 aimDirection = new Vector3(rightStickInput.X, 0, rightStickInput.Y);
            float inputMagnitude = GetMagnitude(rightStickInput);
            float dynamicOffsetDistance = maxOffsetDistance * inputMagnitude;

            Vector3 camForward = cameraTransform.forward;
            Vector3 camRight = cameraTransform.right;

            camForward.Y = 0;
            camRight.Y = 0;

            camForward = Vector3.Normalize(camForward);
            camRight = Vector3.Normalize(camRight);

            targetOffset = baseOffset + (-camForward * dynamicOffsetDistance * aimDirection.Z + camRight * dynamicOffsetDistance * -aimDirection.X);
        }
        else
        {
            timeSinceInput += deltaTime;
            if (timeSinceInput > returnDelay)
            {
                targetOffset = baseOffset;
            }
        }

        if (isDashingCamera)
        {
            dashOffsetTimer += deltaTime;
            if (dashOffsetTimer > dashOffsetReturnDelay)
            {
                dashOffset = SmoothDampVector3(dashOffset, Vector3.Zero, ref dashOffsetVelocity, 1f / offsetSmoothness, deltaTime);
                if (dashOffset.Length() < 0.01f)
                {
                    dashOffset = Vector3.Zero;
                    dashOffsetTimer = 0f;
                    isDashingCamera = false;
                }
            }
        }


        if (isPanning)
        {
            panTimer += deltaTime;
            float t;

            switch (panState)
            {
                case PanState.MovingToPoint:
                    t = Mathf.Clamp01(panTimer / panLerpToSpeed);
                    totalOffset = Vector3.Lerp(panStartOffset, panTargetOffset, t);
                    if (t >= 1f)
                    {
                        panState = PanState.Holding;
                        panTimer = 0f;
                    }
                    break;

                case PanState.Holding:
                    totalOffset = panTargetOffset;
                    if (panTimer >= panHoldDuration)
                    {
                        panState = PanState.Returning;
                        panTimer = 0f;
                    }
                    break;

                case PanState.Returning:
                    t = Mathf.Clamp01(panTimer / panLerpBackSpeed);
                    totalOffset = Vector3.Lerp(panTargetOffset, panStartOffset, t);
                    if (t >= 1f)
                    {
                        panState = PanState.Inactive;
                        isPanning = false;
                        followPlayer = true;
                        playerInput.UnblockMovement();
                        cameraRef.SetFollowTarget(playerRef, currentOffset, 0, true, true, true, smoothness);
                    }
                    break;
            }
            
        }
        else
        {
            totalOffset = targetOffset + dashOffset;
            if (shakeManager != null)
                totalOffset += shakeManager.currentShakeOffset;
        }

        currentOffset = SmoothDampVector3(currentOffset, totalOffset, ref offsetVelocity, 1f / offsetSmoothness, deltaTime);
        cameraRef.SetOffset(currentOffset);

        if (targetFOV != currentFOV)
        {
            currentFOV = SmoothDamp(currentFOV, targetFOV, ref fovVelocity, 1f / zoomSpeed, deltaTime);
            cameraRef.SetCameraFieldOfView(currentFOV * (Math.PI / 180.0));
        }
    }

    public void CameraDebugUpdate(float deltaTime)
    {
        Vector2 rightStickInput = Input.GetRightStick();
        Vector2 leftStickInput = Input.GetLeftStick();


        if (Input.GetKeyDown(KeyCode.O))
        {
            followPlayer = !followPlayer;
            if (followPlayer)
            {
                cameraRef.SetFollowTarget(playerRef, currentOffset, 0, true, true, true, smoothness);
                cameraRef.SetCameraFieldOfView(originalFOV * (Math.PI / 180.0));
                cameraTransform.SetRotation(originalRotation.X, originalRotation.Y, originalRotation.Z);
            }
            else
            {
                cameraRef.SetFollowTarget(null, Vector3.Zero, 0, false, false, false, 0);
            }
        }


        if (!followPlayer)
        {

            if (leftStickInput != Vector2.Zero)
            {
                Vector3 moveDirection = new Vector3(-leftStickInput.X, 0, -leftStickInput.Y);
                Vector3 camForward = cameraTransform.forward;
                Vector3 camRight = cameraTransform.right;

                //Si quereis que la camara se mantenga con el angulo del juego descomentar esto
                //camForward.Y = 0;
                //camRight.Y = 0;

                camForward = Vector3.Normalize(camForward);
                camRight = Vector3.Normalize(camRight);

                Vector3 movement = camForward * moveDirection.Z + camRight * moveDirection.X;
                cameraTransform.position += movement * deltaTime * 50.0f;
            }


            if (rightStickInput != Vector2.Zero)
            {
                float rotationSpeed = 2.0f;


                cameraTransform.RotateLocal(-rightStickInput.X * rotationSpeed * deltaTime, Vector3.UnitY);


                cameraTransform.RotateLocal(rightStickInput.Y * rotationSpeed * deltaTime, cameraTransform.right);


                cameraTransform.AlignToGlobalUp(Vector3.UnitY);
            }

            if (Input.GetKeyDown(KeyCode.P) || Input.GetControllerButtonDown(ControllerButton.B))
            {
                Vector3 rayOrigin = cameraTransform.position;
                Vector3 rayDirection = cameraTransform.forward;
                GameObject hitObject = null;

                RayCast ray = new RayCast();
                ray.PerformRaycast(rayOrigin, rayDirection, 400);

                if (ray.hit.isHit)
                {
                    hitObject = ray.hit.gameObject;
                }

                if (hitObject != null)
                {
                    playerRef.GetComponent<Collider>().SetPosition(ray.hit.point + new Vector3(0, 2, 0));
                    Engineson.print("Player spawned at: " + ray.hit.point);
                }
                else
                {
                    Engineson.print("Raycast did not hit the floor.");
                }
            }

            return;
        }
    }

    public void StartDash(Vector3 dashDirection)
    {
        targetFOV = dashFOV;

        Vector3 up = cameraTransform.up;

        Vector3 flattenedDash = dashDirection - Vector3.Dot(dashDirection, up) * up;
        if (flattenedDash == Vector3.Zero)
            flattenedDash = cameraTransform.forward - Vector3.Dot(cameraTransform.forward, up) * up;

        flattenedDash = Vector3.Normalize(flattenedDash);

        Vector3 camForward = cameraTransform.forward - Vector3.Dot(cameraTransform.forward, up) * up;
        Vector3 camRight = cameraTransform.right - Vector3.Dot(cameraTransform.right, up) * up;

        camForward = Vector3.Normalize(camForward);
        camRight = Vector3.Normalize(camRight);

        float forwardComponent = Vector3.Dot(flattenedDash, camForward);
        float rightComponent = Vector3.Dot(flattenedDash, camRight);

        Vector3 offsetDir = camForward * forwardComponent + camRight * rightComponent;
        dashOffset = Vector3.Normalize(offsetDir) * 1.0f; // Puedes ajustar la intensidad

        dashOffsetTimer = 0f;
        isDashingCamera = true;
    }

    public void EndDash()
    {
        targetFOV = originalFOV;
    }

    public Vector3 SmoothDampVector3(Vector3 current, Vector3 target, ref Vector3 velocity, float smoothTime, float deltaTime)
    {
        if (smoothTime < 0.0001f) smoothTime = 0.0001f;

        float omega = 2f / smoothTime;
        float x = omega * deltaTime;
        float exp = 1f / (1f + x + 0.48f * x * x + 0.235f * x * x * x);

        Vector3 change = current - target;
        Vector3 originalTarget = target;
        float sqrmag = change.X * change.X + change.Y * change.Y + change.Z * change.Z;
        float maxChange = 1000f * smoothTime;
        float maxChangeSq = maxChange * maxChange;

        if (sqrmag > maxChangeSq)
        {
            float mag = (float)Math.Sqrt(sqrmag);
            change = change * (maxChange / mag);
        }

        target = current - change;

        Vector3 temp = (velocity + change * omega) * deltaTime;
        velocity = (velocity - temp * omega) * exp;

        Vector3 output = target + (change + temp) * exp;

        Vector3 diffOriginalToCurrent = originalTarget - current;
        Vector3 diffOutputToTarget = output - originalTarget;

        float alignment =
            diffOriginalToCurrent.X * diffOutputToTarget.X +
            diffOriginalToCurrent.Y * diffOutputToTarget.Y +
            diffOriginalToCurrent.Z * diffOutputToTarget.Z;

        if (alignment > 0)
        {
            output = originalTarget;
            velocity = Vector3.Zero;
        }

        return output;
    }

    public double SmoothDamp(double current, double target, ref double velocity, float smoothTime, float deltaTime)
    {
        if (smoothTime < 0.0001f) smoothTime = 0.0001f;

        double omega = 2.0 / smoothTime;
        double x = omega * deltaTime;
        double exp = 1.0 / (1.0 + x + 0.48 * x * x + 0.235 * x * x * x);

        double change = current - target;
        double temp = (velocity + change * omega) * deltaTime;
        velocity = (velocity - temp * omega) * exp;

        double output = target + (change + temp) * exp;

        if ((target - current) * (output - target) > 0)
        {
            output = target;
            velocity = 0;
        }

        return output;
    }

    private float GetMagnitude(Vector2 vector)
    {
        return (float)Math.Sqrt((vector.X * vector.X) + (vector.Y * vector.Y));
    }

    public double Lerp(double start, double end, float t)
    {
        t = Clamp01(t);
        return start + (end - start) * t;
    }
    
    public Vector3 LerpVector3(Vector3 start, Vector3 end, float t)
    {
        t = Clamp01(t);
        return new Vector3(
            start.X + (end.X - start.X) * t,
            start.Y + (end.Y - start.Y) * t,
            start.Z + (end.Z - start.Z) * t
        );
    }

    private float Clamp01(float value)
    {
        if (value < 0) return 0;
        if (value > 1) return 1;
        return value;
    }
}
