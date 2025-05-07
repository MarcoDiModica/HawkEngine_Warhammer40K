using System;
using System.Collections.Generic;
using System.Numerics;
using HawkEngine;

public class ShakeManager : MonoBehaviour
{

    private PlayerCamera mainCamera;
    private class ActiveShake
    {
        public Camera camera;
        public Vector3 originalOffset;
        public float intensity;
        public float duration;
        public float elapsedTime;
        public float nextUpdateTime;

        public ActiveShake(Camera camera, Vector3 originalOffset, float intensity, float duration)
        {
            this.camera = camera;
            this.originalOffset = originalOffset;
            this.intensity = intensity;
            this.duration = duration;
            this.elapsedTime = 0f;
            this.nextUpdateTime = 0f;
        }
    }

    private List<ActiveShake> activeShakes = new List<ActiveShake>();
    private Random random;

    private void Awake()
    {
        mainCamera = GameObject.Find("MainCamera").GetComponent<PlayerCamera>();
        random = new Random();
        activeShakes = new List<ActiveShake>();
    }

    //Default shake function that will use Main Camera
    public void ApplyShake(float intensity, float duration)
    {
        mainCamera.cameraRef.SetOffset(mainCamera.currentOffset);
        activeShakes.Add(new ActiveShake(mainCamera.cameraRef, mainCamera.currentOffset, intensity, duration));
    }  
    
    //Function to apply shake to any other camera 
    public void ApplyShake(float intensity, float duration, Camera camera, Vector3 originalOffset)
    {
        Vector3 offset = originalOffset ;
        camera.SetOffset(offset);
        activeShakes.Add(new ActiveShake(camera, offset, intensity, duration));
    }

    public override void Update(float deltaTime)
    {
        for (int i = activeShakes.Count - 1; i >= 0; i--)
        {
            var shake = activeShakes[i];
            shake.elapsedTime += deltaTime;

            if (shake.elapsedTime >= shake.duration)
            {
                shake.camera.SetOffset(shake.originalOffset);
                activeShakes.RemoveAt(i);
                continue;
            }

            if (shake.elapsedTime >= shake.nextUpdateTime)
            {
                float t = shake.elapsedTime / shake.duration;
                float decay = 1.0f - t;
                float currentIntensity = shake.intensity * decay;

                Vector3 shakeOffset = RandomShakeOffset(currentIntensity);
                shake.camera.SetOffset(shake.originalOffset + shakeOffset);

                shake.nextUpdateTime += 0.1f;
            }
        }
    }

    private Vector3 RandomShakeOffset(float amount)
    {
        float x = ((float)random.NextDouble() * 2f - 1f) * amount;
        float y = ((float)random.NextDouble() * 2f - 1f) * amount;
        float z = ((float)random.NextDouble() * 2f - 1f) * amount;
        return new Vector3(x, y, z);
    }
}
