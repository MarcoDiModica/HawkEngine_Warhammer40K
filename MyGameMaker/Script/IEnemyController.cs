using System;
using System.Numerics;

public interface IEnemyController
{
    void Start();
    void Update(float deltaTime);
    void Attack();

    void TakeDamage(float damage);
}