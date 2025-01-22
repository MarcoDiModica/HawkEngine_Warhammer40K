using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using HawkEngine;
using System.Diagnostics;
using static System.Net.Mime.MediaTypeNames;
using System.Runtime.CompilerServices;

public class TestingComponent : MonoBehaviour
{
    GameObject actor;
    Transform transfr;
    Test test;
    TestingComponent testing1;
    GameObject projectile;
    TankController turret;
    Transform turretTransform;

    public float timer = 0.0f;
    public int count = 1;
    public float projectileSpeed = 10.0f;

    public float moveAmount = 1.0f;
    private Vector3 currentPosition;

    Vector2 prev_mouse;

    float lookPosX = 0;
    public override void Start()
    {
        //actor = Engineson.CreateGameObject("FuckingTank", null);
        transfr = gameObject.GetComponent<Transform>();

        transfr.SetScale(2, 2, 2);

        timer = 0.0f;
        //lookPosX = transfr.position.X;
        //actor.AddComponent<Camera>();
        prev_mouse = new Vector2(0, 0);

        //test = actor.AddComponent<Test>();
        testing1 = gameObject.GetComponent<TestingComponent>();
        if (testing1 != null)
        {
            Engineson.print("I have a TestingComponent");
        }
        else
        {
            Engineson.print("I dont have a TestingComponent");
        }
    }

    public override void Update(float deltaTime)
    {
        Vector3 movement = Vector3.Zero;
        Vector3 forwardDirection = Vector3.Zero;

        // Tank controls
        if (Input.GetKey(KeyCode.UP))
        {
            movement = new Vector3(0, 0, moveAmount);
            forwardDirection = new Vector3(0, 0, 1);
        }
        else if (Input.GetKey(KeyCode.DOWN))
        {
            movement = new Vector3(0, 0, -moveAmount);
            forwardDirection = new Vector3(0, 0, -1);
        }


        if (movement != Vector3.Zero)
        {
            transfr.position += transfr.forward * movement.Z * deltaTime;

            Engineson.print($"Moved to {transfr.GetLocalPosition()}, facing {forwardDirection}.");
        }

        if (Input.GetKey(KeyCode.E))
        {
            //GenerateProjectile();
        }

    }

    private void GenerateProjectile()
    {
        //projectile = Engineson.CreateGameObject("Projectile");
    }
}

