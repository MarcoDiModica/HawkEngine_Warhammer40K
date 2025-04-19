using System;
using System.Numerics;
using HawkEngine;

public class PlayerCamera : MonoBehaviour
{
    public GameObject playerRef;
    private Camera cameraRef;
    private PlayerInput playerInput;
    private Transform cameraTransform;
    public float smoothness = 19.0f;

    public float maxOffsetDistance = 3.6f;
    public float offsetSmoothness = 25.0f;

    public Vector3 currentOffset = new Vector3(0, 20, -10.5f);
    private Vector3 targetOffset = new Vector3(0, 20, 0);
    private double fieldOfView;

    public double  originalFOV = 60.0f;
    public float dashFOV = 55.0f;
    public double currentFOV;

    private double targetFOV;
    private float zoomSpeed = 5.0f;
    private Vector3 offsetVelocity = Vector3.Zero;
    private double fovVelocity = 0;

    public override void Awake()
    {
        currentFOV = originalFOV;
        targetFOV = originalFOV;
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
        else
        {
                cameraRef.SetFollowTarget(playerRef, currentOffset, 0, true, true, true, smoothness);
                cameraRef.SetCameraFieldOfView(originalFOV * (System.Math.PI / 180.0));
                Engineson.print("Camera FOV: " + originalFOV * (System.Math.PI / 180.0));
 
        }

        if (cameraRef == null)
        {
            Engineson.print("ERROR: PlayerCamera requires a Camera component!");
            return;
        }
    }

    public override void Update(float deltaTime)
    {
        Vector2 leftStickInput = Input.GetLeftStick();
        Vector2 rightStickInput = Input.GetRightStick();

        Vector3 baseOffset = new Vector3(-11.9f, 19.8f, -12.2f);

        //if (leftStickInput != Vector2.Zero)
        //{
        //    Vector3 movementDirection = new Vector3(leftStickInput.X, 0, leftStickInput.Y);

        //    float inputMagnitude = GetMagnitude(leftStickInput);

        //    Vector3 camForward = cameraTransform.forward;
        //    Vector3 camRight = cameraTransform.right;

        //    camForward.Y = 0;
        //    camRight.Y = 0;

        //    camForward = Vector3.Normalize(camForward);
        //    camRight = Vector3.Normalize(camRight);

        //    targetOffset = baseOffset + (-camForward * maxOffsetDistance * movementDirection.Z + camRight * maxOffsetDistance * movementDirection.X);
        //}
         if (rightStickInput != Vector2.Zero)
        {
            Vector3 aimDirection = new Vector3(rightStickInput.X, 0, rightStickInput.Y);

            float inputMagnitude = GetMagnitude(rightStickInput);

            Vector3 camForward = cameraTransform.forward;
            Vector3 camRight = cameraTransform.right;

            camForward.Y = 0;
            camRight.Y = 0;

            camForward = Vector3.Normalize(camForward);
            camRight = Vector3.Normalize(camRight);

            targetOffset = baseOffset + (-camForward * maxOffsetDistance * aimDirection.Z + camRight * maxOffsetDistance * -aimDirection.X);
        }
        else
        {
            targetOffset = baseOffset;
        }

        currentOffset = SmoothDampVector3(currentOffset, targetOffset, ref offsetVelocity, 1f / offsetSmoothness, deltaTime);

        cameraRef.SetOffset(currentOffset);

        if (targetFOV != currentFOV)
        {
            currentFOV = SmoothDamp(currentFOV, targetFOV, ref fovVelocity, 1f / zoomSpeed, deltaTime);
            cameraRef.SetCameraFieldOfView(currentFOV * (System.Math.PI / 180.0));
        }
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
        return (float)System.Math.Sqrt((vector.X * vector.X) + (vector.Y * vector.Y));
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
    public void StartDash()
    {
        targetFOV = dashFOV;
    }

    public void EndDash()
    {
        targetFOV = originalFOV; 
    }
}