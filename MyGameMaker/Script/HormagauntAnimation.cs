using System;
using System.Numerics;
using HawkEngine;

public class HormagauntAnimation : MonoBehaviour
{
    private SkeletalAnimation hormagauntesk;
    int animIndex = 0;

    public override void Awake()
    {

    }
    public override void Start()
    {
        hormagauntesk = gameObject.GetComponent<SkeletalAnimation>();
        if (hormagauntesk == null)
        {
            Engineson.print("ERROR: PlayerAnimation requires a SkeletalAnimation component!");
            return;
        }
        hormagauntesk.SetAnimation(14);

    }

    public override void Update(float deltaTime)
    {
        if (hormagauntesk.GetAnimationTime() >= hormagauntesk.GetAnimationLength() - 0.3f)
        {
            hormagauntesk.SetAnimationPlayState(false);
        }
    }

    public void SetStandardIdleAnimation()
    {

        hormagauntesk.SetAnimation(14);
        hormagauntesk.SetAnimationSpeed(1.0f);
    }

    public void SetRandomAttackAnimation()
    {
        Random rand = new Random();
        int attackIndex = rand.Next(1, 4);
        Engineson.print("" + attackIndex);
        switch (attackIndex)
        {
            case 1:
                hormagauntesk.SetAnimation(1);
                //hormagauntesk.TransitionAnimations(15, 1, 0.1f);
                hormagauntesk.SetAnimationSpeed(1.0f);
                break;
            case 2:
                hormagauntesk.SetAnimation(2);
                //hormagauntesk.TransitionAnimations(15, 2, 0.1f);
                hormagauntesk.SetAnimationSpeed(1.0f);
                break;
            case 3:
                hormagauntesk.SetAnimation(3);
                //hormagauntesk.TransitionAnimations(15, 3, 0.1f);
                hormagauntesk.SetAnimationSpeed(1.0f);
                break;
        }
    }

    public void SetDeathAnimation()
    {
        if (hormagauntesk.GetAnimationTime() >= hormagauntesk.GetAnimationLength() - 0.3f)
        {
            Engineson.print("Cosa");
            hormagauntesk.SetAnimationPlayState(false);
        } else
        {
            hormagauntesk.SetAnimation(8);
            hormagauntesk.SetAnimationSpeed(1.0f);
        }
    }
}