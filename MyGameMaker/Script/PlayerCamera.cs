using System.Numerics;
using HawkEngine;

public class PlayerCamera : MonoBehaviour
{
    private GameObject playerRef;
    private Camera cameraRef;
    private PlayerInput playerInput;

    public float smoothness = 19.0f;

    public float maxOffsetDistance = 2.2f;
    public float offsetSmoothness = 25.0f;

    private Vector3 currentOffset = new Vector3(0, 10, -7.5f);
    private Vector3 targetOffset = new Vector3(0, 10, -7.5f);

    public override void Start()
    {
        playerRef = GameObject.Find("Player");
        cameraRef = gameObject.GetComponent<Camera>();

        if (playerRef == null)
        {
            Engineson.print("ERROR: PlayerCamera requires a GameObject named 'Player' in the scene!");
            return;
        }

        if (cameraRef == null)
        {
            Engineson.print("ERROR: PlayerCamera requires a Camera component!");
            return;
        }

        cameraRef.SetFollowTarget(playerRef, currentOffset, 0, true, true, true, smoothness);
    }

    public override void Update(float deltaTime)
    {
        Vector2 leftStickInput = Input.GetLeftStick();
        Vector2 rightStickInput = Input.GetRightStick();

        Vector3 baseOffset = new Vector3(0, 10, -7.5f);

        if (leftStickInput != Vector2.Zero)
        {
            Vector3 movementDirection = new Vector3(leftStickInput.X, 0, leftStickInput.Y);

            float inputMagnitude = GetMagnitude(leftStickInput);
            targetOffset = baseOffset + (-movementDirection * maxOffsetDistance * inputMagnitude);
        }
        else if (rightStickInput != Vector2.Zero)
        {
            Vector3 aimDirection = new Vector3(rightStickInput.X, 0, rightStickInput.Y);

            float inputMagnitude = GetMagnitude(rightStickInput);
            targetOffset = baseOffset + (-aimDirection * maxOffsetDistance * inputMagnitude);
        }
        else
        {
            targetOffset = baseOffset;
        }

        currentOffset = LerpVector3(currentOffset, targetOffset, offsetSmoothness * deltaTime);

        cameraRef.SetOffset(currentOffset);
    }

    private float GetMagnitude(Vector2 vector)
    {
        return (float)System.Math.Sqrt((vector.X * vector.X) + (vector.Y * vector.Y));
    }

    private Vector3 LerpVector3(Vector3 start, Vector3 end, float t)
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