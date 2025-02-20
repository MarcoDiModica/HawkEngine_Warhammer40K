﻿using System.Numerics;
using HawkEngine;
using System.Collections.Generic;
using System;

public class PlayerController : MonoBehaviour
{
    #region Movement Settings
    public float moveSpeed = 5.0f;
    public float rotationSpeed = 8.0f;
    public float acceleration = 20.0f;
    public float deceleration = 15.0f;
    #endregion

    #region Dash Settings
    public float dashSpeed = 15.0f;
    public float dashDuration = 0.2f;
    public float dashCooldown = 1.0f;
    public int maxDashCharges = 2;
    #endregion

    #region Projectile Settings
    public float projectileSpeed = 20.0f;
    public float projectileLifetime = 1.0f;
    public float shootCooldown = 0.2f;
    #endregion

    #region Private Variables
    private Transform transform;
    private Vector3 currentVelocity;
    private Vector3 targetVelocity;
    private float currentRotationAngle;
    private float shootTimer = 0f;

    private List<(GameObject obj, float lifetime)> activeProjectiles = new List<(GameObject obj, float lifetime)>();

    private bool isDashing;
    private float currentDashTime;
    private float dashCooldownTimer;
    private Vector3 dashDirection;
    private int currentDashCharges;
    private float lastDashRechargeTime;
    private const float DASH_RECHARGE_RATE = 2.0f;
    #endregion

    public override void Start()
    {
        InitializeComponents();
        ResetDashCharges();
    }

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
    }

    private void ResetDashCharges()
    {
        currentDashCharges = maxDashCharges;
        dashCooldownTimer = 0;
        lastDashRechargeTime = 0;
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
    }

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
        GameObject projectile = Engineson.CreateGameObject("Projectile", null);
        if (projectile != null)
        {
            Transform projTransform = projectile.GetComponent<Transform>();
            if (projTransform != null)
            {
                Vector3 forward = transform.forward;
                Vector3 spawnPos = transform.position + forward * 1.0f;
                projTransform.position = spawnPos;

                projTransform.SetScale(0.3f, 0.3f, 0.3f);

                activeProjectiles.Add((projectile, projectileLifetime));

                Engineson.print("Projectile fired!");
            }
        }
    }

    private void UpdateProjectiles(float deltaTime)
    {
        for (int i = activeProjectiles.Count - 1; i >= 0; i--)
        {
            var (projectile, lifetime) = activeProjectiles[i];

            Transform projTransform = projectile.GetComponent<Transform>();
            if (projTransform != null)
            {
                projTransform.position += transform.forward * projectileSpeed * deltaTime;
            }

            float newLifetime = lifetime - deltaTime;
            if (newLifetime <= 0)
            {
                Engineson.Destroy(projectile);
                activeProjectiles.RemoveAt(i);
                Engineson.print("Projectile destroyed!");
            }
            else
            {
                activeProjectiles[i] = (projectile, newLifetime);
            }
        }
    }

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

    private float Lerp(float start, float end, float t)
    {
        return start + (end - start) * Math.Min(1, Math.Max(0, t));
    }
}