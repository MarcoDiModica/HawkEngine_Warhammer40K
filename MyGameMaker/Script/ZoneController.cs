using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class ZoneController : MonoBehaviour
{
    public string spawnerTag = "Spawner";
    public string barrierTag = "Barrier";
    public float zoneRadius = 10f;
    public float colliderRadius = 200f;

    public float fallbackDelay = 5f;

    private List<EnemySpawner> _spawners = new List<EnemySpawner>();
    private List<Collider> _barriers = new List<Collider>();
    private int _totalToSpawn;
    private int _spawnedCount;
    private float _timeSinceLast;
    private bool _barrierUp;

    public override void Start()
    {
        var myPos = this.gameObject.GetComponent<Transform>().GetPosition();

        var foundSpawners = GameObject.FindGameObjectsWithTag(spawnerTag);
        foreach (var go in foundSpawners)
        {
            var t = go.GetComponent<Transform>();
            var pos = t.GetPosition();
            if (Vector3.Distance(pos, myPos) <= zoneRadius)
            {
                Engineson.print("Spawner found");
                var sp = go.GetComponent<EnemySpawner>();
                if (sp != null)
                {
                    sp.onEnemySpawned += OnEnemySpawned;
                    _spawners.Add(sp);
                    _totalToSpawn += sp.maxEnemiesToSpawn;
                }
            }
        }

        var foundBarriers = GameObject.FindGameObjectsWithTag(barrierTag);
        foreach (var go in foundBarriers)
        {
            var t = go.GetComponent<Transform>();
            var pos = t.GetPosition();
            if (Vector3.Distance(pos, myPos) <= colliderRadius)
            {
                //Engineson.print("Colliders dentro de rango");
                var col = go.GetComponent<Collider>();
                if (col != null)
                {
                    col.SetActive(false);
                    _barriers.Add(col);
                }
            } else
            {
                
                //Engineson.print("Colliders fuera de rango");
            }
        }
    }

    public override void Update(float deltaTime)
    {
        if (!_barrierUp) return;

        _timeSinceLast += deltaTime;

        if (_spawnedCount >= _totalToSpawn && _timeSinceLast >= fallbackDelay)
        {
            SetBarriers(false);
            _barrierUp = false;
        }
    }

    private void OnEnemySpawned(GameObject enemy)
    {
        if (!_barrierUp)
        {
            SetBarriers(true);
            _barrierUp = true;
        }
        _spawnedCount++;
        _timeSinceLast = 0f;
    }

    private void SetBarriers(bool active)
    {
        foreach (var c in _barriers)
            c.SetActive(active);
    }
}
