using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class AmmunitionBlessing : PowerUp
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
        Engineson.print("AmmunitionBlessing PowerUp applied");
        playerController.playerData.infiniteBullets = true;

    }
}
