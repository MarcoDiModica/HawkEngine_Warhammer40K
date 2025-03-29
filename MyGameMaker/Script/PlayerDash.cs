using System.Numerics;
using HawkEngine;

public class PlayerDash : MonoBehaviour
{
    public float dashSpeed = 1600.0f;
    public float dashDuration = 0.05f;
    public float dashCooldown = 3.0f; 

    private Rigidbody rb;
    private bool isDashing;
    private float currentDashTime;
    private Vector3 dashDirection;
    private float lastDashTime;

    public bool IsDashing => isDashing;

    public override void Awake()
    {
    }

    public override void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
        if (rb == null)
        {
            Engineson.print("ERROR: PlayerDash requires a Rigidbody component!");
            return;
        }
        lastDashTime = -dashCooldown; 
    }

    public override void Update(float deltaTime)
    {
        if (isDashing)
        {
            HandleActiveDash(deltaTime);
        }
    }

    public bool CanDash(float currentTime)
    {
        return !isDashing && (currentTime - lastDashTime >= dashCooldown);
    }

    public void InitiateDash(Vector3 direction, float currentTime)
    {
        if (!CanDash(currentTime)) return;

        isDashing = true;
        currentDashTime = dashDuration;
        dashDirection = direction == Vector3.Zero ? gameObject.GetComponent<Transform>().forward : Vector3.Normalize(direction);
        lastDashTime = currentTime;
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
}
