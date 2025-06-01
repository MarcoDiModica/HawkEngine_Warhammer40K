using HawkEngine;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;

public class Arc : MonoBehaviour
{
    public float value = 0.0f;
    private Transform transform;
    public List<string> collisionNames = new List<string>();
    float damage = 20;
    Rigidbody rigidbody;
    bool isExploded = false;
    GameObject explosion;
    GameObject explosionFX;
    ParticleFX particleFX;
    float deathtimer = 0.2f;
    public bool needsDestroy = false;
    float deathTimerPrevention = 0;

    //private AudioSource sound;
    private string arcExplosion = "Assets/Audio/SFX/Weapons/Boltgun/ArcSnareExplosion.wav";
    //AudioClip arcFX;

    public override void Awake()
    {

    }
    public override void Start()
    {
        //sound = gameObject.GetComponent<AudioSource>();
        //if (sound == null)
        //{
        //    Engineson.print("PlayerShooting: Audio component not found");
        //}

        //arcFX = new AudioClip(arcExplosion, "ArcExplosionFX", false, false);

        //sound.LoadAudioClip(arcFX);
    }

    public void Init(Vector3 pos, Vector3 dir)
    {
        //AddComponent<MeshRenderer>();
        GetComponent<Transform>().position = pos + dir * 3.0f + new Vector3(0, 2, 0);
        GetComponent<Transform>().SetScale(0.25f, 0.25f, 0.25f);
        AddComponent<CapsuleCollider>();
        AddComponent<Rigidbody>();
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.SetMass(0.05f);
        rigidbody.SetGravity(new Vector3(0.0f, 0.0f, 0.0f) * 20);
        rigidbody.AddForce(dir * 140);
        rigidbody.SetFriction(0.5f);
        gameObject.AddComponent<ParticleFX>();
        particleFX = gameObject.GetComponent<ParticleFX>();
        particleFX.ApplyPreset(14);
        particleFX.EmitBurst(1);

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
            if (explosion != null)
            {
                //Engineson.Destroy(explosion);
                //isExploded = false;
            }
        }

    }





    void Explode()
    {
        rigidbody.SetVelocity(new Vector3(0, 0, 0));
        explosion = Engineson.CreateGameObject("Explosion", null);
        Audio.PlayOneShot(arcExplosion);
        explosion.GetComponent<Transform>().SetPosition(GetComponent<Transform>().GetPosition().X, GetComponent<Transform>().GetPosition().Y, GetComponent<Transform>().GetPosition().Z);
        explosion.GetComponent<Transform>().SetScale(4f, 0.25f, 4f);
        gameObject.AddChild(explosion);
        explosionFX = Engineson.CreateGameObject("ExplosionGranadeFX", null);
        gameObject.AddChild(explosionFX); explosionFX.AddComponent<ParticleFX>().ApplyPreset(17);
        explosionFX.GetComponent<ParticleFX>().EmitBurst(1);
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
                other.GetComponent<EnemyControllerMelee>()?.getSlowed();
                break;
            case "Ranged":
                other.GetComponent<EnemyControllerRanged>()?.TakeDamage(damage);
                other.GetComponent<EnemyControllerRanged>()?.getSlowed();
                break;
            case "Stalker":
                other.GetComponent<EnemyControllerStalker>()?.TakeDamage(damage);
                other.GetComponent<EnemyControllerStalker>()?.getSlowed();
                break;
            case "Boss":
                other.GetComponent<EnemyControllerBoss>()?.TakeDamage(damage);
                break;
            case "Warrior":
                other.GetComponent<EnemyControllerWarrior>()?.TakeDamage(damage);
                other.GetComponent<EnemyControllerWarrior>()?.getSlowed();
                break;
            case "Destroyable":
                other.GetComponent<DestroyEnviormentObject>()?.DestroyObject();
                break;
        }
    }

}
