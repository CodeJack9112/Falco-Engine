using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class UIEvents : MonoBehaviour
{
	public GameObject text1;
	public GameObject text2;
	public GameObject text3;
	public GameObject button;

	void Start()
	{
		((Button)button.attachedObject).onMouseUp += (UIElement sender) =>
		{
			((Text)text3.attachedObject).text = "Text 3 from button 3";
			Debug.Log("Text 3 from button 3");
		};
	}

	void Update(float deltaTime)
	{
		
	}

	public void setText1()
	{
		((Text)text1.attachedObject).text = "Text 1 from button 1";
		Debug.Log("Text 1 from button 1");
	}

	public void setText2()
	{
		((Text)text2.attachedObject).text = "Text 2 from button 2";
		Debug.Log("Text 2 from button 2");
	}
}
