using HawkEngine;

public class TyranidCorpse : MonoBehaviour
{
    private Collider acidCollider;
    private bool playerInside = false;
    private GameObject player;

    private float exposureTime = 0f;       
    private float timeSinceLastTick = 0f;   

    public override void Start()
    {
        acidCollider = gameObject.GetComponent<Collider>();
        if (acidCollider == null)
        {
            Engineson.print("Error: There is no collider");
        }
    }

    public override void Update(float deltaTime)
    {
        if (playerInside && player != null)
        {
            exposureTime += deltaTime;
            timeSinceLastTick += deltaTime;

            if (timeSinceLastTick >= 0.5f)
            {
                PlayerController playerController = player.GetComponent<PlayerController>();
                if (playerController != null)
                {
                    PlayerData data = playerController.playerData;
                    if (data != null && !data.GodMode)
                    {
                        float damage = 2f * (exposureTime * 0.3f + 1f);
                        data.TakeDamage(damage);

                        Engineson.print($"Damage: {damage:F2} | Current Health: {data.GetHealth():F2}");

                        timeSinceLastTick = 0f;
                    }
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
            timeSinceLastTick = 0f;
        }
    }

    public override void OnTriggerExit(GameObject other)
    {
        if (other.tag == "Player")
        {
            playerInside = false;
            player = null;
            exposureTime = 0f;
        }
    }
}
