using HawkEngine;
using System;
using System.Numerics;

public class ImageAsSlider : MonoBehaviour
{
    private UIImage image;
    private PlayerData playerData;

    public override void Start()
    {
        playerData = new PlayerData();

        image = gameObject.GetComponent<UIImage>();
        if (image == null)
        {
            Engineson.print("ERROR: ImageAsSlider requires a UIImage component!");
        }
    }

    public override void Update(float deltaTime)
    {
        Engineson.print("PlayerData: " + playerData.GetHealth());

        if (Input.GetKeyDown(KeyCode.B))
        {
            playerData.SetHealth(playerData.GetHealth() - 10);
        }

        Transform transform = gameObject.GetComponent<Transform>();
        transform.position = new Vector3(0, 0, 0);
    }
}