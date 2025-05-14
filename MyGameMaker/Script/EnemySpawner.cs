using System.Collections;
using System.Numerics;
using HawkEngine;

public class EnemySpawner : MonoBehaviour
{
    public float spawnTimer = 0;
    public float secondsBetweenSpawns;
    public int currentEnemiesSpawned = 0;
    public int maxEnemiesToSpawn;

    private Transform transform;

    public Prefab hormagauntPrefab;
    public Prefab rangedEnemyPrefab;
    public Prefab stalkerEnemyPrefab;
    public Prefab warriorEnemyPrefab;
    public Prefab climberHormagauntPrefab;

    public bool spawnHormagaunt;
    public bool spawnRanged;
    public bool spawnStalker;
    public bool spawnWarrior;
    public bool spawnClimberHormagaunt;
  

    public override void Start()
    {
        transform = GetComponent<Transform>();
    }

    public override void Update(float deltaTime)
    {
        spawnTimer += deltaTime;

        if (spawnTimer >= secondsBetweenSpawns && currentEnemiesSpawned < maxEnemiesToSpawn)
        {
            SpawnEnemy();
            currentEnemiesSpawned++;
            spawnTimer = 0;
        }
    }

    public void SpawnEnemy()
    {

        if (spawnHormagaunt)
        {
            GameObject meleeEnemy = Instantiate(hormagauntPrefab, transform);
            meleeEnemy.GetComponent<Collider>().SetPosition(transform.position);
        }
        else if (spawnRanged)
        {
            GameObject rangedEnemy = Instantiate(rangedEnemyPrefab, transform);
            rangedEnemy.GetComponent<Collider>().SetPosition(transform.position);
        }
        else if (spawnStalker)
        {
            GameObject stalkerEnemy = Instantiate(stalkerEnemyPrefab, transform);
            stalkerEnemy.GetComponent<Collider>().SetPosition(transform.position);
        }
        else if (spawnWarrior)
        {
            GameObject warriorEnemy = Instantiate(warriorEnemyPrefab, transform);
            warriorEnemy.GetComponent<Collider>().SetPosition(transform.position);
        }
        else if (spawnClimberHormagaunt)
        {
            GameObject climberHormagaunt = Instantiate(climberHormagauntPrefab, transform);
            climberHormagaunt.GetComponent<Collider>().SetPosition(transform.position);
        }
    }
}