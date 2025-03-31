using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net;
using System.Numerics;
using HawkEngine;

public class EnemyControllerBoss : EnemyController
{
    private float hurtboxActivationTime = 1.5f; // Tiempo que el jugador debe estar en la hurtbox para activarla
    private float hurtboxTimer = 0f;
    private Vector3 hurtboxSize = new Vector3(2.0f, 2.0f, 2.0f); // Tamaño de la hurtbox
    private Vector3 hurtboxOffset = new Vector3(4.0f, 2.0f, 0.0f); // Desplazamiento de la hurtbox hacia adelante
    private GameObject hurtboxObject;
    private bool dodgewindow = false;
    private float dodgeActivationTime = 0.5f;
    private float dodgeTimer = 0f;
    private List<ProjectileInfo> activeProjectiles = new List<ProjectileInfo>();
    public float shootCooldown = 2.0f;
    public float projectileSpeed = 90.0f;
    public float projectileLifetime = 0.5f;
    protected float shootTimer = 0f;

    //stats
    private float health = 1500.0f;
    private float damage = 50.0f;

    private enum BossPhase
    {
        PHASE1,
        PHASE2,
        PHASE3
    }

    private BossPhase currentPhase;

    private class ProjectileInfo
    {
        public GameObject gameObject;
        public Transform transform;
        public float lifetime;
        public Vector3 direction;
        public bool markedForDestruction;

        public ProjectileInfo(GameObject obj, Transform trans, Vector3 dir)
        {
            gameObject = obj;
            transform = trans;
            direction = dir;
            lifetime = 0f;
            markedForDestruction = false;
        }
    }
    public override void Start()
    {
        playerTransform = GameObject.Find("Player").GetComponent<Transform>();
        rb = gameObject.GetComponent<Rigidbody>();
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
        soundAttack = gameObject.GetComponent<Audio>();
        if (soundAttack == null)
        {
            Engineson.print("PlayerShooting: Audio component not found");
        }
        enemyTransform = gameObject.GetComponent<Transform>();
        if (enemyTransform == null)
        {
            Engineson.print("ERROR: PlayerMovement requires a Transform component!");
            return;
        }
        currentPhase = BossPhase.PHASE1;
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
    }

    public override void Attack()
    {

    }
}