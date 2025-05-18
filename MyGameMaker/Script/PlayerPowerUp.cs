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

    private const string AmmunitionBlessingActivated = "Assets/Audio/PowerUps/AmmoBless2.wav";
    private const string BlackHeartActivated = "Assets/Audio/PowerUps/Black_Heart.wav";
    private const string ChapterStandardActivated = "Assets/Audio/SFX/PickUps/PowerUps/ChapterStandard/ChapterStandardActivated.wav";
    private const string MagnetEffect = "Assets/Audio/SFX/PickUps/PowerUps/Magnet/MagnetEffect.wav";
    private const string MedicaeStimmActivated = "Assets/Audio/PowerUps/Stim_2.wav";
    private const string PiercingBulletsPicked = "Assets/Audio/PowerUps/Piercing_ammo.wav";
    private const string BoltgunBulletsPicked = "Assets/Audio/SFX/PickUps/Ammunition/BoltgunBulletsPicked.wav";
    private const string ShotgunShellsPicked = "Assets/Audio/SFX/PickUps/Ammunition/ShotgunShellsPicked.wav";
    private const string ShotgunObtained = "Assets/Audio/SFX/Weapons/Shotgun/ShotgunEqquiped.wav";
    private const string RailgunObtained = "Assets/Audio/SFX/Weapons/Railgun/RailGunEquipped.wav";
    private const string PowerUpDown = "Assets/Audio/PowerUps/PowerUpDown.wav";

    private ParticleFX medicaeStimmSpeed;
    private ParticleFX ammunitionBlessing;
    private bool particleEffectsFound = false;

    public override void Awake()
    {
    }

    public override void Start()
    {
        playerController = gameObject.GetComponent<PlayerController>();

        GameObject speedBoostObj = GameObject.Find("SpeedBoostFX");
        GameObject ammoBoostObj = GameObject.Find("AmmunitionBlessingFX");

        if (speedBoostObj != null && ammoBoostObj != null)
        {
            medicaeStimmSpeed = speedBoostObj.GetComponent<ParticleFX>();
            ammunitionBlessing = ammoBoostObj.GetComponent<ParticleFX>();

            if (medicaeStimmSpeed != null && ammunitionBlessing != null)
            {
                particleEffectsFound = true;
                ammunitionBlessing.Stop();
                medicaeStimmSpeed.Stop();
            }
            else
            {
                Engineson.print("ERROR: Could not find particle effects components");
            }
        }
        else
        {
            Engineson.print("ERROR: Could not find particle effect objects");
        }
    }

    public override void Update(float deltatime)
    {
        if (playerController == null)
            return;

        if (hasMedicaeStimm)
        {
            medicaeStimmTimer += deltatime;

            if (particleEffectsFound)
                medicaeStimmSpeed.Play();

            if (medicaeStimmTimer >= medicaeStimmDuration)
            {
                if (particleEffectsFound)
                    medicaeStimmSpeed.Stop();

                hasMedicaeStimm = false;
                medicaeStimmTimer = 0.0f;

                if (playerController.playerData != null)
                    playerController.playerData.movSpeed = playerController.playerData.stimmSpeed = 0;

                Engineson.print("Medicae Stimm effect passed");
            }
        }

        if (hasAmmunitionBlessing)
        {
            ammunitionBlessingTimer += deltatime;

            if (particleEffectsFound)
                ammunitionBlessing.Play();

            if (ammunitionBlessingTimer >= ammunitionBlessingDuration)
            {
                if (particleEffectsFound)
                    ammunitionBlessing.Stop();

                hasAmmunitionBlessing = false;
                ammunitionBlessingTimer = 0.0f;

                if (playerController.playerData != null)
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

                if (playerController.playerData != null)
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

                if (playerController.playerShooting != null)
                {
                    if (playerController.playerShooting.boltgun != null)
                        playerController.playerShooting.boltgun.shootCadence *= 2f;

                    if (playerController.playerShooting.shotgun != null)
                        playerController.playerShooting.shotgun.shootCadence *= 2f;
                }

                Engineson.print("Magnet effect passed");
            }
            else
            {
                Transform playerTransform = null;
                if (playerController != null && playerController.gameObject != null)
                {
                    playerTransform = playerController.gameObject.GetComponent<Transform>();
                }

                if (playerTransform != null)
                {
                    GameObject[] ammunition = Physics.OverlapSphere(playerTransform.position, 20f, "Ammunition");

                    if (ammunition != null)
                    {
                        foreach (GameObject obj in ammunition)
                        {
                            if (obj != null && obj.tag == "Ammunition")
                            {
                                BoltgunBullets boltgunBullets = obj.GetComponent<BoltgunBullets>();
                                if (boltgunBullets != null)
                                {
                                    boltgunBullets.isDetected = true;
                                    continue;
                                }

                                ShotgunShells shotgunShells = obj.GetComponent<ShotgunShells>();
                                if (shotgunShells != null)
                                {
                                    shotgunShells.isDetected = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    public override void OnTriggerEnter(GameObject other)
    {
        if (other == null || playerController == null)
            return;

        if (other.tag == "PowerUp")
        {
            Engineson.print("Player Collided with:" + other.tag);

            BlackHeart blackHeart = other.GetComponent<BlackHeart>();
            if (blackHeart != null)
            {
                blackHeart.OnPickUp(playerController);
                int audioBlackHeart = Audio.PlayOneShot(BlackHeartActivated);
            }
            else
            {
                MedicaeStimm medicaeStimm = other.GetComponent<MedicaeStimm>();
                if (medicaeStimm != null)
                {
                    medicaeStimm.OnPickUp(playerController);
                    hasMedicaeStimm = true;
                    int audioMedicaeStimm = Audio.PlayOneShot(MedicaeStimmActivated);
                }
                else
                {
                    ChapterStandard chapterStandard = other.GetComponent<ChapterStandard>();
                    if (chapterStandard != null)
                    {
                        chapterStandard.OnPickUp(playerController);
                        int audioChapterStandard = Audio.PlayOneShot(ChapterStandardActivated);
                    }
                    else
                    {
                        AmmunitionBlessing ammoBlessing = other.GetComponent<AmmunitionBlessing>();
                        if (ammoBlessing != null)
                        {
                            ammoBlessing.OnPickUp(playerController);
                            hasAmmunitionBlessing = true;
                            int audioAmmunitionBlessing = Audio.PlayOneShot(AmmunitionBlessingActivated);
                        }
                        else
                        {
                            Magnet magnet = other.GetComponent<Magnet>();
                            if (magnet != null)
                            {
                                if(hasMagnet == true)
                                {
                                    magnetTimer = 0.0f;
                                }
                                else
                                {
                                    magnet.OnPickUp(playerController);
                                    hasMagnet = true;
                                    int audioMagnet = Audio.PlayOneShot(MagnetEffect);
                                }
                               
                            }
                            else
                            {
                                PiercingBullets piercingBullets = other.GetComponent<PiercingBullets>();
                                if (piercingBullets != null)
                                {
                                    piercingBullets.OnPickUp(playerController);
                                    hasPiercingBullets = true;
                                    int audioPiercingBullets = Audio.PlayOneShot(PiercingBulletsPicked);
                                }
                            }
                        }
                    }
                }
            }

            Engineson.Destroy(other);
        }
        else if (other.tag == "Ammunition")
        {
            Engineson.print("Player Collided with:" + other.tag);

            BoltgunBullets boltgunBullets = other.GetComponent<BoltgunBullets>();
            if (boltgunBullets != null && playerController.playerShooting != null &&
                playerController.playerShooting.boltgun != null &&
                playerController.playerShooting.boltgun.currentTotalAmmo < playerController.playerShooting.boltgun.maxAmmo)
            {
                boltgunBullets.OnPickUp(playerController);
                int audioBoltgun = Audio.PlayOneShot(BoltgunBulletsPicked);
                Engineson.Destroy(other);
            }
            else
            {
                ShotgunShells shotgunShells = other.GetComponent<ShotgunShells>();
                if (shotgunShells != null && playerController.playerShooting != null &&
                    playerController.playerShooting.shotgun != null &&
                    playerController.playerShooting.shotgun.currentTotalAmmo < playerController.playerShooting.shotgun.maxAmmo)
                {
                    shotgunShells.OnPickUp(playerController);
                    int audioShotgunShells = Audio.PlayOneShot(ShotgunShellsPicked);
                    Engineson.Destroy(other);
                }
            }
        }
        else if (other.tag == "Upgrade")
        {
            Engineson.print("Player Collided with:" + other.tag);

            BoltgunUpgradePickUp boltgunUpgrade = other.GetComponent<BoltgunUpgradePickUp>();
            if (boltgunUpgrade != null)
            {
                boltgunUpgrade.OnPickUp(playerController);
                Engineson.Destroy(other);
            }
            else
            {
                ShotgunUpgradePickUp shotgunUpgrade = other.GetComponent<ShotgunUpgradePickUp>();
                if (shotgunUpgrade != null)
                {
                    shotgunUpgrade.OnPickUp(playerController);
                    Engineson.Destroy(other);
                }
                else
                {
                    RailgunUpgradePickUp railgunUpgrade = other.GetComponent<RailgunUpgradePickUp>();
                    if (railgunUpgrade != null)
                    {
                        railgunUpgrade.OnPickUp(playerController);
                        Engineson.Destroy(other);
                    }
                }
            }
        }
        else if (other.tag == "Weapon")
        {
            Engineson.print("Player Collided with:" + other.tag);

            ShotgunPickUp shotgunPickUp = other.GetComponent<ShotgunPickUp>();
            if (shotgunPickUp != null)
            {
                shotgunPickUp.OnPickUp(playerController);
                int audioShotgun = Audio.PlayOneShot(ShotgunObtained);
                Engineson.Destroy(other);
            }
            else
            {
                RailgunPickUp railgunPickUp = other.GetComponent<RailgunPickUp>();
                if (railgunPickUp != null)
                {
                    railgunPickUp.OnPickUp(playerController);
                    int audioRailgun = Audio.PlayOneShot(RailgunObtained);
                    Engineson.Destroy(other);
                }
            }
        }
        else if (other.tag == "BiblePage")
        {
            Engineson.print("Player Collided with:" + other.tag);

            BiblePagePickUp biblePagePickUp = other.GetComponent<BiblePagePickUp>();
            if (biblePagePickUp != null)
            {
                biblePagePickUp.OnPickUp(playerController);
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