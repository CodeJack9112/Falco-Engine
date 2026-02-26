using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class Racket : MonoBehaviour
{
	public GameObject cameraObject;
	
	Camera camera;

	void Start()
	{
		camera = (Camera)cameraObject.attachedObject;
	}
	
	void Update(float deltaTime)
	{
		Vector2 cursorPos = FalcoEngine.Input.cursorRelativePosition;
		Vector3 screenPos = new Vector3(cursorPos.x, cursorPos.y, -transform.position.z * 2);

		Vector3 worldPos = camera.ScreenToWorldPoint(screenPos);
		worldPos.z = -32.9760f;
		worldPos.y = transform.position.y;

		transform.position = worldPos;
	}
}
