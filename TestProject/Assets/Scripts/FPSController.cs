using FalcoEngine;

public class FPSController : MonoBehaviour
{
    public int health = 100;
    public float jumpStrength = 24.0f;
    public float walkSpeed = 25.0f;
    public float runMultiplayer = 2.0f;
    public float gravity = -0.5f;
    public string crossHImage;
    public string crossVImage;
    public string bulletHolePrefab;
    public string sparksPrefab;

    bool isGrounded = false;
    float runSpeed = 1.0f;

    float forwardSpeed = 0;
    float leftSpeed = 0;

    float fallSpeed = 0.0f;

    CapsuleCollider collider;

    Weapon[] weapons;
    Weapon currentWeapon = null;
    int currentWeaponIndex = -1;

    Font hudFont;
    Font smallFont;
    Texture2D weaponPanel;

    float wpa = 0.0f;

    void Start()
    {
        hudFont = GUI.AddFont("Fonts/Roboto-Regular.ttf", 35);
        smallFont = GUI.AddFont("Fonts/Roboto-Regular.ttf", 15);

        weaponPanel = Texture2D.LoadFromFile("UI/Panel_256.psd");

        collider = GetComponent<CapsuleCollider>();
        weapons = FindObjectsOfType<Weapon>(true);

        foreach (Weapon weapon in weapons)
            weapon.gameObject.enabled = false;

        SwitchWeapon(0);
    }

    void Update(float deltaTime)
    {
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

        if (Input.GetMouseWheel().y > 0)
        {
            wpa = 20.0f;

            if (currentWeapon != null)
            {
                Weapon curWeapon = currentWeapon.GetMonoBehaviour<Weapon>();

                if (!curWeapon.IsFiring() && !curWeapon.IsReloading()/* && !curWeapon.IsGettingReady()*/)
                {
                    int cwi = currentWeaponIndex;

                    int idx = currentWeaponIndex - 1;
                    if (idx < 0)
                        idx = weapons.Length - 1;

                    while (weapons[idx].enabled == false)
                    {
                        idx--;
                        if (idx < 0)
                            idx = weapons.Length - 1;
                    }

                    if (cwi != idx)
                    {
                        SwitchWeapon(idx);
                    }
                }
            }
        }

        if (Input.GetMouseWheel().y < 0)
        {
            wpa = 20.0f;

            if (currentWeapon != null)
            {
                Weapon curWeapon = currentWeapon.GetMonoBehaviour<Weapon>();

                if (!curWeapon.IsFiring() && !curWeapon.IsReloading()/* && !curWeapon.IsGettingReady()*/)
                {
                    int cwi = currentWeaponIndex;

                    int idx = currentWeaponIndex + 1;
                    if (idx > weapons.Length - 1)
                        idx = 0;

                    while (weapons[idx].enabled == false)
                    {
                        idx++;
                        if (idx > weapons.Length - 1)
                            idx = 0;
                    }

                    if (cwi != idx)
                    {
                        SwitchWeapon(idx);
                    }
                }
            }
        }

        if (Input.GetKeyDown(ScanCode.Alpha1))
        {
            if (weapons[0].enabled)
            {
                wpa = 20.0f;
                SwitchWeapon(0);
            }
        }
        if (Input.GetKeyDown(ScanCode.Alpha2))
        {
            if (weapons[1].enabled)
            {
                wpa = 20.0f;
                SwitchWeapon(1);
            }
        }
        if (Input.GetKeyDown(ScanCode.Alpha3))
        {
            if (weapons[2].enabled)
            {
                wpa = 20.0f;
                SwitchWeapon(2);
            }
        }
        if (Input.GetKeyDown(ScanCode.Alpha4))
        {
            if (weapons[3].enabled)
            {
                wpa = 20.0f;
                SwitchWeapon(3);
            }
        }
        if (Input.GetKeyDown(ScanCode.Alpha5))
        {
            if (weapons[4].enabled)
            {
                wpa = 20.0f;
                SwitchWeapon(4);
            }
        }
        if (Input.GetKeyDown(ScanCode.Alpha6))
        {
            if (weapons[5].enabled)
            {
                wpa = 20.0f;
                SwitchWeapon(5);
            }
        }
        if (Input.GetKeyDown(ScanCode.Alpha7))
        {
            if (weapons[6].enabled)
            {
                wpa = 20.0f;
                SwitchWeapon(6);
            }
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

        rigidbody.linearVelocity = forward * forwardSpeed + left * leftSpeed + new Vector3(0, fallSpeed, 0);

        //First way to handle key down events
        if (Input.GetKeyDown(ScanCode.Space))
        {
            if (isGrounded)
            {
                fallSpeed = jumpStrength;
            }
        }

        if (Input.GetKeyDown(ScanCode.Escape))
            Application.Quit();
    }

    public void SwitchWeapon(int index)
    {
        if (index == currentWeaponIndex)
            return;

        bool canSwitch = false;

        if (currentWeapon != null)
        {
            //Weapon curWeapon = currentWeapon.GetMonoBehaviour<Weapon>();
            //if (!curWeapon.IsFiring() && !curWeapon.IsReloading() && !curWeapon.IsGettingReady())
                canSwitch = true;
        }
        else
            canSwitch = true;

        if (canSwitch)
        {
            if (currentWeapon != null)
                currentWeapon.gameObject.enabled = false;

            if (index < weapons.Length && index >= 0)
            {
                if (weapons[index].enabled)
                {
                    currentWeaponIndex = index;
                    currentWeapon = weapons[index];
                    currentWeapon.gameObject.enabled = true;
                    currentWeapon.GetMonoBehaviour<Weapon>().Ready();
                }
            }
        }
    }

    void FixedUpdate()
    {
        if (!isGrounded)
        {
            if (fallSpeed > -25.0f)
            {
                fallSpeed += gravity;
            }
        }

        float colH = collider.height / 2.0f + 1.0f;
        Collider[] colliders = Physics.OverlapSphere(transform.position - new Vector3(0, colH, 0), 2.0f);

        int l = colliders.Length;

        foreach (Collider c in colliders)
        {
            if (c != null)
            {
                if (c.gameObject == gameObject)
                {
                    l -= 1;
                }
            }
        }

        if (isGrounded)
        {
            if (l == 0)
            {
                isGrounded = false;
            }
        }
        else
        {
            if (l > 0)
            {
                isGrounded = true;
                fallSpeed = 0.0f;
            }
        }

        if (wpa > 0)
            wpa -= 0.1f;
        else
            wpa = 0.0f;
    }

    public Weapon[] GetWeapons()
    {
        return weapons;
    }

    void OnGUI()
    {
        Color color = new Color(0, 1, 0, 1);

        if (Time.frameRate >= 30 && Time.frameRate < 60)
            color = new Color(1, 1, 0, 1);
        if (Time.frameRate < 30)
            color = new Color(1, 0, 0, 1);

        GUI.Label("FPS: " + Time.frameRate.ToString(), new Vector2(20, 20), color);

        GUI.SetFont(hudFont);
        GUI.Label(health.ToString() + "%%", new Vector2(30, Screen.height - 60));

        if (currentWeapon != null)
        {
            GUI.Label(currentWeapon.currentMagazineSize.ToString() + "/" + currentWeapon.totalAmmo.ToString(), new Vector2(Screen.width - 130, Screen.height - 60));
        }
        GUI.UnsetFont();

        float sz = 64.0f;
        float lsz = 128.0f;
        float pad = 10.0f;
        float sx = Screen.width / 2.0f - ((6 * (sz + pad) + (lsz + pad)) / 2.0f);

        for (int i = 0; i < weapons.Length; ++i)
        {
            float _x = 0;
            float _sz = sz;

            if (currentWeaponIndex == i)
            {
                _sz = lsz;
            }
            else if (currentWeaponIndex < i)
            {
                _x = (lsz - sz);
            }

            Vector2 pos = new Vector2(sx + i * (sz + pad) + _x, 25);
            GUI.Image(weaponPanel, pos, new Vector2(_sz, _sz), new Color(0, 0, 0, 0.5f * Mathf.Clamp01(wpa)));
            GUI.SetFont(smallFont);
            GUI.Label((i + 1).ToString(), pos + new Vector2(6, 5), new Color(1, 1, 1, 0.5f * Mathf.Clamp01(wpa)));
            GUI.UnsetFont();

            if (weapons[i].icon != null)
                GUI.Image(weapons[i].icon, pos + new Vector2(6, 5), new Vector2(_sz - 16, _sz - 15), new Color(1, 1, 1, 0.5f * Mathf.Clamp01(wpa)));
        }
    }

    public bool IsMoving()
    {
        return forwardSpeed != 0 || leftSpeed != 0;
    }

    public void ReceiveDamage(float damage)
    {
        if (!enabled)
            return;

        health -= (int)damage;
        if (health <= 0)
        {
            enabled = false;
            //muzzleFlash.enabled = false;
            //weaponObject.enabled = false;

            Cursor.locked = false;

            CameraController cam = Camera.main.gameObject.GetMonoBehaviour<CameraController>();
            if (cam != null)
                cam.enabled = false;

            SwitchWeapon(-1);
            Prefab.Instantiate("Prefabs/RestartMenu.prefab");
        }
    }
}
