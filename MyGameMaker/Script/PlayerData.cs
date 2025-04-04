using HawkEngine;

public class PlayerData
{
    float health;
    float healthTemp;
    float maxHealth = 100;
    float maxHealthTemp = 50;
    public float movSpeed = 20;
    public float collectionArea = 1;
    public float bonusCadence = 1;
    public bool isPiercing = false;
    public bool infiniteBullets = false;




    public PlayerData()
    {
        health = 100;
        healthTemp = 50;
    }

    public void TakeDamage(float damage)
    {
        //First take damage from the temporary health, then, if it is 0, take damage from the max health
        if (healthTemp > 0)
        {
            healthTemp -= damage;
            if (healthTemp < 0)
            {
                healthTemp = 0;
            }
        }
        else
        {
            health -= damage;
        }

        Engineson.print("PlayerData: " + this.health + " " + this.healthTemp);

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
        Engineson.print("PlayerData: " + this.health + " " + this.healthTemp);
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
        Engineson.print("PlayerData: " + this.health + " " + this.healthTemp);

    }
    public float GetHealth() { return health; }

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