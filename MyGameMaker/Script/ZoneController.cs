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
    private List<int> _lastCounts = new List<int>();
    private int _totalToSpawn;
    private int _spawnedCount;
    private float _timeSinceLast;
    private bool _barrierUp;

    private List<Collider> _barriers = new List<Collider>();

    public override void Start()
    {
        var centerPos = GetComponent<Transform>().GetPosition();

        var allSpawnerGOs = GameObject.FindGameObjectsWithTag(spawnerTag);
        foreach (var go in allSpawnerGOs)
        {
            var spawnerTransform = go.GetComponent<Transform>();
            var spawnerPos = spawnerTransform.GetPosition();
            if (Vector3.Distance(spawnerPos, centerPos) <= zoneRadius)
            {
                Engineson.print($"[ZoneController] Found spawner at {spawnerPos}");
                var sp = go.GetComponent<EnemySpawner>();
                if (sp != null)
                {
                    _spawners.Add(sp);
                    _lastCounts.Add(sp.currentEnemiesSpawned);
                    _totalToSpawn += sp.maxEnemiesToSpawn;
                }
            }
        }
        if (_spawners.Count == 0)
            Engineson.print("[ZoneController] WARNING: no spawners found in zone.");

        var allBarrierGOs = GameObject.FindGameObjectsWithTag(barrierTag);
        foreach (var go in allBarrierGOs)
        {
            var barrierTransform = go.GetComponent<Transform>();
            var barrierPos = barrierTransform.GetPosition();
            if (Vector3.Distance(barrierPos, centerPos) <= colliderRadius)
            {
                Engineson.print($"[ZoneController] Found barrier at {barrierPos}");
                var col = go.GetComponent<Collider>();
                if (col != null)
                {
                    col.SetActive(false);
                    _barriers.Add(col);
                }
            }
            else
            {
                Engineson.print($"[ZoneController] Skipping barrier at {barrierPos} (outside colliderRadius)");
            }
        }
        if (_barriers.Count == 0)
            Engineson.print("[ZoneController] WARNING: no barriers found in zone.");
    }

    public override void Update(float deltaTime)
    {
        for (int i = 0; i < _spawners.Count; i++)
        {
            var sp = _spawners[i];
            int prevCount = _lastCounts[i];
            int currCount = sp.currentEnemiesSpawned;

            if (currCount > prevCount)
            {
                int newlySpawned = currCount - prevCount;
                _spawnedCount += newlySpawned;
                _lastCounts[i] = currCount;
                _timeSinceLast = 0f;

                Engineson.print($"[ZoneController] Detected {newlySpawned} new spawn(s) from spawner #{i}.");

                if (!_barrierUp)
                {
                    Engineson.print("[ZoneController] Raising ALL barriers.");
                    foreach (var c in _barriers)
                        c.SetActive(true);
                    _barrierUp = true;
                }
            }
        }

        if (_barrierUp)
        {
            _timeSinceLast += deltaTime;

            if (_spawnedCount >= _totalToSpawn && _timeSinceLast >= fallbackDelay)
            {
                Engineson.print("[ZoneController] All spawns done & fallback timer elapsed. Dropping barriers.");
                foreach (var c in _barriers)
                    c.SetActive(false);
                _barrierUp = false;
            }
        }
    }
}
