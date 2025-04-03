using System.Collections;
using System.Numerics;
using HawkEngine;

public class PlayerPowerUp : MonoBehaviour
{
    private PlayerController playerController;

    private bool hasMedicaeStimm = false;
    private bool hasAmmunitionBlessing = false;
    private bool hasMagnet = false;
    private bool hasChapterStandard = false;
    private float medicaeStimmDuration = 5.0f;
    private float medicaeStimmTimer = 0.0f;
    private float ammunitionBlessingDuration = 5.0f;
    private float ammunitionBlessingTimer = 0.0f;
    private float magnetDuration = 5.0f;
    private float magnetTimer = 0.0f;



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

        if(hasMagnet)
        {
            magnetTimer += deltatime;

            if (magnetTimer >= magnetDuration)
            {
                hasMagnet = false;
                magnetTimer = 0.0f;
                playerController.playerShooting.boltgun.shootCadence = playerController.playerShooting.boltgun.shootCadence * 1.5f;
                playerController.playerShooting.shotgun.shootCadence = playerController.playerShooting.shotgun.shootCadence * 1.5f;
                Engineson.print("Magnet effect passed");
            }
            else
            {
                GameObject[] Ammunition = Physics.OverlapSphere(playerController.gameObject.GetComponent<Transform>().position, 20f, "Ammunition");

                foreach (GameObject obj in Ammunition)
                {
                    if (obj != null && obj.tag == "Ammunition")
                    {

                        if (obj != null && obj.tag == "Ammunition")
                        {

                            if (obj.GetComponent<ShotgunShells>() != null)
                            {
                                obj.GetComponent<ShotgunShells>().isDetected = true;

                            }
                            else if (obj.GetComponent<BoltgunBullets>() != null)
                            {
                                obj.GetComponent<BoltgunBullets>().isDetected = true;
                            }


                        }


                    }
                }
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
            else if (other.GetComponent<Magnet>() != null)
            {
                other.GetComponent<Magnet>().OnPickUp(playerController);
                hasMagnet = true;
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
        if (other.tag == "Weapon")
        {
            Engineson.print("Player Collided with:" + other.tag);
            if (other.GetComponent<ShotgunPickUp>() != null)
            {
                other.GetComponent<ShotgunPickUp>().OnPickUp(playerController);
                Engineson.Destroy(other);

            }
            else if (other.GetComponent<RailgunPickUp>() != null)
            {
                other.GetComponent<RailgunPickUp>().OnPickUp(playerController);
                Engineson.Destroy(other);

            }


        }
    }
}