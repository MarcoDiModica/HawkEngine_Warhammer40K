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

    private UITransform transform_hpBar;
    private UITransform transform_hpTempBar;

 

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
    }
    public override void Update(float deltaTime)
    {
        transform_hpBar.SetScaleUI(new Vector3(CalculateHPBarWidth(), 0.032f, 1.0f));
        transform_hpTempBar.SetScaleUI(new Vector3(CalculateHPTempBarWidth(), 0.018f, 1.0f));

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
                break;
        }
    }
}