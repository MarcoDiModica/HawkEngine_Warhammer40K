using HawkEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

    public class MouseFollower : MonoBehaviour
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
            transfr.SetPosition(Input.GetMousePosition().X, Input.GetMousePosition().Y, Input.GetMousePosition().Z);
        }
    }

