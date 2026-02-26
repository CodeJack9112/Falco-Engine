using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class Fog : PostEffect
{
    public float fogStart = 10.0f;
    public float fogEnd = 2000.0f;
    public float fogDensity = 0.7f;
    public float fogTransition = 0.004f;
    public Vector3 fogColor = new Vector3(0.8f, 0.8f, 0.8f);

	void Start()
	{
        Create("Effects/Fog");

        PostEffectTechnique tech = CreateTechnique();
        PostEffectTexture rt_output = tech.CreateTexture("rt_output");
        PostEffectTexture rt0 = tech.CreateTexture("rt0");
        rt0.referenceTarget = "DeferredShading/GBuffer";
        rt0.referenceName = "mrt_output";

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
        output.materialName = "PostEffects/Materials/Fog/Fog.material";
        output.SetInput(0, "rt_output");
        output.SetInput(1, "rt0", 1);
        output.identifier = 9;

        //GUI Pass
        PostEffectPass gui = outputPass.CreatePass();
        gui.passType = PostEffectPass.PassType.RenderScene;
        gui.SetFirstRenderQueue(80);

        //Enable post effect
        enabled = true;

        onMaterialRender += Fog_onMaterialRender;
    }

    private void Fog_onMaterialRender(int pass_id, Material material)
    {
        if (pass_id == 9)
        {
            material.SetParameter("Near", fogStart);
            material.SetParameter("Far", fogEnd);
            material.SetParameter("Density", fogDensity);
            material.SetParameter("LinearGradient", fogTransition);
            material.SetParameter("FogColor", new Vector4(fogColor.x, fogColor.y, fogColor.z, 1.0f));
        }
    }
}
