using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net;
using System.Numerics;
using HawkEngine;

public class EnemyControllerBoss : EnemyController
{
    private float hurtboxActivationTime = 0.5f; // Tiempo que el jugador debe estar en la hurtbox para activarla
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(2.0f, 2.0f, 2.0f); // Tamaño de la hurtbox
    private Vector3 hurtboxOffset = new Vector3(4.0f, 2.0f, 0.0f); // Desplazamiento de la hurtbox hacia adelante
    private GameObject hurtboxObject;
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;

    private List<BulletData> activeProjectiles = new List<BulletData>();
    public float shootCooldown = 2.0f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
    protected float shootTimer = 0f;

    //stats
    private float health = 1500.0f;
    private float damage = 25.0f;

    // unburrowing attack stats
    private float unburrowingAttackCooldown = 5.0f;
    private float postUnburrowingAttackDelay = 1.5f;
    private float restAfterThirdAttack = 4.0f;
    private float timer = 0.0f;
    private int attackCount = 0;
    private bool isBuried = true;

    // phase 2 unburrowing/slam stats
    private float unburrowingAttackCooldownPhase2 = 5.0f;
    private float postAttackDelay = 2.0f;
    private float burrowTime = 2.0f;
    private bool isPreparingAttack = false;
    private Vector3[] fixedPositions = new Vector3[]
    {
        new Vector3(10,0,10),
        new Vector3(-10,0,10),
        new Vector3(10,0,-10),
        new Vector3(-10,0,-10)
    };
    private float slamAttackDistance = 20.0f;
    private float slamAttackCooldown = 2.0f;
    private float slamAttackTimer = 0.0f;

    private enum BossPhase
    {
        PHASE1,
        PHASE2,
        PHASE3
    }

    private BossPhase currentPhase;

    public override void Awake()
    {
 
    }

    public override void Start()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        if (playerTransform == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }
        collider = gameObject.GetComponent<BoxCollider>();
        if (collider == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Collider component!");
            return;
        }
        sound = gameObject.GetComponent<Audio>();
        if (sound == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
        }
        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }
        currentPhase = BossPhase.PHASE2;
    }
    public override void Update(float deltaTime)
    {
        if (health < 500)
        {
            currentPhase = BossPhase.PHASE3;
        }
        else if (health < 1000)
        {
            currentPhase = BossPhase.PHASE2;
        }
        switch (currentPhase)
        {
            case BossPhase.PHASE1:

                break;
            case BossPhase.PHASE2:

                break;
            case BossPhase.PHASE3:

                break;
        }

        if (currentPhase == BossPhase.PHASE1)
        {
            timer += deltaTime;

            if (isBuried && timer >= unburrowingAttackCooldown)
            {
                UnburrowingAttack();
                timer = 0.0f;
            }
            else if (!isBuried && timer >= postUnburrowingAttackDelay)
            {
                if (attackCount % 3 == 0 && attackCount > 0)
                {
                    if (timer >= restAfterThirdAttack)
                    {
                        Burrow();
                        timer = 0.0f;
                    }
                }
                else
                {
                    Burrow();
                    timer = 0.0f;
                }
            }
        }

        if (currentPhase == BossPhase.PHASE2)
        {
            timer += deltaTime;

            if(isBuried && timer >= unburrowingAttackCooldown)
            {
                isPreparingAttack = true;
                timer = 0.0f;
            }
            else if (isPreparingAttack && timer >= burrowTime)
            {
                UnburrowingAttack();
                isPreparingAttack = false;
                timer = 0.0f;
            }
            else if (!isBuried && timer >= postAttackDelay)
            {
                if (playerTransform != null)
                {
                    float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

                    if (distanceToPlayer <= slamAttackDistance && slamAttackTimer <= 0.0f)
                    {
                        SlamAttack();
                        slamAttackTimer = slamAttackCooldown;
                    }
                    else
                    {
                        ChangePositionToClosest();
                    }
                }
                timer = 0.0f;
            }

            if (slamAttackTimer > 0.0f)
            {
                slamAttackTimer -= deltaTime;
            }
        }
      
    }

    override public void OnCollisionEnter(GameObject other)
    {
        if (other.tag == "BoltgunProjectile")
        {
            currentHealth -= 20.0f;
            Engineson.print("Boltgun hit!");
        }
        else if (other.tag == "ShotgunProjectile")
        {
            //cosas de la shotgun
        }
        else if (other.tag == "RailgunProjectile")
        {
            //Cosas de railgun
        }
        if (currentHealth <= 0)
        {
            Engineson.print("This man is dead man.");
            //Destroy(gameObject);
        }
        //Engineson.print("Player hit!");
    }

    public override void Attack()
    {

    }

    private void UnburrowingAttack()
    {
        if (playerTransform != null)
        {
            Engineson.print("Unburrowing Attack");
            enemyTransform.position = playerTransform.position;
            collider.SetPosition(playerTransform.position);
        }
        attackCount++;
        isBuried = false;

        // Hurtbox i tot a la pesca
    }

    private void UnburrowingAttackPhase2()
    {
        if (playerTransform != null)
        {
            enemyTransform.position = fixedPositions[FindClosestFixedPosition()];
            collider.SetPosition(enemyTransform.position);
            Engineson.print("Unburrowing Attack ");
        }
    }

    public void ChangePositionToClosest()
    {
        enemyTransform.position = fixedPositions[FindClosestFixedPosition()];
        Burrow();
    }

    private int FindClosestFixedPosition()
    {
        int closestIndex = 0;
        float closestDistance = float.MaxValue;

        for (int i = 0; i < fixedPositions.Length; i++)
        {
            float distance = Vector3.Distance(playerTransform.position, fixedPositions[i]);
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestIndex = i;
            }
        }
        return closestIndex;
    }

    private void SlamAttack()
    {
        Vector3 hurtboxSize = new Vector3(3.0f, 2.0f, 5.0f);
        Vector3 hurtbOffset = enemyTransform.forward * 3.0f;
        Vector3 hurtboxPosition = enemyTransform.position + hurtboxOffset;

        GameObject hurtboxObject = Engineson.CreateGameObject("SlamHurtbox", null);
        hurtboxObject.AddComponent<MeshRenderer>();
        var hurtboxTransform = hurtboxObject.AddComponent<Transform>();
        hurtboxTransform.position = hurtboxPosition;
        hurtboxTransform.SetScale(hurtboxSize.X, hurtboxSize.Y, hurtboxSize.Z);
        var hurtboxCollider = hurtboxObject.AddComponent<BoxCollider>();
    }

    private void Burrow()
    {
        Engineson.print("Burrowed");
        enemyTransform.position = new Vector3(0.0f, -100.0f, 0.0f);
        isBuried = true;
    }

    private void Die()
    {
        enemyTransform.position = new Vector3(0.0f, -100.0f, 0.0f);
        collider.SetPosition(enemyTransform.position);
    }
}