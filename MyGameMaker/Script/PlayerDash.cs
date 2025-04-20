using System.Numerics;
using HawkEngine;

public class PlayerDash : MonoBehaviour
{
    public float dashSpeed = 1600.0f;
    public float dashDuration = 0.05f;
    public float dashCooldown = 1.25f;
    public bool canDash = true;

    private Rigidbody rb;
    private bool isDashing;
    private float currentDashTime;
    private Vector3 dashDirection;
    private float lastDashTime;

    public bool IsDashing => isDashing;

    public bool isInvulnerable = false;
    private float invulnerabilityTime = 0.25f;
    private float iTimeCounter = 0;
    private GameObject playerCamera;

    private float targetFOV;
    private float zoomSpeed = 0.5f;
    private Audio sound;
    private string DashSound = "Assets/Audio/SFX/Player/PlayerDash_ready.wav";

    

    public override void Awake()
    {

    }

    public override void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
        lastDashTime = -dashCooldown;

        playerCamera = GameObject.Find("MainCamera");
        playerCamera.GetComponent<PlayerCamera>();    
        sound = gameObject.GetComponent<Audio>();
    }

    public override void Update(float deltaTime)
    {
        if (isDashing)
        {
            HandleActiveDash(deltaTime);
            
        }

       

        HandleInvulnerability(deltaTime);
    }

    public bool CanDash(float currentTime)
    {
        return canDash && !isDashing && (currentTime - lastDashTime >= dashCooldown);
    }

    public void InitiateDash(Vector3 direction, float currentTime)
    {
        if (!CanDash(currentTime)) return;

        
        isDashing = true;
        currentDashTime = dashDuration;
        dashDirection = direction == Vector3.Zero ? gameObject.GetComponent<Transform>().forward : Vector3.Normalize(direction);
        lastDashTime = currentTime;
        isInvulnerable = true;
        rb.AddForce(dashDirection * dashSpeed);
        playerCamera.GetComponent<PlayerCamera>().StartDash(dashDirection);
    }

    private void HandleActiveDash(float deltaTime)
    {
        if (currentDashTime > 0)
        {
            rb.AddForce(dashDirection * dashSpeed);
            currentDashTime -= deltaTime;
            sound.LoadAudio(DashSound);
            sound.Play();
        }
        else
        {
            isDashing = false;
            playerCamera.GetComponent<PlayerCamera>().EndDash();

        }
    }

    private void HandleInvulnerability(float deltaTime)
    {
        if (isInvulnerable)
        {
            iTimeCounter += deltaTime;
            if (iTimeCounter >= invulnerabilityTime)
            {
                isInvulnerable = false;
                iTimeCounter = 0;
            }
        }
    }
}
