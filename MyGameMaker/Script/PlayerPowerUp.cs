using System.Collections;
using System.Numerics;
using HawkEngine;

public class PlayerPowerUp : MonoBehaviour
{
    private PlayerController playerController;

    private bool hasMedicaeStimm = false;
    private float medicaeStimmDuration = 5.0f;
    private float medicaeStimmTimer = 0.0f;

    public override void Awake()
    {

    }

    public override void Start()
    {
        playerController = gameObject.GetComponent<PlayerController>();
        
    }

    public override void Update(float deltatime)
    {
        if (hasMedicaeStimm)
        {
            medicaeStimmTimer += deltatime;

            if (medicaeStimmTimer >= medicaeStimmDuration)
            {
                hasMedicaeStimm = false;
                medicaeStimmTimer = 0.0f;
                playerController.playerData.movSpeed = playerController.playerData.movSpeed / 2;
                Engineson.print("Medicae Stimm effect passed");
            }
        }
    }

    public override void OnTriggerEnter(GameObject other)
    {
        if (other.tag == "PowerUp")
        {
            Engineson.print("Player Collided with:" + other.tag);

            if (other.GetComponent<BlackHeart>() != null)
            {
                other.GetComponent<BlackHeart>().ApplyPowerUpOnPickup(playerController);
                
            }
            else if (other.GetComponent<MedicaeStimm>() != null)
            {
                other.GetComponent<MedicaeStimm>().ApplyPowerUpOnPickup(playerController);
                hasMedicaeStimm = true;
            }

            Engineson.Destroy(other);
        }
    }
}