using HawkEngine;

public class TyranidTentacles : MonoBehaviour
{
    private Collider tentacleCollider;
    private GameObject player;
    private PlayerController playerController;
    private TyranidTentaclesAnim tentacleAnim;

    private const float activationRadius = 0.25f;
    private const float exposureBeforeStun = 0.3f;
    private const float stunDuration = 1.0f;

    private float exposureTimer = 0f;
    private float stunTimer = 0f;
    private bool isPlayerExposed = false;
    private bool isPlayerStunned = false;
    private bool canInteractToEscape = false;

    public override void Start()
    {
        tentacleCollider = gameObject.GetComponent<Collider>();
        if (tentacleCollider == null)
            Engineson.print("ERROR: Tentacle collider not found.");

        tentacleAnim = gameObject.GetChild("TyranidTentacleMesh").GetComponent<TyranidTentaclesAnim>();
        if (tentacleAnim == null)
            Engineson.print("ERROR: TyranidTentaclesAnim script not found.");
    }

    public override void Update(float deltaTime)
    {
        if (player == null || playerController == null)
            return;

        if (!isPlayerStunned)
        {
            exposureTimer += deltaTime;

            if (exposureTimer >= exposureBeforeStun)
            {
                TriggerTrap();
            }
        }
        else
        {
            stunTimer += deltaTime;

            if (stunTimer >= stunDuration)
            {
                canInteractToEscape = true;

                //Interact Button
                if (Input.GetKeyDown(KeyCode.E) || Input.GetControllerButtonDown(ControllerButton.B))
                {
                    ReleasePlayer();
                    ResetState();
                }
            }
        }
    }

    private void TriggerTrap()
    {
        isPlayerStunned = true;
        stunTimer = 0f;
        canInteractToEscape = false;

        if (!playerController.playerData.GodMode)
            playerController.BlockMovement();

        tentacleAnim?.PlayStunAnim();
    }

    private void ReleasePlayer()
    {
        playerController.UnBlockMovement();
        tentacleAnim?.PlayHideAnim();

        ResetState();
    }

    private void ResetState()
    {
        isPlayerExposed = false;
        isPlayerStunned = false;
        canInteractToEscape = false;
        exposureTimer = 0f;
        stunTimer = 0f;
        player = null;
        playerController = null;
    }

    public override void OnTriggerEnter(GameObject other)
    {
        if (other.tag != "Player") return;

        ResetState(); // ensure fresh start
        player = other;
        playerController = player.GetComponent<PlayerController>();
    }

    public override void OnTriggerExit(GameObject other)
    {
        //if (other != player) return;

        //if (isPlayerStunned)
        //    playerController.UnBlockMovement();

        //tentacleAnim?.PlayHideAnim();
        //player = null;
        //playerController = null;
        //ResetState();
    }
}
