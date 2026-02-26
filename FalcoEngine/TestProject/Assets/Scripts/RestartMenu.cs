using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class RestartMenu : MonoBehaviour
{
    Font menuGuiFont;

    void Start()
	{
        menuGuiFont = GUI.AddFont("Fonts/Roboto-Regular.ttf", 20);
    }
	
	void Update(float deltaTime)
	{
		
	}

    void OnGUI()
    {
        GUI.SetFont(menuGuiFont);
        GUI.BeginWindow("Game Over", new Vector2(Screen.width / 2 - 100, 150), new Vector2(200, 150), 0.8f, false, WindowFlags.NoResize | WindowFlags.NoCollapse);

        if (GUI.Button("Restart", new Vector2(50, 45), new Vector2(100, 30)))
        {
            SceneManager.LoadScene(SceneManager.loadedScene);
        }

        if (GUI.Button("Quit", new Vector2(50, 95), new Vector2(100, 30)))
        {
            Application.Quit();
        }
           
        GUI.EndWindow();
        GUI.UnsetFont();
    }
}
