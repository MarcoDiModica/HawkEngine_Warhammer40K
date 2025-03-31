using HawkEngine;

public class PlayerData
{
    float health;
    float maxHealth = 100;
    public float movSpeed = 20;
    public float collectionArea = 1;
    public float bonusCadence = 1;
    public bool isPiercing = false;
    public bool infiniteBullets = false;




    public PlayerData()
    {
        health = 100;
    }

    public void TakeDamage(float damage)
    {
        health -= damage;
        if (health < 0)
        {
            health = 0;
        }
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
    }

    public void AddHealth(float health)
    {
        if (this.health + health > maxHealth)
        {
            this.health = maxHealth;
            return;
        }
        this.health += health;
    }
    public float GetHealth() { return health; }
    public float GetMaxHealth()
    {
        return maxHealth;
    }
}