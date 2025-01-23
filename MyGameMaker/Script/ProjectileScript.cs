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
    bool isMoving = true;
    public float speed = 2.0f;

    public override void Start()
    {
        transform = gameObject.GetComponent<Transform>();

        transform.SetScale(0.3f, 0.3f, 0.3f);
        transform.SetPosition(0.0f, 3.0f, 0.0f);

        Vector2 mousePos = new Vector2(Input.GetMousePosition().X, Input.GetMousePosition().Y);
        Vector3 worldMousePos = new Vector3(mousePos.X, transform.position.Y, Input.GetMousePosition().Z);
        direction = new Vector3(0,0,1);

        initialPosition = transform.position;

        Engineson.print("Projectile spawned at {transform.position}, moving towards {worldMousePos} with direction {direction}");
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetKey(KeyCode.UP))
        {
            direction = new Vector3(0, 0, 1);
        }
        else if (Input.GetKey(KeyCode.DOWN))
        {
            direction = new Vector3(0, 0, -1);
        }
        else if (Input.GetKey(KeyCode.LEFT))
        {
            direction = new Vector3(-1, 0, 0);
        }
        else if (Input.GetKey(KeyCode.RIGHT))
        {
            direction = new Vector3(1, 0, 0);
        }

        if (Input.GetKeyDown(KeyCode.D))
        {
            if (isMoving == true)
            {
                ResetProjectile();
            }
            else
            {
                FireProjectile();
            }
        }

        if (isMoving)
        {
            UpdateProjectile(deltaTime);
        }

        Engineson.print($"Projectile move to {transform.position}");
    }

    private void FireProjectile()
    {
        isMoving = true;
    }

    private void ResetProjectile()
    {
        transform.position = initialPosition;
        isMoving = false;
    }

    private void UpdateProjectile(float deltaTime)
    {
        transform.position += direction * speed * deltaTime;
    }
}