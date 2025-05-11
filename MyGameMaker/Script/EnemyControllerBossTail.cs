using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net;
using System.Numerics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using HawkEngine;

public class EnemyControllerBossTail : EnemyController
{
    private float bossTime = 0.0f;
    private float stateStartTime = 0.0f;
    bool isBuried = false;
    bool isActive = false;

    private float emergeDuration = 2.0f;
    private float waitDuration = 2.0f;
    private float attackWarningDuration = 0.5f;
    private float shiftDuration = 5.0f;
    private float actionTimer = 0.0f;

    bool waitingToEmerge = false;
    float emergeTimer = 0f;

    private Vector3[] fixedPositions = new Vector3[]
    {
        new Vector3(-108.0f, 10.0f, -32.0f),
        new Vector3(-108.0f, 10.0f, 29.0f),
        new Vector3(-41.0f, 10.0f, -32.0f),
        new Vector3(-41.0f, 10.0f, 29.0f)
    };
    private int currentPositionIndex = -1;

    private Vector3 playerVelocity;

    private List<(GameObject, float)> activeHurtboxes = new List<(GameObject, float)>();

    private MawlocTailAnimation anim;

    public override void Awake()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        playerVelocity = GameObject.Find("Player").GetComponent<Rigidbody>().GetVelocity();
        if (playerTransform == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }
        rb = gameObject.GetComponent<Rigidbody>();
        rb.SetMass(10000.0f);
        collider = gameObject.GetComponent<BoxCollider>();
        collider.SetSize(new Vector3(2.0f, 2.0f, 2.0f));
        if (collider == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Collider component!");
            return;
        }
        anim = gameObject.GetChild("MawlocTailMesh").GetComponent<MawlocTailAnimation>();
        if (anim == null)
        {
            Engineson.print("ERROR: Mawloc animation requires a script component!");
            return;
        }
        //sound = gameObject.GetComponent<Audio>();
        //if (sound == null)
        //{
        //    Engineson.print("PlayerShooting: Audio component not found");
        //}
        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }
        gameObject.tag = "Boss";

    }

    public override void Start()
    {
        //enemyTransform.position = fixedPositions[currentPositionIndex];
    }

    public override void Update(float deltaTime)
    {
        if (!isDead)
        {
            actionTimer -= deltaTime;
            UpdateHurtboxes(deltaTime);

            float distanceToPlayer = Vector3.Distance(enemyTransform.position, playerTransform.position);

            if (playerTransform != null)
            {
                Vector3 directionToPlayer = Vector3.Normalize(playerTransform.position - enemyTransform.position);
                float targetAngle = (float)Math.Atan2(directionToPlayer.X, directionToPlayer.Z) * (180.0f / (float)Math.PI);
                Quaternion newRotation = Quaternion.CreateFromYawPitchRoll(targetAngle * ((float)Math.PI / 180.0f), 0, 0);
                enemyTransform.SetRotationQuat(newRotation);
                collider.SetRotation(newRotation);
            }

            if (waitingToEmerge)
            {
                emergeTimer -= deltaTime;
                if (emergeTimer <= 0f)
                {
                    currentPositionIndex = FindClosestFixedPosition();
                    Emerge();
                    waitingToEmerge = false;
                }
                return;
            }


            if (actionTimer <= 0f)
            {
                if (distanceToPlayer <= 15f)
                {
                    TailSlash();
                    actionTimer = 2.0f;
                }
                else if (distanceToPlayer <= 25f)
                {
                    TailStab();
                    actionTimer = 2.0f;
                }
                else
                {
                    Burrow();
                    actionTimer = 5.0f;
                    waitingToEmerge = true;
                    emergeTimer = 2.0f;
                }
            }
        }
        
        if (isDead)
        {
            collider.SetActive(false);
            //sound?.Stop();
        }
    }

    public override void ResetEnemyCheckPoint()
    {
       
    }

    public void ChangePositionToClosest()
    {
        if (isDead == false)
        {
            Burrow();
            enemyTransform.position = fixedPositions[FindClosestFixedPosition()];
            collider.SetPosition(enemyTransform.position);
            actionTimer = shiftDuration;
        }
    }

    private void Burrow()
    {
        if (isDead == false)
        {
            Engineson.print("Burrowed");
            enemyTransform.position = new Vector3(0.0f, -40.0f, 0.0f);
            collider.SetPosition(enemyTransform.position);
            anim.SetBurrowingAnimation();
            isBuried = true;
        }
    }

    private void Emerge()
    {
        if (isDead == false)
        {
            if (playerTransform != null)
            {
                enemyTransform.position = fixedPositions[FindClosestFixedPosition()];
                collider.SetPosition(enemyTransform.position);
                anim.SetUnburrowingAnimation();
                isBuried = false;
            }
        }
    }

    private int FindClosestFixedPosition()
    {
        int closestIndex = 0;
        float closestDistance = float.MaxValue;

        if (isDead == false)
        {
            for (int i = 0; i < fixedPositions.Length; i++)
            {
                float distance = Vector3.Distance(playerTransform.position, fixedPositions[i]);
                if (distance < closestDistance)
                {
                    closestDistance = distance;
                    closestIndex = i;
                }
            }
        }

        return closestIndex;
    }

    private void TailSlash()
    {
        if (isDead == false)
        {
            actionTimer = 1.5f;
            anim.SetSlashAnimation();
            CreateTailSlashHurtbox();
        }
    }

    private void TailStab()
    {
        if (isDead == false)
        {
            actionTimer = 1.5f;
            anim.SetStabAnimation();
            CreateTailStabHurtbox();
        }
    }

    public override void Attack()
    {

    }

    public override void TakeDamage(float damage)
    {

    }

    private void CreateTailSlashHurtbox()
    {
        var hurtbox = Engineson.CreateGameObject("TailSlashHurtbox", null);
        hurtbox.AddComponent<MeshRenderer>();
        hurtbox.AddComponent<BoxCollider>();
        hurtbox.GetComponent<BoxCollider>().SetTrigger(true);
        hurtbox.tag = "EnemyAttack";

        Vector3 direction = enemyTransform.forward;
        float range = 10f;
        float width = 2.5f;
        float height = 2f;
        float angle = (float)Math.Atan2(direction.X, direction.Z);
        Quaternion rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle);

        Vector3 position = enemyTransform.position + direction * (range / 2f);
    
        var hurtboxTransform = hurtbox.GetComponent<Transform>();
        hurtboxTransform.position = position;
        hurtboxTransform.SetScale(width, height, range);
        hurtboxTransform.SetRotationQuat(rotation);

        activeHurtboxes.Add((hurtbox, 1.0f));
    }

    private void CreateTailStabHurtbox()
    {
        var hurtbox = Engineson.CreateGameObject("TailStabHurtbox", null);
        hurtbox.AddComponent<MeshRenderer>();
        hurtbox.AddComponent<BoxCollider>();
        hurtbox.GetComponent<BoxCollider>().SetTrigger(true);
        hurtbox.tag = "EnemyAttack";

        float ti = 0.0f;
        Vector3 predictedPosition = playerTransform.position + playerVelocity * ti;

        Vector3 direction = Vector3.Normalize(predictedPosition - enemyTransform.position);
        float angle = (float)Math.Atan2(direction.X, direction.Z);
        Quaternion rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle);

        float length = 20f;
        float width = 1f;
        Vector3 position = enemyTransform.position + direction * (length / 2f);

        var hurtboxTransform = hurtbox.GetComponent<Transform>();
        hurtboxTransform.position = position;
        hurtboxTransform.SetScale(width, 2f, length);
        hurtboxTransform.SetRotationQuat(rotation);

        activeHurtboxes.Add((hurtbox, 0.8f));
    }

    private void UpdateHurtboxes(float deltaTime)
    {
        for (int i = activeHurtboxes.Count - 1; i >= 0; i--)
        {
            var (hurtbox, timer) = activeHurtboxes[i];
            timer -= deltaTime;
            if (timer <= 0f)
            {
                Engineson.Destroy(hurtbox);
                activeHurtboxes.RemoveAt(i);
            }
            else
            {
                activeHurtboxes[i] = (hurtbox, timer);
            }
        }
    }

    public void Die()
    {
        anim.SetDeathAnimation();
        Engineson.Destroy(GetGameObject());
    }

    public void Activate()
    {
        if (isActive == false)
        {
            GetGameObject().SetActive(true);
            isActive = true;
        }

    }

}