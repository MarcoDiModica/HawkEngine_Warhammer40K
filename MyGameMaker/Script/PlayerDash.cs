using System.Numerics;
using HawkEngine;

public class PlayerDash : MonoBehaviour
{
    public float dashSpeed = 45.0f;
    public float dashDuration = 0.15f;
    public int maxDashCharges = 2;
    public float dashRechargeRate = 2.0f;

    private Rigidbody rb;
    private int currentDashCharges;
    private float lastDashRechargeTime;
    private bool isDashing;
    private float currentDashTime;
    private Vector3 dashDirection;

    public bool IsDashing => isDashing;

    public override void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
        if (rb == null)
        {
            Engineson.print("ERROR: PlayerDash requires a Rigidbody component!");
            return;
        }

        ResetDashCharges();
    }

    public override void Update(float deltaTime)
    {
        if (isDashing)
        {
            HandleActiveDash(deltaTime);
        }

        UpdateDashRecharge(deltaTime);
    }

    public bool CanDash()
    {
        return !isDashing && currentDashCharges > 0;
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

    private void HandleActiveDash(float deltaTime)
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

    private void UpdateDashRecharge(float deltaTime)
    {
        if (currentDashCharges < maxDashCharges)
        {
            lastDashRechargeTime += deltaTime;
            if (lastDashRechargeTime >= dashRechargeRate)
            {
                currentDashCharges++;
                lastDashRechargeTime = 0;
            }
        }
    }

    private void ResetDashCharges()
    {
        currentDashCharges = maxDashCharges;
        lastDashRechargeTime = 0;
    }
}