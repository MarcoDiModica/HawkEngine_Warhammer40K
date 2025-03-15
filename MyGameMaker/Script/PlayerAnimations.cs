using System;
using System.Numerics;
using HawkEngine;

public class PlayerAnimations : MonoBehaviour
{
    private SkeletalAnimation esk;
    int animIndex = 0;

    public override void Start()
    {
        esk = gameObject.GetComponent<SkeletalAnimation>();
        if (esk == null)
        {
            Engineson.print("ERROR: PlayerAnimation requires a SkeletalAnimation component!");
            return;
        }

        animIndex = esk.GetAnimationIndex();
    }

    public override void Update(float deltaTime)
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            animIndex += 1;
            esk.SetAnimation(animIndex);
        }
    }

    public void SetStandardIdleAnimation()
    {
        esk.SetAnimation(4);
        esk.SetAnimationSpeed(2f);
    }

    public void SetIdleRandomAnimation()
    {
        if (esk.GetAnimationTime() >= esk.GetAnimationLength() - 0.2f)
        {
            Random rand = new Random();
            int idleIndex = rand.Next(2, 6);
            esk.SetAnimation(idleIndex);
            esk.SetAnimationSpeed(2f);
        }
        
    }

    public void SetShootingStandingAnimation()
    {
        esk.SetAnimation(8);
        esk.SetAnimationSpeed(3f);
    }

    public void SetShootingRunningAnimation()
    {
        esk.SetAnimation(10);
        esk.SetAnimationSpeed(4.5f);
    }

    public void SetRunAnimation()
    {
        esk.SetAnimation(6);
        esk.SetAnimationSpeed(4.5f);
    }
}