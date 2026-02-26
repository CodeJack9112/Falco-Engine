using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class FlyingCamera : MonoBehaviour
{
    public float sensivity = 40.0f;

    float rotationX = 0F;
    float rotationY = 0F;

    float spd = 1.0f;

    Quaternion originalRotation;

    void Start()
	{
        originalRotation = transform.rotation;
        Cursor.locked = true;
        Cursor.visible = false;
    }
	
	void Update(float deltaTime)
	{
        Vector2 mouse = Input.cursorDirection;
        rotationX -= mouse.y * sensivity * deltaTime;
        rotationY -= mouse.x * sensivity * deltaTime;

        rotationX = ClampAngle(rotationX, -80.0f, 80.0f);
        rotationY = ClampAngle(rotationY, -360.0f, 360.0f);

        Quaternion yQuaternion = Quaternion.AngleAxis(rotationX, new Vector3(1, 0, 0));
        Quaternion xQuaternion = Quaternion.AngleAxis(rotationY, new Vector3(0, 1, 0));

        transform.rotation = originalRotation * xQuaternion * yQuaternion;

        Vector3 forward = transform.rotation * new Vector3(0.0f, 0.0f, 1.0f);
        Vector3 left = transform.rotation * new Vector3(1.0f, 0.0f, 0.0f);

        if (Input.GetKey(ScanCode.LeftShift))
        {
            spd = 2.0f;
        }
        else
        {
            spd = 1.0f;
        }

        if (Input.GetKey(ScanCode.W) || Input.GetKey(ScanCode.UpArrow))
        {
            transform.position -= forward * sensivity * spd * deltaTime;
        }

        if (Input.GetKey(ScanCode.S) || Input.GetKey(ScanCode.DownArrow))
        {
            transform.position += forward * sensivity * spd * deltaTime;
        }

        if (Input.GetKey(ScanCode.A) || Input.GetKey(ScanCode.LeftArrow))
        {
            transform.position -= left * sensivity * spd * deltaTime;
        }

        if (Input.GetKey(ScanCode.D) || Input.GetKey(ScanCode.RightArrow))
        {
            transform.position += left * sensivity * spd * deltaTime;
        }
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
