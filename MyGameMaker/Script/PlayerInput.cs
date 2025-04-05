using System.Numerics;
using HawkEngine;

public class PlayerInput : MonoBehaviour
{
    private Vector3 currentMoveDirection = Vector3.Zero;
    private Vector3 currentLookDirection = Vector3.Zero;
    private bool isDashPressed = false;
    private bool isShootPressed = false;
    private bool isReloadPressed = false;
    private bool isAbility1Pressed = false;
    private bool isAbility2Pressed = false;
    private bool isInteractPressed = false;
    private bool isRunningPressed = false;
    private bool isKeyboardMoving = false;

    private bool isMovementBlocked = false;
    public override void Awake()
    {

    }

    public override void Start()
    {
    }

    public override void Update(float deltaTime)
    {
        if (!isMovementBlocked)
        {
            UpdateMovementDirection();
            UpdateLookDirection();
        }

        isDashPressed = Input.GetKeyDown(KeyCode.SPACE) || Input.GetControllerButtonDown(ControllerButton.A);
        isShootPressed = Input.GetKey(KeyCode.Q) || Input.GetControllerAxis(0, 5) > 0.5f;

        isInteractPressed = Input.GetKeyDown(KeyCode.E) || Input.GetControllerButtonDown(ControllerButton.B);
        isReloadPressed = Input.GetKeyDown(KeyCode.R) || Input.GetControllerButtonDown(ControllerButton.X);
        isAbility1Pressed = Input.GetKeyDown(KeyCode.Y) || Input.GetControllerButtonDown(ControllerButton.RightShoulder);
        isAbility2Pressed = Input.GetKeyDown(KeyCode.G) || Input.GetControllerButtonDown(ControllerButton.LeftShoulder);

      

        if (Input.GetKeyDown(KeyCode.V))
        {
            SceneManager.LoadScene("Level2");
        }
    }

    private void UpdateMovementDirection()
    {
        Vector3 direction = Vector3.Zero;

        bool isW = Input.GetKey(KeyCode.W);
        bool isS = Input.GetKey(KeyCode.S);
        bool isA = Input.GetKey(KeyCode.A);
        bool isD = Input.GetKey(KeyCode.D);

        if (isW) direction += Vector3.UnitZ;
        if (isS) direction -= Vector3.UnitZ;
        if (isD) direction -= Vector3.UnitX;
        if (isA) direction += Vector3.UnitX;

        bool shiftHeld = Input.GetKey(KeyCode.CAPSLOCK);
        bool isKeyboardMoving = isW || isS || isA || isD;

        isRunningPressed = !shiftHeld && isKeyboardMoving;

        float leftStickMagnitude = Input.GetLeftStick().Length();

        if (leftStickMagnitude > 0.75f)
        {
            isRunningPressed = true;
            Vector2 leftStickInput = Input.GetLeftStick();
            direction = new Vector3(-leftStickInput.X, 0, -leftStickInput.Y);
        }
        else if (leftStickMagnitude > 0.1f)
        {
            isRunningPressed = false;
            Vector2 leftStickInput = Input.GetLeftStick();
            direction = new Vector3(-leftStickInput.X, 0, -leftStickInput.Y);
        }
        currentMoveDirection = direction != Vector3.Zero ? Vector3.Normalize(direction) : direction;

    }
    public bool IsRunningPressed()
    {
        return isRunningPressed;
    }
    public bool IsKeyboardMoving()
    {
        return isKeyboardMoving;
    }
    private void UpdateLookDirection()
    {
        Vector3 direction = Vector3.Zero;

        if (Input.GetMouseButton(1))
        {
            Vector3 mousePosition = Input.GetMousePosition();
            Vector3 playerPosition = gameObject.GetComponent<Transform>().GetPosition();
            Vector3 directionToMouse = Vector3.Normalize(mousePosition - playerPosition);
            direction = new Vector3(directionToMouse.X, 0, directionToMouse.Z);
        }

        if (Input.GetRightStick() != Vector2.Zero)
        {
            Vector2 rightStickInput = Input.GetRightStick();
            direction = new Vector3(-rightStickInput.X, 0, -rightStickInput.Y);
        }

        currentLookDirection = direction != Vector3.Zero ? Vector3.Normalize(direction) : direction;
    }

    public Vector3 GetCurrentMoveDirection()
    {
        return currentMoveDirection;
    }

    public Vector3 GetCurrentLookDirection()
    {
        return currentLookDirection;
    }

    public bool GetShootInput()
    {
        return isShootPressed;
    }

    public bool IsShooting()
    {
        return isShootPressed;
    }
    public bool GetDashInput()
    {
        return isDashPressed;
    }
    public bool IsReloading()
    {
        return isReloadPressed;
    }

    public bool IsAbility1Pressed()
    {
        return isAbility1Pressed;
    }

    public bool IsAbility2Pressed()
    {
        return isAbility2Pressed;
    }

    public bool IsInteracting()
    {
        return isInteractPressed;  
    }

    public bool IsChangingWeaponRight()
    {
        return Input.GetControllerButtonDown(ControllerButton.DPadRight);
    }

    public bool IsChangingWeaponLeft()
    {
        return Input.GetControllerButtonDown(ControllerButton.DPadLeft);
    }

    public bool IsChangingRailgunMode()
    {
        return Input.GetControllerButtonDown(ControllerButton.DPadDown) || Input.GetControllerButtonDown(ControllerButton.DPadUp);
    }

    public void BlockMovement()
    {
        isMovementBlocked = true;
    }

    public void UnblockMovement()
    {
        isMovementBlocked = false;
    }
}