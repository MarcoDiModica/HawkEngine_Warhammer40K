using HawkEngine;
using System;
using System.Numerics;

public class UIGameplay : MonoBehaviour
{
    private UIImage boltgunIcon;
    private UIImage shotgunIcon;
    private UIImage railgunIcon;
    private PlayerData playerData;
    private UITransform transform;
    private PlayerShooting playerShootingScript;

    public override void Start()
    {
        playerData = new PlayerData();
        playerShootingScript = GameObject.Find("Player").GetComponent<PlayerShooting>();
        transform = GameObject.Find("PlayerLife").GetComponent<UITransform>();
        boltgunIcon = GameObject.Find("BoltgunIcon").GetComponent<UIImage>();
        shotgunIcon = GameObject.Find("ShotgunIcon").GetComponent<UIImage>();
        railgunIcon = GameObject.Find("RailgunIcon").GetComponent<UIImage>();
        if (railgunIcon == null || transform == null)
        {
            Engineson.print("ERROR: ImageAsSlider requires a UIImage component!");
        }
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            playerData.SetHealth(playerData.GetHealth() - 10);
        }

        if (Input.GetKeyDown(KeyCode.M))
        {
            playerData.SetHealth(playerData.GetHealth() + 10);
        }
        transform.SetUIScale(new Vector3(CaclulateLifeBarWidth(), 0.054f, 1.0f));

        switch (playerShootingScript.GetCurrentGun())
        {
            case 0:
                boltgunIcon.SetImageEnabled(true);
                shotgunIcon.SetImageEnabled(false);
                railgunIcon.SetImageEnabled(false);
                break;
            case 1:
                boltgunIcon.SetImageEnabled(false);
                shotgunIcon.SetImageEnabled(true);
                railgunIcon.SetImageEnabled(false);
                break;
            case 2:
                boltgunIcon.SetImageEnabled(false);
                shotgunIcon.SetImageEnabled(false);
                railgunIcon.SetImageEnabled(true);
                break;
        }
    }

    float CaclulateLifeBarWidth()
    {
        if (playerData.GetMaxHealth() == 0) return 0;

        return 0 + ((playerData.GetHealth() - 0) / (playerData.GetMaxHealth() - 0)) * ((float)0.195 - 0);
    }

}


