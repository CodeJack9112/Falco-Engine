using FalcoEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class SSAO : PostEffect
{
    public int samplesCount = 6;
    public float radius = 0.5125f;

    void Start()
	{
        //Create post effect
        Create("Effects/SSAO");

        //Create textures
        PostEffectTechnique tech = CreateTechnique();

        PostEffectTexture geom = tech.CreateTexture("geom");
        geom.referenceTarget = "DeferredShading/GBuffer";
        geom.referenceName = "mrt_output";

        PostEffectTexture scene = tech.CreateTexture("scene");
        scene.width = 0;
        scene.height = 0;

        PostEffectTexture ssao = tech.CreateTexture("ssao");
        ssao.targetWidthScaled = 0.5f;
        ssao.targetHeightScaled = 0.5f;

        PostEffectTexture ssaoBlurX = tech.CreateTexture("ssaoBlurX");
        ssaoBlurX.width = 0;
        ssaoBlurX.height = 0;

        PostEffectTexture ssaoBlurY = tech.CreateTexture("ssaoBlurY");
        ssaoBlurY.width = 0;
        ssaoBlurY.height = 0;

        //Create passes
        PostEffectTargetPass previous = tech.CreateTargetPass();
        previous.inputMode = PostEffectTargetPass.InputMode.Previous;
        previous.output = "scene";

        //SSAO
        PostEffectTargetPass ssaoPass = tech.CreateTargetPass();
        ssaoPass.inputMode = PostEffectTargetPass.InputMode.None;
        ssaoPass.output = "ssao";
        PostEffectPass clearPass = ssaoPass.CreatePass(); //Clear pass
        clearPass.passType = PostEffectPass.PassType.Clear;
        PostEffectPass ssaoRender = ssaoPass.CreatePass();
        ssaoRender.passType = PostEffectPass.PassType.RenderQuad;
        ssaoRender.identifier = 42;
        ssaoRender.materialName = "PostEffects/Materials/SSAO/SSAO.material";
        ssaoRender.SetInput(0, "geom", 1);
        ssaoRender.SetInput(1, "geom", 6);

        //BlurX
        PostEffectTargetPass blurXPass = tech.CreateTargetPass();
        blurXPass.inputMode = PostEffectTargetPass.InputMode.None;
        blurXPass.output = "ssaoBlurX";
        PostEffectPass blurXClearPass = blurXPass.CreatePass();
        blurXClearPass.passType = PostEffectPass.PassType.Clear;

        PostEffectPass blurXRender = blurXPass.CreatePass();
        blurXRender.passType = PostEffectPass.PassType.RenderQuad;
        blurXRender.identifier = 43;
        blurXRender.materialName = "PostEffects/Materials/SSAO/SSAOBlurX.material";
        blurXRender.SetInput(0, "ssao");
        blurXRender.SetInput(1, "geom", 1);

        //BlurY
        PostEffectTargetPass blurYPass = tech.CreateTargetPass();
        blurYPass.inputMode = PostEffectTargetPass.InputMode.None;
        blurYPass.output = "ssaoBlurY";
        PostEffectPass blurYClearPass = blurYPass.CreatePass();
        blurYClearPass.passType = PostEffectPass.PassType.Clear;

        PostEffectPass blurYRender = blurYPass.CreatePass();
        blurYRender.passType = PostEffectPass.PassType.RenderQuad;
        blurYRender.identifier = 43;
        blurYRender.materialName = "PostEffects/Materials/SSAO/SSAOBlurY.material";
        blurYRender.SetInput(0, "ssaoBlurX");
        blurYRender.SetInput(1, "geom", 1);

        //Output
        PostEffectTargetPass output = tech.outputPass;
        output.inputMode = PostEffectTargetPass.InputMode.None;
        PostEffectPass outPass = output.CreatePass();
        outPass.passType = PostEffectPass.PassType.RenderQuad;
        outPass.materialName = "PostEffects/Materials/SSAO/SSAOModulate.material";
        outPass.SetInput(0, "scene");
        outPass.SetInput(1, "ssaoBlurY");

        onMaterialRender += SSAO_onMaterialRender;

        enabled = true;
    }

    private void SSAO_onMaterialRender(int pass_id, Material material)
    {
        if (pass_id != 42) // Not SSAO, return
            return;

        // set the projection matrix we need
        Matrix4 CLIP_SPACE_TO_IMAGE_SPACE = new Matrix4(
            0.5f, 0f, 0f, 0.5f,
            0f, -0.5f, 0f, 0.5f,
            0f, 0f, 1f, 0f,
            0f, 0f, 0f, 1f);

        Matrix4 ptMat = CLIP_SPACE_TO_IMAGE_SPACE * camera.GetProjectionMatrixWithRSDepth();

        material.SetParameter("ptMat", ptMat);
        material.SetParameter("samplesCount", samplesCount);
        material.SetParameter("radius", radius);
    }
}
