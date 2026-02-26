using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class SunShafts : PostEffect
{
    public GameObject lightSource;

	void Start()
	{
        Create("Effects/SunShafts");

        PostEffectTechnique tech = CreateTechnique();
        PostEffectTexture rt_output = tech.CreateTexture("rt_output");
        PostEffectTexture rt_output0 = tech.CreateTexture("rt_output0");
        PostEffectTexture rt_output1 = tech.CreateTexture("rt_output1");
        PostEffectTexture mrt = tech.CreateTexture("mrt");
        mrt.referenceTarget = "DeferredShading/GBuffer";
        mrt.referenceName = "mrt_output";

        rt_output.width = 0;
        rt_output.height = 0;

        rt_output0.targetWidthScaled = 0.5f;
        rt_output0.targetHeightScaled = 0.5f;

        rt_output1.targetWidthScaled = 0.5f;
        rt_output1.targetHeightScaled = 0.5f;

        PostEffectTargetPass targetPass1 = tech.CreateTargetPass();
        targetPass1.inputMode = PostEffectTargetPass.InputMode.Previous;
        targetPass1.output = "rt_output";

        //Render sun
        PostEffectTargetPass outputPass0 = tech.CreateTargetPass();
        outputPass0.inputMode = PostEffectTargetPass.InputMode.None;
        outputPass0.output = "rt_output0";

        PostEffectPass output0 = outputPass0.CreatePass();
        output0.identifier = 79;
        output0.materialName = "PostEffects/Materials/SunShafts/SunShaftsFirstPass.material";

        //Render sun with occludes
        PostEffectTargetPass outputPass1 = tech.CreateTargetPass();
        outputPass1.inputMode = PostEffectTargetPass.InputMode.None;
        outputPass1.output = "rt_output1";

        PostEffectPass output1 = outputPass1.CreatePass();
        output1.materialName = "PostEffects/Materials/SunShafts/SunShaftsCombine.material";        
        output1.SetInput(0, "rt_output0");
        output1.SetInput(1, "mrt", 1);

        //Output
        PostEffectTargetPass outputPass = tech.outputPass;
        outputPass.inputMode = PostEffectTargetPass.InputMode.None;

        PostEffectPass output = outputPass.CreatePass();
        output.identifier = 80;
        output.materialName = "PostEffects/Materials/SunShafts/SunShafts.material";
        output.SetInput(0, "rt_output");
        output.SetInput(1, "rt_output1");

        //Render event
        onMaterialRender += SunShafts_onMaterialRender;

        //Enable post effect
        enabled = true;
    }

    private void SunShafts_onMaterialRender(int pass_id, Material material)
    {
        if (pass_id != 79 && pass_id != 80)
            return;

        Vector3 lightPos = camera.gameObject.transform.position + lightSource.transform.position;
        Vector3 lightScreenPos = camera.WorldToScreenPoint(lightPos);

        material.SetParameter("lightPositionOnScreen", lightScreenPos);
    }
}
