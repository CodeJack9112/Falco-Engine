using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class LookAtCamera : MonoBehaviour
{
	public float distance = 100;
	public float speed = 1;

	float x = 0;
	float y = 0;

	GameObject lookAt;

	void Start()
	{
		Vehicle vehicle = FindObjectOfType<Vehicle>(false);
		if (vehicle != null)
			lookAt = vehicle.gameObject;

		Cursor.locked = true;
	}
	
	void Update(float deltaTime)
	{
		if (Input.GetKeyDown(ScanCode.L))
			Cursor.locked = !Cursor.locked;

		if (Input.GetKeyDown(ScanCode.Escape))
			Cursor.locked = false;

		x += Input.cursorDirection.x * deltaTime * 80.0f * speed * distance * 0.02f;
		y += Input.cursorDirection.y * deltaTime * 80.0f * speed * distance * 0.02f;

		y = ClampAngle(y, 5, 80);

		var rotation = Quaternion.Euler(new Vector3(-y, -x, 0));

        distance = Mathf.Clamp(distance - Input.GetMouseWheel().y * 5, 5, 100);

        var position = rotation * new Vector3(0.0f, 0.0f, distance) + lookAt.transform.position;

		transform.rotation = rotation;
		transform.position = position;
	}

	float ClampAngle(float angle, float min, float max)
	{
		if (angle < -360)
			angle += 360;
		if (angle > 360)
			angle -= 360;
		return Mathf.Clamp(angle, min, max);
	}
}
