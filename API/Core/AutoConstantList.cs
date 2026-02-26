using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FalcoEngine
{
    public enum AutoConstantType
    {
        /// The current world matrix
        WorldMatrix,
        /// The current world matrix, inverted
        InverseWorldMatrix,
        /** Provides transpose of world matrix.
            Equivalent to RenderMonkey's "WorldTranspose".
        */
        TransposeWorldMatrix,
        /// The current world matrix, inverted & transposed
        InverseTransposeWorldMatrix,

        /// An array of world matrices, each represented as only a 3x4 matrix (3 rows of
        /// 4columns) usually for doing hardware skinning.
        /// You should make enough entries available in your vertex program for the number of
        /// bones in use, i.e. an array of numBones*3 float4’s.
        WorldMatrixArray3x4,
        /// The current array of world matrices, used for blending
        WorldMatrixArray,
        /// The current array of world matrices transformed to an array of dual quaternions,
        /// represented as a 2x4 matrix
        WorldDualQuaternionArray2x4,
        /// The scale and shear components of the current array of world matrices
        WorldScaleShearMatrixArray3x4,

        /// The current view matrix
        ViewMatrix,
        /// The current view matrix, inverted
        InverseViewMatrix,
        /** Provides transpose of view matrix.
            Equivalent to RenderMonkey's "ViewTranspose".
        */
        TransposeViewMatrix,
        /** Provides inverse transpose of view matrix.
            Equivalent to RenderMonkey's "ViewInverseTranspose".
        */
        InverseTransposeViewMatrix,

        /// The current projection matrix
        ProjectionMatrix,
        /** Provides inverse of projection matrix.
            Equivalent to RenderMonkey's "ProjectionInverse".
        */
        InverseProjectionMatrix,
        /** Provides transpose of projection matrix.
            Equivalent to RenderMonkey's "ProjectionTranspose".
        */
        TransposeProjectionMatrix,
        /** Provides inverse transpose of projection matrix.
            Equivalent to RenderMonkey's "ProjectionInverseTranspose".
        */
        InverseTransposeProjectionMatrix,

        /// The current view & projection matrices concatenated
        ViewProjMatrix,
        /** Provides inverse of concatenated view and projection matrices.
            Equivalent to RenderMonkey's "ViewProjectionInverse".
        */
        InverseViewProjMatrix,
        /** Provides transpose of concatenated view and projection matrices.
            Equivalent to RenderMonkey's "ViewProjectionTranspose".
        */
        TransposeViewProjMatrix,
        /** Provides inverse transpose of concatenated view and projection matrices.
            Equivalent to RenderMonkey's "ViewProjectionInverseTranspose".
        */
        InverseTransposeViewProjMatrix,

        /// The current world & view matrices concatenated
        WorldViewMatrix,
        /// The current world & view matrices concatenated, then inverted
        InverseWorldViewMatrix,
        /** Provides transpose of concatenated world and view matrices.
            Equivalent to RenderMonkey's "WorldViewTranspose".
        */
        TransposeWorldViewMatrix,
        /// The current world & view matrices concatenated, then inverted & transposed
        InverseTransposeWorldViewMatrix,
        // view matrices.

        /// The current world, view & projection matrices concatenated
        WorldViewProjMatrix,
        /** Provides inverse of concatenated world, view and projection matrices.
            Equivalent to RenderMonkey's "WorldViewProjectionInverse".
        */
        InverseWorldViewProjMatrix,
        /** Provides transpose of concatenated world, view and projection matrices.
            Equivalent to RenderMonkey's "WorldViewProjectionTranspose".
        */
        TransposeWorldViewProjMatrix,
        /** Provides inverse transpose of concatenated world, view and projection
            matrices. Equivalent to RenderMonkey's "WorldViewProjectionInverseTranspose".
        */
        InverseTransposeWorldViewProjMatrix,

        // render target related values
        /** -1 if requires texture flipping, +1 otherwise. It's useful when you bypassed
            projection matrix transform, still able use this value to adjust transformed y
           position.
        */
        RenderTargetFlipping,

        /** -1 if the winding has been inverted (e.g. for reflections), +1 otherwise.
         */
        VertexWinding,

        /// Fog colour
        FogColour,
        /// Fog params: density, linear start, linear end, 1/(end-start)
        FogParams,

        /// Surface ambient colour, as set in Pass::setAmbient
        SurfaceAmbientColour,
        /// Surface diffuse colour, as set in Pass::setDiffuse
        SurfaceDiffuseColour,
        /// Surface specular colour, as set in Pass::setSpecular
        SurfaceSpecularColour,
        /// Surface emissive colour, as set in Pass::setSelfIllumination
        SurfaceEmissiveColour,
        /// Surface shininess, as set in Pass::setShininess
        SurfaceShininess,
        /// Surface alpha rejection value, not as set in Pass::setAlphaRejectionValue, but a
        /// floating number between 0.0f and 1.0f instead (255.0f /
        /// Pass::getAlphaRejectionValue())
        SurfaceAlphaRejectionValue,

        /// The number of active light sources (better than gl_MaxLights)
        LightCount,

        /// The ambient light colour set in the scene
        AmbientLightColour,

        /// Light diffuse colour (index determined by setAutoConstant call)
        ///
        /// this requires an index in the ’extra_params’ field, and relates to the ’nth’ closest
        /// light which could affect this object
        /// (i.e. 0 refers to the closest light - note that directional lights are always first
        /// in the list and always present).
        /// NB if there are no lights this close, then the parameter will be set to black.
        LightDiffuseColour,
        /// Light specular colour (index determined by setAutoConstant call)
        LightSpecularColour,
        /// Light attenuation parameters, Vector4(range, constant, linear, quadric)
        LightAttenuation,
        /** Spotlight parameters, Vector4(innerFactor, outerFactor, falloff, isSpot)
            innerFactor and outerFactor are cos(angle/2)
            The isSpot parameter is 0.0f for non-spotlights, 1.0f for spotlights.
            Also for non-spotlights the inner and outer factors are 1 and nearly 1 respectively
        */
        SpotlightParams,
        /** A light position in world space (index determined by setAutoConstant call)

         This requires an index in the ’extra_params’ field, and relates to the ’nth’ closest
         light which could affect this object (i.e. 0 refers to the closest light).
         NB if there are no lights this close, then the parameter will be set to all zeroes.
         Note that this property will work with all kinds of lights, even directional lights,
         since the parameter is set as a 4D vector.
         Point lights will be (pos.x, pos.y, pos.z, 1.0f) whilst directional lights will be
         (-dir.x, -dir.y, -dir.z, 0.0f).
         Operations like dot products will work consistently on both.
         */
        LightPosition,
        /// A light position in object space (index determined by setAutoConstant call)
        LightPositionObjectSpace,
        /// A light position in view space (index determined by setAutoConstant call)
        LightPositionViewSpace,
        /// A light direction in world space (index determined by setAutoConstant call)
        /// @deprecated this property only works on directional lights, and we recommend that
        /// you use light_position instead since that returns a generic 4D vector.
        LightDirection,
        /// A light direction in object space (index determined by setAutoConstant call)
        LightDirectionObjectSpace,
        /// A light direction in view space (index determined by setAutoConstant call)
        LightDirectionViewSpace,
        /** The distance of the light from the center of the object
            a useful approximation as an alternative to per-vertex distance
            calculations.
        */
        LightDistanceObjectSpace,
        /** Light power level, a single scalar as set in Light::setPowerScale  (index determined
           by setAutoConstant call) */
        LightPowerScale,
        /// Light diffuse colour pre-scaled by Light::setPowerScale (index determined by
        /// setAutoConstant call)
        LightDiffuseColourPowerScaled,
        /// Light specular colour pre-scaled by Light::setPowerScale (index determined by
        /// setAutoConstant call)
        LightSpecularColourPowerScaled,
        /// Array of light diffuse colours (count set by extra param)
        LightDiffuseColourArray,
        /// Array of light specular colours (count set by extra param)
        LightSpecularColourArray,
        /// Array of light diffuse colours scaled by light power (count set by extra param)
        LightDiffuseColourPowerScaledArray,
        /// Array of light specular colours scaled by light power (count set by extra param)
        LightSpecularColourPowerScaledArray,
        /// Array of light attenuation parameters, Vector4(range, constant, linear, quadric)
        /// (count set by extra param)
        LightAttenuationArray,
        /// Array of light positions in world space (count set by extra param)
        LightPositionArray,
        /// Array of light positions in object space (count set by extra param)
        LightPositionObjectSpaceArray,
        /// Array of light positions in view space (count set by extra param)
        LightPositionViewSpaceArray,
        /// Array of light directions in world space (count set by extra param)
        LightDirectionArray,
        /// Array of light directions in object space (count set by extra param)
        LightDirectionObjectSpaceArray,
        /// Array of light directions in view space (count set by extra param)
        LightDirectionViewSpaceArray,
        /** Array of distances of the lights from the center of the object
            a useful approximation as an alternative to per-vertex distance
            calculations. (count set by extra param)
        */
        LightDistanceObjectSpaceArray,
        /** Array of light power levels, a single scalar as set in Light::setPowerScale
            (count set by extra param)
        */
        LightPowerScaleArray,
        /** Spotlight parameters array of Vector4(innerFactor, outerFactor, falloff, isSpot)
            innerFactor and outerFactor are cos(angle/2)
            The isSpot parameter is 0.0f for non-spotlights, 1.0f for spotlights.
            Also for non-spotlights the inner and outer factors are 1 and nearly 1 respectively.
            (count set by extra param)
        */
        SpotlightParamsArray,

        /** The derived ambient light colour, with 'r', 'g', 'b' components filled with
            product of surface ambient colour and ambient light colour, respectively,
            and 'a' component filled with surface ambient alpha component.
        */
        DerivedAmbientLightColour,
        /** The derived scene colour, with 'r', 'g' and 'b' components filled with sum
            of derived ambient light colour and surface emissive colour, respectively,
            and 'a' component filled with surface diffuse alpha component.
        */
        DerivedSceneColour,

        /** The derived light diffuse colour (index determined by setAutoConstant call),
            with 'r', 'g' and 'b' components filled with product of surface diffuse colour,
            light power scale and light diffuse colour, respectively, and 'a' component filled
           with surface
            diffuse alpha component.
        */
        DerivedLightDiffuseColour,
        /** The derived light specular colour (index determined by setAutoConstant call),
            with 'r', 'g' and 'b' components filled with product of surface specular colour
            and light specular colour, respectively, and 'a' component filled with surface
            specular alpha component.
        */
        DerivedLightSpecularColour,

        /// Array of derived light diffuse colours (count set by extra param)
        DerivedLightDiffuseColourArray,
        /// Array of derived light specular colours (count set by extra param)
        DerivedLightSpecularColourArray,
        /** The absolute light number of a local light index. Each pass may have
            a number of lights passed to it, and each of these lights will have
            an index in the overall light list, which will differ from the local
            light index due to factors like setStartLight and setIteratePerLight.
            This binding provides the global light index for a local index.
        */
        LightNumber,
        /// Returns (int) 1 if the  given light casts shadows, 0 otherwise (index set in extra
        /// param)
        LightCastsShadows,
        /// Returns (int) 1 if the  given light casts shadows, 0 otherwise (index set in extra
        /// param)
        LightCastsShadowsArray,

        /** The distance a shadow volume should be extruded when using
            finite extrusion programs.
        */
        ShadowExtrusionDistance,
        /// The current camera's position in world space
        CameraPosition,
        /// The current camera's position in object space
        CameraPositionObjectSpace,
        /** The view/projection matrix of the assigned texture projection frustum

         Applicable to vertex programs which have been specified as the ’shadow receiver’ vertex
         program alternative, or where a texture unit is marked as content_type shadow; this
         provides details of the view/projection matrix for the current shadow projector. The
         optional ’extra_params’ entry specifies which light the projector refers to (for the
         case of content_type shadow where more than one shadow texture may be present in a
         single pass), where 0 is the default and refers to the first light referenced in this
         pass.
         */
        TextureViewProjMatrix,
        /// Array of view/projection matrices of the first n texture projection frustums
        TextureViewProjMatrixArray,
        /** The view/projection matrix of the assigned texture projection frustum,
            combined with the current world matrix
        */
        TextureWorldViewProjMatrix,
        /// Array of world/view/projection matrices of the first n texture projection frustums
        TextureWorldViewProjMatrixArray,
        /// The view/projection matrix of a given spotlight
        SpotlightViewProjMatrix,
        /// Array of view/projection matrix of a given spotlight
        SpotlightViewProjMatrixArray,
        /** The view/projection matrix of a given spotlight projection frustum,
            combined with the current world matrix
        */
        SpotlightWorldViewProjMatrix,
        /** An array of the view/projection matrix of a given spotlight projection frustum,
            combined with the current world matrix
        */
        SpotlightWorldViewProjMatrixArray,
        /** A custom parameter which will come from the renderable, using 'data' as the
         identifier

         This allows you to map a custom parameter on an individual Renderable (see
         Renderable::setCustomParameter) to a parameter on a GPU program. It requires that you
         complete the ’extra_params’ field with the index that was used in the
         Renderable::setCustomParameter call, and this will ensure that whenever this Renderable
         is used, it will have it’s custom parameter mapped in. It’s very important that this
         parameter has been defined on all Renderables that are assigned the material that
         contains this automatic mapping, otherwise the process will fail.
         */
        Custom,
        /** provides current elapsed time
         */
        Time,
        /** Single float value, which repeats itself based on given as
            parameter "cycle time". Equivalent to RenderMonkey's "Time0_X".
        */
        Time0x,
        /// Cosine of "Time0_X". Equivalent to RenderMonkey's "CosTime0_X".
        CosTime0x,
        /// Sine of "Time0_X". Equivalent to RenderMonkey's "SinTime0_X".
        SinTime0x,
        /// Tangent of "Time0_X". Equivalent to RenderMonkey's "TanTime0_X".
        TanTime0x,
        /** Vector of "Time0_X", "SinTime0_X", "CosTime0_X",
            "TanTime0_X". Equivalent to RenderMonkey's "Time0_X_Packed".
        */
        Time0xPacked,
        /** Single float value, which represents scaled time value [0..1],
            which repeats itself based on given as parameter "cycle time".
            Equivalent to RenderMonkey's "Time0_1".
        */
        Time01,
        /// Cosine of "Time0_1". Equivalent to RenderMonkey's "CosTime0_1".
        CosTime01,
        /// Sine of "Time0_1". Equivalent to RenderMonkey's "SinTime0_1".
        SinTime01,
        /// Tangent of "Time0_1". Equivalent to RenderMonkey's "TanTime0_1".
        TanTime01,
        /** Vector of "Time0_1", "SinTime0_1", "CosTime0_1",
            "TanTime0_1". Equivalent to RenderMonkey's "Time0_1_Packed".
        */
        Time01Packed,
        /** Single float value, which represents scaled time value [0..2*Pi],
            which repeats itself based on given as parameter "cycle time".
            Equivalent to RenderMonkey's "Time0_2PI".
        */
        Time02pi,
        /// Cosine of "Time0_2PI". Equivalent to RenderMonkey's "CosTime0_2PI".
        CosTime02pi,
        /// Sine of "Time0_2PI". Equivalent to RenderMonkey's "SinTime0_2PI".
        SinTime02pi,
        /// Tangent of "Time0_2PI". Equivalent to RenderMonkey's "TanTime0_2PI".
        TanTime02pi,
        /** Vector of "Time0_2PI", "SinTime0_2PI", "CosTime0_2PI",
            "TanTime0_2PI". Equivalent to RenderMonkey's "Time0_2PI_Packed".
        */
        Time02piPacked,
        /// provides the scaled frame time, returned as a floating point value.
        FrameTime,
        /// provides the calculated frames per second, returned as a floating point value.
        FPS,
        // viewport-related values
        /** Current viewport width (in pixels) as floating point value.
            Equivalent to RenderMonkey's "ViewportWidth".
        */
        ViewportWidth,
        /** Current viewport height (in pixels) as floating point value.
            Equivalent to RenderMonkey's "ViewportHeight".
        */
        ViewportHeight,
        /** This variable represents 1.0/ViewportWidth.
            Equivalent to RenderMonkey's "ViewportWidthInverse".
        */
        InverseViewportWidth,
        /** This variable represents 1.0/ViewportHeight.
            Equivalent to RenderMonkey's "ViewportHeightInverse".
        */
        InverseViewportHeight,
        /** Packed of "ViewportWidth", "ViewportHeight", "ViewportWidthInverse",
            "ViewportHeightInverse".
        */
        ViewportSize,

        // view parameters
        /** This variable provides the view direction vector (world space).
            Equivalent to RenderMonkey's "ViewDirection".
        */
        ViewDirection,
        /** This variable provides the view side vector (world space).
            Equivalent to RenderMonkey's "ViewSideVector".
        */
        ViewSideVector,
        /** This variable provides the view up vector (world space).
            Equivalent to RenderMonkey's "ViewUpVector".
        */
        ViewUpVector,
        /** This variable provides the field of view as a floating point value.
            Equivalent to RenderMonkey's "FOV".
        */
        FOV,
        /** This variable provides the near clip distance as a floating point value.
            Equivalent to RenderMonkey's "NearClipPlane".
        */
        NearClipDistance,
        /** This variable provides the far clip distance as a floating point value.
            Equivalent to RenderMonkey's "FarClipPlane".
        */
        FarClipDistance,

        /** provides the pass index number within the technique
            of the active materil.
        */
        PassNumber,

        /** provides the current iteration number of the pass. The iteration
            number is the number of times the current render operation has
            been drawn for the active pass.
        */
        PassIterationNumber,

        /** Provides a parametric animation value [0..1], only available
            where the renderable specifically implements it.

           For morph animation, sets the parametric value
           (0..1) representing the distance between the first position keyframe (bound to
           positions) and the second position keyframe (bound to the first free texture
           coordinate) so that the vertex program can interpolate between them. For pose
           animation, indicates a group of up to 4 parametric weight values applying to a
           sequence of up to 4 poses (each one bound to x, y, z and w of the constant), one for
           each pose. The original positions are held in the usual position buffer, and the
           offsets to take those positions to the pose where weight == 1.0 are in the first ’n’
           free texture coordinates; ’n’ being determined by the value passed to
           includes_pose_animation. If more than 4 simultaneous poses are required, then you’ll
           need more than 1 shader constant to hold the parametric values, in which case you
           should use this binding more than once, referencing a different constant entry; the
           second one will contain the parametrics for poses 5-8, the third for poses 9-12, and
           so on.
        */
        AnimationParametric,

        /** Provides the texel offsets required by this rendersystem to map
            texels to pixels. Packed as
            float4(absoluteHorizontalOffset, absoluteVerticalOffset,
            horizontalOffset / viewportWidth, verticalOffset / viewportHeight)
        */
        TexelOffsets,

        /** Provides information about the depth range of the scene as viewed
            from the current camera.
            Passed as float4(minDepth, maxDepth, depthRange, 1 / depthRange)
        */
        SceneDepthRange,

        /** Provides information about the depth range of the scene as viewed
            from a given shadow camera. Requires an index parameter which maps
            to a light index relative to the current light list.
            Passed as float4(minDepth, maxDepth, depthRange, 1 / depthRange)
        */
        ShadowSceneDepthRange,

        /** Provides an array of information about the depth range of the scene as viewed
            from a given shadow camera. Requires an index parameter which maps
            to a light index relative to the current light list.
            Passed as float4(minDepth, maxDepth, depthRange, 1 / depthRange)
        */
        ShadowSceneDepthRangeArray,

        /** Provides the fixed shadow colour as configured via SceneManager::setShadowColour;
            useful for integrated modulative shadows.
        */
        ShadowColour,
        /** Provides texture size of the texture unit (index determined by setAutoConstant
            call). Packed as float4(width, height, depth, 1)
        */
        TextureSize,
        /** Provides inverse texture size of the texture unit (index determined by
           setAutoConstant
            call). Packed as float4(1 / width, 1 / height, 1 / depth, 1)
        */
        InverseTextureSize,
        /** Provides packed texture size of the texture unit (index determined by
           setAutoConstant
            call). Packed as float4(width, height, 1 / width, 1 / height)
        */
        PackedTextureSize,

        /** Provides the current transform matrix of the texture unit (index determined by
           setAutoConstant
            call), as seen by the fixed-function pipeline.

            This requires an index in the ’extra_params’ field, and relates to the ’nth’ texture
           unit of the pass in question.
            NB if the given index exceeds the number of texture units available for this pass,
           then the parameter will be set to Matrix4::IDENTITY.
        */
        TextureMatrix,

        /** Provides the position of the LOD camera in world space, allowing you
            to perform separate LOD calculations in shaders independent of the rendering
            camera. If there is no separate LOD camera then this is the real camera
            position. See Camera::setLodCamera.
        */
        LODCameraPosition,
        /** Provides the position of the LOD camera in object space, allowing you
            to perform separate LOD calculations in shaders independent of the rendering
            camera. If there is no separate LOD camera then this is the real camera
            position. See Camera::setLodCamera.
        */
        LODCameraPositionObjectSpace,
        /** Binds custom per-light constants to the shaders. */
        LightCustom,
        /// Point params: size; constant, linear, quadratic attenuation
        PointParams
    };
}
