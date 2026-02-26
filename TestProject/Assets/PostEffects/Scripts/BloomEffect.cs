using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class BloomEffect : PostEffect
{
	void Start()
	{
        Create("Effects/Bloom");

        PostEffectTechnique tech = CreateTechnique();
        PostEffectTexture rt_output = tech.CreateTexture("rt_output");
        PostEffectTexture rt0 = tech.CreateTexture("rt0");
        PostEffectTexture rt1 = tech.CreateTexture("rt1");

        rt_output.width = 0; //Target width
        rt_output.height = 0; //Target height

        rt0.targetWidthScaled = 0.25f;
        rt0.targetHeightScaled = 0.25f;

        rt1.targetWidthScaled = 0.25f;
        rt1.targetHeightScaled = 0.25f;

        PostEffectTargetPass targetPass1 = tech.CreateTargetPass();
        targetPass1.inputMode = PostEffectTargetPass.InputMode.Previous;
        targetPass1.output = "rt_output";

        //Target 0
        PostEffectTargetPass targetPass2 = tech.CreateTargetPass();
        targetPass2.inputMode = PostEffectTargetPass.InputMode.None;
        targetPass2.output = "rt0";

        PostEffectPass effectPass1 = targetPass2.CreatePass();
        effectPass1.materialName = "PostEffects/Materials/Bloom/BloomBrightPass.material";
        effectPass1.SetInput(0, "rt_output");

        //Target 1
        PostEffectTargetPass targetPass3 = tech.CreateTargetPass();
        targetPass3.inputMode = PostEffectTargetPass.InputMode.None;
        targetPass3.output = "rt1";

        PostEffectPass effectPass2 = targetPass3.CreatePass();
        effectPass2.materialName = "PostEffects/Materials/Bloom/BloomBlurV.material";
        effectPass2.SetInput(0, "rt0");

        //Target 2
        PostEffectTargetPass targetPass4 = tech.CreateTargetPass();
        targetPass4.inputMode = PostEffectTargetPass.InputMode.None;
        targetPass4.output = "rt0";

        PostEffectPass effectPass3 = targetPass4.CreatePass();
        effectPass3.materialName = "PostEffects/Materials/Bloom/BloomBlurH.material";
        effectPass3.SetInput(0, "rt1");

        //Output
        PostEffectTargetPass outputPass = tech.outputPass;
        outputPass.inputMode = PostEffectTargetPass.InputMode.None;

        PostEffectPass output = outputPass.CreatePass();
        output.materialName = "PostEffects/Materials/Bloom/BloomEffect.material";
        output.SetInput(0, "rt_output");
        output.SetInput(1, "rt0");

        //Enable post effect
        enabled = true;
    }
}
