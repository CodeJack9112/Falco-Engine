#pragma once

#include <OgreVector3.h>
#include <OgreQuaternion.h>

class Mathf
{
public:
	static float Clamp01(float value)
	{
		if (value < 0.0f)
		{
			return 0.0f;
		}
		if (value > 1.0f)
		{
			return 1.0f;
		}
		return value;
	}

	static float lerp(float a, float b, float t)
	{
		return (1.0 - t) * a + t * b;
	}

	static Vector3 lerp(Vector3 a, Vector3 b, float t)
	{
		t = Clamp01(t);
		return Vector3(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t);
	}

	static float normalize(float input, float min = -1, float max = 1)
	{
		float average = (min + max) / 2;
		float range = (max - min) / 2;
		float normalized_x = (input - average) / range;
		return normalized_x;
	}

	static float RandomFloat(float min, float max)
	{
		assert(max > min);
		float random = ((float)rand()) / (float)RAND_MAX;

		float range = max - min;
		return (random * range) + min;
	}

	static float smoothstep(float edge0, float edge1, float x)
	{
		float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return t * t * (3.0 - 2.0 * t);
	}

	static Ogre::Quaternion toQuaternion(Vector3 value)
	{
		return toQuaternion(value.z, value.y, value.x);
	}

	// yaw (Z), pitch (Y), roll (X)
	static Ogre::Quaternion toQuaternion(double yaw, double pitch, double roll)
	{
		yaw *= Math::fDeg2Rad;
		pitch *= Math::fDeg2Rad;
		roll *= Math::fDeg2Rad;

		// Abbreviations for the various angular functions
		double cy = cos(yaw * 0.5);
		double sy = sin(yaw * 0.5);
		double cp = cos(pitch * 0.5);
		double sp = sin(pitch * 0.5);
		double cr = cos(roll * 0.5);
		double sr = sin(roll * 0.5);

		Ogre::Quaternion q;
		q.w = cy * cp * cr + sy * sp * sr;
		q.x = cy * cp * sr - sy * sp * cr;
		q.y = sy * cp * sr + cy * sp * cr;
		q.z = sy * cp * cr - cy * sp * sr;

		return q;
	}

	static Ogre::Vector3 toEuler(Ogre::Quaternion q) {
		Ogre::Vector3 angles;

		float sqw = q.w * q.w;
		float sqx = q.x * q.x;
		float sqy = q.y * q.y;
		float sqz = q.z * q.z;
		float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
		float test = q.x * q.w - q.y * q.z;
		
		if (test > 0.4995f * unit) { // singularity at north pole
			angles.y = 2.0f * atan2(q.y, q.x);
			angles.x = Math::PI / 2.0f;
			angles.z = 0;
			return normalizeAngles(angles * Math::fRad2Deg);
		}
		if (test < -0.4995f * unit) { // singularity at south pole
			angles.y = -2.0f * atan2(q.y, q.x);
			angles.x = -Math::PI / 2.0f;
			angles.z = 0;
			return normalizeAngles(angles * Math::fRad2Deg);
		}

		// roll (x-axis rotation)
		double sinr_cosp = 2.0 * (q.w * q.x + q.y * q.z);
		double cosr_cosp = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
		angles.x = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = 2.0 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1.0)
			angles.y = std::copysign(Ogre::Math::PI / 2.0, sinp); // use 90 degrees if out of range
		else
			angles.y = std::asin(sinp);

		// yaw (z-axis rotation)
		double siny_cosp = 2.0 * (q.w * q.z + q.x * q.y);
		double cosy_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
		angles.z = std::atan2(siny_cosp, cosy_cosp);

		return normalizeAngles(angles * Math::fRad2Deg);
	}

	static Vector3 normalizeAngles(Vector3 angles)
	{
		angles.x = normalizeAngle(angles.x);
		angles.y = normalizeAngle(angles.y);
		angles.z = normalizeAngle(angles.z);
		return angles;
	}

	static float normalizeAngle(float angle)
	{
		while (angle > 360.0f)
			angle -= 360.0f;
		while (angle < 0.0f)
			angle += 360.0f;
		return angle;
	}

	static Vector3 inverseTransformPoint(SceneNode* relativeTo, Vector3 point)
	{
		Vector3 lossyScale = relativeTo->_getDerivedScale();
		Vector3 position = relativeTo->_getDerivedPosition();
		Quaternion rotation = relativeTo->_getDerivedOrientation();

		return Vector3(1.0f / lossyScale.x, 1.0f / lossyScale.y, 1.0f / lossyScale.z) * (rotation.Inverse() * ((point - position)));
	}

	static Vector3 transformPoint(SceneNode* relativeTo, Vector3 point)
	{
		Vector3 lossyScale = relativeTo->_getDerivedScale();
		Vector3 position = relativeTo->_getDerivedPosition();
		Quaternion rotation = relativeTo->_getDerivedOrientation();

		return position + rotation * (lossyScale * point);
	}

	static Vector3 inverseTransformDirection(SceneNode* relativeTo, Vector3 axis)
	{
		return relativeTo->getOrientation() * axis;
	}
};