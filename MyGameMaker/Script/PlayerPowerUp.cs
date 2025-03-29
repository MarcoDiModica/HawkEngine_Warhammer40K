using System.Collections;
using System.Numerics;
using HawkEngine;

public class PlayerPowerUp : MonoBehaviour
{
    private PlayerController playerController;

    private bool hasMedicaeStimm = false;
    private bool hasAmmunitionBlessing = false;
    private float medicaeStimmDuration = 5.0f;
    private float medicaeStimmTimer = 0.0f;
    private float ammunitionBlessingDuration = 5.0f;
    private float ammunitionBlessingTimer = 0.0f;

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
        if (hasAmmunitionBlessing)
        {
            ammunitionBlessingTimer += deltatime;

            if(ammunitionBlessingTimer >= ammunitionBlessingDuration)
            {
                hasAmmunitionBlessing = false;
                ammunitionBlessingTimer = 0.0f;
                playerController.playerData.infiniteBullets = false;
                Engineson.print("Ammunition Blessing effect passed");
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
                other.GetComponent<BlackHeart>().OnPickUp(playerController);
                
            }
            else if (other.GetComponent<MedicaeStimm>() != null)
            {
                other.GetComponent<MedicaeStimm>().OnPickUp(playerController);
                hasMedicaeStimm = true;
            }
            else if (other.GetComponent<ChapterStandard>() != null)
            {
                other.GetComponent<ChapterStandard>().OnPickUp(playerController);

            }
            else if (other.GetComponent<AmmunitionBlessing>() != null)
            {
                other.GetComponent<AmmunitionBlessing>().OnPickUp(playerController);
                hasAmmunitionBlessing = true;
            }

            Engineson.Destroy(other);
        }

        if(other.tag == "Ammunition")
        {
            Engineson.print("Player Collided with:" + other.tag);

            if (other.GetComponent<BoltgunBullets>() != null && playerController.playerShooting.boltgun.currentTotalAmmo < playerController.playerShooting.boltgun.maxAmmo)
            {
                other.GetComponent<BoltgunBullets>().OnPickUp(playerController);
                Engineson.Destroy(other);
            }
            else if (other.GetComponent<ShotgunShells>() != null && playerController.playerShooting.shotgun.currentTotalAmmo < playerController.playerShooting.shotgun.maxAmmo)
            {
                other.GetComponent<ShotgunShells>().OnPickUp(playerController);
                Engineson.Destroy(other);
            }
        }
    }
}