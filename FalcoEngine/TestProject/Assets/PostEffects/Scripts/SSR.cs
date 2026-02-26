using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class SSR : PostEffect
{
	void Start()
	{
        Create("Effects/SSR");

        PostEffectTechnique tech = CreateTechnique();
        PostEffectTexture rt_output = tech.CreateTexture("rt_output");
        PostEffectTexture reflections = tech.CreateTexture("reflections");
        PostEffectTexture rt0 = tech.CreateTexture("rt0");
        rt0.referenceTarget = "DeferredShading/GBuffer";
        rt0.referenceName = "mrt_output";

        rt_output.width = 0; //Target width
        rt_output.height = 0; //Target height

        rt0.width = 0;
        rt0.height = 0;

        reflections.targetWidthScaled = 0.5f;
        reflections.targetHeightScaled = 0.5f;
        //reflections.width = 0;
        //reflections.height = 0;

        PostEffectTargetPass targetPass1 = tech.CreateTargetPass();
        targetPass1.inputMode = PostEffectTargetPass.InputMode.Previous;
        targetPass1.output = "rt_output";

        //Output
        PostEffectTargetPass targetPass2 = tech.CreateTargetPass();
        targetPass2.inputMode = PostEffectTargetPass.InputMode.None;
        targetPass2.output = "reflections";

        PostEffectPass output = targetPass2.CreatePass();
        output.materialName = "PostEffects/Materials/SSR/SSR.material";
        output.SetInput(0, "rt0", 0);
        output.SetInput(1, "rt0", 1);
        output.SetInput(2, "rt0", 2);
        //output.identifier = 77;

        PostEffectTargetPass outputPass = tech.outputPass;
        outputPass.inputMode = PostEffectTargetPass.InputMode.None;

        PostEffectPass output2 = outputPass.CreatePass();
        output2.materialName = "PostEffects/Materials/SSR/SSR_Blur.material";
        output2.SetInput(0, "reflections");
        output2.SetInput(1, "rt0", 1);
        output2.SetInput(2, "rt_output");

        //Enable post effect
        enabled = true;

        //onMaterialRender += SSR_onMaterialRender;
    }

    private void SSR_onMaterialRender(int pass_id, Material material)
    {
        //if (pass_id != 77)
        //    return;

        //Vector3 camPos = camera.gameObject.transform.position;

        //material.SetParameter("cameraPosition", camPos);
    }
}
