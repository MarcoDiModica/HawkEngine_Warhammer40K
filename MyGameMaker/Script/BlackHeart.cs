using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class BlackHeart : PowerUp
{
    public override void Awake()
    {
        
    }

    public override void Start()
    {
        
    }

    public override void Update(float deltatime)
    {

    }

    public override void ApplyPowerUpOnPickup(PlayerController playerController)
    {
        Engineson.print("BlackHeart PowerUp applied");
        playerController.playerData.SetHealth(playerController.playerData.GetMaxHealth());
        
    }
}