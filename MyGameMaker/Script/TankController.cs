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

public class TankController : MonoBehaviour
{
    GameObject turret;
    Transform transform;
    GameObject projectile;
    Transform projTransform;

    Vector2 prev_mouse;
    float lookPosX = 0;
    public override void Start()
    {
        transform = gameObject.GetComponent<Transform>();
        transform.SetPosition(0, 2, 0);

        prev_mouse = new Vector2(0,0);
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetKey(KeyCode.SPACE))
        {
            lookPosX++;
            Vector3 look_position = new Vector3(lookPosX, transform.position.Y, Input.GetMousePosition().Z);

            transform.LookAt(look_position);

            Vector2 mouse = new Vector2(Input.GetMousePosition().X, Input.GetMousePosition().Y);
            var delta = mouse - prev_mouse;

            transform.Rotate(delta.X * 0.01f, new Vector3(0, 1, 0));
        }

        if (Input.GetKey(KeyCode.E))
        {
            GenerateProjectile();
        }
    }

    private void GenerateProjectile()
    {
        projectile = Engineson.CreateGameObject("Projectile", null);
        //projectile.AddComponent<ProjectileScript>(); esto da nullptr el ProjectileScript;
        projTransform = projectile.GetComponent<Transform>();

        projTransform.position = transform.position + transform.forward;

    }
}