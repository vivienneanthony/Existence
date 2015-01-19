#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "Lighting.glsl"
#include "Fog.glsl"

varying vec2 vTexCoord;
varying vec3 vWorldPos;
#ifdef PERPIXEL
    varying vec4 vLightVec;
    #ifdef SPECULAR
        varying vec3 vEyeVec;
    #endif
    varying vec3 vNormal;
    #ifdef SHADOW
        varying vec4 vShadowPos[NUMCASCADES];
    #endif
    #ifdef SPOTLIGHT
        varying vec4 vSpotPos;
    #endif
    #ifdef POINTLIGHT
        varying vec3 vCubeMaskVec;
    #endif
#else
    varying vec4 vVertexLight;
    varying vec3 vNormal;
    varying vec4 vScreenPos;
#endif

#ifdef COMPILEPS
uniform sampler2D sWeightMap0;
uniform sampler2D sDetailMap1;
uniform sampler2D sDetailMap2;
uniform sampler2D sDetailMap3;
uniform vec2 cDetailTiling;
#endif

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vTexCoord = GetTexCoord(iTexCoord);
    vNormal = GetWorldNormal(modelMatrix);
	//vNormal = iNormal;

    vWorldPos = worldPos;
    
    #ifdef PERPIXEL
        // Per-pixel forward lighting
        vec4 projWorldPos = vec4(worldPos, 1.0);

        #ifdef SHADOW
            // Shadow projection: transform from world space to shadow space
            for (int i = 0; i < NUMCASCADES; i++)
                vShadowPos[i] = GetShadowPos(i, projWorldPos);
        #endif

        #ifdef SPOTLIGHT
            // Spotlight projection: transform from world space to projector texture coordinates
            vSpotPos = cLightMatrices[0] * projWorldPos;
        #endif

        #ifdef POINTLIGHT
            vCubeMaskVec = mat3(cLightMatrices[0][0].xyz, cLightMatrices[0][1].xyz, cLightMatrices[0][2].xyz) * (cLightPos.xyz - worldPos);
        #endif

        #ifdef DIRLIGHT
            vLightVec = vec4(cLightDir, GetDepth(gl_Position));
        #else
            vLightVec = vec4((cLightPos.xyz - worldPos) * cLightPos.w, GetDepth(gl_Position));
        #endif
        #ifdef SPECULAR
            vEyeVec = cCameraPos - worldPos;
        #endif
    #else
        // Ambient & per-vertex lighting
        vVertexLight = vec4(GetAmbient(GetZonePos(worldPos)), GetDepth(gl_Position));

        #ifdef NUMVERTEXLIGHTS
            for (int i = 0; i < NUMVERTEXLIGHTS; ++i)
                vVertexLight.rgb += GetVertexLight(i, worldPos, vNormal) * cVertexLights[i * 3].rgb;
        #endif
        
        vScreenPos = GetScreenPos(gl_Position);
    #endif
}

void PS()
{
    // Get material diffuse albedo
    vec3 weights = texture2D(sWeightMap0, vTexCoord.xy).rgb;
    float sumWeights = weights.r + weights.g + weights.b;
    weights /= sumWeights;
	
	vec3 normal = normalize(vNormal);
	vec2 detailTexCoordZY = cDetailTiling * vWorldPos.zy/2050;
	vec2 detailTexCoordXZ = cDetailTiling * vWorldPos.xz/2050;
	detailTexCoordXZ = cDetailTiling * vec2(vWorldPos.x, -vWorldPos.z)/2050;
	vec2 detailTexCoordXY = cDetailTiling * vWorldPos.xy/2050;
	
	
    vec4 diffColorX = cMatDiffColor * (weights.r * texture2D(sDetailMap1, detailTexCoordZY) +
        weights.g * texture2D(sDetailMap2, detailTexCoordZY) + weights.b * texture2D(sDetailMap3, detailTexCoordZY));

	vec4 diffColorY = cMatDiffColor * (weights.r * texture2D(sDetailMap1, detailTexCoordXZ) +
        weights.g * texture2D(sDetailMap2, detailTexCoordXZ) + weights.b * texture2D(sDetailMap3, detailTexCoordXZ));

	vec4 diffColorZ = cMatDiffColor * (weights.r * texture2D(sDetailMap1, detailTexCoordXY) +
        weights.g * texture2D(sDetailMap2, detailTexCoordXY) + weights.b * texture2D(sDetailMap3, detailTexCoordXY));

	vec4 diffColor = diffColorY;	
	diffColor = diffColorY;
	
	vec3 blending = abs( normal );
    blending = normalize(max(blending, 0.00001));
    // Force weights to sum to 1.0
	float b = (blending.x + blending.y + blending.z);
    blending /= vec3(b, b, b);

	vec4 tex = diffColorX * blending.x + diffColorY * blending.y + diffColorZ * blending.z;
	diffColor = tex;
	 
    // Get material specular albedo
    vec3 specColor = cMatSpecColor.rgb;

    #if defined(PERPIXEL)
        // Per-pixel forward lighting
        vec3 lightColor;
        vec3 lightDir;
        vec3 finalColor;
        float diff;

        

        diff = GetDiffuse(normal, vLightVec.xyz, lightDir);

        #ifdef SHADOW
            diff *= GetShadow(vShadowPos, vLightVec.w);
        #endif

        #if defined(SPOTLIGHT)
            lightColor = vSpotPos.w > 0.0 ? texture2DProj(sLightSpotMap, vSpotPos).rgb * cLightColor.rgb : vec3(0.0, 0.0, 0.0);
        #elif defined(CUBEMASK)
            lightColor = textureCube(sLightCubeMap, vCubeMaskVec).rgb * cLightColor.rgb;
        #else
            lightColor = cLightColor.rgb;
        #endif

        #ifdef SPECULAR
            float spec = GetSpecular(normal, vEyeVec, lightDir, cMatSpecColor.a);
            finalColor = diff * lightColor * (diffColor.rgb + spec * specColor * cLightColor.a);
        #else
            finalColor = diff * lightColor * diffColor.rgb;
        #endif

        #ifdef HEIGHTFOG
            float fogFactor = GetHeightFogFactor(vLightVec.w, vWorldPos.y);
        #else
            float fogFactor = GetFogFactor(vLightVec.w);
        #endif

        #ifdef AMBIENT
            finalColor += cAmbientColor * diffColor.rgb;
            gl_FragColor = vec4(GetFog(finalColor, fogFactor), diffColor.a);
        #else
            gl_FragColor = vec4(GetLitFog(finalColor, fogFactor), diffColor.a);
        #endif
    #elif defined(PREPASS)
        // Fill light pre-pass G-Buffer
        vec3 normal = vNormal;

        float specPower = cMatSpecColor.a / 255.0;

        gl_FragData[0] = vec4(normal * 0.5 + 0.5, specPower);
        gl_FragData[1] = vec4(EncodeDepth(vVertexLight.a), 0.0);
    #elif defined(DEFERRED)
        // Fill deferred G-buffer
        vec3 normal = vNormal;

        float specIntensity = specColor.g;
        float specPower = cMatSpecColor.a / 255.0;

        #ifdef HEIGHTFOG
            float fogFactor = GetHeightFogFactor(vVertexLight.a, vWorldPos.y);
        #else
            float fogFactor = GetFogFactor(vVertexLight.a);
        #endif

        gl_FragData[0] = vec4(GetFog(vVertexLight.rgb * diffColor.rgb, fogFactor), 1.0);
        gl_FragData[1] = fogFactor * vec4(diffColor.rgb, specIntensity);
        gl_FragData[2] = vec4(normal * 0.5 + 0.5, specPower);
        gl_FragData[3] = vec4(EncodeDepth(vVertexLight.a), 0.0);
    #else
        // Ambient & per-vertex lighting
        vec3 finalColor = vVertexLight.rgb * diffColor.rgb;

        #ifdef MATERIAL
            // Add light pre-pass accumulation result
            // Lights are accumulated at half intensity. Bring back to full intensity now
            vec4 lightInput = 2.0 * texture2DProj(sLightBuffer, vScreenPos);
            vec3 lightSpecColor = lightInput.a * lightInput.rgb / max(GetIntensity(lightInput.rgb), 0.001);

            finalColor += lightInput.rgb * diffColor.rgb + lightSpecColor * specColor;
        #endif

        #ifdef HEIGHTFOG
            float fogFactor = GetHeightFogFactor(vVertexLight.a, vWorldPos.y);
        #else
            float fogFactor = GetFogFactor(vVertexLight.a);
        #endif

        gl_FragColor = vec4(GetFog(finalColor, fogFactor), diffColor.a);
    #endif
}
