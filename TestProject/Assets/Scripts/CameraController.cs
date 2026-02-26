using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class CameraController : MonoBehaviour
{
    public GameObject root;
    public float sensivity = 0.8f;

    float rotationX = 0F;
    float rotationY = 0F;

    Quaternion originalRotation;
    Vector2 mouse;

    void Start()
	{
        originalRotation = root.rigidbody.rotation;
        Cursor.locked = true;
    }
		
	void FixedUpdate()
	{
        
    }

    void Update(float deltaTime)
    {
        if (Input.GetKeyDown(ScanCode.L))
            Cursor.locked = !Cursor.locked;

        mouse = new Vector2(Input.cursorDirection.x * deltaTime * 80.0f, Input.cursorDirection.y * deltaTime * 80.0f);

        rotationX += -mouse.y * sensivity;
        rotationY -= mouse.x * sensivity;

        rotationX = ClampAngle(rotationX, -80.0f, 80.0f);
        rotationY = ClampAngle(rotationY, -360.0f, 360.0f);

        Quaternion yQuaternion = Quaternion.AngleAxis(rotationX, new Vector3(1, 0, 0));
        Quaternion xQuaternion = Quaternion.AngleAxis(rotationY, new Vector3(0, 1, 0));

        root.rigidbody.rotation = originalRotation * xQuaternion;
        transform.localRotation = yQuaternion;
    }

    public float ClampAngle(float angle, float min, float max)
    {
        angle = angle % 360;

        if ((angle >= -360F) && (angle <= 360F))
        {
            if (angle < -360F)
            {
                angle += 360F;
            }
            if (angle > 360F)
            {
                angle -= 360F;
            }
        }

        return Mathf.Clamp(angle, min, max);
    }
}