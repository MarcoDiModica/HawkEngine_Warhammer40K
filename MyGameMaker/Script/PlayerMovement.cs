using HawkEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

public class PlayerMovement
{
    private Rigidbody rb;
    private Collider collider;
    private float moveSpeed = 9.0f;
    private float rotationSpeed = 14.0f;
    private float acceleration = 20.0f;
    private float deceleration = 15.0f;
    private float currentRotationAngle;

    public PlayerMovement(GameObject player)
    {
        rb = player.GetComponent<Rigidbody>();
        rb.SetMass(1.0f);
        collider = player.GetComponent<Collider>();
    }

    public void UpdateMovement(Vector3 moveDirection, float deltaTime)
    {
        Vector3 currentVelocity = rb.GetVelocity();
        Vector3 desiredVelocity = moveDirection * moveSpeed;

        if (desiredVelocity.LengthSquared() > 0)
        {
            desiredVelocity = Vector3.Normalize(desiredVelocity) * moveSpeed;
        }

        Vector3 newVelocity = Vector3.Lerp(currentVelocity, desiredVelocity, acceleration * deltaTime);
        rb.SetVelocity(new Vector3(newVelocity.X, currentVelocity.Y, newVelocity.Z));
    }

    public void UpdateRotation(Vector3 moveDirection, float deltaTime)
    {
        if (moveDirection != Vector3.Zero)
        {
            float targetAngle = (float)Math.Atan2(moveDirection.X, moveDirection.Z);
            float targetAngleDegrees = targetAngle * (180.0f / (float)Math.PI);

            while (targetAngleDegrees - currentRotationAngle > 180.0f) targetAngleDegrees -= 360.0f;
            while (targetAngleDegrees - currentRotationAngle < -180.0f) targetAngleDegrees += 360.0f;

            currentRotationAngle = Lerp(currentRotationAngle, targetAngleDegrees, rotationSpeed * deltaTime);

            Vector3 eulerRotation = new Vector3(0, currentRotationAngle, 0);
            Quaternion newRotation = Quaternion.CreateFromYawPitchRoll(
                eulerRotation.Y * ((float)Math.PI / 180.0f),
                eulerRotation.X * ((float)Math.PI / 180.0f),
                eulerRotation.Z * ((float)Math.PI / 180.0f)
            );

            collider.SetRotation(newRotation);
        }
    }

    private float Lerp(float start, float end, float t)
    {
        return start + (end - start) * Math.Min(1, Math.Max(0, t));
    }
}

