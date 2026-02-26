using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class ItemPickup : MonoBehaviour
{
	public delegate void PickupEvent();
	public event PickupEvent OnPickup;

	FPSController fpsController;

	public virtual void Start()
	{
		fpsController = FindObjectOfType<FPSController>(true);
	}

	public void Pickup()
	{
		OnPickup?.Invoke();
		Destroy(gameObject);
	}

	public virtual void Update()
	{
		float dist = Vector3.Distance(transform.position, fpsController.transform.position);

		if (dist <= 12.0f)
			Pickup();
	}
}
