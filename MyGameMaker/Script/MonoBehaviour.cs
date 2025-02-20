using HawkEngine;
using System.Runtime.CompilerServices;
using System;

public abstract class MonoBehaviour
{
    private UIntPtr CplusplusInstance;

    public GameObject gameObject
    {
        get { return GetGameObject(); }
    }

    public virtual void Start() { }
    public virtual void Update(float deltaTime) { }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private extern GameObject GetGameObject();

    public T GetComponent<T>()
    {
        return gameObject.GetComponent<T>();
    }

    public T AddComponent<T>()
    {
        return gameObject.AddComponent<T>();
    }
}