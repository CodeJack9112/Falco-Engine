using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class NPCOptimizer : MonoBehaviour
{
	public static float NPCSeeDistance = 120;

	public static List<MonsterController> monsters;
	FPSController player;

	void Start()
	{
		monsters = new List<MonsterController>(FindObjectsOfType<MonsterController>());
		player = FindObjectOfType<FPSController>();

		foreach (MonsterController monsterController in monsters)
			monsterController.OnDie += () => { monsters.Remove(monsterController); };
	}
	
	void Update(float deltaTime)
	{
		foreach (MonsterController monsterController in monsters)
		{
			float dist = Vector3.Distance(player.transform.position, monsterController.transform.position);

			if (dist > NPCSeeDistance * 1.5f)
			{
				if (monsterController.gameObject.enabled)
					monsterController.gameObject.enabled = false;
			}
			else
			{
				if (!monsterController.gameObject.enabled)
					monsterController.gameObject.enabled = true;
			}
		}
	}
}
