using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class TimedObjectDestructor : MonoBehaviour
{
    public float timeToDestruct = 50;
	public bool fadeOut = true;

	public delegate void DestroyEvent();
	public event DestroyEvent OnDestroy;

	void Start()
	{

	}
	
	void FixedUpdate()
	{
        timeToDestruct -= 0.1f;

		if (fadeOut)
		{
			if (timeToDestruct < 5.0f)
			{
				Entity entity = (Entity)transform.gameObject.attachedObject;

				if (entity != null)
				{
					Vector4 col = Vector4.one;
					Material material = entity.GetSubEntity(0).material;
					if (material.renderingType == RenderingType.Forward)
					{
						for (int i = 0; i < material.passCount; ++i)
						{
							material.GetPass(i).blendingMode = BlendingMode.TransparentAlpha;
							material.GetPass(i).iterationType = IterationType.Once;
							material.GetParameter("diffuseColor", out col);
							material.SetParameter("diffuseColor", new Vector4(col.x, col.y, col.z, 1.0f / 5.0f * timeToDestruct));
						}
					}
				}

				for (int i = 0; i < transform.childCount; ++i)
				{
					Transform child = transform.GetChild(i);
					entity = (Entity)child.gameObject.attachedObject;

					if (entity != null)
					{
						Vector4 col = Vector4.one;
						Material material = entity.GetSubEntity(0).material;
						if (material.renderingType == RenderingType.Forward)
						{
							for (int j = 0; j < material.passCount; ++j)
							{
								material.GetPass(j).blendingMode = BlendingMode.TransparentAlpha;
								material.GetParameter("diffuseColor", out col);
								material.SetParameter("diffuseColor", new Vector4(col.x, col.y, col.z, 1.0f / 5.0f * timeToDestruct));
							}
						}
					}
				}
			}
		}

		if (timeToDestruct <= 0)
		{
			Destroy(gameObject);

			OnDestroy?.Invoke();
		}
	}
}
