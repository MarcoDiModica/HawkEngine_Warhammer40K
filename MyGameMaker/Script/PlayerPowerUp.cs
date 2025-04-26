using System.Collections;
using System.Numerics;
using HawkEngine;

public class PlayerPowerUp : MonoBehaviour
{
    private PlayerController playerController;

    private bool hasMedicaeStimm = false;
    private bool hasAmmunitionBlessing = false;
    private bool hasMagnet = false;
    private bool hasPiercingBullets = false;
    private float medicaeStimmDuration = 5.0f;
    private float medicaeStimmTimer = 0.0f;
    private float ammunitionBlessingDuration = 5.0f;
    private float ammunitionBlessingTimer = 0.0f;
    private float magnetDuration = 5.0f;
    private float magnetTimer = 0.0f;
    private float piercingBulletsDuration = 5.0f;
    private float piercingBulletsTimer = 0.0f;

    private AudioSource sound;
    private string AmmunitionBlessingActivated = "Assets/Audio/SFX/PickUps/PowerUps/AmmunitionBlessing/AmmunitionBlessingActivated.wav";
    private string BlackHeartActivated = "Assets/Audio/SFX/PickUps/PowerUps/BlackHeart/BlackHeartActivated.wav";
    private string ChapterStandardActivated = "Assets/Audio/SFX/PickUps/PowerUps/ChapterStandard/ChapterStandardActivated.wav";
    private string MagnetEffect = "Assets/Audio/SFX/PickUps/PowerUps/Magnet/MagnetEffect.wav";
    private string MedicaeStimmActivated = "Assets/Audio/SFX/PickUps/PowerUps/MedicaeStimm/Injection Heal Sound Effect.wav";
    private ParticleFX MedicaeStimmSpeed;
    private ParticleFX AmmunitionBlessing;
    private AudioClip ammunitionBlessingFX;
    private AudioClip blackHeartFX;
    private AudioClip chapterStandardFX;
    private AudioClip magnetFX;
    private AudioClip medicaeStimmFX;

    public override void Awake()
    {

    }

    public override void Start()
    {
        playerController = gameObject.GetComponent<PlayerController>();
        sound = gameObject.GetComponent<AudioSource>();

        if (sound == null)
        {
            Engineson.print("PlayerPowerUp: Audio component not found");
        }
        MedicaeStimmSpeed = GameObject.Find("SpeedBoostFX").GetComponent<ParticleFX>();
        AmmunitionBlessing = GameObject.Find("AmmunitionBlessingFX").GetComponent<ParticleFX>();
        AmmunitionBlessing.Stop();
        MedicaeStimmSpeed.Stop();
        ammunitionBlessingFX = new AudioClip(AmmunitionBlessingActivated, "AmmunitionBlessingFX", false, false);
        blackHeartFX = new AudioClip(BlackHeartActivated, "BlackHeartFX", false, false);
        chapterStandardFX = new AudioClip(ChapterStandardActivated, "ChapterStandardFX", false, false);
        magnetFX = new AudioClip(MagnetEffect, "MagnetFX", false, false);
        medicaeStimmFX = new AudioClip(MedicaeStimmActivated, "MedicaeStimmFX", false, false);
        sound.LoadAudioClip(ammunitionBlessingFX);
        sound.LoadAudioClip(blackHeartFX);
        sound.LoadAudioClip(chapterStandardFX);
        sound.LoadAudioClip(magnetFX);
        sound.LoadAudioClip(medicaeStimmFX);

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
                sound.Stop(magnetFX);
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
                sound.Play(blackHeartFX);

            }
            else if (other.GetComponent<MedicaeStimm>() != null)
            {
                other.GetComponent<MedicaeStimm>().OnPickUp(playerController);
                hasMedicaeStimm = true;
                sound.Play(medicaeStimmFX);
            }
            else if (other.GetComponent<ChapterStandard>() != null)
            {
                other.GetComponent<ChapterStandard>().OnPickUp(playerController);
                sound.Play(chapterStandardFX);

            }
            else if (other.GetComponent<AmmunitionBlessing>() != null)
            {
                other.GetComponent<AmmunitionBlessing>().OnPickUp(playerController);
                hasAmmunitionBlessing = true;
                sound.Play(ammunitionBlessingFX);
            }
            else if (other.GetComponent<Magnet>() != null)
            {
                other.GetComponent<Magnet>().OnPickUp(playerController);
                hasMagnet = true;
                sound.Play(magnetFX);
            }
            else if(other.GetComponent<PiercingBullets>() != null)
            {
                other.GetComponent<PiercingBullets>().OnPickUp(playerController);
                hasPiercingBullets = true;
                //sound.LoadAudio(MagnetEffect);
                //sound.Play();
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