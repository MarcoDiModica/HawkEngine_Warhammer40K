using System;
using System.Numerics;
using HawkEngine;

public class PlayerCamera : MonoBehaviour
{
    private GameObject playerRef;
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
            //Engineson.print("ERROR: PlayerCamera requires a GameObject named 'Player' in the scene!");
            return;
        }
        else
        {
            cameraRef.SetFollowTarget(playerRef, currentOffset, 0, true, true, true, smoothness);
        }

        if (cameraRef == null)
        {
            //Engineson.print("ERROR: PlayerCamera requires a Camera component!");
            return;
        }

        cameraRef.SetFollowTarget(playerRef, currentOffset, 0, true, true, true, smoothness);
        cameraRef.SetCameraFieldOfView(originalFOV*(System.Math.PI/180.0));
        Engineson.print("Camera FOV: " + originalFOV * (System.Math.PI / 180.0));
    }

    public override void Update(float deltaTime)
    {
        Vector2 leftStickInput = Input.GetLeftStick();
        Vector2 rightStickInput = Input.GetRightStick();

        Vector3 baseOffset = new Vector3(-11.9f, 19.8f, -12.2f);

        // Si se está usando el left stick, ajustamos la posición de la cámara en base a la dirección de movimiento
        if (leftStickInput != Vector2.Zero)
        {
            Vector3 movementDirection = new Vector3(leftStickInput.X, 0, leftStickInput.Y);

            // Calculamos la magnitud del input del stick izquierdo
            float inputMagnitude = GetMagnitude(leftStickInput);

            // Obtenemos la dirección de la cámara (local)
            Vector3 camForward = cameraTransform.forward;
            Vector3 camRight = cameraTransform.right;

            // Descartamos la componente Y de los vectores forward y right de la cámara, ya que no nos interesa el movimiento en el eje Y
            camForward.Y = 0;
            camRight.Y = 0;

            // Normalizamos los vectores para asegurarnos de que son direcciones unitarias
            camForward = Vector3.Normalize(camForward);
            camRight = Vector3.Normalize(camRight);

            // Calculamos el nuevo offset basándonos en la rotación de la cámara
            // Movimiento basado en la dirección hacia adelante (forward) y derecha (right) de la cámara
            targetOffset = baseOffset + (-camForward * maxOffsetDistance * movementDirection.Z + camRight * maxOffsetDistance * movementDirection.X);
        }
        // Si se está utilizando el right stick, ajustamos la cámara según la dirección de la mira
        else if (rightStickInput != Vector2.Zero)
        {
            Vector3 aimDirection = new Vector3(rightStickInput.X, 0, rightStickInput.Y);

            // Calculamos la magnitud del input del stick derecho
            float inputMagnitude = GetMagnitude(rightStickInput);

            // Obtenemos la dirección de la cámara (local)
            Vector3 camForward = cameraTransform.forward;
            Vector3 camRight = cameraTransform.right;

            // Descartamos la componente Y de los vectores forward y right de la cámara, ya que no nos interesa el movimiento en el eje Y
            camForward.Y = 0;
            camRight.Y = 0;

            // Normalizamos los vectores para asegurarnos de que son direcciones unitarias
            camForward = Vector3.Normalize(camForward);
            camRight = Vector3.Normalize(camRight);

            // Calculamos el nuevo offset basándonos en la rotación de la cámara
            // Movimiento basado en la dirección hacia adelante (forward) y derecha (right) de la cámara
            targetOffset = baseOffset + (-camForward * maxOffsetDistance * aimDirection.Z + camRight * maxOffsetDistance * -aimDirection.X);
        }
        // Si no hay input, mantenemos el offset base
        else
        {
            targetOffset = baseOffset;
        }

        // Interpolamos suavemente el offset actual hacia el offset objetivo
        currentOffset = LerpVector3(currentOffset, targetOffset, offsetSmoothness * deltaTime);

        // Aplicamos el nuevo offset a la cámara
        cameraRef.SetOffset(currentOffset);

        // Si el FOV objetivo no coincide con el actual, interpolamos hacia el objetivo
        if (targetFOV != currentFOV)
        {
            currentFOV = Lerp(currentFOV, targetFOV, deltaTime * zoomSpeed);
            cameraRef.SetCameraFieldOfView(currentFOV * (System.Math.PI / 180.0)); // Convertimos el FOV de grados a radianes
        }
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