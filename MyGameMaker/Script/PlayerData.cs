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
        this.health = health;
    }

    public float GetHealth() { return health; }
    public float GetMaxHealth()
    {
        return maxHealth;
    }
}