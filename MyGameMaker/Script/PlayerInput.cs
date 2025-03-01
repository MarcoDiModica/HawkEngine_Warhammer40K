using System.Numerics;
using HawkEngine;

public class PlayerInput : MonoBehaviour
{
    private Vector3 currentMoveDirection = Vector3.Zero;
    private bool isDashPressed = false;
    private bool isShootPressed = false;

    public override void Start()
    {
    }

    public override void Update(float deltaTime)
    {
        UpdateMovementDirection();

        isDashPressed = Input.GetKeyDown(KeyCode.SPACE);
        isShootPressed = Input.GetKey(KeyCode.J);
    }

    private void UpdateMovementDirection()
    {
        Vector3 direction = Vector3.Zero;

        if (Input.GetKey(KeyCode.W)) direction += Vector3.UnitZ;
        if (Input.GetKey(KeyCode.S)) direction -= Vector3.UnitZ;
        if (Input.GetKey(KeyCode.D)) direction += Vector3.UnitX;
        if (Input.GetKey(KeyCode.A)) direction -= Vector3.UnitX;

        currentMoveDirection = direction != Vector3.Zero ? Vector3.Normalize(direction) : direction;
    }

    public Vector3 GetCurrentMoveDirection()
    {
        return currentMoveDirection;
    }

    public bool IsDashPressed()
    {
        return isDashPressed;
    }

    public bool IsShooting()
    {
        return isShootPressed;
    }
}