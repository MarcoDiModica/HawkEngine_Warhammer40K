using HawkEngine;

public class TyranidTentacles : MonoBehaviour
{
    private Collider tentacleCollider;
    private bool playerInside = false;
    private GameObject player;

    private float exposureTime = 0f;
    private bool trapTriggered = false;
    private float immobilizeTimer = 0f;
    private bool isPlayerImmobilized = false;

    public override void Start()
    {
        tentacleCollider = gameObject.GetComponent<Collider>();
        if (tentacleCollider == null)
        {
            Engineson.print("Error: Tentacle collider not found.");
        }
    }

    public override void Update(float deltaTime)
    {
        if (playerInside && player != null)
        {
            exposureTime += deltaTime;

            if (!trapTriggered && exposureTime >= 0.4f)
            {
                PlayerController playerController = player.GetComponent<PlayerController>();
                if (playerController != null && !playerController.playerData.GodMode)
                {
                    trapTriggered = true;
                    isPlayerImmobilized = true;
                    immobilizeTimer = 1f;

                    playerController.canMove = false;

                }
            }

            if (isPlayerImmobilized)
            {
                immobilizeTimer -= deltaTime;
                if (immobilizeTimer <= 0f)
                {
                    PlayerController playerController = player.GetComponent<PlayerController>();
                    if (playerController != null)
                    {
                        playerController.canMove = true;

                    }

                    isPlayerImmobilized = false;
                }
            }
        }
    }

    public override void OnTriggerEnter(GameObject other)
    {
        if (other.tag == "Player")
        {
            playerInside = true;
            player = other;
            exposureTime = 0f;
            trapTriggered = false;
            immobilizeTimer = 0f;
            isPlayerImmobilized = false;
        }
    }

    public override void OnTriggerExit(GameObject other)
    {
        if (other.tag == "Player")
        {
            if (isPlayerImmobilized)
            {
                // Por si sale antes de terminar el timer
                PlayerController playerController = player.GetComponent<PlayerController>();
                if (playerController != null)
                {
                    playerController.canMove = true;
                }
            }

            playerInside = false;
            player = null;
            exposureTime = 0f;
            trapTriggered = false;
            isPlayerImmobilized = false;
        }
    }
}
