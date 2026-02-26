using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class DemoVehicle : MonoBehaviour
{
	Vehicle vehicle;

	public float acceleration = 5.0f;
	public float maxSteering = 35.0f;
	public float maxEngineForce = 1000.0f;
	public float maxBreakPower = 20.0f;

	float steering = 0;
	float engineForce = 0.0f;
	float breakPower = 0.0f;

	void Start()
	{
		vehicle = GetComponent<Vehicle>();
	}
	
	void Update(float deltaTime)
	{
		if (Input.GetKeyDown(ScanCode.R))
			SceneManager.LoadScene(SceneManager.loadedScene);

		if (Input.GetKeyDown(ScanCode.Escape))
			Application.Quit();

		if (vehicle != null)
        {
			float _maxSteering = 15;

			//if (Mathf.Abs(engineForce) > 0.1f)
			//	_maxSteering = 15 * Mathf.Clamp(1.0f - (Mathf.Abs(engineForce) / maxEngineForce), 0.1f, 1.0f);

			//Steering
			if (Input.GetKey(ScanCode.A))
				steering = Mathf.Lerp(steering, maxSteering, deltaTime * _maxSteering);
			else if (Input.GetKey(ScanCode.D))
				steering = Mathf.Lerp(steering, -maxSteering, deltaTime * _maxSteering);
			else
				steering = Mathf.Lerp(steering, 0, deltaTime * _maxSteering);

			float _steering = steering;

			if (Mathf.Abs(rigidbody.linearVelocity.sqrMagnitude) > 0.1f)
				_steering = steering * Mathf.Clamp(1.0f - (Mathf.Abs(rigidbody.linearVelocity.sqrMagnitude) / maxEngineForce), 0.15f, 1.0f);

			vehicle.SetSteering(_steering, 0);
			vehicle.SetSteering(_steering, 1);

			float _maxAcceleration = acceleration;

			if (Mathf.Abs(steering) > 0.1f)
				_maxAcceleration = acceleration * Mathf.Clamp(1.0f - (Mathf.Abs(steering) / maxSteering), 0.25f, 1.0f);

			//Engine force
			if (Input.GetKey(ScanCode.W))
				engineForce = Mathf.Lerp(engineForce, maxEngineForce, deltaTime * _maxAcceleration);
			else if (Input.GetKey(ScanCode.S))
			{
				engineForce = Mathf.Lerp(engineForce, -maxEngineForce, deltaTime * _maxAcceleration);
			}
			else
				engineForce = Mathf.Lerp(engineForce, 0, deltaTime * _maxAcceleration);

			float _engineForce = engineForce;

			if (Mathf.Abs(steering) > 0.1f)
				_engineForce = engineForce * Mathf.Clamp(1.0f - (Mathf.Abs(steering) / maxSteering), 0.5f, 1.0f);

			if (Input.GetKey(ScanCode.W))
			{
				vehicle.ApplyEngineForce(_engineForce, 0);
				vehicle.ApplyEngineForce(_engineForce, 1);
				vehicle.ApplyEngineForce(_engineForce * 0.3f, 2);
				vehicle.ApplyEngineForce(_engineForce * 0.3f, 3);
			}
			else if (Input.GetKey(ScanCode.S))
            {
				vehicle.ApplyEngineForce(_engineForce * 0.3f, 0);
				vehicle.ApplyEngineForce(_engineForce * 0.3f, 1);
				vehicle.ApplyEngineForce(_engineForce, 2);
				vehicle.ApplyEngineForce(_engineForce, 3);
			}
			else
            {
				vehicle.ApplyEngineForce(_engineForce, 0);
				vehicle.ApplyEngineForce(_engineForce, 1);
				vehicle.ApplyEngineForce(_engineForce, 2);
				vehicle.ApplyEngineForce(_engineForce, 3);
			}

			//Hand break
			if (Input.GetKey(ScanCode.Space))
			{
				breakPower = maxBreakPower;
				engineForce = 0;
			}
			else
				breakPower = 0;

			vehicle.SetBreak(breakPower, 0);
			vehicle.SetBreak(breakPower, 1);
			vehicle.SetBreak(breakPower, 2);
			vehicle.SetBreak(breakPower, 3);
		}
	}
}
