using System.Collections;
using System.Numerics;
using HawkEngine;

public class EnemySpawner : MonoBehaviour
{
    public float spawnTimer;
    public float secondsBetweenSpawns;

    private Transform transform;

    public Prefab hormagauntPrefab;
    public Prefab rangedEnemyPrefab;
    public Prefab stalkerEnemyPrefab;
    public Prefab warriorEnemyPrefab;
    public Prefab climberHormagauntPrefab;

    // Cambiar los nombres a los enemigos si hace falta (no me los sé xd)
    public enum EnemyType
    {
        Melee,
        Ranged,
        Stalker,
        Warrior,
        ClimberHormagaunt,
    }

    public EnemyType enemyTypeToSpawn;

    

    public override void Start()
    {
        transform = GetComponent<Transform>();
    }

    public override void Update(float deltaTime)
    {
        spawnTimer += deltaTime;

        if (spawnTimer >= secondsBetweenSpawns)
        {
            SpawnEnemy();
            spawnTimer = 0;
        }
    }

    public void SpawnEnemy()
    {
        switch(enemyTypeToSpawn)
        {
            case EnemyType.Melee:
                GameObject meleeEnemy = Instantiate(hormagauntPrefab, transform);
                meleeEnemy.GetComponent<Collider>().SetPosition(transform.position);
                break;
            case EnemyType.Ranged:
                GameObject rangedEnemy = Instantiate(rangedEnemyPrefab, transform);
                rangedEnemy.GetComponent<Collider>().SetPosition(transform.position);
                break;
            case EnemyType.Stalker:
                GameObject stalkerEnemy = Instantiate(stalkerEnemyPrefab, transform);
                stalkerEnemy.GetComponent<Collider>().SetPosition(transform.position);
                break;
            case EnemyType.Warrior:
                GameObject warriorEnemy = Instantiate(warriorEnemyPrefab, transform);
                warriorEnemy.GetComponent<Collider>().SetPosition(transform.position);
                break;
            case EnemyType.ClimberHormagaunt:
                GameObject climberHormagaunt = Instantiate(climberHormagauntPrefab, transform);
                climberHormagaunt.GetComponent<Collider>().SetPosition(transform.position);
                break;

        }
    }
}