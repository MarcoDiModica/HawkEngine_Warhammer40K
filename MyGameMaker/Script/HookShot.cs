using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class HookShot : BaseAbilities
{
    public string name;
    public bool enabled;
    public float cooldown;
    private float yHeight = 0.0f;
    private float timer = 0;
    private bool exploded = false;
    GameObject hook;
    Rigidbody rigidbody;
    BoxCollider collider;
    bool canThrow = true;
    private float abilityCooldown = 3.0f; 
    private float abilityTimer = 0.0f;
    private Audio sound;
    private string hookHit = "Assets/Audio/SFX/Weapons/Shotgun/HookLaunch.wav";
    

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
        // Manejo del cooldown de la habilidad
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
        sound.LoadAudio(hookHit);
        sound.Play();
        hook = Engineson.CreateGameObject("Hook", null);
        if (hook == null)
        {
            Engineson.print("ERROR: No se pudo crear el hook.");
            return;
        }
        hook.AddScript("Hook");
        hook.GetComponent<Hook>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
        hook.AddComponent<Audio>();
        hook.GetComponent<Hook>().Start();
        canThrow = false;
        abilityTimer = 0.0f;
    }
    public override void ResetCooldowns()
    {
        canThrow = true;
    }
}
