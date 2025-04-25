using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public enum EnemyState { IDLE, CHASE, ATTACK, STUNNED, DEAD };

public abstract class EnemyController : MonoBehaviour, IEnemyController
{
    // Variables comunes
    protected Transform playerTransform;
    protected Rigidbody rb;
    protected BoxCollider collider;
    protected Transform enemyTransform;
    protected Audio sound;
    protected ParticleFX particles;
    protected EnemyState currentState = EnemyState.IDLE;

    public float currentHealth;
    public float maxHealth;
    public bool isStunned = false;
    protected float stunDuration = 2.0f;
    protected float stunTimer = 0.0f;
    protected bool isDead = false;
    protected bool isAttacking = false;
    protected bool isShooting = false;
    protected bool isFootstepPlaying = false;
    protected bool hasStoppedFootsteps = false;

    public float distToChase = 50.0f;
    public float minDistToChase = 10.0f;
    public float speedMovement = 10.0f;
    public float acceleration = 15.0f;
    public float rotationSpeed = 300.0f;
    protected Vector3 moveDirection;
    protected float currentRotationAngle;

    public abstract void Start();
    public abstract void Update(float deltaTime);
    public abstract void Attack();
    public abstract void TakeDamage(float damage);
    public float Lerp(float start, float end, float t)
    {
        return start + (end - start) * Math.Min(1, Math.Max(0, t));
    }
}