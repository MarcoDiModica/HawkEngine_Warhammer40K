using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using HawkEngine;
using System.Diagnostics;

public class TestingComponent : MonoBehaviour
{
    GameObject actor;
    Transform transfr;

    public float timer = 0.0f;
    public int count = 0;

    public float moveAmount = 1.0f;
    private Vector3 currentPosition;

    public override void Start()
    {
        actor = Engineson.CreateGameObject("FuckingTank");
        transfr = actor.GetComponent<Transform>();

        if (transfr != null)
        {
            Engineson.print("the transform was gotten");
            currentPosition = transfr.GetPosition();
        }
        else
        {
            Engineson.print("Waaaawaaaa");
        }

        timer = 0.0f;

        actor.AddComponent<Camera>();
    }

    public override void Update(float deltaTime)
    {
        Vector3 movement = Vector3.Zero;
        Vector3 forwardDirection = Vector3.Zero;

        //Vector3 currentPosition = transfr.GetPosition();
        //Engineson.print($"Current position: {currentPosition}");


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
        else if (Input.GetKey(KeyCode.LEFT))
        {
            movement = new Vector3(-moveAmount, 0, 0);
            forwardDirection = new Vector3(-1, 0, 0);
        }
        else if (Input.GetKey(KeyCode.RIGHT))
        {
            movement = new Vector3(moveAmount, 0, 0);
            forwardDirection = new Vector3(1, 0, 0);
        }

        if (movement != Vector3.Zero)
        {
            currentPosition += movement;

            transfr.SetPosition(currentPosition.X, currentPosition.Y, currentPosition.Z);

            if (forwardDirection != Vector3.Zero)
            {
                Quaternion rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, (float)Math.Atan2(forwardDirection.X,forwardDirection.Z));
                transfr.SetRotationQuat(rotation);
            }

            Engineson.print($"Moved to {currentPosition}, facing {forwardDirection}.");
        }

        if (Input.GetKeyDown(KeyCode.SPACE))
        {
            Vector3 testPosition = new Vector3(10, 0, 0);
            transfr.SetPosition(testPosition.X, testPosition.Y, testPosition.Z);
            transfr.GetPosition();
        }


        timer += deltaTime;

        if (timer > 1.0f)
        {
            Engineson.print("1 second has passed");
            //Engineson.print("count: " + count);
            count++;
            timer = 0.0f;

            //actor.name = count.ToString();
            //transfr.position = new System.Numerics.Vector3(transfr.position.X + 1, transfr.position.Y, transfr.position.Z);
            //transfr.eulerAngles = new System.Numerics.Vector3(transfr.position.X , transfr.position.Y +1, transfr.position.Z);
            //Engineson.print(transfr.eulerAngles.Y.ToString() + "Yaw !!!!");
           // Engineson.print(transfr.eulerAngles.ToString() + " euleeers");
        }
    }
}

