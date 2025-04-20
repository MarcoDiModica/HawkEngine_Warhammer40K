using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class GrenadeLauncher : BaseAbilities
{
    public string name;
    public bool enabled;
    public float cooldown;
    GameObject grenade;
    bool canThrow = true;
    public bool needsDestroy = false;
    private float abilityCooldown = 3.0f; 
    private float abilityTimer = 0.0f;    
    private Audio sound;
    private string granadeLaunch = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunAbility1GrenadeShot.wav";
    private string granadeExplosion = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunAbility1GrenadeExplosion.wav";

    public override void Awake()
    {

    }

    public override void Start()
    {
        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
        }
    }

    public override void Update(float deltaTime)
    {
        if (!canThrow)
        {
            abilityTimer += deltaTime;
            if (abilityTimer >= abilityCooldown)
            {
                canThrow = true;
                abilityTimer = 0.0f;
            }
        } 
    }

    public override void TriggerAbility()
    {
        if (!canThrow)
        {
            return;
        }
        sound?.LoadAudio(granadeLaunch);
        sound?.Play();
        grenade = Engineson.CreateGameObject("Grenade", null);

        if (grenade == null)
        {
            return;
        }

        grenade.AddScript("Grenade");
        grenade.GetComponent<Grenade>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
        grenade.AddComponent<Audio>();
        grenade.GetComponent<Grenade>().Start();

        canThrow = false; 
        abilityTimer = 0.0f;
    }

    

    public override void ResetCooldowns()
    {
        canThrow = true;
    }
}
