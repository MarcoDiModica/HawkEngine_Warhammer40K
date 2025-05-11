using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public enum EnemyState { IDLE, CHASE, ATTACK, STUNNED, DEAD, LEAP };

public abstract class EnemyController : MonoBehaviour, IEnemyController
{
    // Variables comunes
    protected Transform playerTransform;
    protected Rigidbody rb;
    protected BoxCollider collider;
    protected Transform enemyTransform;
    //protected AudioSource sound;
    protected ParticleFX particles;
    protected EnemyState currentState = EnemyState.IDLE;
    protected MeshRenderer renderer;
    protected Vector4 originalColor = new Vector4(1 ,1 ,1 ,1);
    protected bool isFlashingColor = false;
    protected float flashDuration = 0.1f;
    protected float flashTimer = 0.0f;
    protected Vector4 flashColor = new Vector4(1, 0, 0, 1); // rojo

    public float currentHealth;
    public float maxHealth;
    public bool isStunned = false;
    protected float stunDuration = 2.0f;
    protected float stunTimer = 0.0f;
    public bool isDead = false;
    protected bool isAttacking = false;
    protected bool isShooting = false;
    protected bool isFootstepPlaying = false;
    protected bool hasStoppedFootsteps = false;

    public float distToChase = 50.0f;
    public float minDistToChase = 10.0f;
    public float speedMovement = 25.0f;
    public float acceleration = 40.0f;
    public float rotationSpeed = 300.0f;
    protected Vector3 moveDirection;
    protected float currentRotationAngle;

    public Vector3 startPosition;

    protected Pathfinding pathfinder;
    protected List<Vector3> chasePath;
    protected int chaseIndex;
    protected bool pathInitialized;

    protected const int gridWidth = 500, gridHeight = 500;
    protected const float cellSize = 1f;
    public abstract void Start();
    public abstract void Update(float deltaTime);
    public abstract void ResetEnemyCheckPoint();
    public abstract void Attack();
    public abstract void TakeDamage(float damage);
    public float Lerp(float start, float end, float t)
    {
        return start + (end - start) * Math.Min(1, Math.Max(0, t));
    }
}