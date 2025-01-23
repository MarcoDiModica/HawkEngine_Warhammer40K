using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using HawkEngine;
using System.Diagnostics;
using static System.Net.Mime.MediaTypeNames;
using System.Runtime.InteropServices;
using System.Security.Policy;

public class ProjectileScript : MonoBehaviour
{
    Transform transform;
    Vector3 initialPosition;
    Vector3 direction = Vector3.Zero;
    bool isMoving = false;
    public float speed = 0.5f;
    public float projectileLifeTime = 3.0f;
    private float lifeTimer = 0f;


    public override void Start()
    {
        transform = GetComponent<Transform>();

        transform.SetScale(0.3f, 0.3f, 0.3f);
        transform.SetPosition(0.0f, 3.0f, 0.0f);

        initialPosition = transform.position;
    }

    public override void Update(float deltaTime)
    {
        if (isMoving)
        {
            transform.position += direction * speed * deltaTime;

            lifeTimer += deltaTime;
        
        }
    }

    public void SetDirection(Vector3 newDirection, float newSpeed)
    {
        direction = Vector3.Normalize(newDirection);
        speed = newSpeed;
        isMoving = true;
    }

    public void StopMovement()
    {
        isMoving = false;
        speed = 0f;
    }
}