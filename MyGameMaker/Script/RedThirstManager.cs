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
    public override void Awake()
    {
        playerController = gameObject.GetComponent<PlayerController>();
    }

    public override void Start()
    {
    
    }

    public override void Update(float deltaTime)
    {
        if (abilityCount > 0)
        {
            abilityUseTimer += deltaTime;
            if (abilityUseTimer >= abilityTimeLimit)
            {
                ResetAbilityCombo();
            }
        }
        if (differentGunsUsed > 0)
        {
            differentGunsUsedTimer += deltaTime;
            if (differentGunsUsedTimer >= differentGunsUsedTimeLimit)
            {
                ResetWeaponCombo();
            }
        }
        //redThirstDamageBonus = 5f + (biblePages * biblePages);
        if (isInBlackRage)
        {
            HandleBlackRage(deltaTime);
        }
        else
        {
           
        }
        if(Input.GetKeyDown(KeyCode.U))
        {
            AddRedThirstPoint(1);
        }
    }

    public void OnAbilityUsed()
    {
        abilityCount++;

        if (abilityCount == 2)
        {
            AddRedThirstPoint(1);
            ResetAbilityCombo();
        }
        lastActionTime = 0f;
    }

    public void OnWeaponUsed()
    {
        //differentGunsUsed++;
        if(boltgunUsed && shotgunUsed || boltgunUsed && railgunUsed || shotgunUsed && railgunUsed)
        {
            AddRedThirstPoint(1);
            ResetWeaponCombo();
        }
        lastActionTime = 0f;
        //if (differentGunsUsed == 2)
        //{
        //    AddRedThirstPoint(1);
        //    ResetWeaponCombo();
        //}
        //lastActionTime = 0f;
    }
    public void OnBoltgunUsed()
    {
        boltgunUsed = true;
        OnWeaponUsed();
    }
    public void OnShotgunUsed()
    {
        shotgunUsed = true;
        OnWeaponUsed();
    }
    public void OnRailgunUsed()
    {
        railgunUsed = true;
        OnWeaponUsed();
    }
    private void ResetWeaponCombo()
    {
        differentGunsUsed = 0;
        differentGunsUsedTimer = 0f;
        boltgunUsed = false;
        shotgunUsed = false;
        railgunUsed = false;
    }
    private void ResetAbilityCombo()
    {
        abilityCount = 0;
        abilityUseTimer = 0f;
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
