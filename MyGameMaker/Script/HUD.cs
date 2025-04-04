using HawkEngine;
using System;
using System.Numerics;

public class HUD : MonoBehaviour
{
    private GameObject hpBar;
    private GameObject hpTempBar;
    private GameObject Player;
    private PlayerData playerData;
    private PlayerShooting playerShootingScript;
    private GameObject bigBoltGun;
    private GameObject bigShotGun;
    private GameObject bigRailGun;
    private GameObject smallBoltgunL;
    private GameObject smallShotgunL;
    private GameObject smallRailgunL;
    private GameObject smallBoltgunR;
    private GameObject smallShotgunR;
    private GameObject smallRailgunR;
    private GameObject lockL;
    private GameObject lockR;
    private Railgun railgunScript;

    private GameObject boltgunAbility1;
    private GameObject boltgunAbility2;
    private GameObject shotgunAbility1;
    private GameObject shotgunAbility2;
    private GameObject railgunAbility1;
    private GameObject railgunAbility2a;
    private GameObject railgunAbility2b;

    private UITransform transform_hpBar;
    private UITransform transform_hpTempBar;
    private UITransform transform_redThirstBar;

    private RedThirstManager redThirstManager;
    private GameObject redThirstBar;

    private GameObject nodash;
    private GameObject msup;
    private GameObject defenseup;
    private GameObject attackup;
    private GameObject noreload;
    private GameObject asup;
    private GameObject magnet;

    private GameObject bible1;
    private GameObject bible2;
    private GameObject bible3;
    private GameObject bible4;
    private GameObject bible5;

    private PlayerPowerUp playerPowerUp;

    float CalculateHPBarWidth()
    {
        float hp = playerData.GetHealth();
        float maxHp = playerData.GetMaxHealth();
        float width = (hp / maxHp) * 0.1f;
        return width;
    }

    float CalculateHPTempBarWidth()
    {
        float hpTemp = playerData.GetHealthTemp();
        float maxHpTemp = playerData.GetMaxHealthTemp();
        float width = (hpTemp / maxHpTemp) * 0.055f;
        return width;
    }

    float CalculateRedThirstBarHeight()
    {
        float redThirst = redThirstManager.GetRedThirstPoints();
        float maxRedThirst = 5;
        float height = (redThirst / maxRedThirst) * 0.08f;
        return height;
    }

    public override void Awake()
    {
    }
    public override void Start()
    {
        hpBar = GameObject.Find("blood_bar1");
        hpTempBar = GameObject.Find("blood_bar2");
        if (hpBar == null || hpTempBar == null)
        {
            Engineson.print("ERROR: ImageAsSlider requires a UIImage component!");
        }

        Player = GameObject.Find("Player");
        if (Player == null)
        {
            Engineson.print("ERROR: Player not found");
        }

        playerData = Player.GetComponent<PlayerController>().playerData;
        if (playerData == null)
        {
            Engineson.print("ERROR: PlayerData not found");
        }

        playerShootingScript = Player.GetComponent<PlayerShooting>();
        if (playerShootingScript == null)
        {
            Engineson.print("ERROR: PlayerShooting not found");
        }

        transform_hpBar = hpBar.GetComponent<UITransform>();
        transform_hpTempBar = hpTempBar.GetComponent<UITransform>();

        if (transform_hpBar == null || transform_hpTempBar == null)
        {
            Engineson.print("ERROR: UITransform not found");
        }

        //transform_hpBar.SetScaleUI(new Vector3(1, 1, 1));

        bigBoltGun = GameObject.Find("boltgun");
        bigRailGun = GameObject.Find("railgun");
        bigShotGun = GameObject.Find("shotgun");
        if (bigBoltGun == null || bigRailGun == null || bigShotGun == null)
        {
            Engineson.print("ERROR: BigGun not found");
        }

        smallBoltgunL = GameObject.Find("boltgun_small_L");
        smallShotgunL = GameObject.Find("shotgun_small_L");
        smallRailgunL = GameObject.Find("railgun_small_L");
        smallBoltgunR = GameObject.Find("boltgun_small_R");
        smallShotgunR = GameObject.Find("shotgun_small_R");
        smallRailgunR = GameObject.Find("railgun_small_R");
        if (smallBoltgunL == null || smallShotgunL == null || smallRailgunL == null || smallBoltgunR == null || smallShotgunR == null || smallRailgunR == null)
        {
            Engineson.print("ERROR: SmallGun not found");
        }

        lockL = GameObject.Find("lock_L");
        lockR = GameObject.Find("lock_R");

        boltgunAbility1 = GameObject.Find("boltgun_ability_1");
        boltgunAbility2 = GameObject.Find("boltgun_ability_2");
        shotgunAbility1 = GameObject.Find("shotgun_ability_1");
        shotgunAbility2 = GameObject.Find("shotgun_ability_2");
        railgunAbility1 = GameObject.Find("railgun_ability_1");
        railgunAbility2a = GameObject.Find("railgun_ability_2a");
        railgunAbility2b = GameObject.Find("railgun_ability_2b");

        if (boltgunAbility1 == null || boltgunAbility2 == null || shotgunAbility1 == null || shotgunAbility2 == null || railgunAbility1 == null || railgunAbility2a == null || railgunAbility2b == null)
        {
            Engineson.print("ERROR: GunAbilities not found");
        }

        railgunScript = playerShootingScript.railgun;

        if (railgunScript == null)
        {
            Engineson.print("ERROR: Railgun not found");
        }

        redThirstManager = Player.GetComponent<RedThirstManager>();
        if (redThirstManager == null)
        {
            Engineson.print("ERROR: RedThirstManager not found");
        }

        redThirstBar = GameObject.Find("red_thirst_bar");
        if (redThirstBar == null)
        {
            Engineson.print("ERROR: RedThirstBar not found");
        }
        transform_redThirstBar = redThirstBar.GetComponent<UITransform>();
        if (transform_redThirstBar == null)
        {
            Engineson.print("ERROR: RedThirstBar not found");
        }
        nodash = GameObject.Find("nodash");
        msup = GameObject.Find("msup");
        defenseup = GameObject.Find("defenseup");
        attackup = GameObject.Find("attackup");
        noreload = GameObject.Find("noreload");
        asup = GameObject.Find("asup");
        magnet = GameObject.Find("magnet");
        if (nodash == null || msup == null || defenseup == null || attackup == null || noreload == null || asup == null || magnet == null)
        {
            Engineson.print("ERROR: Buffs not found");
        }

        bible1 = GameObject.Find("bible1");
        bible2 = GameObject.Find("bible2");
        bible3 = GameObject.Find("bible3");
        bible4 = GameObject.Find("bible4");
        bible5 = GameObject.Find("bible5");
        if (bible1 == null || bible2 == null || bible3 == null || bible4 == null || bible5 == null)
        {
            Engineson.print("ERROR: Bibles not found");
        }

        playerPowerUp = Player.GetComponent<PlayerPowerUp>();

        if (playerPowerUp == null)
        {
            Engineson.print("ERROR: PlayerPowerUp not found");
        }
    }
    public override void Update(float deltaTime)
    {
        transform_hpBar.SetScaleUI(new Vector3(CalculateHPBarWidth(), 0.032f, 1.0f));
        transform_hpTempBar.SetScaleUI(new Vector3(CalculateHPTempBarWidth(), 0.018f, 1.0f));
        transform_redThirstBar.SetScaleUI(new Vector3(0.037f, CalculateRedThirstBarHeight(), 1.0f));

        if (redThirstManager.biblePages >= 1)
        {
            bible1.SetActive(true);
        }
        else
        {
            bible1.SetActive(false);
        }
        if (redThirstManager.biblePages >= 2)
        {
            bible2.SetActive(true);
        }
        else
        {
            bible2.SetActive(false);
        }
        if (redThirstManager.biblePages >= 3)
        {
            bible3.SetActive(true);
        }
        else
        {
            bible3.SetActive(false);
        }
        if (redThirstManager.biblePages >= 4)
        {
            bible4.SetActive(true);
        }
        else
        {
            bible4.SetActive(false);
        }
        if (redThirstManager.biblePages >= 5)
        {
            bible5.SetActive(true);
        }
        else
        {
            bible5.SetActive(false);
        }

        if (redThirstManager.IsInBlackRage())
        {
            nodash.SetActive(true);
            msup.SetActive(true);
            defenseup.SetActive(true);
        }
        else
        {
            nodash.SetActive(false);
            msup.SetActive(false);
            defenseup.SetActive(false);
        }

        switch (playerShootingScript.GetCurrentGun())
        {
            case 0:
                //Boltgun equipped
                bigBoltGun.SetActive(true);
                bigShotGun.SetActive(false);
                bigRailGun.SetActive(false);

                smallBoltgunL.SetActive(false);
                smallShotgunL.SetActive(false);
                smallRailgunL.SetActive(true);

                smallBoltgunR.SetActive(false);
                smallShotgunR.SetActive(true);
                smallRailgunR.SetActive(false);

                boltgunAbility1.SetActive(true);
                boltgunAbility2.SetActive(true);
                shotgunAbility1.SetActive(false);
                shotgunAbility2.SetActive(false);
                railgunAbility1.SetActive(false);
                railgunAbility2a.SetActive(false);
                railgunAbility2b.SetActive(false);

                if(playerShootingScript.hasShotgun)
                {
                    lockR.SetActive(false);
                }
                else
                {
                    lockR.SetActive(true);
                }

                if (playerShootingScript.hasRailgun)
                {
                    lockL.SetActive(false);
                }
                else
                {
                    lockL.SetActive(true);
                }

                break;
            case 1:
                //Shotgun equipped
                bigBoltGun.SetActive(false);
                bigShotGun.SetActive(true);
                bigRailGun.SetActive(false);

                smallBoltgunL.SetActive(true);
                smallShotgunL.SetActive(false);
                smallRailgunL.SetActive(false);

                smallBoltgunR.SetActive(false);
                smallShotgunR.SetActive(false);
                smallRailgunR.SetActive(true);



                boltgunAbility1.SetActive(false);
                boltgunAbility2.SetActive(false);
                shotgunAbility1.SetActive(true);
                shotgunAbility2.SetActive(true);
                railgunAbility1.SetActive(false);
                railgunAbility2a.SetActive(false);
                railgunAbility2b.SetActive(false);

                if(playerShootingScript.hasRailgun)
                {
                    lockR.SetActive(false);
                }
                else
                {
                    lockR.SetActive(true);
                }
                if (playerShootingScript.hasBoltgun)
                {
                    lockL.SetActive(false);
                }
                else
                {
                    lockL.SetActive(true);
                }
                break;
            case 2:
                //Railgun equipped
                bigBoltGun.SetActive(false);
                bigShotGun.SetActive(false);
                bigRailGun.SetActive(true);

                smallBoltgunL.SetActive(false);
                smallShotgunL.SetActive(true);
                smallRailgunL.SetActive(false);

                smallBoltgunR.SetActive(true);
                smallShotgunR.SetActive(false);
                smallRailgunR.SetActive(false);

                boltgunAbility1.SetActive(false);
                boltgunAbility2.SetActive(false);
                shotgunAbility1.SetActive(false);
                shotgunAbility2.SetActive(false);
                railgunAbility1.SetActive(true);
                switch (railgunScript.railgunMode)
                {
                    case Railgun.RailgunMode.SEMIAUTOMATIC:
                        railgunAbility2a.SetActive(true);
                        railgunAbility2b.SetActive(false);
                        break;
                    case Railgun.RailgunMode.AUTOMATIC:
                        railgunAbility2a.SetActive(false);
                        railgunAbility2b.SetActive(true);
                        break;
                }

                if (playerShootingScript.hasBoltgun)
                {
                    lockR.SetActive(false);
                }
                else
                {
                    lockR.SetActive(true);
                }
                if (playerShootingScript.hasShotgun)
                {
                    lockL.SetActive(false);
                }
                else
                {
                    lockL.SetActive(true);
                }
                break;
        }



        if(playerPowerUp.GetHasMedicaeStimm())
        {
            msup.SetActive(true);
        }
        else if (!redThirstManager.IsInBlackRage())
        {
            msup.SetActive(false);

        }

        if (playerPowerUp.GetHasAmmunitionBlessing())
        {
            noreload.SetActive(true);
        }
        else
        {
            noreload.SetActive(false);
        }

        if(playerPowerUp.GetHasMagnet())
        {
            magnet.SetActive(true);
        }
        else
        {
            magnet.SetActive(false);
        }

    }

    
}