public class PlayerData
{
    float health;

    public PlayerData()
    {
        health = 100;
    }

    public void SetHealth(float health)
    {
        this.health = health;
    }

    public float GetHealth() { return health; }
}