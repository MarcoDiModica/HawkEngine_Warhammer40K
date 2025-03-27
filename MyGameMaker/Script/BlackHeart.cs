using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class BlackHeart : PowerUp
{
    public override void Awake()
    {
        powerUpType = PowerUpType.BLACKHEART;
    }

    public override void ApplyPowerUpOnPickup(PlayerController playerController)
    {
        playerController.playerData.SetHealth(playerController.playerData.GetMaxHealth());
    }
}