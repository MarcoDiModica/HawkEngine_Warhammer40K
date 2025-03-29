using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public abstract class PowerUp : MonoBehaviour
{

    public enum PowerUpType
    {
        BLACKHEART,
        MEDICAE_STIMM,
    }
    public PowerUpType powerUpType;

    public abstract void ApplyPowerUpOnPickup(PlayerController playerController);

}