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
    private enum TailState { Emerging, Waiting, Attacking, Shifting }
    private TailState currentState = TailState.Emerging;

    private float bossTime = 0.0f;
    private float stateStartTime = 0.0f;
    bool isBuried = false;

    private float emergeDuration = 2.0f;
    private float waitDuration = 2.0f;
    private float attackWarningDuration = 0.5f;
    private float shiftDuration = 5.0f;
    private float actionTimer = 0.0f;

    private Vector3[] fixedPositions = new Vector3[]
    {
        new Vector3(10, 0, -10),
        new Vector3(-10, 0, -10),
        new Vector3(10, 0, 10),
        new Vector3(-10, 0, 10)
    };
    private int currentPositionIndex = -1;

    private bool waitingForTorsoShift = false;
    private EnemyControllerBoss torsoController;
    private Transform torsoTransform;
    private Vector3 playerVelocity;

    private List<(GameObject, float)> activeHurtboxes = new List<(GameObject, float)>();

    public override void Awake()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        playerVelocity = GameObject.Find("Player").GetComponent<Rigidbody>().GetVelocity();
        rb = gameObject.GetComponent<Rigidbody>();
        rb.SetMass(10000.0f);
        collider = gameObject.GetComponent<BoxCollider>();
        sound = gameObject.GetComponent<Audio>();
        enemyTransform = gameObject.GetComponent<Transform>();
        torsoController = GameObject.Find("Mawloc").GetComponent<EnemyControllerBoss>();
        torsoTransform = GameObject.Find("Mawloc").transform;
    }

    public override void Start()
    {
        currentState = TailState.Waiting;

        //enemyTransform.position = fixedPositions[currentPositionIndex];
    }

    public override void Update(float deltaTime)
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

        if (currentState == TailState.Waiting)
        {
            if (distanceToPlayer <= 15f)
            {
                TailSlash();
            }
            else if (distanceToPlayer <= 30f)
            {
                TailStab();
            }
            else
            {
                ChangePositionToClosest();
            }
        }
        else if (currentState == TailState.Attacking)
        {
            if (actionTimer <= 0f)
            {
                currentState = TailState.Waiting;

            }

        }
    }


    private bool ShouldTailSlash()
    {
        if (playerTransform == null) return false;
        float dist = Vector3.Distance(enemyTransform.position, playerTransform.position);
        return dist <= 5.0f;
    }

    private bool ShouldTailStab()
    {
        if (playerTransform == null) return false;
        float dist = Vector3.Distance(enemyTransform.position, playerTransform.position);
        return dist > 5.0f && dist <= 8.0f;
    }

    public void ChangePositionToClosest()
    {
        if (isDead == false)
        {
            enemyTransform.position = fixedPositions[FindClosestFixedPosition()];
            collider.SetPosition(enemyTransform.position);
            Burrow();
        }
    }

    private void Burrow()
    {
        if (isDead == false)
        {
            Engineson.print("Burrowed");
            enemyTransform.position = new Vector3(0.0f, -40.0f, 0.0f);
            collider.SetPosition(enemyTransform.position);
            isBuried = true;
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
        currentState = TailState.Attacking;
        actionTimer = 1.5f;
        CreateTailSlashHurtbox();
    }

    private void TailStab()
    {
        currentState = TailState.Attacking;
        actionTimer = 1.5f;
        CreateTailStabHurtbox();
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
        float range = 5f;
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

        float length = 8f;
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

    public void Disable()
    {

    }

}