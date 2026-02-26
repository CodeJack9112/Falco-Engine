using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;

public class MainMenu : MonoBehaviour
{
	public GameObject btnStart;
	public GameObject btnContinue;
	public GameObject btnExit;

	void Start()
	{
		string fileName = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Arkanoid.dat");

		((Button)btnStart.attachedObject).onMouseUp += (UIElement sender) => {
			Ball.level = 0;
			Ball.points = 0;
			Ball.pointsPrev = 0;

			File.Delete(fileName);
			SceneManager.LoadScene("Scenes/Arkanoid/Level" + Ball.level + ".scene");
		};

		((Button)btnContinue.attachedObject).onMouseUp += (UIElement sender) => {

			if (File.Exists(fileName))
			{
				string[] txt = File.ReadAllText(fileName).Split('\n');
				Ball.level = int.Parse(txt[0]);
				Ball.points = int.Parse(txt[1]);
				Ball.pointsPrev = Ball.points;
			}

			SceneManager.LoadScene("Scenes/Arkanoid/Level" + Ball.level + ".scene");
		};

		((Button)btnExit.attachedObject).onMouseUp += (UIElement sender) => {
			Application.Quit();
		};

		if (File.Exists(fileName))
		{
			((Button)btnContinue.attachedObject).interactable = true;
		}
	}
	
	void Update(float deltaTime)
	{

	}
}
