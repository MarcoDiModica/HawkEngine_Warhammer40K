using System;
using System.Collections.Generic;
using HawkEngine;

public class Test2 : MonoBehaviour
{
	// Variables públicas que se mostrarán en el inspector
	public float value = 3.0f;
	public bool isActive = true;

	// Variables privadas
	private GameObject owner;
	private Transform transform;

	// Inicialización al cargar el script
	public override void Start()
	{
		transform = GetComponent<Transform>();
		Engineson.print("Script " + gameObject.name + " started");
	}

	// Actualización cada frame
	public override void Update(float deltaTime)
	{
		if (isActive)
		{
			// Lógica de actualización
		}
	}

	// Llamado cuando ocurre una colisión
	public override void OnCollisionEnter(GameObject other)
	{
		Engineson.print($"Collision with {other.name}");
	}

	// Llamado cuando otra entidad entra en un trigger
	public override void OnTriggerEnter(GameObject other)
	{
		Engineson.print($"Trigger with {other.name}");
	}

	// Métodos personalizados
	public void CustomMethod()
	{
		// Tu código aquí
	}
}
		