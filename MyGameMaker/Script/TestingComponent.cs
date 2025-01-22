using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using HawkEngine;
using System.Diagnostics;
using static System.Net.Mime.MediaTypeNames;

public class TestingComponent : MonoBehaviour
{
    GameObject actor;
    Transform transfr;
    Test test;

    public float timer = 0.0f;
    public int count = 1;

    public float moveAmount = 1.0f;
    private Vector3 currentPosition;

    Vector2 prev_mouse;

    float lookPosX = 0;
    public override void Start()
    {
        //actor = Engineson.CreateGameObject("FuckingTank");
        transfr = gameObject.GetComponent<Transform>();

        timer = 0.0f;
        //lookPosX = transfr.position.X;
        //actor.AddComponent<Camera>();
        prev_mouse = new Vector2(0, 0);

        //test = actor.AddComponent<Test>();
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

        if (Input.GetKey(KeyCode.SPACE)) /* Tank Rotation */
        {
            lookPosX++;
            Vector3 look_position = new Vector3( lookPosX , transfr.position.Y, Input.GetMousePosition().Z);

            transfr.LookAt(  look_position);
            Engineson.print("Looking at mouse " + transfr.GetEulerAngles());


            Vector2 mouse = new Vector2( Input.GetMousePosition().X , Input.GetMousePosition().Y);
            var delta = mouse - prev_mouse;

            transfr.Rotate(delta.X * 0.01f, new Vector3(0, 1, 0));
        }

    }
}

