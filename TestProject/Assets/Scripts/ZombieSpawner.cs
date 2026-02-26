using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class ZombieSpawner : MonoBehaviour
{
    public string zombiePrefab;

    float spawnTimer = 0.0f;
    float spawnInterval = 150.0f;

    GameObject player;
    PlayerController playerCtrl;

    public static int monsterCount = 0;

	void Start()
	{
        player = GameObject.Find("Empty_Object_4");
        playerCtrl = player.GetMonoBehaviour<PlayerController>();

        Prefab.Instantiate(zombiePrefab, transform.position);
    }
	
	void FixedUpdate()
	{
        if (player == null)
            return;

        if (playerCtrl == null)
            return;

        if (!playerCtrl.enabled)
            return;

        spawnTimer += 0.1f;

        if (spawnTimer > spawnInterval)
        {
            if (monsterCount < 7)
            {
                Prefab.Instantiate(zombiePrefab, transform.position);
                monsterCount += 1;
            }

            spawnTimer = 0;
        }
	}
}
