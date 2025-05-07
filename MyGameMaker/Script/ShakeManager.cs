using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;
public class ShakeManager : MonoBehaviour
{
    public Vector3 currentShakeOffset = Vector3.Zero;

    private class ActiveShake
    {
        public float intensity;
        public float duration;
        public float elapsedTime;
        public float nextUpdateTime;
    }

    private List<ActiveShake> activeShakes = new List<ActiveShake>();
    private Random random;

    private void Awake()
    {
        random = new Random();
    }

    public void ApplyShake(float intensity, float duration)
    {
        activeShakes.Add(new ActiveShake
        {
            intensity = intensity,
            duration = duration,
            elapsedTime = 0f,
            nextUpdateTime = 0f
        });
    }

    public override void Update(float deltaTime)
    {
        currentShakeOffset = Vector3.Zero;

        for (int i = activeShakes.Count - 1; i >= 0; i--)
        {
            var shake = activeShakes[i];
            shake.elapsedTime += deltaTime;

            if (shake.elapsedTime >= shake.duration)
            {
                activeShakes.RemoveAt(i);
                continue;
            }

            if (shake.elapsedTime >= shake.nextUpdateTime)
            {
                float t = shake.elapsedTime / shake.duration;
                float decay = 1.0f - t;
                float currentIntensity = shake.intensity * decay;

                currentShakeOffset += GenerateRandomOffset(currentIntensity);

                shake.nextUpdateTime += 0.1f;
            }
        }
    }

    private Vector3 GenerateRandomOffset(float amount)
    {
        float x = ((float)random.NextDouble() * 2f - 1f) * amount;
        float y = ((float)random.NextDouble() * 2f - 1f) * amount;
        float z = ((float)random.NextDouble() * 2f - 1f) * amount;
        return new Vector3(x, y, z);
    }
}

