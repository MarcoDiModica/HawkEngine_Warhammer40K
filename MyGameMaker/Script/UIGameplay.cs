using HawkEngine;
using System;
using System.Numerics;

public class UIGameplay : MonoBehaviour
{
    private UIImage image;
    private PlayerData playerData;
    private UITransform transform;

    public override void Start()
    {
        playerData = new PlayerData();

        transform = GameObject.Find("PlayerLife").GetComponent<UITransform>();
        image = GameObject.Find("RailgunIcon").GetComponent<UIImage>();
        if (image == null || transform == null)
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
        transform.SetUIScale(new Vector3(CaclulateLifeBarWidth(), 0.054f, 1.0f));

        if (Input.GetKeyDown(KeyCode.N))
        {
            image.SetImage("../MyGameEditor/Assets/Textures/shotgun_icon.png");
        }
    }

    float CaclulateLifeBarWidth()
    {
        if (playerData.GetMaxHealth() == 0) return 0;

        return 0 + ((playerData.GetHealth() - 0) / (playerData.GetMaxHealth() - 0)) * ((float)0.195 - 0);
    }

}


