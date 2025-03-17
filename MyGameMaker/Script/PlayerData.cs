public class PlayerData
{
    float health;
    float maxHealth = 100;

    public PlayerData()
    {
        health = 100;
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

    public float GetHealth() { return health; }
    public float GetMaxHealth()
    {
        return maxHealth;
    }
}