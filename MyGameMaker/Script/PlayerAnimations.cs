using System;
using System.Numerics;
using HawkEngine;

public class PlayerAnimations : MonoBehaviour
{
    private SkeletalAnimation esk;
    float currentSpeed = 0.0f;

    public override void Start()
    {
        //esk = gameObject.GetComponent<SkeletalAnimation>();
        //if (esk == null)
        //{
        //    Engineson.print("ERROR: PlayerAnimation requires a SkeletalAnimation component!");
        //    return;
        //}

        //currentSpeed = esk.GetAnimationSpeed();
    }

    public override void Update(float deltaTime)
    {
        //Engineson.print("Current Anim Speed: " + currentSpeed);

        //if (Input.GetKeyDown(KeyCode.B))
        //{
        //    currentSpeed += 0.1f;
        //    esk.SetAnimationSpeed(currentSpeed);
        //}
    }
}