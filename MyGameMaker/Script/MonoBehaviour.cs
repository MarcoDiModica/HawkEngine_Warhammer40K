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
}