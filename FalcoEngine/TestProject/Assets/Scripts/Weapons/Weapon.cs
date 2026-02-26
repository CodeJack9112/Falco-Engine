using FalcoEngine;
using System.Collections;
using System.Collections.Generic;
using System.Security.Policy;

public class Weapon : MonoBehaviour
{
	public int magazineSize = 12;
	public int totalAmmo = 120;
	public float fireSpeed = 1.0f;
	public float fireAccuracy = 1.0f;
	public int raysPerShot = 1;
	public string soundFire;
	public string soundReload;
	public string soundReady;
	public bool automatic = false;
	public bool reloadOnce = false;
	public int damage = 10;

	public int currentMagazineSize = 0;
	public Texture2D icon;

	FPSController fpsController;
	Camera camera;

	Texture2D crossH;
	Texture2D crossV;
	float fireInterval = 0.0f;
	float crossSize = 8.0f;
	float crossSizeLerp = 0.0f;

	Transform muzzleFlash;
	float muzzleTimer = 0.0f;

	bool shouldReload = false;

	//////----------Weapon bob-----------//////
	float movementSpeed = 0.35f;
	float limitMin = -0.15f;
	float limitMax = 0.15f;
	float bobSpeed = 0.1f;

	float movementX;
	float movementY;
	Vector3 newPosition;
	Vector3 defaultPosition;

	private float m_Time, m_Time2;
	float sinx, cosx = 0;
	float hSpeed = 1;
	float vSpeed = 1;
	//////----------Weapon bob-----------//////

	public void Init()
	{
		fpsController = FindObjectOfType<FPSController>();
		camera = Camera.main;

		defaultPosition = transform.localPosition;

		hSpeed = bobSpeed;
		vSpeed = bobSpeed;

		crossH = Texture2D.LoadFromFile(fpsController.crossHImage);
		crossV = Texture2D.LoadFromFile(fpsController.crossVImage);

		currentMagazineSize = magazineSize;

		animation.onEnded += Animation_onEnded;
		Ready();

		crossSizeLerp = crossSize;
		muzzleFlash = transform.FindChild("MuzzleFlash");
		muzzleFlash.gameObject.enabled = false;
	}

	public void Ready()
	{
		audioSource.fileName = soundReady;
		audioSource.Play();
		animation.Stop();
		animation.Play("Show");
	}

	private void Animation_onEnded(Animation sender, string animName)
	{
		if (animName == "Fire")
		{
			if (currentMagazineSize == 0)
				Reload();
			else
				animation.CrossFade("Idle", 1.0f);
		}

		if (animName == "Reload")
		{
			int add = 0;

			if (reloadOnce)
				add = 1;
			else
			{
				add = magazineSize - currentMagazineSize;
			}

			if (totalAmmo < add)
				add = totalAmmo;

			currentMagazineSize += add;
			totalAmmo -= add;

			if (reloadOnce)
			{
				if (shouldReload)
				{
					if (currentMagazineSize == magazineSize || totalAmmo == 0)
						animation.CrossFade("Idle", 1.0f);
					else
						Reload(true);
				}
				else
					animation.CrossFade("Idle", 1.0f);
			}
			else
				animation.CrossFade("Idle", 1.0f);
		}

		if (animName == "Show")
		{
			animation.CrossFade("Idle", 1.0f);
		}
	}

	public void UpdateFrame()
	{
		if (automatic)
		{
			if (Input.GetMouseButton(0))
				Fire();
		}
		else
		{
			if (Input.GetMouseButtonDown(0))
				Fire();
		}

		if (Input.GetKeyDown(ScanCode.R))
		{
			if (currentMagazineSize < magazineSize)
			{
				if (!IsFiring())
					Reload();
			}
		}
	}
	
	public void UpdateFixed()
	{
		Vector2 mouse = Input.cursorDirection;
		movementX += mouse.x;
		movementY += mouse.y;

		if (movementX > limitMax) movementX = limitMax;
		if (movementX < limitMin) movementX = limitMin;
		if (movementY > limitMax) movementY = limitMax;
		if (movementY < limitMin) movementY = limitMin;

		if (fpsController.IsMoving())
		{
			m_Time += hSpeed;
			m_Time2 += vSpeed;

			sinx = Mathf.Sin(m_Time2) * 0.025f;
			cosx = Mathf.Cos(m_Time) * 0.025f;
		}
		else
		{
			sinx = Mathf.Lerp(sinx, 0, 0.5f);
			cosx = Mathf.Lerp(cosx, 0, 0.5f);
		}

		movementX = Mathf.Lerp(movementX, 0, 0.15f);
		movementY = Mathf.Lerp(movementY, 0, 0.15f);

		newPosition = new Vector3(defaultPosition.x - movementX, defaultPosition.y + movementY + cosx, defaultPosition.z);

		transform.localPosition = Vector3.Lerp(transform.localPosition, newPosition, movementSpeed * 0.2f);

		if (fireInterval > 0)
			fireInterval -= 0.1f;
		else
			fireInterval = 0.0f;

		crossSizeLerp = Mathf.Lerp(crossSizeLerp, crossSize, 0.1f);

		if (muzzleTimer > 0)
			muzzleTimer -= 0.1f;
		else
		{
			muzzleTimer = 0.0f;

			if (muzzleFlash.gameObject.enabled)
				muzzleFlash.gameObject.enabled = false;
		}
	}

	public void Fire()
	{
		if (currentMagazineSize > 0)
		{
			if (!IsReloading() && !IsGettingReady())
			{
				if (fireInterval == 0.0f)
				{
					animation.Stop();
					animation.Play("Fire");

					audioSource.fileName = soundFire;
					audioSource.Play();

					currentMagazineSize -= 1;

					fireInterval = fireSpeed;
					crossSizeLerp += fireAccuracy * 25.0f;

					muzzleFlash.gameObject.enabled = true;
					muzzleFlash.rotation *= Quaternion.Euler(new Vector3(0, 0, 15));
					muzzleTimer = 0.5f;

					//Raycast
					//Check shoot raycast
					for (int i = 0; i < raysPerShot; ++i)
					{
						float _r = Random.Range(-fireAccuracy * 10, fireAccuracy * 10);
						float _r1 = Random.Range(-fireAccuracy * 10, fireAccuracy * 10);
						float _r2 = Random.Range(-fireAccuracy * 10, fireAccuracy * 10);
						Vector3 rand = new Vector3(_r, _r1, _r2);

						RaycastHit shootHit = Physics.Raycast(camera.gameObject.transform.position, camera.gameObject.transform.position + ((camera.gameObject.transform.rotation * new Vector3(0.0f, 0.0f, -1.0f)) * 1000.0f) + rand);
						if (shootHit.hasHit)
						{
							if (shootHit.rigidbody != null)
							{
								Vector3 dir = shootHit.hitPoint - camera.gameObject.transform.position;
								dir.Normalize();

								Transform hitObj = shootHit.rigidbody.gameObject.transform;
								DoDamage(hitObj, hitObj, damage, shootHit.hitPoint, shootHit.worldNormal);
								shootHit.rigidbody.AddForce(dir * damage * (1000.0f / raysPerShot), Vector3.zero);
							}
						}
					}
				}
			}
		}

		if (IsReloading())
		{
			if (reloadOnce)
			{
				shouldReload = false;
			}
		}
	}

	void Reload(bool force = false)
	{
		if (!force)
		{
			if (IsReloading())
				return;
		}

		if (totalAmmo > 0)
		{
			audioSource.fileName = soundReload;
			audioSource.Play();

			animation.CrossFade("Reload", 1.0f);
			shouldReload = true;
		}
	}

	public bool IsReloading()
	{
		return animation.IsPlaying("Reload");
	}

	public bool IsFiring()
	{
		return animation.IsPlaying("Fire");
	}

	public bool IsGettingReady()
	{
		return animation.IsPlaying("Show");
	}

	void DoDamage(Transform root, Transform from, int damage, Vector3 point, Vector3 normal)
	{
		MonsterController cnt = from.gameObject.GetMonoBehaviour<MonsterController>();

		if (cnt != null)
		{
			cnt.DoDamage(damage, point);
		}
		else
		{
			Quaternion rot = Quaternion.LookRotation(normal, Vector3.up);

			if (from.parent != null)
			{
				DoDamage(root, from.parent, damage, point, normal);
			}
			else
			{
				float _min = 0.15f, _max = 0.25f;
				float _r = Random.Range(_min, _max);

				GameObject bul = Prefab.Instantiate(fpsController.bulletHolePrefab, point + normal * _r, rot);
				bul.transform.parent = root.gameObject.transform;

				Prefab.Instantiate(fpsController.sparksPrefab, point, rot);
			}
		}
	}

	public void DrawCross()
	{
		GUI.Image(crossH, new Vector2(Screen.width / 2.0f - crossH.width - crossSizeLerp, Screen.height / 2.0f - crossH.height / 2.0f), new Vector2(crossH.width, crossH.height));
		GUI.Image(crossH, new Vector2(Screen.width / 2.0f + crossSizeLerp, Screen.height / 2.0f - crossH.height / 2.0f), new Vector2(crossH.width, crossH.height));

		GUI.Image(crossV, new Vector2(Screen.width / 2.0f - crossV.width / 2.0f, Screen.height / 2.0f - crossV.height - crossSizeLerp), new Vector2(crossV.width, crossV.height));
		GUI.Image(crossV, new Vector2(Screen.width / 2.0f - crossV.width / 2.0f, Screen.height / 2.0f + crossSizeLerp), new Vector2(crossV.width, crossV.height));
	}

	public virtual void SetIcon()
	{

	}
}
