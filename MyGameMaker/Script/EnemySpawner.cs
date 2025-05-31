using System;
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
    public event Action<GameObject> onEnemySpawned;

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

        if ((!hasSpawnedFirst && spawnTimer >= firstSpawnTime) ||
            (hasSpawnedFirst && spawnTimer >= secondsBetweenSpawns && currentEnemiesSpawned < maxEnemiesToSpawn))
        {
            SpawnEnemy();
            currentEnemiesSpawned++;
            hasSpawnedFirst = true;
            spawnTimer = 0;

            onEnemySpawned?.Invoke(_lastSpawnedEnemy);
        }


    }

    private GameObject _lastSpawnedEnemy;
    public void SpawnEnemy()
    {
        GameObject enemy = null;
        if (spawnHormagaunt)
        {
            enemy = Instantiate(hormagauntPrefab, transform);
            enemy.GetComponent<Collider>().SetPosition(transform.position);
        }
        else if (spawnRanged)
        {
            enemy = Instantiate(rangedEnemyPrefab, transform);
            enemy.GetComponent<Collider>().SetPosition(transform.position);
        }
        else if (spawnStalker)
        {
            enemy = Instantiate(stalkerEnemyPrefab, transform);
            enemy.GetComponent<Collider>().SetPosition(transform.position);
        }
        else if (spawnWarrior)
        {
            enemy = Instantiate(warriorEnemyPrefab, transform);
            enemy.GetComponent<Collider>().SetPosition(transform.position);
        }
        else if (spawnClimberHormagaunt)
        {
            enemy = Instantiate(climberHormagauntPrefab, transform);
            enemy.GetComponent<Collider>().SetPosition(transform.position);
        }
        _lastSpawnedEnemy = enemy;
    }
}