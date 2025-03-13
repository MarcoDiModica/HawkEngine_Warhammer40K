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
}