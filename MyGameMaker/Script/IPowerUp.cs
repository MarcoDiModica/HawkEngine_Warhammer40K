using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public abstract class PowerUp : MonoBehaviour
{

    public abstract void ApplyPowerUpOnPickup(PlayerController playerController);

}