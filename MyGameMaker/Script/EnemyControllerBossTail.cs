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
    private float hurtboxDuration = 1.0f;
    private float bossTime = 0.0f;
    private float stateStartTime = 0.0f;
    bool isBuried = false;
    bool isActive = false;

    private float emergeDuration = 2.0f;
    private float waitDuration = 2.0f;
    private float attackWarningDuration = 0.5f;
    private float shiftDuration = 5.0f;
    private float actionTimer = 0.0f;

    private float stabDamage = 10.0f;
    private float slashDamage = 10.0f;
    private string hitStab = "Assets/Audio/Mawloc_Tail_Stab_2.wav";
    private string hitSlash = "Assets/Audio/Mawloc_Tail_Slash.wav";
    private const string BurrowClip = "Assets/Audio/Mawloc_Underground_move.wav";
    private const string UnburrowClip = "Assets/Audio/Mawloc_Underground_Attack.wav";
    private const string MoveUndergroundClip = "Assets/Audio/Mawloc_Rumble_Underground.wav";

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

    private List<(GameObject, float, float)> activeHurtboxes = new List<(GameObject, float, float)>();

    private MawlocTailAnimation anim;
    private PlayerController pc;

    private GameObject slashHurtbox;
    private GameObject stabHurtbox;
    private BoxCollider boxcollider;

    public override void Awake()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        pc = GameObject.Find("Player").GetComponent<PlayerController>();
        if (playerTransform == null)
        {
            Engineson.print("ERROR: Player couldn't be found!");
        }
        rb = gameObject.GetComponent<Rigidbody>();
        //rb.SetMass(10000.0f);
        //collider = gameObject.GetComponent<CapsuleCollider>();
        //collider.SetSize(new Vector3(2.0f, 2.0f, 2.0f));
        boxcollider = gameObject.GetComponent<BoxCollider>();
        //if (collider == null)
        //{
        //    Engineson.print("ERROR: PlayerMovement requires a Collider component!");
        //    return;
        //}
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
        if (SceneManager.isPaused)
            return;

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
                boxcollider.SetRotation(newRotation);
            }

            if (waitingToEmerge)
            {
                emergeTimer -= deltaTime;
                if (emergeTimer <= 0f)
                {
                    currentPositionIndex = FindClosestFixedPosition();
                    Emerge();
                    waitingToEmerge = false;
                    emergeTimer = 2.0f;
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
            boxcollider.SetActive(false);
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
            boxcollider.SetPosition(enemyTransform.position);
            actionTimer = shiftDuration;
        }
    }

    private void Burrow()
    {
        if (isDead == false)
        {
            Audio.PlayOneShot(BurrowClip);
            Engineson.print("Burrowed");
            enemyTransform.position = new Vector3(0.0f, -40.0f, 0.0f);
            boxcollider.SetPosition(enemyTransform.position);
            anim.SetBurrowingAnimation();
            AddComponent<ParticleFX>().ApplyPreset(25);
            GetComponent<ParticleFX>().EmitBurst(25);
            isBuried = true;
        }
    }

    private void Emerge()
    {
        if (isDead == false)
        {
            Audio.PlayOneShot(UnburrowClip);
            if (playerTransform != null)
            {
                enemyTransform.position = fixedPositions[FindClosestFixedPosition()];
                boxcollider.SetPosition(enemyTransform.position);
                anim.SetUnburrowingAnimation();
                AddComponent<ParticleFX>().ApplyPreset(25);
                GetComponent<ParticleFX>().EmitBurst(25);
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
            Audio.PlayOneShot(MoveUndergroundClip);
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
            Audio.PlayOneShot(hitSlash);
            actionTimer = 1.5f;
            anim.SetSlashAnimation();
            // note: pass in your desired width/height/length and duration
            CreateHurtbox("TailSlashHurtbox",
                          width: 5f, height: 2f, length: 10f,
                          direction: enemyTransform.forward,
                          damage: slashDamage,
                          duration: hurtboxDuration,
                          forwardOffset: 0f);
        }
    }

    private void TailStab()
    {
        if (isDead == false)
        {
            Audio.PlayOneShot(hitStab);
            actionTimer = 1.5f;
            anim.SetStabAnimation();
            Vector3 dir = Vector3.Normalize(playerTransform.position - enemyTransform.position);
            CreateHurtbox("TailStabHurtbox",
                          width: 3f, height: 2f, length: 20f,
                          direction: dir,
                          damage: stabDamage,
                          duration: hurtboxDuration,
                          forwardOffset: 0f);
        }
    }

    public override void Attack()
    {

    }

    public override void TakeDamage(float damage)
    {

    }

    private bool IsPlayerInCollider(GameObject hurtbox, Vector3 playerPos)
    {
        var bc = hurtbox.GetComponent<BoxCollider>();
        Vector3 halfSize = bc.GetSize() * 0.5f;
        Vector3 center = hurtbox.GetComponent<Transform>().position;

        return (playerPos.X >= center.X - halfSize.X && playerPos.X <= center.X + halfSize.X) &&
               (playerPos.Y >= center.Y - halfSize.Y && playerPos.Y <= center.Y + halfSize.Y) &&
               (playerPos.Z >= center.Z - halfSize.Z && playerPos.Z <= center.Z + halfSize.Z);
    }


    private void ApplyBossTailDamage(float amount)
    {
        if (pc.redThirstManager.redThirstBonus < amount)
        {
            pc.playerData.TakeDamage(amount - pc.redThirstManager.redThirstBonus);
            pc.StartFlashColor(pc.flashColor, pc.flashDuration);
        }
        else
        {
            pc.playerData.TakeDamage(0.0f);
        }
    }

    private void CheckBossTailHurtboxes()
    {
        if (pc == null || pc.playerData == null) return;

        Vector3 playerPos = playerTransform.position;
        Engineson.print("" + playerPos);
        if (slashHurtbox != null && IsPlayerInCollider(slashHurtbox, playerPos))
        {
            ApplyBossTailDamage(slashDamage);
        }

        if (stabHurtbox != null && IsPlayerInCollider(stabHurtbox, playerPos))
        {
            ApplyBossTailDamage(stabDamage);
        }
    }
    private void CreateHurtbox(
        string name,
        float width, float height, float length,
        Vector3 direction,
        float damage,
        float duration,
        float forwardOffset = 0f
    )
    {
        var hb = Engineson.CreateGameObject(name, null);
        var bc = hb.AddComponent<BoxCollider>();
        bc.SetTrigger(true);
        hb.tag = "EnemyAttack";

        const float scaleFactor = 4f;
        Vector3 scaledSize = new Vector3(
            width * scaleFactor,
            height * scaleFactor,
            length * scaleFactor
        );
        bc.SetSize(scaledSize);

        var t = hb.GetComponent<Transform>();
        t.SetScale(scaledSize.X, scaledSize.Y, scaledSize.Z);

        float angle = (float)Math.Atan2(direction.X, direction.Z);
        t.SetRotationQuat(Quaternion.CreateFromAxisAngle(Vector3.UnitY, angle));

        float baseDistance = length / 2f;
        Vector3 spawnPos = enemyTransform.position
                         + direction * (baseDistance + forwardOffset);
        spawnPos.Y = playerTransform.position.Y;
        t.position = spawnPos;

        activeHurtboxes.Add((hb, damage, duration));
        Engineson.print(
          $"[HURTBOX x4] {name} center={spawnPos} " +
          $"halfSize=<{scaledSize.X / 2},{scaledSize.Y / 2},{scaledSize.Z / 2}>"
        );
    }


    private void UpdateHurtboxes(float deltaTime)
    {
        for (int i = activeHurtboxes.Count - 1; i >= 0; i--)
        {
            var (hb, dmg, timer) = activeHurtboxes[i];
            timer -= deltaTime;
            if (timer <= 0f)
            {
                if (IsPlayerInCollider(hb, playerTransform.position))
                {
                    Engineson.print("We're in");
                    ApplyBossTailDamage(dmg);
                }
                Engineson.Destroy(hb);
                activeHurtboxes.RemoveAt(i);
            }
            else
            {
                activeHurtboxes[i] = (hb, dmg, timer);
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