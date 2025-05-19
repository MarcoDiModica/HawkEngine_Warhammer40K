using System.Collections;
using System.Diagnostics;
using System.Numerics;
using HawkEngine;

public class EnemySpawner : MonoBehaviour
{
    public float spawnTimer = 0;
    public float secondsBetweenSpawns;
    public float firstSpawnTime;
    public int currentEnemiesSpawned = 0;
    public int maxEnemiesToSpawn;
    public bool hasSpawnedFirst = false;

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

    public bool spawnerActive = false;
    public float spawnRadius = 5f;

    public GameObject player;

    public override void Start()
    {
        transform = GetComponent<Transform>();
        player = GameObject.Find("Player");
    }

    public override void Update(float deltaTime)
    {
        if (Vector3.Distance(transform.position, player.transform.position) < spawnRadius)
        {
            spawnerActive = true;
        }
        else
        {
            spawnerActive = false;
            spawnTimer = 0;
        }

        if (spawnerActive)
        {
            spawnTimer += deltaTime;
        }

        if(!hasSpawnedFirst && spawnTimer >= firstSpawnTime && spawnerActive)
        {
            SpawnEnemy();
            currentEnemiesSpawned++;
            hasSpawnedFirst = true;
            spawnTimer = 0;
        }
        else if (hasSpawnedFirst && spawnTimer >= secondsBetweenSpawns && currentEnemiesSpawned < maxEnemiesToSpawn && spawnerActive)
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