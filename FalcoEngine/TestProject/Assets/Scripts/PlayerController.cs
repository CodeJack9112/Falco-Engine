using FalcoEngine;
using System.Collections;
using System.Collections.Generic;

public class PlayerController : MonoBehaviour
{
    public float Health = 100;
    public float jumpStrength = 45.0f;
    public float walkSpeed = 25.0f;
    public float runMultiplayer = 2.0f;
    public float gravity = -25.0f;
    public GameObject weaponObject;
    public GameObject muzzleFlash;
    public string bulletHolePrefab;
    public string sparksPrefab;
    public GameObject camera;
    public GameObject patronSlot;
    public string patronPrefab;
    public string restartMenuPrefab;

    bool isGrounded = false;
    float runSpeed = 1.0f;
    float jumpSpeed = 0.0f;
    float jumpTimer = 0.0f;

    bool isShooting = false;
    float shootInterval = 10.0f;
    float currentShootInterval = 0.0f;
    float muzzleTimer = 0.5f;
    float currentMuzzleTimer = 0.0f;
    int ammoMagazineCurrent = 8;
    int ammoMagazineSize = 8;
    int ammoMagazineTotal = 100;

    //Weapon bob
    public float MovementSpeed = 0.35f;
    public float limitMin = -0.15f;
    public float limitMax = 0.15f;
    public float bobSpeed = 0.1f;
    bool reloading = false;

    float MovementX;
    float MovementY;
    Vector3 newGunPosition;
    Vector3 DefaultPosition;

    private float m_Time, m_Time2;
    float sinx, cosx = 0;

    float _speed = 1;
    float pSpeed = 1;

    float forwardSpeed = 0;
    float leftSpeed = 0;
    
    Font guiFont;
    Font smallGuiFont;
    Font smallHudGuiFont;
    Texture2D crosshairTexture;
    Texture2D hudPanelTexture;

    float fps = 0;
    float fpsTime = 2.0f;

    int monstersKilled = 0;

    BloomEffect bloom = null;
    FXAA fxaa = null;
    SSAO ssao = null;

    void Start()
	{
        bloom = camera.GetMonoBehaviour<BloomEffect>();
        fxaa = camera.GetMonoBehaviour<FXAA>();
        ssao = camera.GetMonoBehaviour<SSAO>();

        currentShootInterval = shootInterval;

        if (muzzleFlash != null)
            muzzleFlash.enabled = false;

        if (weaponObject != null)
            DefaultPosition = weaponObject.transform.localPosition;

        _speed = bobSpeed;
        pSpeed = bobSpeed;

        //---Fonts---//
        guiFont = GUI.AddFont("Fonts/Arial.ttf", 38);
        smallGuiFont = GUI.AddFont("Fonts/ariblk.ttf", 24);
        smallHudGuiFont = GUI.AddFont("Fonts/Arial.ttf", 14);

        //---Textures---//
        crosshairTexture = Texture2D.LoadFromFile("UI/Images/Crosshair.psd");
        hudPanelTexture = Texture2D.LoadFromFile("UI/Images/HUD_Panel.psd");
    }
		
	void Update(float deltaTime)
	{
        if (Input.GetKeyDown(ScanCode.Escape))
        {
            Application.Quit();
        }

        Vector3 forward = rigidbody.rotation * new Vector3(0.0f, 0.0f, 1.0f);
        Vector3 left = rigidbody.rotation * new Vector3(1.0f, 0.0f, 0.0f);

        if (Input.GetKey(ScanCode.LeftShift))
        {
            runSpeed = runMultiplayer;
        }
        else
        {
            runSpeed = 1.0f;
        }

        rigidbody.angularVelocity = new Vector3(0, 0, 0);

        forwardSpeed = 0;
        leftSpeed = 0;

        if (Input.GetKey(ScanCode.W) || Input.GetKey(ScanCode.UpArrow))
        {
            forwardSpeed = -(walkSpeed * runSpeed);
        }

        if (Input.GetKey(ScanCode.S) || Input.GetKey(ScanCode.DownArrow))
        {
            forwardSpeed = walkSpeed * runSpeed;
        }

        if (Input.GetKey(ScanCode.A) || Input.GetKey(ScanCode.LeftArrow))
        {
            leftSpeed = -(walkSpeed * runSpeed);
        }

        if (Input.GetKey(ScanCode.D) || Input.GetKey(ScanCode.RightArrow))
        {
            leftSpeed = walkSpeed * runSpeed;
        }

        if (Input.GetKeyDown(ScanCode.PageDown))
            Time.timeScale -= 0.1f;
        if (Input.GetKeyDown(ScanCode.PageUp))
            Time.timeScale += 0.1f;

        Vector2 mouse = Input.cursorDirection;
        MovementX += mouse.x;
        MovementY += mouse.y;

        if (MovementX > limitMax) MovementX = limitMax;
        if (MovementX < limitMin) MovementX = limitMin;
        if (MovementY > limitMax) MovementY = limitMax;
        if (MovementY < limitMin) MovementY = limitMin;
        
        //First way to handle key down events
        if (Input.GetKeyDown(ScanCode.Space))
        {
            if (isGrounded)
            {
                jumpSpeed = jumpStrength;
                jumpTimer = 3.0f;
            }
        }

        rigidbody.linearVelocity = forward * forwardSpeed + left * leftSpeed + new Vector3(0, 1, 0) * jumpSpeed;
            
        //Detect collision with ground
        isGrounded = false;

        for (int i = -2; i < 2; ++i)
        {
            for (int j = -2; j < 2; ++j)
            {
                RaycastHit hit = Physics.Raycast(transform.position + new Vector3((float)(i * 0.25f), -0.5f, (float)(j * 0.25f)), transform.position + new Vector3((float)(i * 0.25f), -9.5f, (float)(j * 0.25f)));
                if (hit.hasHit)
                {
                    isGrounded = true;
                    break;
                }
            }
        }

        //Fire
        if (weaponObject != null)
        {
            if (isShooting)
            {
                if (!reloading)
                {
                    if (!weaponObject.animation.IsPlaying("Reload"))
                    {
                        if (ammoMagazineCurrent > 0)
                        {
                            if (currentShootInterval >= shootInterval)
                            {
                                if (weaponObject != null && weaponObject.animation != null && !weaponObject.animation.IsPlaying("Fire"))
                                {
                                    weaponObject.animation.Play("Fire");
                                    ammoMagazineCurrent -= 1;
                                    GameObject pat = Prefab.Instantiate(patronPrefab);
                                    pat.transform.position = patronSlot.transform.position;
                                    pat.GetComponent<Rigidbody>().AddForce(transform.rotation * new Vector3(-1, 0, 0) * 55, new Vector3(0, 0, 0));

                                    if (muzzleFlash != null)
                                    {
                                        muzzleFlash.transform.rotation *= Quaternion.Euler(new Vector3(0, 0, 5));
                                        muzzleFlash.enabled = true;
                                        currentMuzzleTimer = 0.0f;
                                    }

                                    if (audioSource != null)
                                        audioSource.Play();

                                    //Check shoot raycast
                                    for (int i = 0; i < 8; ++i)
                                    {
                                        float _min = -35, _max = 35;
                                        float _r = Random.Range(-_min, _max);
                                        float _r1 = Random.Range(-_min, _max);
                                        float _r2 = Random.Range(-_min, _max);
                                        Vector3 rand = new Vector3(_r, _r1, _r2);

                                        RaycastHit shootHit = Physics.Raycast(camera.transform.position, camera.transform.position + ((camera.transform.rotation * new Vector3(0.0f, 0.0f, -1.0f)) * 1000.0f) + rand);
                                        if (shootHit.hasHit)
                                        {
                                            if (shootHit.rigidbody != null)
                                            {
                                                Vector3 dir = shootHit.hitPoint - camera.transform.position;
                                                dir.Normalize();

                                                DoDamage(shootHit.rigidbody.gameObject.transform, 25, shootHit.hitPoint, shootHit.worldNormal);
                                                shootHit.rigidbody.AddForce(dir * 1200.0f, Vector3.zero);
                                                //MessageBox.Show(shootHit.rigidbody.gameObject.name);
                                            }
                                        }
                                    }
                                }

                                currentShootInterval = 0.0f;
                            }
                            else
                            {
                                currentShootInterval += deltaTime * 65;
                            }
                        }
                        else
                        {
                            Reload();
                        }
                    }
                }
                else
                {
                    reloading = false;
                }
            }
            else
            {
                if (weaponObject != null && weaponObject.animation != null && !reloading)
                {
                    if (!weaponObject.animation.IsPlaying("Idle") && !weaponObject.animation.IsPlaying("Fire"))
                        weaponObject.animation.CrossFade("Idle");
                }
            }
        }
    }

    void FixedUpdate()
    {
        if (forwardSpeed != 0 || leftSpeed != 0)
        {
            m_Time += pSpeed;

            m_Time2 += _speed;

            sinx = Mathf.Sin(m_Time2) * 0.025f;
            cosx = Mathf.Cos(m_Time) * 0.025f;
        }
        else
        {
            sinx = Mathf.Lerp(sinx, 0, 0.5f);
            cosx = Mathf.Lerp(cosx, 0, 0.5f);
        }

        MovementX = Mathf.Lerp(MovementX, 0, 0.15f);
        MovementY = Mathf.Lerp(MovementY, 0, 0.15f);

        newGunPosition = new Vector3(DefaultPosition.x - MovementX, DefaultPosition.y + MovementY + cosx, DefaultPosition.z);

        if (weaponObject != null)
            weaponObject.transform.localPosition = Vector3.Lerp(weaponObject.transform.localPosition, newGunPosition, MovementSpeed * 0.2f);

        if (!isGrounded)
        {
            if (jumpSpeed > 0)
            {
                jumpSpeed -= 0.7f;
            }
            else
            {
                jumpSpeed -= 0.7f;
            }
        }
        else
        {
            if (jumpTimer == 0.0f)
                jumpSpeed = 0;
        }

        if (jumpTimer > 0)
            jumpTimer -= 0.1f;
        else
            jumpTimer = 0.0f;

        if (muzzleFlash != null)
        {
            if (muzzleFlash.enabled)
            {
                if (currentMuzzleTimer < muzzleTimer)
                {
                    currentMuzzleTimer += 0.1f;
                }
                else
                {
                    muzzleFlash.enabled = false;
                }
            }
        }

        //Count fps
        if (fpsTime > 0)
        {
            fpsTime -= 0.1f;
        }
        else
        {
            fps = Time.frameRate;
            fpsTime = 2.0f;
        }

        if (reloading)
        {
            if (!weaponObject.animation.IsPlaying("Reload"))
            {
                if (ammoMagazineCurrent < ammoMagazineSize)
                {
                    if (ammoMagazineTotal > 0)
                    {
                        ammoMagazineTotal -= 1;
                        ammoMagazineCurrent += 1;

                        if (ammoMagazineCurrent < ammoMagazineSize)
                        {
                            weaponObject.animation.CrossFade("Reload");
                            weaponObject.audioSource.Play();
                        }
                    }
                    else
                    {
                        reloading = false;
                    }
                }
                else
                {
                    reloading = false;
                }
            }
        }
        else
        {
            if (!weaponObject.animation.IsPlaying("Fire"))
            {
                if (ammoMagazineCurrent == 0 && ammoMagazineTotal > 0)
                {
                    Reload();
                }
            }
        }
    }

    //Second way to handle key down events
    void KeyDown(ScanCode key)
    {
        if (key == ScanCode.R)
        {
            Reload();
        }

        if (key == ScanCode.P)
        {
            if (bloom != null)
                bloom.enabled = !bloom.enabled;
        }

        if (key == ScanCode.O)
        {
            if (fxaa != null)
                fxaa.enabled = !fxaa.enabled;
        }

        if (key == ScanCode.I)
        {
            if (ssao != null)
                ssao.enabled = !ssao.enabled;
        }

        if (key == ScanCode.M)
        {
            GameObject monster = GameObject.GetByUniqueName("RootNode_Object_1");
            monster.enabled = !monster.enabled;
        }
    }

    void Reload()
    {
        if (ammoMagazineCurrent < ammoMagazineSize && ammoMagazineTotal > 0)
        {
            if (weaponObject != null && weaponObject.animation != null)
            {
                if (!reloading && !weaponObject.animation.IsPlaying("Fire"))
                {
                    reloading = true;
                    weaponObject.animation.CrossFade("Reload");
                    weaponObject.audioSource.Play();
                }
            }
        }
    }

    void MouseDown(int button)
    {
        if (button == 0)
        {
            isShooting = true;
            currentShootInterval = shootInterval;
        }
    }

    void MouseUp(int button)
    {
        if (button == 0)
        {
            isShooting = false;
        }
    }

    void DoDamage(Transform from, int damage, Vector3 point, Vector3 normal)
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
                DoDamage(from.parent, damage, point, normal);

                if (from.gameObject.GetComponent<Rigidbody>() != null)
                {
                    if (from.gameObject.GetComponent<Rigidbody>().mass == 0)
                    {
                        float _min = 0.1f, _max = 0.2f;
                        float _r = Random.Range(_min, _max);

                        Prefab.Instantiate(bulletHolePrefab, point + normal * _r, rot);
                    }
                }
            }
            else
            {
                Prefab.Instantiate(sparksPrefab, point, rot);
            }
        }
    }

    public void ReceiveDamage(float damage)
    {
        if (!enabled)
            return;

        Health -= damage;
        if (Health <= 0)
        {
            enabled = false;
            muzzleFlash.enabled = false;
            weaponObject.enabled = false;

            Cursor.locked = false;

            CameraController cam = camera.GetMonoBehaviour<CameraController>();
            if (cam != null)
                cam.enabled = false;

            Prefab.Instantiate(restartMenuPrefab);
        }
    }

    public void MonsterKill()
    {
        monstersKilled += 1;
    }

    void OnGUI()
    {
        if (enabled)
        {
            //---FPS Counter---//
            GUI.SetFont(smallGuiFont);
            GUI.Label("FPS: " + fps.ToString(), new Vector2(20, 20), new Color(0, 1, 0, 0.7f));
            GUI.UnsetFont();

            //---Monsters killed counter---//
            GUI.SetFont(smallGuiFont);
            GUI.Label("Monsters killed: " + monstersKilled.ToString(), new Vector2(20, 50), new Color(1, 0, 1, 0.7f));
            GUI.UnsetFont();

            //---GUI Info---//
            //Health
            GUI.Image(hudPanelTexture, new Vector2(20, Screen.height - 20 - hudPanelTexture.height), new Vector2(hudPanelTexture.width, hudPanelTexture.height));
            //Ammo
            GUI.Image(hudPanelTexture, new Vector2(Screen.width - hudPanelTexture.width - 20, Screen.height - 20 - hudPanelTexture.height), new Vector2(hudPanelTexture.width, hudPanelTexture.height));

            GUI.SetFont(smallHudGuiFont);
            GUI.Label("HEALTH", new Vector2(30, Screen.height - 43), new Color(1, 1, 0, 0.8f));
            GUI.UnsetFont();

            GUI.SetFont(guiFont);
            GUI.Label(Health.ToString(), new Vector2(90, Screen.height - 20 - hudPanelTexture.height / 2 - 19), new Color(1, 1, 0, 0.8f));
            GUI.Label(ammoMagazineCurrent.ToString() + "/" + ammoMagazineTotal.ToString(), new Vector2(Screen.width - 120, Screen.height - 20 - hudPanelTexture.height / 2 - 19), new Color(1, 1, 0, 0.8f));
            GUI.UnsetFont();

            GUI.Image(crosshairTexture, new Vector2(Screen.width / 2 - 25, Screen.height / 2 - 25), new Vector2(50, 50));

            //Post effects info

            GUI.SetFont(smallGuiFont);

            //---SSAO---//
            if (ssao.enabled)
                GUI.Label("SSAO: ON", new Vector2(20, 90), new Color(0, 1, 0, 0.7f));
            else
                GUI.Label("SSAO: OFF", new Vector2(20, 90), new Color(1, 0, 0, 0.7f));

            //---FXAA---//
            if (fxaa.enabled)
                GUI.Label("FXAA: ON", new Vector2(20, 120), new Color(0, 1, 0, 0.7f));
            else
                GUI.Label("FXAA: OFF", new Vector2(20, 120), new Color(1, 0, 0, 0.7f));

            //---Bloom---//
            if (bloom.enabled)
                GUI.Label("Bloom: ON", new Vector2(20, 150), new Color(0, 1, 0, 0.7f));
            else
                GUI.Label("Bloom: OFF", new Vector2(20, 150), new Color(1, 0, 0, 0.7f));

            GUI.UnsetFont();

            GUI.Label("Time scale: " + Time.timeScale.ToString(), new Vector2(Screen.width - 140, 20));
        }
    }
}