using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class RotateObject : MonoBehaviour
{
	void Start()
	{
		
	}
	
	void Update(float deltaTime)
	{
		
	}

	void FixedUpdate()
	{
		transform.rotation *= Quaternion.Euler(new Vector3(0.01f, 0, 0));
	}
}
