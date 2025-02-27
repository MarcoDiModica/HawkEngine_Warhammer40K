using System.Numerics;
using HawkEngine;
using System.Collections.Generic;
using System;

public class PlayerController : MonoBehaviour
{
    #region Movement Settings
    public float moveSpeed = 9.0f;
    public float rotationSpeed = 14.0f;
    public float acceleration = 20.0f;
    public float deceleration = 15.0f;

    public float dashSpeed = 45.0f;
    public float dashDuration = 0.15f;
    public float dashCooldown = 1.0f;
    public int maxDashCharges = 2;

    public float projectileSpeed = 20.0f;
    public float projectileLifetime = 1.0f;
    public float shootCooldown = 0.2f;
    #endregion

    #region Private Classes
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
    #endregion

    #region Private Variables
    // Components
    private Transform transform;

    // Movement
    private Vector3 currentVelocity;
    private Vector3 targetVelocity;
    private float currentRotationAngle;

    // Shooting
    private float shootTimer = 0f;
    private List<ProjectileInfo> activeProjectiles = new List<ProjectileInfo>();

    // Dash state
    private bool isDashing;
    private float currentDashTime;
    private float dashCooldownTimer;
    private Vector3 dashDirection;
    private int currentDashCharges;
    private float lastDashRechargeTime;
    private const float DASH_RECHARGE_RATE = 2.0f;

    //Collider
    private Collider collider;
    #endregion

    #region Lifecycle
    public override void Start()
    {
        InitializeComponents();
        ResetDashCharges();
    }

    public override void Update(float deltaTime)
    {
        Vector3 moveDirection = GetInputDirection();

        if (!isDashing)
        {
            UpdateMovement(moveDirection, deltaTime);
            UpdateRotation(moveDirection, deltaTime);

            if (Input.GetKey(KeyCode.SPACE) && CanDash() && moveDirection != Vector3.Zero)
            {
                InitiateDash(moveDirection);
            }

            UpdateShooting(deltaTime);
        }
        else
        {
            HandleActiveDash(deltaTime);
        }

        UpdateDashRecharge(deltaTime);
        UpdateProjectiles(deltaTime);
        CleanupProjectiles();

        //Engineson.print("Is Trigger:" + collider.IsTrigger());
    }
    #endregion

    #region Initialization
    private void InitializeComponents()
    {
        transform = gameObject.GetComponent<Transform>();
        if (transform == null)
        {
            Engineson.print("ERROR: PlayerController requires a Transform component!");
            return;
        }

        currentVelocity = Vector3.Zero;
        targetVelocity = Vector3.Zero;
        currentRotationAngle = transform.eulerAngles.Y;

        collider = gameObject.GetComponent<Collider>();
        if (collider == null)
        {
            Engineson.print("ERROR: PlayerController requires a Collider component!");
            return;
        }
    }

    private void ResetDashCharges()
    {
        currentDashCharges = maxDashCharges;
        dashCooldownTimer = 0;
        lastDashRechargeTime = 0;
    }
    #endregion

    #region Movement System
    private Vector3 GetInputDirection()
    {
        Vector3 direction = Vector3.Zero;

        if (Input.GetKey(KeyCode.W)) direction += Vector3.UnitZ;
        if (Input.GetKey(KeyCode.S)) direction -= Vector3.UnitZ;
        if (Input.GetKey(KeyCode.D)) direction += Vector3.UnitX;
        if (Input.GetKey(KeyCode.A)) direction -= Vector3.UnitX;

        return direction != Vector3.Zero ? Vector3.Normalize(direction) : direction;
    }

    private void UpdateMovement(Vector3 moveDirection, float deltaTime)
    {
        targetVelocity = moveDirection * moveSpeed;

        float smoothFactor = moveDirection != Vector3.Zero ? acceleration : deceleration;
        currentVelocity = Vector3.Lerp(currentVelocity, targetVelocity, smoothFactor * deltaTime);

        if (currentVelocity.LengthSquared() > 0.01f)
        {
            transform.position += currentVelocity * deltaTime;
        }
    }

    private void UpdateRotation(Vector3 moveDirection, float deltaTime)
    {
        if (moveDirection != Vector3.Zero)
        {
            float targetAngle = (float)Math.Atan2(moveDirection.X, moveDirection.Z);

            float targetAngleDegrees = targetAngle * (180.0f / (float)Math.PI);

            while (targetAngleDegrees - currentRotationAngle > 180.0f) targetAngleDegrees -= 360.0f;
            while (targetAngleDegrees - currentRotationAngle < -180.0f) targetAngleDegrees += 360.0f;

            currentRotationAngle = Lerp(currentRotationAngle, targetAngleDegrees, rotationSpeed * deltaTime);

            float angleRadians = currentRotationAngle * ((float)Math.PI / 180.0f);
            transform.SetRotation(0, angleRadians, 0);
        }
    }
    #endregion

    #region Dash System
    private bool CanDash()
    {
        return !isDashing && currentDashCharges > 0;
    }

    private void UpdateDashRecharge(float deltaTime)
    {
        if (currentDashCharges < maxDashCharges)
        {
            lastDashRechargeTime += deltaTime;
            if (lastDashRechargeTime >= DASH_RECHARGE_RATE)
            {
                currentDashCharges++;
                lastDashRechargeTime = 0;
            }
        }
    }

    private void InitiateDash(Vector3 direction)
    {
        isDashing = true;
        currentDashTime = dashDuration;
        dashDirection = direction;
        currentDashCharges--;
    }

    private void HandleActiveDash(float deltaTime)
    {
        if (currentDashTime > 0)
        {
            transform.position += dashDirection * dashSpeed * deltaTime;
            currentDashTime -= deltaTime;
        }
        else
        {
            isDashing = false;
            currentVelocity = dashDirection * moveSpeed;
        }
    }
    #endregion

    #region Shooting System
    private void UpdateShooting(float deltaTime)
    {
        shootTimer -= deltaTime;

        if (Input.GetKey(KeyCode.J) && shootTimer <= 0)
        {
            Shoot();
            shootTimer = shootCooldown;
        }
    }

    private void Shoot()
    {
        try
        {
            GameObject projectile = Engineson.CreateGameObject("Projectile", null);

            // TODO: add mesh to the projectile

            if (projectile != null)
            {
                Transform projTransform = projectile.GetComponent<Transform>();
                if (projTransform != null)
                {
                    Vector3 forward = transform.forward;
                    Vector3 spawnPos = transform.position + forward * 1.0f;
                    projTransform.position = spawnPos;
                    projTransform.SetScale(0.3f, 0.3f, 0.3f);

                    ProjectileInfo projInfo = new ProjectileInfo(projectile, projTransform, forward);
                    activeProjectiles.Add(projInfo);

                    Engineson.print("Projectile fired!");
                }
            }
        }
        catch (System.Exception e)
        {
            Engineson.print($"Error creating projectile: {e.Message}");
        }
    }

    private void UpdateProjectiles(float deltaTime)
    {
        foreach (var proj in activeProjectiles)
        {
            if (proj.markedForDestruction) continue;

            proj.lifetime += deltaTime;

            if (proj.lifetime >= projectileLifetime)
            {
                proj.markedForDestruction = true;
                continue;
            }

            try
            {
                if (proj.transform != null)
                {
                    proj.transform.position += proj.direction * projectileSpeed * deltaTime;
                }
            }
            catch (System.Exception e)
            {
                proj.markedForDestruction = true;
                Engineson.print($"Error updating projectile: {e.Message}");
            }
        }
    }

    private void CleanupProjectiles()
    {
        foreach (var proj in activeProjectiles)
        {
            if (proj.markedForDestruction)
            {
                try
                {
                    var index = activeProjectiles.IndexOf(proj);
                    Engineson.Destroy(proj.gameObject);
                    activeProjectiles.RemoveAt(index);
                }
                catch (System.Exception e)
                {
                    Engineson.print($"Error destroying projectile: {e.Message}");
                    activeProjectiles.Remove(proj);
                }
            }
        }
    }
    #endregion

    #region Utilities
    private float Lerp(float start, float end, float t)
    {
        return start + (end - start) * Math.Min(1, Math.Max(0, t));
    }
    #endregion
}