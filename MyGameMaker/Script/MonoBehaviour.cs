using HawkEngine;
using System;
using System.Runtime.CompilerServices;

public  class MonoBehaviour
{
    public GameObject gameObject
    {
        get { return GetGameObject(); }
    }

    public UIntPtr CplusplusInstance;

    public virtual void Start()
    {
        
    }

    public virtual void Update(float deltaTime)
    {
        
    }


    [MethodImpl(MethodImplOptions.InternalCall)]
    public extern GameObject  GetGameObject();
    public T GetComponent<T>()
    {
        return gameObject.GetComponent<T>();
    }

    public T AddComponent<T>()
    {
        return gameObject.AddComponent<T>();
    }

    // --- Collision Events (Overridable) ---
    public virtual void OnCollisionEnter(GameObject other) {
    }
    public virtual void OnCollisionStay(GameObject other) {
    }
    public virtual void OnCollisionExit(GameObject other) {
    }

    public virtual void OnTriggerEnter(GameObject other) {
    }

    public virtual void OnTriggerStay(GameObject other) {
    }
    public virtual void OnTriggerExit(GameObject other) {
    }
}