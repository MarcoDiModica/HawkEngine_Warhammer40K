using System;
using System.Collections.Generic;
using HawkEngine;

public class AudioLvl1 : MonoBehaviour
{
	// Public variables will be shown in the Inspector
	public float value = 3.0f;
	public bool isActive = true;

	// Private variables
	private Transform transform;

	// Script Initialization
	public override void Start()
	{
		transform = GetComponent<Transform>();
		Engineson.print("Script " + gameObject.name + " started");
	}

	// Update on every frame
	public override void Update(float deltaTime)
	{
		if (isActive)
		{
			// Update logic
		}
	}

	// Called when a collision happens
	public override void OnCollisionEnter(GameObject other)
	{
		Engineson.print($"Collision with {other.name}");
	}

	// Called when a trigger happens
	public override void OnTriggerEnter(GameObject other)
	{
		Engineson.print($"Trigger with {other.name}");
	}

	// Custom method
	public void CustomMethod()
	{
		// Your code here
	}
}
		