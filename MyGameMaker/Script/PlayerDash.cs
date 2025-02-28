using System.Numerics;
using HawkEngine;

public class PlayerDash
{
    private Rigidbody rb;
    private float dashSpeed = 45.0f;
    private float dashDuration = 0.15f;
    private float dashCooldown = 1.0f;
    private int maxDashCharges = 2;
    private int currentDashCharges;
    private float lastDashRechargeTime;
    private const float DASH_RECHARGE_RATE = 2.0f;
    private bool isDashing;
    private float currentDashTime;
    private Vector3 dashDirection;

    public bool IsDashing => isDashing;

    public PlayerDash(GameObject player)
    {
        rb = player.GetComponent<Rigidbody>();
        ResetDashCharges();
    }

    public bool CanDash()
    {
        return !isDashing && currentDashCharges > 0;
    }

    public void UpdateDashRecharge(float deltaTime)
    {
        if (currentDashCharges < maxDashCharges)
        {
            lastDashRechargeTime += deltaTime;
            if (lastDashRechargeTime >= DASH_RECHARGE_RATE)
            {
                currentDashCharges++;
                lastDashRechargeTime = 0;
            }
        }
    }

    public void InitiateDash(Vector3 direction)
    {
        if (direction == Vector3.Zero || !CanDash()) return;

        isDashing = true;
        currentDashTime = dashDuration;
        dashDirection = Vector3.Normalize(direction);
        currentDashCharges--;

        rb.AddForce(dashDirection * dashSpeed);
    }

    public void HandleActiveDash(float deltaTime)
    {
        if (currentDashTime > 0)
        {
            rb.AddForce(dashDirection * dashSpeed);
            currentDashTime -= deltaTime;
        }
        else
        {
            isDashing = false;
        }
    }

    private void ResetDashCharges()
    {
        currentDashCharges = maxDashCharges;
        lastDashRechargeTime = 0;
    }
}