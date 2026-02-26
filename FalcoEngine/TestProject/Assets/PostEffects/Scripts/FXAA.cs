using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class FXAA : PostEffect
{
	void Start()
	{
        Create("Effects/FXAA");

        PostEffectTechnique tech = CreateTechnique();
        PostEffectTexture rt_output = tech.CreateTexture("rt_output");
        PostEffectTexture rt0 = tech.CreateTexture("rt0");

        rt_output.width = 0; //Target width
        rt_output.height = 0; //Target height

        rt0.width = 0;
        rt0.height = 0;

        PostEffectTargetPass targetPass1 = tech.CreateTargetPass();
        targetPass1.inputMode = PostEffectTargetPass.InputMode.Previous;
        targetPass1.output = "rt_output";

        //Output
        PostEffectTargetPass outputPass = tech.outputPass;
        outputPass.inputMode = PostEffectTargetPass.InputMode.None;

        PostEffectPass output = outputPass.CreatePass();
        output.materialName = "PostEffects/Materials/FXAA/FXAA.material";
        output.SetInput(0, "rt_output");

        //Enable post effect
        enabled = true;
    }
}
