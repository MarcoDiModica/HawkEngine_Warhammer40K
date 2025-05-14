using HawkEngine;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;
using static System.Net.Mime.MediaTypeNames;

public class Ball : MonoBehaviour
{
    private Collider collider;
    public float value = 0.0f;
    private Rigidbody rigidbody;
    private float deathtimer = 5.0f;
    public bool needsDestroy = false;
    private float deathTimerPrevention = 0;
    public List<string> collisionNames = new List<string>();
    private float damage = 100.0f; // Placeholder damage value
    private string energyBallAudio = "Assets/Audio/SFX/Weapons/Railgun/EnergyBallMoving.wav";
    public override void Awake(){ }

    public override void Start() 
    {
        
    }
    public void Init(Vector3 pos, Vector3 dir)
    {
        //AddComponent<MeshRenderer>();
        GetComponent<Transform>().position = pos + dir * 3.0f + new Vector3(0, 2, 0);
        GetComponent<Transform>().SetScale(2.0f, 2.0f, 2.0f);
        AddComponent<BoxCollider>();
        AddComponent<Rigidbody>();
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(new Vector3(0.0f, 0.0f, 0.0f) * 20);
        rigidbody.AddForce(dir * 20);
        rigidbody.SetFriction(0.5f);
        collider = GetComponent<BoxCollider>();
        collider.SetTrigger(true);
        var energyBallFX = Engineson.CreateGameObject("ExplosionFX", null);
        gameObject.AddChild(energyBallFX);
        energyBallFX.AddComponent<ParticleFX>().ApplyPreset(12);
        //energyBallFX.GetComponent<ParticleFX>().EmitBurst(1);
        energyBallFX.GetComponent<ParticleFX>().Play();
        
    }
    public override void Update(float deltaTime)
    {
        if (needsDestroy)
        {
            deathTimerPrevention += deltaTime;
            if (deathTimerPrevention >= deathtimer)
            {
                Engineson.Destroy(gameObject);
                Audio.Stop(energyBallAudio);
                needsDestroy = false;
            }
        }
        

    }
    public override void OnTriggerEnter(GameObject other)
    {
        switch (other.tag)
        {
            case "Melee":
                other.GetComponent<EnemyControllerMelee>()?.TakeDamage(damage);
                break;
            case "Ranged":
                other.GetComponent<EnemyControllerRanged>()?.TakeDamage(damage);
                break;
            case "Stalker":
                other.GetComponent<EnemyControllerStalker>()?.TakeDamage(damage);
                break;
            case "Boss":
                other.GetComponent<EnemyControllerBoss>()?.TakeDamage(damage);
                break;
            case "Warrior":
                other.GetComponent<EnemyControllerWarrior>()?.TakeDamage(damage);
                break;
            case "Destroyable":
                other.GetComponent<DestroyEnviormentObject>()?.DestroyObject();
                break;
        }
    }

}
