using System.Numerics;
using HawkEngine;

public class PlayerInput
{
    public Vector3 GetInputDirection()
    {
        Vector3 direction = Vector3.Zero;

        if (Input.GetKey(KeyCode.W)) direction += Vector3.UnitZ;
        if (Input.GetKey(KeyCode.S)) direction -= Vector3.UnitZ;
        if (Input.GetKey(KeyCode.D)) direction += Vector3.UnitX;
        if (Input.GetKey(KeyCode.A)) direction -= Vector3.UnitX;

        return direction != Vector3.Zero ? Vector3.Normalize(direction) : direction;
    }

    public bool IsDashPressed()
    {
        return Input.GetKey(KeyCode.SPACE);
    }

    public bool IsShooting()
    {
        return Input.GetKey(KeyCode.J);
    }
}
