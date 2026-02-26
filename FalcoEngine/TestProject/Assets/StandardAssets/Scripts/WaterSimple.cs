using FalcoEngine;

public class WaterSimple : MonoBehaviour
{
    public float waveSpeed = 0.001f;
    public float waveAmplitude = 0.005f;
    public float waveLength = 0.25f;

    Material waterMaterial;
    float time = 0.0f;

    void Start()
	{
        Entity entity = (Entity)gameObject.attachedObject;
        SubEntity subEntity = entity.GetSubEntity(0);

        waterMaterial = subEntity.material;

        for (int i = 0; i < 8; ++i)
        {
            float _amplitude = waveAmplitude / ((float)i + 1.0f);
            waterMaterial.SetParameter("amplitude[" + i.ToString() + "]", _amplitude);

            float _wavelength = waveLength * (float)Mathf.PI / ((float)i + 1.0f);
            waterMaterial.SetParameter("wavelength[" + i.ToString() + "]", _wavelength);

            float _speed = 1.0f + 2.0f * (float)i;
            waterMaterial.SetParameter("speed[" + i.ToString() + "]", _speed);

            float angle = Random.Range(-(float)Mathf.PI / 3, (float)Mathf.PI / 3);
            waterMaterial.SetParameter("direction[" + i.ToString() + "]", new Vector2((float)Mathf.Cos(angle), (float)Mathf.Sin(angle)));
        }
    }

    void FixedUpdate()
	{
		if (waterMaterial != null)
        {
            time += waveSpeed;
            waterMaterial.SetParameter("time", time);
        }
	}
}
