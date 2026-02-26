using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;

public class Ball : MonoBehaviour
{
	public float speed = 20.0f;
	public string collisionEffect;
	public GameObject levelTextObject;
	public GameObject pointsTextObject;
	public GameObject blocksTextObject;

	Rigidbody body;
	Text pointsText;
	Text blocksText;
	Text levelText;

	public static int points = 0;
	public static int pointsPrev = 0;
	public static int level = 0;

	void Start()
	{
		body = GetComponent<Rigidbody>();
		body.AddForce(new Vector3(0, -speed * 4, 0), new Vector3());

		pointsText = (Text)pointsTextObject.attachedObject;
		blocksText = (Text)blocksTextObject.attachedObject;
		levelText = (Text)levelTextObject.attachedObject;

		Block[] blocks = FindObjectsOfType<Block>();
		blocksText.text = "Blocks left: " + (blocks.Length).ToString();
		levelText.text = "Level: " + (level + 1).ToString();
	}
	
	void Update(float deltaTime)
	{
		pointsText.text = "Points: " + points.ToString();

		body.linearVelocity = speed * (body.linearVelocity.normalized);

		if (Input.GetKeyDown(ScanCode.Escape))
		{
			SceneManager.LoadScene("Scenes/Arkanoid/MainMenu.scene");
		}
	}

	void OnCollisionEnter(Collision col)
	{
		if (col.other.gameObject.name == "WallDie")
		{
			points = pointsPrev;
			string scene = "Scenes/Arkanoid/Level" + level + ".scene";

			SceneManager.LoadScene(scene);
		}
	}

	void OnCollisionExit(Collision col)
	{
		AudioSource audio = GetComponent<AudioSource>();
		if (audio != null)
			audio.Play();

		Transform tr = col.other.gameObject.transform.parent;
		if (tr != null)
		{
			GameObject obj = tr.gameObject;
			Block block = obj.GetMonoBehaviour<Block>();
			Racket racket = obj.GetMonoBehaviour<Racket>();
			TimedObjectDestructor destructor = obj.GetMonoBehaviour<TimedObjectDestructor>();
			
			if (block != null)
			{
				Prefab.Instantiate(collisionEffect, col.point);

				block.health -= 10;

				for (int i = 0; i < obj.transform.childCount; ++i)
				{
					Transform child = obj.transform.GetChild(i);
					Entity entity = (Entity)child.gameObject.attachedObject;

					if (entity != null)
					{
						Vector3 c = new Vector3(1, 1, 1);
						entity.GetSubEntity(0).material.GetParameter("cDiffuseColor", out c);

						c -= new Vector3(0.1f, 0.1f, 0.1f);
						if (c.x < 0) c.x = 0;
						if (c.y < 0) c.y = 0;
						if (c.z < 0) c.z = 0;

						entity.GetSubEntity(0).material.SetParameter("cDiffuseColor", c);
					}
				}

				if (block.health <= 0)
				{
					obj.GetComponent<Animation>().CrossFade("Destroy");
					if (destructor != null)
					{
						if (!destructor.enabled)
						{
							destructor.enabled = true;

							destructor.OnDestroy += () =>
							{
								Block[] blocks = FindObjectsOfType<Block>();
								blocksText.text = "Blocks left: " + (blocks.Length - 1).ToString();

								if (blocks.Length - 1 == 0)
								{
									level += 1;
									pointsPrev = points;
									string scene = "Scenes/Arkanoid/Level" + level + ".scene";

									string fileName = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Arkanoid.dat");
									File.WriteAllText(fileName, level.ToString() + "\n" + points.ToString());

									if (Asset.Exists(scene))
									{
										SceneManager.LoadScene(scene);
									}
									else
									{
										SceneManager.LoadScene("Scenes/Arkanoid/MainMenu.scene");
									}
								}
							};
						}
					}

					points += block.pointsAmount;

					for (int i = 0; i < obj.transform.childCount; ++i)
					{
						Transform child = obj.transform.GetChild(i);
						Entity entity = (Entity)child.gameObject.attachedObject;

						MeshCollider collider = child.gameObject.GetComponent<MeshCollider>();
						if (collider != null)
						{
							Destroy(collider);
						}

						if (entity != null)
						{
							entity.GetSubEntity(0).material.renderingType = RenderingType.Forward;
							entity.GetSubEntity(0).material.GetPass(0).blendingMode = BlendingMode.TransparentAlpha;
							entity.GetSubEntity(0).material.GetPass(0).vertexShader = Shader.Find("Shaders/diffuseColor_vp.glslv");
							entity.GetSubEntity(0).material.GetPass(0).fragmentShader = Shader.Find("Shaders/onlyColor.glslf");

							entity.GetSubEntity(0).material.SetParameter("cWorldViewProj", AutoConstantType.WorldViewProjMatrix);
						}
					}
				}
			}

			if (racket != null)
			{
				racket.gameObject.GetComponent<Animation>().CrossFade("Hit");
			}
		}
	}
}
