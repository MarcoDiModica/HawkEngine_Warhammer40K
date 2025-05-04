using HawkEngine;
using System;
using System.Numerics;

public class HUD : MonoBehaviour
{
    private GameObject hpBar;
    private GameObject hpBarAnim;
    private GameObject hpTempBar;
    private GameObject hpTempBarAnim;
    private GameObject redThirstBar;
    private GameObject redThirstBarAnim;
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
    private UITransform transform_hpBarAnim;
    private UITransform transform_hpTempBar;
    private UITransform transform_hpTempBarAnim;
    private UITransform transform_redThirstBar;
    private UITransform transform_redThirstBarAnim;

    private RedThirstManager redThirstManager;

    private GameObject nodash;
    private GameObject msup;
    private GameObject defenseup;
    private GameObject attackup;
    private GameObject noreload;
    private GameObject asup;
    private GameObject magnet;

    private GameObject canister1;
    private GameObject canister2;
    private GameObject canister3;
    private GameObject canister4;
    private GameObject canister5;

    private PlayerPowerUp playerPowerUp;

    private GameObject pauseMenu;
    private GameObject optionMenu;

    private UIImage hpBarAnimImage;
    private UIImage hpTempBarAnimImage;
    private UIImage redThirstBarAnimImage;


    void win()
    {
        SceneManager.LoadScene("WinScene");
    }

    void lose()
    {
        SceneManager.LoadScene("LoseScene");
    }

    float CalculateHPBarWidth()
    {
        float hp = playerData.GetHealth();
        float maxHp = playerData.GetMaxHealth();
        float width = (hp / maxHp) * 0.2f;
        return width;
    }

    float CalculateHPBarAnimPos()
    {
        float hp = playerData.GetHealth();
        float maxHp = playerData.GetMaxHealth();
        float pos = (hp / maxHp) * 0.2f + 0.043f;
        return pos;
    }

    float CalculateHPTempBarWidth()
    {
        float hpTemp = playerData.GetHealthTemp();
        float maxHpTemp = playerData.GetMaxHealthTemp();
        float width = (hpTemp / maxHpTemp) * 0.138f;
        return width;
    }

    float CalculateHPTempBarAnimPos()
    {
        float hpTemp = playerData.GetHealthTemp();
        float maxHpTemp = playerData.GetMaxHealthTemp();
        float pos = (hpTemp / maxHpTemp) * 0.138f + 0.057f;
        return pos;
    }


    float CalculateRedThirstBarHeight()
    {
        float redThirst = redThirstManager.GetRedThirstPoints();
        float maxRedThirst = 5;
        float height = (redThirst / maxRedThirst) * 0.08f;
        return height;
    }

    float CalculateRedThirstBarAnimPos()
    {
        float redThirst = redThirstManager.GetRedThirstPoints();
        float maxRedThirst = 5;
        float pos = 0.961f - (redThirst / maxRedThirst) * 0.08f;
        return pos;
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

        if (playerShootingScript.hasRailgun)
        {
            railgunScript = playerShootingScript.railgun;
            if (railgunScript == null)
            {
                Engineson.print("ERROR: railgun unlocked but not instantiated!");
            }
            else
            {
                railgunScript.railgunMode = Railgun.RailgunMode.SEMIAUTOMATIC;
            }
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

        canister1 = GameObject.Find("canister1");
        canister2 = GameObject.Find("canister2");
        canister3 = GameObject.Find("canister3");
        canister4 = GameObject.Find("canister4");
        canister5 = GameObject.Find("canister5");
        if (canister1 == null || canister2 == null || canister3 == null || canister4 == null || canister5 == null)
        {
            Engineson.print("ERROR: Canisters not found");
        }

        playerPowerUp = Player.GetComponent<PlayerPowerUp>();

        if (playerPowerUp == null)
        {
            Engineson.print("ERROR: PlayerPowerUp not found");
        }

        pauseMenu = GameObject.Find("Canvas_PauseMenu");
        optionMenu = GameObject.Find("Canvas_OptionsMenu");

        hpBarAnim = GameObject.Find("blood_animation_main");
        transform_hpBarAnim = hpBarAnim.GetComponent<UITransform>();
        hpBarAnimImage = hpBarAnim.GetComponent<UIImage>();
        hpBarAnimImage.SetImageHasAnimation(true);
        hpBarAnimImage.SetImageSpriteSize(50, 80);
        hpBarAnimImage.SetImageAnimationSpeed(0.5f);
        hpBarAnimImage.SetImageAnimationIndexLimit(4);

        hpTempBarAnim = GameObject.Find("blood_animation_temp");
        transform_hpTempBarAnim = hpTempBarAnim.GetComponent<UITransform>();
        hpTempBarAnimImage = hpTempBarAnim.GetComponent<UIImage>();
        hpTempBarAnimImage.SetImageHasAnimation(true);
        hpTempBarAnimImage.SetImageSpriteSize(50, 80);
        hpTempBarAnimImage.SetImageAnimationSpeed(0.5f);
        hpTempBarAnimImage.SetImageAnimationIndexLimit(4);

        redThirstBarAnim = GameObject.Find("thirst_animation");
        transform_redThirstBarAnim = redThirstBarAnim.GetComponent<UITransform>();
        redThirstBarAnimImage = redThirstBarAnim.GetComponent<UIImage>();
        redThirstBarAnimImage.SetImageHasAnimation(true);
        redThirstBarAnimImage.SetImageSpriteSize(175, 40);
        redThirstBarAnimImage.SetImageAnimationSpeed(0.5f);
        redThirstBarAnimImage.SetImageAnimationIndexLimit(6);
    }
    public override void Update(float deltaTime)
    {
        transform_hpBar.SetScaleUI(new Vector3(CalculateHPBarWidth(), 0.032f, 1.0f));
        transform_hpBarAnim.DOMoveXUI(CalculateHPBarAnimPos(), 0f, Modes.LINEAR);
        transform_hpTempBar.SetScaleUI(new Vector3(CalculateHPTempBarWidth(), 0.018f, 1.0f));
        transform_hpTempBarAnim.DOMoveXUI(CalculateHPTempBarAnimPos(), 0f, Modes.LINEAR);
        transform_redThirstBar.SetScaleUI(new Vector3(0.037f, CalculateRedThirstBarHeight(), 1.0f));
        transform_redThirstBarAnim.DOMoveYUI(CalculateRedThirstBarAnimPos(), 0f, Modes.LINEAR);

        if (playerShootingScript.hasRailgun && railgunScript == null)
        {
            railgunScript = playerShootingScript.railgun;
        }

        if (redThirstManager.biblePages >= 1)
        {
            canister1.SetActive(true);
        }
        else
        {
            canister1.SetActive(false);
        }
        if (redThirstManager.biblePages >= 2)
        {
            canister2.SetActive(true);
        }
        else
        {
            canister2.SetActive(false);
        }
        if (redThirstManager.biblePages >= 3)
        {
            canister3.SetActive(true);
        }
        else
        {
            canister3.SetActive(false);
        }
        if (redThirstManager.biblePages >= 4)
        {
            canister4.SetActive(true);
        }
        else
        {
            canister4.SetActive(false);
        }
        if (redThirstManager.biblePages >= 5)
        {
            canister5.SetActive(true);
        }
        else
        {
            canister5.SetActive(false);
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

                if (playerShootingScript.hasShotgun)
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

                if (playerShootingScript.hasRailgun)
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
                if (railgunScript == null)
                {
                    Engineson.print("ERROR: Hud.Update – railgunScript is null!");
                    break;
                }
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



        if (playerPowerUp.GetHasMedicaeStimm())
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

        if (playerPowerUp.GetHasMagnet())
        {
            magnet.SetActive(true);
        }
        else
        {
            magnet.SetActive(false);
        }

        if (playerData.GetHealth() <= 0)
        {
            lose();
        }

        if (Input.GetKeyDown(KeyCode.TAB))
        {
            win();
        }


        if (Input.GetKeyDown(KeyCode.P) || Input.GetControllerButtonDown(ControllerButton.Start))
        {
            if (pauseMenu.IsActive())
            {
                pauseMenu.SetActive(false);
            }
            else
            {
                pauseMenu.SetActive(true);
            }
            optionMenu.SetActive(false);
        }

        if (Input.GetKeyDown(KeyCode.L))
        {
            redThirstManager.AddBiblePages(1);
        }
        if (Input.GetKeyDown(KeyCode.O))
        {
            redThirstManager.AddRedThirstPoint(1);
        }
    }
}