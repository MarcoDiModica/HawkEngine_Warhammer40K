using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
using static BaseWeapon;

public class EnergyBall : BaseAbilities
{
    public string name;
    public bool enabled;
    public float cooldown;
    GameObject energyBall;
    public bool canThrow = true;
    private float abilityCooldown = 3.0f; // Cooldown de la habilidad
    private float abilityTimer = 0.0f;    // Contador del cooldown
    private float time = 0.0f;
    private Audio sound;
    private string energyBallLaunch = "Assets/Audio/SFX/Weapons/Railgun/EnergyBallLaunch.wav";
    private string energyBallSound= "Assets/Audio/SFX/Weapons/Railgun/EnergyBallMoving.wav";
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
        if (canThrow)
        {
            sound.LoadAudio(energyBallLaunch);
            sound.Play();
            energyBall = Engineson.CreateGameObject("energyBall", null);
            if (energyBall == null)
            {
                Engineson.print("ERROR: No se pudo crear la granada.");
                return;
            }
            energyBall.AddScript("Ball");
            energyBall.GetComponent<Ball>().Init(gameObject.GetComponent<Transform>().GetPosition(), gameObject.GetComponent<Transform>().forward);
            var energyBallFX = Engineson.CreateGameObject("ExplosionFX", null);
            energyBall.AddChild(energyBallFX);
            energyBallFX.AddComponent<ParticleFX>().ApplyPreset(12);
            //energyBallFX.GetComponent<Transform>().SetLocalPosition(0, 0.5f, 0);
            energyBallFX.GetComponent<ParticleFX>().EmitBurst(1);
            energyBallFX.GetComponent<Transform>().SetPosition(
                energyBall.GetComponent<Transform>().GetPosition().X,
                energyBall.GetComponent<Transform>().GetPosition().Y,
                energyBall.GetComponent<Transform>().GetPosition().Z
            );
            sound.LoadAudio(energyBallSound);
            sound.Play();
            canThrow = false;
            abilityTimer = 0.0f;
        }
        else
        {
            Engineson.print("Habilidad en cooldown. Espera...");
        }
        if (!canThrow)
        {
            abilityTimer += time;
            if (abilityTimer >= abilityCooldown)
            {
                canThrow = true;
                abilityTimer = 0.0f;
            }
        }
    }
    public override void ResetCooldowns()
    {
        canThrow = true;
    }


}

