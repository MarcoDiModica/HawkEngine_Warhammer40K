using System.Collections;
using System.Numerics;
using HawkEngine;

public class PlayerPowerUp : MonoBehaviour
{
    private PlayerController playerController;

    public bool hasMedicaeStimm = false;
    public bool hasAmmunitionBlessing = false;
    public bool hasMagnet = false;
    public bool hasPiercingBullets = false;
    private float medicaeStimmDuration = 5.0f;
    private float medicaeStimmTimer = 0.0f;
    private float ammunitionBlessingDuration = 5.0f;
    private float ammunitionBlessingTimer = 0.0f;
    private float magnetDuration = 5.0f;
    private float magnetTimer = 0.0f;
    private float piercingBulletsDuration = 5.0f;
    private float piercingBulletsTimer = 0.0f;

   // private AudioSource sound;
    private const string AmmunitionBlessingActivated = "Assets/Audio/SFX/PickUps/PowerUps/AmmunitionBlessing/AmmunitionBlessingActivated.wav";
    private const string BlackHeartActivated = "Assets/Audio/SFX/PickUps/PowerUps/BlackHeart/BlackHeartActivated.wav";
    private const string ChapterStandardActivated = "Assets/Audio/SFX/PickUps/PowerUps/ChapterStandard/ChapterStandardActivated.wav";
    private const string MagnetEffect = "Assets/Audio/SFX/PickUps/PowerUps/Magnet/MagnetEffect.wav";
    private const string MedicaeStimmActivated = "Assets/Audio/SFX/PickUps/PowerUps/MedicaeStimm/Injection Heal Sound Effect.wav";
    private const string PiercingBulletsPicked = "Assets/Audio/SFX/PickUps/PowerUps/PiercingBullets/PiercingBulletsPicked.wav";
    private const string BoltgunBulletsPicked = "Assets/Audio/SFX/PickUps/Ammunition/BoltgunBulletsPicked.wav";
    private const string ShotgunShellsPicked = "Assets/Audio/SFX/PickUps/Ammunition/ShotgunShellsPicked.wav";
    private const string ShotgunObtained = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunEqquiped.wav";
    private const string RailgunObtained = "Assets/Audio/SFX/Weapons/Railgun/RailGunEquipped.wav";
    private ParticleFX MedicaeStimmSpeed;
    private ParticleFX AmmunitionBlessing;


    public override void Awake()
    {

    }

    public override void Start()
    {
        playerController = gameObject.GetComponent<PlayerController>();
        MedicaeStimmSpeed = GameObject.Find("SpeedBoostFX").GetComponent<ParticleFX>();
        AmmunitionBlessing = GameObject.Find("AmmunitionBlessingFX").GetComponent<ParticleFX>();
        AmmunitionBlessing.Stop();
        MedicaeStimmSpeed.Stop();
    }

    public override void Update(float deltatime)
    {
        if (hasMedicaeStimm)
        {
            medicaeStimmTimer += deltatime;
            MedicaeStimmSpeed.Play();
            if (medicaeStimmTimer >= medicaeStimmDuration)
            {
                MedicaeStimmSpeed.Stop();
                hasMedicaeStimm = false;
                medicaeStimmTimer = 0.0f;
                playerController.playerData.movSpeed = playerController.playerData.stimmSpeed = 0;
                Engineson.print("Medicae Stimm effect passed");
            }
        }

        if (hasAmmunitionBlessing)
        {
            ammunitionBlessingTimer += deltatime;
            AmmunitionBlessing.Play();
            if (ammunitionBlessingTimer >= ammunitionBlessingDuration)
            {
                AmmunitionBlessing.Stop();
                hasAmmunitionBlessing = false;
                ammunitionBlessingTimer = 0.0f;
                playerController.playerData.infiniteBullets = false;
                Engineson.print("Ammunition Blessing effect passed");
            }
        }

        if (hasPiercingBullets)
        {
            piercingBulletsTimer += deltatime;
            if (piercingBulletsTimer >= piercingBulletsDuration)
            {
                hasPiercingBullets = false;
                piercingBulletsTimer = 0.0f;
                playerController.playerData.isPiercing = false;
                Engineson.print("Piercing Bullets effect passed");
            }
        }

        if (hasMagnet)
        {
            magnetTimer += deltatime;

            if (magnetTimer >= magnetDuration)
            {
                hasMagnet = false;
                Audio.Stop(MagnetEffect);
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
                int audioBlackHeart = Audio.PlayOneShot(BlackHeartActivated);

            }
            else if (other.GetComponent<MedicaeStimm>() != null)
            {
                other.GetComponent<MedicaeStimm>().OnPickUp(playerController);
                hasMedicaeStimm = true;
                int audioMedicaeStimm = Audio.PlayOneShot(MedicaeStimmActivated);
            }
            else if (other.GetComponent<ChapterStandard>() != null)
            {
                other.GetComponent<ChapterStandard>().OnPickUp(playerController);
                int audioChapterStandard = Audio.PlayOneShot(ChapterStandardActivated);

            }
            else if (other.GetComponent<AmmunitionBlessing>() != null)
            {
                other.GetComponent<AmmunitionBlessing>().OnPickUp(playerController);
                hasAmmunitionBlessing = true;
                int audioAmmunitionBlessing = Audio.PlayOneShot(AmmunitionBlessingActivated);
            }
            else if (other.GetComponent<Magnet>() != null)
            {
                other.GetComponent<Magnet>().OnPickUp(playerController);
                hasMagnet = true;
                int audioMagnet = Audio.PlayOneShot(MagnetEffect);
            }
            else if(other.GetComponent<PiercingBullets>() != null)
            {
                other.GetComponent<PiercingBullets>().OnPickUp(playerController);
                hasPiercingBullets = true;
                int audioPiercingBullets = Audio.PlayOneShot(PiercingBulletsPicked);
            }


            Engineson.Destroy(other);
        }

        if(other.tag == "Ammunition")
        {
            Engineson.print("Player Collided with:" + other.tag);

            if (other.GetComponent<BoltgunBullets>() != null && playerController.playerShooting.boltgun.currentTotalAmmo < playerController.playerShooting.boltgun.maxAmmo)
            {
                other.GetComponent<BoltgunBullets>().OnPickUp(playerController);
                int audioBoltgun = Audio.PlayOneShot(BoltgunBulletsPicked);
                Engineson.Destroy(other);
            }
            else if (other.GetComponent<ShotgunShells>() != null && playerController.playerShooting.shotgun.currentTotalAmmo < playerController.playerShooting.shotgun.maxAmmo)
            {
                other.GetComponent<ShotgunShells>().OnPickUp(playerController);
                int audioShotgunShells = Audio.PlayOneShot(ShotgunShellsPicked);
                Engineson.Destroy(other);
            }
        }
        if (other.tag == "Weapon")
        {
            Engineson.print("Player Collided with:" + other.tag);
            if (other.GetComponent<ShotgunPickUp>() != null)
            {
                other.GetComponent<ShotgunPickUp>().OnPickUp(playerController);
                int audioShotgun = Audio.PlayOneShot(ShotgunObtained);
                Engineson.Destroy(other);

            }
            else if (other.GetComponent<RailgunPickUp>() != null)
            {
                other.GetComponent<RailgunPickUp>().OnPickUp(playerController);
                int audioRailgun = Audio.PlayOneShot(RailgunObtained);
                Engineson.Destroy(other);

            }


        }

        if (other.tag == "BiblePage")
        {
            Engineson.print("Player Collided with:" + other.tag);
            if (other.GetComponent<BiblePagePickUp>() != null)
            {
                other.GetComponent<BiblePagePickUp>().OnPickUp(playerController);
                Engineson.Destroy(other);

            }


        }
    }

    public bool GetHasMedicaeStimm()
    {
        return hasMedicaeStimm;
    }
    public bool GetHasAmmunitionBlessing()
    {
        return hasAmmunitionBlessing;
    }
    public bool GetHasMagnet()
    {
        return hasMagnet;
    }
}