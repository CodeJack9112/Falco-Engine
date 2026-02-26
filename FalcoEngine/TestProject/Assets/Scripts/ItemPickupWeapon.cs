using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

public class ItemPickupWeapon : ItemPickup
{
	public string weaponToActivate = "";

	public override void Start()
	{
		base.Start();

		OnPickup += ItemPickupWeapon_OnPickup;
	}

	public override void Update()
	{
		base.Update();
	}

	private void ItemPickupWeapon_OnPickup()
	{
		FPSController fpsController = FindObjectOfType<FPSController>(true);
		Weapon[] weapons = fpsController.GetWeapons();
		int idx = 0;

		switch (weaponToActivate)
		{
			case "Pistol":
				WeaponPistol pistol = FindObjectOfType<WeaponPistol>(true);
				pistol.enabled = true;
				pistol.SetIcon();
				idx = Array.IndexOf(weapons, pistol);
				fpsController.SwitchWeapon(idx);
				break;

			case "HMG":
				WeaponHMG hmg = FindObjectOfType<WeaponHMG>(true);
				hmg.enabled = true;
				hmg.SetIcon();
				idx = Array.IndexOf(weapons, hmg);
				fpsController.SwitchWeapon(idx);
				break;

			case "Qk47":
				WeaponQk47 qk = FindObjectOfType<WeaponQk47>(true);
				qk.enabled = true;
				qk.SetIcon();
				idx = Array.IndexOf(weapons, qk);
				fpsController.SwitchWeapon(idx);
				break;

			case "Sniper":
				WeaponSniper sniper = FindObjectOfType<WeaponSniper>(true);
				sniper.enabled = true;
				sniper.SetIcon();
				idx = Array.IndexOf(weapons, sniper);
				fpsController.SwitchWeapon(idx);
				break;

			case "Carbine":
				WeaponCarbine carbine = FindObjectOfType<WeaponCarbine>(true);
				carbine.enabled = true;
				carbine.SetIcon();
				idx = Array.IndexOf(weapons, carbine);
				fpsController.SwitchWeapon(idx);
				break;

			case "SMG":
				WeaponSMG smg = FindObjectOfType<WeaponSMG>(true);
				smg.enabled = true;
				smg.SetIcon();
				idx = Array.IndexOf(weapons, smg);
				fpsController.SwitchWeapon(idx);
				break;

			case "Shotgun":
				WeaponShotgun shotgun = FindObjectOfType<WeaponShotgun>(true);
				shotgun.enabled = true;
				shotgun.SetIcon();
				idx = Array.IndexOf(weapons, shotgun);
				fpsController.SwitchWeapon(idx);
				break;
		}
	}
}
