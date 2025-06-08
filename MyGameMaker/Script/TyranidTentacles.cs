using HawkEngine;
using System.Numerics;

public class TyranidTentacles : MonoBehaviour
{
    private Collider tentacleCollider;
    private GameObject player;
    private GameObject interactionCanvas;
    private Interaction interactionScript;
    private PlayerController playerController;
    private TyranidTentaclesAnim tentacleAnim;
    private TyranidTentaclesAnim tentacleAnim2;
    private TyranidTentaclesAnim tentacleAnim3;

    private const float activationRadius = 0.25f;
    private const float exposureBeforeStun = 0.15f;
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
        tentacleAnim2 = gameObject.GetChild("TyranidTentacleMesh2").GetComponent<TyranidTentaclesAnim>();
        tentacleAnim3 = gameObject.GetChild("TyranidTentacleMesh3").GetComponent<TyranidTentaclesAnim>();
        if (tentacleAnim == null)
            Engineson.print("ERROR: TyranidTentaclesAnim script not found.");
        if (tentacleAnim2 == null)
            Engineson.print("ERROR: TyranidTentaclesAnim2 script not found.");
        if (tentacleAnim3 == null)
            Engineson.print("ERROR: TyranidTentaclesAnim3 script not found.");
        interactionCanvas = GameObject.Find("Canvas_Interaction");
        interactionScript = interactionCanvas?.GetComponent<Interaction>();
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
                if (Input.GetKeyDown(KeyCode.E) || Input.GetControllerButtonDown(ControllerButton.A))
                {
                    ReleasePlayer();
                    ResetState();
                }
            }
        }
    }

    private void TriggerTrap()
    {
        player.GetComponent<Collider>().SetPosition(new Vector3(gameObject.transform.position.X - 1, gameObject.transform.position.Y + 0.1f, gameObject.transform.position.Z - 1));
        isPlayerStunned = true;
        stunTimer = 0f;
        canInteractToEscape = false;
        interactionScript.SpawnInteractText(canInteractToEscape);

        if (!playerController.playerData.GodMode)
            playerController.BlockMovement();

        tentacleAnim?.PlayStunAnim();
        tentacleAnim2?.PlayStunAnim();
        tentacleAnim3?.PlayStunAnim();

    }

    private void ReleasePlayer()
    {
        playerController.UnBlockMovement();
        tentacleAnim?.PlayHideAnim();
        tentacleAnim2?.PlayHideAnim();
        tentacleAnim3?.PlayHideAnim();

        ResetState();
    }

    private void ResetState()
    {
        isPlayerExposed = false;
        isPlayerStunned = false;
        canInteractToEscape = false;
        interactionScript.SpawnInteractText(canInteractToEscape);
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
