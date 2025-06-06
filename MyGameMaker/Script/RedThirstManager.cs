using System.Collections;
using System.Collections.Generic;
using HawkEngine;

public class RedThirstManager : MonoBehaviour
{
    private int redThirstPoints = 0;
    private const int maxRedThirstPoints = 5;
    private bool isInBlackRage = false;

    private float abilityUseTimer = 0f;
    private int abilityCount = 0;
    private int differentGunsUsed = 0;
    private float differentGunsUsedTimer = 0;
    private const float differentGunsUsedTimeLimit = 3f;
    private const float abilityTimeLimit = 3f;

    private float redThirstDecayTimer = 0f;
    private const float RED_THIRST_DECAY_TIME = 5f;

    private float blackRageTimer = 0f;
    private const float BLACK_RAGE_DURATION = 5f;
    private const float BLACK_RAGE_EXTENSION = 2f;
    private PlayerController playerController;
    private float lastActionTime = 0f;

    public float redThirstBonus = 5f;
    public float biblePages = 0f;

    private bool boltgunUsed = false;
    private bool shotgunUsed = false;
    private bool railgunUsed = false;

    private bool isActivatingIdle = false;
    private bool isActivatingRunning = false;
    private bool isActivatingWalking = false;

    private ParticleFX Angy;

    public override void Awake()
    {
        playerController = gameObject.GetComponent<PlayerController>();
        Angy = GameObject.Find("RedThirstFX").GetComponent<ParticleFX>();
        if (Angy != null)
        {
            Angy.ApplyPreset(32);
        }
    }

    public override void Start()
    {

    }

    public override void Update(float deltaTime)
    {
        if (isActivatingIdle)
        {
            FinishIdleBlackRageAnimation();
        }
        else if (isActivatingRunning)
        {
            FinishRunningBlackRageAnimation();
        }
        else if (isActivatingWalking)
        {
            FinishWalkingBlackRageAnimation();
        }

        //redThirstDamageBonus = 5f + (biblePages * biblePages);
        if (isInBlackRage)
        {
            HandleBlackRage(deltaTime);
        }
        else
        {

        }
        if (Input.GetKeyDown(KeyCode.U))
        {
            AddRedThirstPoint(1);
            GameObject redThirstVFX = Engineson.CreateGameObject("RedThirstVFX", null);
            gameObject.AddChild(redThirstVFX);
            redThirstVFX.AddComponent<ParticleFX>();
            ParticleFX particleFX = redThirstVFX.GetComponent<ParticleFX>();
            if (particleFX != null)
            {
                particleFX.ApplyPreset(32);
                //particleFX.EmitBurst(100);
                particleFX.EmitBurst(1);
            }

        }
    }

    private void FinishIdleBlackRageAnimation()
    {
        if (playerController.playerAnimations.esk.IsAnimationFinished())
        {
            isActivatingIdle = false;
            playerController.playerAnimations.ActiveBlackRageToIdleAnimation();
        }
    }

    private void FinishRunningBlackRageAnimation()
    {
        if (playerController.playerAnimations.esk.IsAnimationFinished())
        {
            isActivatingRunning = false;
            playerController.playerAnimations.ActiveBlackRageToRunningAnimation();
        }
    }

    private void FinishWalkingBlackRageAnimation()
    {
        if (playerController.playerAnimations.esk.IsAnimationFinished())
        {
            isActivatingWalking = false;
            switch (playerController.currentShootingDirection)
            {
                case PlayerController.ShootingDirection.Forward:
                    playerController.playerAnimations.ActiveBlackRageToWalkingForwardAnimation();
                    break;
                case PlayerController.ShootingDirection.Backward:
                    playerController.playerAnimations.ActiveBlackRageToWalkingBackwardsAnimation();
                    break;
                case PlayerController.ShootingDirection.Left:
                    playerController.playerAnimations.ActiveBlackRageToWalkingLeftAnimation();
                    break;
                case PlayerController.ShootingDirection.Right:
                    playerController.playerAnimations.ActiveBlackRageToWalkingRightAnimation();
                    break;
            }
        }
    }

   public void ManageCombo()
    {

    }
    public void AddRedThirstPoint(int points)
    {
        //Actualizar el HUD por cada Red Thirst Point
        redThirstPoints += points;
        if (redThirstPoints > maxRedThirstPoints)
        {
            redThirstPoints = maxRedThirstPoints;
        }
        Engineson.print($"Red Thirst Points: {redThirstPoints}");

        if (redThirstPoints >= maxRedThirstPoints && isInBlackRage == false)
        {
            ActivateBlackRage();

            if (playerController.isIdle || playerController.isShootingStanding)
            {
                isActivatingIdle = true;
                playerController.playerAnimations.ActivateBlackRageWhileIdleAnimation();
            }
            else if (playerController.isRunning)
            {
                isActivatingRunning = true;
                playerController.playerAnimations.ActivateBlackRageWhileRunningAnimation();
            }
            else if (playerController.isShootingRunning)
            {
                isActivatingWalking = true;
                switch (playerController.currentShootingDirection)
                {
                    case PlayerController.ShootingDirection.Forward:
                        playerController.playerAnimations.ActivateBlackRageWhileWalkingStraightAnimation();
                        break;
                    case PlayerController.ShootingDirection.Backward:
                        playerController.playerAnimations.ActivateBlackRageWhileWalkingBackwardsAnimation();
                        break;
                    case PlayerController.ShootingDirection.Left:
                        playerController.playerAnimations.ActivateBlackRageWhileWalkingLeftAnimation();
                        break;
                    case PlayerController.ShootingDirection.Right:
                        playerController.playerAnimations.ActivateBlackRageWhileWalkingRightAnimation();
                        break;
                }
            }

        }

        redThirstDecayTimer = 0f;
        lastActionTime = 0f;
    }

    public void AddBiblePages(float points)
    {
        biblePages += points;
        redThirstBonus = 5f + (biblePages * biblePages);
    }

    private void ActivateBlackRage()
    {
        isInBlackRage = true;
        blackRageTimer = 0f;
        Engineson.print("Black Rage Activated!");
        playerController.playerData.blackRageSpeed = redThirstBonus;
        playerController.playerDash.canDash = false;
        Angy.Play();
        GameObject redThirstVFX = Engineson.CreateGameObject("RedThirstVFX", null);
        gameObject.AddChild(redThirstVFX);
        redThirstVFX.AddComponent<ParticleFX>();
        ParticleFX particleFX = redThirstVFX.GetComponent<ParticleFX>();
        if (particleFX != null)
        {
            particleFX.ApplyPreset(32);
            //particleFX.EmitBurst(100);
            particleFX.EmitBurst(10);
        }

    }
    private void DeactivateBlackRage()
    {
        // Actualizar el HUD para mostrar que Black Rage ha terminado
        isInBlackRage = false;
        redThirstPoints = 0;
        Engineson.print("Black Rage Deactivated");
        playerController.playerData.blackRageSpeed = 0f;
        playerController.playerDash.canDash = true;
    }
    private void HandleBlackRage(float deltaTime)
    {
        blackRageTimer += deltaTime;

        lastActionTime += deltaTime;

        if (blackRageTimer >= BLACK_RAGE_DURATION)
        {
            DeactivateBlackRage();
        }
    }

    public int GetRedThirstPoints()
    {
        return redThirstPoints;
    }

    public bool IsInBlackRage()
    {
        return isInBlackRage;
    }
}