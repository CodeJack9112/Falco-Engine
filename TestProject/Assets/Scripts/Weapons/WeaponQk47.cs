using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class WeaponQk47 : Weapon
{
	void Start()
	{
		Init();

		SetIcon();
	}

	void Update(float deltaTime)
	{
		UpdateFrame();
	}

	void FixedUpdate()
	{
		UpdateFixed();
	}

	void OnGUI()
	{
		DrawCross();
	}

	public override void SetIcon()
	{
		base.SetIcon();

		icon = Texture2D.LoadFromFile("Weapons/UI/icons/Qk47.png");
	}
}
