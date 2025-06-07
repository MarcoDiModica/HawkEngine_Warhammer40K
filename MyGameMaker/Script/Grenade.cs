using HawkEngine;
using System.Collections;
using System.Numerics;

public class Grenade : MonoBehaviour
{
    public float value = 0.0f;
    private Transform transform;

    float distance = 6;
    float damage = 100;
    float explosionRadius = 2;
    Rigidbody rigidbody;
    bool isExploded = false;
    GameObject explosion;
    float deathtimer = 0.8f;
    GameObject explosionFX;
    GameObject granadeVFX;
    float deathTimerPrevention = 0;
    private const string granadeExplosion = "Assets/Audio/SFX/Weapons/Boltgun/BoltgunAbility1GrenadeExplosion.wav";

    public override void Awake()
    {

    }
    public override void Start()
    {

    }

    public void Init(Vector3 pos, Vector3 dir)
    {
        //AddComponent<MeshRenderer>();      
        GetComponent<Transform>().position = pos + dir * 4.0f + new Vector3(0, 3, 0);
        GetComponent<Transform>().SetScale(0.25f, 0.25f, 0.25f);
        AddComponent<CapsuleCollider>();
        AddComponent<Rigidbody>();
        AddComponent<ParticleFX>();
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(new Vector3(0.0f, -9.81f, 0.0f) * 20);
        rigidbody.AddForce(dir * 250);
        rigidbody.SetFriction(0.5f);
        granadeVFX = Engineson.CreateGameObject("ExplosionGranadeFX", null);
        gameObject.AddChild(granadeVFX);
        granadeVFX.AddComponent<ParticleFX>();
        ParticleFX particleFX = granadeVFX.GetComponent<ParticleFX>();
        if (particleFX != null)
        {


            particleFX.ApplyPreset(43);
            //    particleFX.EmitBurst(100);
            particleFX.EmitBurst(1);

        }
    }

    public override void Update(float deltaTime)
    {
        if (isExploded)
        {
            deathtimer -= deltaTime;
            if (deathtimer <= 0)
            {
                if (explosion != null)
                {
                    Engineson.Destroy(explosion);
                    Engineson.Destroy(gameObject);
                    Engineson.print("Destroyed");
                    isExploded = false;
                }
            }
        }
        deathTimerPrevention += deltaTime;
        //Engineson.print(deathTimerPrevention.ToString());
        if (deathTimerPrevention > .1f)
        {
            if (explosion != null) {
                //Engineson.Destroy(explosion);
                //isExploded = false;
            }
        }
        
    }

    void Explode()
    {
        rigidbody.SetVelocity(new Vector3(0, 0, 0));
        explosion = Engineson.CreateGameObject("Explosion", null);
        Audio.PlayOneShot(granadeExplosion);
        explosion.GetComponent<Transform>().SetPosition(GetComponent<Transform>().GetPosition().X, GetComponent<Transform>().GetPosition().Y, GetComponent<Transform>().GetPosition().Z);
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);
        gameObject.AddChild(explosion);
        explosionFX = Engineson.CreateGameObject("ExplosionGranadeFX", null);
        gameObject.AddChild(explosionFX);
        explosionFX.AddComponent<ParticleFX>().ApplyPreset(4);
        explosionFX.GetComponent<ParticleFX>().EmitBurst(80);
        explosionFX.GetComponent<Transform>().SetPosition(
            GetComponent<Transform>().GetPosition().X,
            GetComponent<Transform>().GetPosition().Y,
            GetComponent<Transform>().GetPosition().Z
        );
        isExploded = true;
    }

    public override void OnCollisionEnter(GameObject other)
    {
        if (other.tag != "Player")
        {
            Explode();
        }

        switch (other.tag)
        {
            case "Melee":
                other.GetComponent<EnemyControllerMelee>()?.TakeDamage(damage);
                GetComponent<ParticleFX>().ApplyPreset(43);
                GetComponent<ParticleFX>().EmitBurst(1);
                break;
            case "Ranged":
                other.GetComponent<EnemyControllerRanged>()?.TakeDamage(damage);
                GetComponent<ParticleFX>().ApplyPreset(43);
                GetComponent<ParticleFX>().EmitBurst(1);
                break;
            case "Stalker":
                other.GetComponent<EnemyControllerStalker>()?.TakeDamage(damage);
                GetComponent<ParticleFX>().ApplyPreset(43);
                GetComponent<ParticleFX>().EmitBurst(1);
                break;
            case "Boss":
                other.GetComponent<EnemyControllerBoss>()?.TakeDamage(damage);
                GetComponent<ParticleFX>().ApplyPreset(43);
                GetComponent<ParticleFX>().EmitBurst(1);
                break;
            case "Warrior":
                other.GetComponent<EnemyControllerWarrior>()?.TakeDamage(damage);
                GetComponent<ParticleFX>().ApplyPreset(43);
                GetComponent<ParticleFX>().EmitBurst(1);
                break;
            case "Destroyable":
                other.GetComponent<DestroyEnviormentObject>()?.DestroyObject();
                GetComponent<ParticleFX>().ApplyPreset(43);
                GetComponent<ParticleFX>().EmitBurst(1);
                break;
        }
    }

}
