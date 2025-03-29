using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public abstract class EnemyController : MonoBehaviour, IEnemyController
{
    // Variables comunes
    protected Transform playerTransform;
    protected Rigidbody rb;
    protected BoxCollider collider;
    protected Transform enemyTransform;
    protected Audio soundAttack;

    public float distToChase = 20.0f;
    public float minDistToChase = 5.0f;
    public float speedMovement = 5.0f;
    public float acceleration = 15.0f;
    public float rotationSpeed = 300.0f;
    protected Vector3 moveDirection;
    protected float currentRotationAngle;

    public abstract void Start();
    public abstract void Update(float deltaTime);
    public abstract void Attack();
    public float Lerp(float start, float end, float t)
    {
        return start + (end - start) * Math.Min(1, Math.Max(0, t));
    }
}