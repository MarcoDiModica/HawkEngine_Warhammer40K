using HawkEngine;

public class PlayerData
{

    private static PlayerData instance = null;

    float health;
    float healthTemp;
    float maxHealth = 100;
    float maxHealthTemp = 50;
    public bool isHit = false;
    public float movSpeed = 10;
    public float collectionArea = 1;
    public float bonusCadence = 1;
    public bool isPiercing = false;
    public bool infiniteBullets = false;
    public bool GodMode = false;
    public float blackRageSpeed = 0f;
    public float stimmSpeed = 0f;
    public bool hasBoltgun = true;
    public bool hasShotgun = false;
    public bool hasRailgun = false;
    public bool BoltgunUpgraded = false;
    public bool ShotgunUpgraded = false;
    public bool RailgunUpgraded = false;
    private string HealthSFX = "Assets/Audio/UI/Lose_Temporary_heart_2.wav";

    private PlayerData()
    {
        health = maxHealth;
        healthTemp = maxHealthTemp;
    }

    public static PlayerData Instance
    {
        get
        {
            if (instance == null)
            {
                instance = new PlayerData();
            }
            return instance;
        }
    }

    public void TakeDamage(float damage)
    {
        //First take damage from the temporary health, then, if it is 0, take damage from the max health
        if (healthTemp > 0)
        {
            isHit = true;
            healthTemp -= damage;
            if (healthTemp < 0)
            {
                Audio.PlayOneShot(HealthSFX);
                healthTemp = 0;
            }
        }
        else
        {
            isHit = true;
            health -= damage;
        }

        //Engineson.print("PlayerData: " + this.health + " " + this.healthTemp);

    }

    public void SetHealth(float health)
    {
        if (health > maxHealth)
        {
            this.health = maxHealth;
            return;
        }else if (health < 0)
        {
            this.health = 0;
            return;
        }
        this.health = health;
        //Engineson.print("PlayerData: " + this.health + " " + this.healthTemp);
    }

    public void SetTempHealth(float health)
    {
        if (health > maxHealthTemp)
        {
            healthTemp = maxHealthTemp;
            return;
        }
        else if (health < 0)
        {
            healthTemp = 0;
            return;
        }
        healthTemp = health;
    }

    public void AddHealth(float health)
    {
        if (healthTemp > 0)
        {
            healthTemp += health;
            if (healthTemp > maxHealthTemp)
            {
                healthTemp = maxHealthTemp;
            }
        }
        else
        {
            this.health += health;
            if (this.health > maxHealth)
            {
                this.health = maxHealth;
            }
        }
        //Engineson.print("PlayerData: " + this.health + " " + this.healthTemp);

    }
    public float GetHealth() { return health; }
    public void FullHealth()
    {
        health = maxHealth;
        healthTemp = maxHealthTemp;
        //Engineson.print("PlayerData: " + this.health + " " + this.healthTemp);
    }
    public float GetHealthTemp() { return healthTemp; }

    public float GetMaxHealth()
    {
        return maxHealth;
    }

    public float GetMaxHealthTemp()
    {
        return maxHealthTemp;
    }
}