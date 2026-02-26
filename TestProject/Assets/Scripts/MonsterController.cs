using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class MonsterController : MonoBehaviour
{
    public int Health = 100;
    public GameObject Player;
    public string BloodPrefabPath;
    public string PlayerName = "";
    GameObject target = null;

    bool died = false;
    float beatTimer = 0;

    FPSController playerController;

    public delegate void DieEvent();
    public event DieEvent OnDie;

    void Start()
    {
        ActivateRagdoll(transform, false);

        playerController = Player.GetMonoBehaviour<FPSController>();

        TimedObjectDestructor timedObjectDestructor = GetMonoBehaviour<TimedObjectDestructor>();

        if (timedObjectDestructor != null)
            timedObjectDestructor.enabled = false;

        target = GameObject.Find(PlayerName);
    }

    void Update(float deltaTime)
    {
        if (Health <= 0)
            return;

        if (!playerController.enabled)
        {
            animation.CrossFade("Idle", 2.0f);

            if (navMeshAgent != null)
            {
                if (navMeshAgent.enabled)
                {
                    navMeshAgent.enabled = false;
                }
            }
        }
        else
        {
            if (Player != null)
            {
                float dist = Vector3.Distance(transform.position, Player.transform.position);

                if (dist < NPCOptimizer.NPCSeeDistance)
                {
                    if (navMeshAgent != null)
                    {
                        if (target != null)
                            navMeshAgent.targetPosition = target.transform.position;
                    }

                    if (dist < 20)
                    {
                        if (playerController != null)
                        {
                            navMeshAgent.enabled = false;
                            animation.CrossFade("Attack", 2.0f);

                            if (beatTimer < 3.0)
                            {
                                beatTimer += deltaTime * 5;
                            }
                            else
                            {
                                beatTimer = 0;

                                playerController.ReceiveDamage(5);
                            }
                        }
                    }
                    else
                    {
                        animation.CrossFade("Walk", 2.0f);

                        if (!navMeshAgent.enabled)
                        {
                            navMeshAgent.enabled = true;
                        }
                    }
                }
                else
                {
                    animation.CrossFade("Idle", 2.0f);

                    if (navMeshAgent.enabled)
                    {
                        navMeshAgent.enabled = false;
                    }
                }
            }
        }
    }

    void ActivateRagdoll(Transform root, bool active)
    {
        if (root.gameObject.rigidbody != null)
        {
            root.gameObject.rigidbody.isKinematic = !active;
        }

        for (int i = 0; i < root.childCount; i++)
        {
            if (root.GetChild(i).gameObject.rigidbody != null)
                root.GetChild(i).gameObject.rigidbody.isKinematic = !active;

            ActivateRagdoll(root.GetChild(i), active);
        }
    }

    public void DoDamage(int damage, Vector3 point)
    {
        GameObject blood = Prefab.Instantiate(BloodPrefabPath, point, transform.rotation);
        blood.transform.parent = transform;

        if (died)
            return;

        Health -= damage;

        if (Health <= 0)
        {
            Die();

            TimedObjectDestructor timedObjectDestructor = GetMonoBehaviour<TimedObjectDestructor>();
            if (timedObjectDestructor != null)
                timedObjectDestructor.enabled = true;
        }
    }

    public void Die()
    {
        Health = 0;
        died = true;

        animation.Stop();

        if (navMeshAgent != null)
            Destroy(navMeshAgent);

        GetComponent<Animation>().enabled = false;
        ActivateRagdoll(transform, true);

        ZombieSpawner.monsterCount -= 1;

        OnDie?.Invoke();
    }
}
