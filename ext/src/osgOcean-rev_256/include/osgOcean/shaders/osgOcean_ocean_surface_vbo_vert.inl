/*
* This source file is part of the osgOcean library
* 
* Copyright (C) 2009 Kim Bale
* Copyright (C) 2009 The University of Hull, UK
* 
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.

* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
* http://www.gnu.org/copyleft/lesser.txt.
*/

// ------------------------------------------------------------------------------
// -- THIS FILE HAS BEEN CREATED AS PART OF THE BUILD PROCESS -- DO NOT MODIFY --
// ------------------------------------------------------------------------------

static const char osgOcean_ocean_surface_vbo_vert[] =
	"uniform mat4 osg_ViewMatrixInverse;\n"
	"uniform float osg_FrameTime;\n"
	"\n"
	"uniform vec3 osgOcean_Eye;\n"
	"\n"
	"uniform vec3 osgOcean_NoiseCoords0;\n"
	"uniform vec3 osgOcean_NoiseCoords1;\n"
	"\n"
	"uniform vec4 osgOcean_WaveTop;\n"
	"uniform vec4 osgOcean_WaveBot;\n"
	"\n"
	"uniform float osgOcean_FoamScale;\n"
	"\n"
	"// Used to blend the waves into a sinus curve near the shore\n"
	"uniform sampler2D osgOcean_Heightmap;\n"
	"uniform bool osgOcean_EnableHeightmap;\n"
	"\n"
	"uniform bool osgOcean_EnableUnderwaterScattering;\n"
	"uniform float osgOcean_WaterHeight;\n"
	"uniform vec3 osgOcean_UnderwaterAttenuation;\n"
	"uniform vec4 osgOcean_UnderwaterDiffuse;\n"
	"\n"
	"varying vec4 vVertex;\n"
	"varying vec4 vWorldVertex;\n"
	"varying vec3 vNormal;\n"
	"varying vec3 vViewerDir;\n"
	"varying vec3 vLightDir;\n"
	"\n"
	"varying vec3 vExtinction;\n"
	"varying vec3 vInScattering;\n"
	"\n"
	"varying vec3 vWorldViewDir;\n"
	"varying vec3 vWorldNormal;\n"
	"\n"
	"varying float height;\n"
	"\n"
	"mat3 get3x3Matrix( mat4 m )\n"
	"{\n"
	"    mat3 result;\n"
	"\n"
	"    result[0][0] = m[0][0];\n"
	"    result[0][1] = m[0][1];\n"
	"    result[0][2] = m[0][2];\n"
	"\n"
	"    result[1][0] = m[1][0];\n"
	"    result[1][1] = m[1][1];\n"
	"    result[1][2] = m[1][2];\n"
	"\n"
	"    result[2][0] = m[2][0];\n"
	"    result[2][1] = m[2][1];\n"
	"    result[2][2] = m[2][2];\n"
	"\n"
	"    return result;\n"
	"}\n"
	"\n"
	"void computeScattering( in vec3 eye, in vec3 worldVertex, out vec3 extinction, out vec3 inScattering )\n"
	"{\n"
	"	float viewDist = length(eye-worldVertex);\n"
	"	\n"
	"	float depth = max(osgOcean_WaterHeight-worldVertex.z, 0.0);\n"
	"	\n"
	"	extinction = exp(-osgOcean_UnderwaterAttenuation*viewDist*2.0);\n"
	"\n"
	"	// Need to compute accurate kd constant.\n"
	"	// const vec3 kd = vec3(0.001, 0.001, 0.001);\n"
	"	inScattering = osgOcean_UnderwaterDiffuse.rgb * (1.0-extinction*exp(-depth*vec3(0.001)));\n"
	"}\n"
	"\n"
	"// -------------------------------\n"
	"//          Main Program\n"
	"// -------------------------------\n"
	"\n"
	"void main( void )\n"
	"{\n"
	"    // Transform the vertex\n"
	"    vec4 inputVertex = gl_Vertex;\n"
	"    inputVertex.xyz += gl_Color.xyz;\n"
	"\n"
	"    gl_Position = gl_ModelViewProjectionMatrix * inputVertex;\n"
	"\n"
	"    // Blend the wave into a sinus curve near the shore\n"
	"    // note that this requires a vertex shader texture lookup\n"
	"    // vertex has to be transformed a second time with the new z-value\n"
	"    if (osgOcean_EnableHeightmap)\n"
	"    {\n"
	"        vec2 screenCoords = gl_Position.xy / gl_Position.w;\n"
	"        \n"
	"        height = pow(clamp(1.0 - texture2D(osgOcean_Heightmap, clamp(screenCoords * 0.5 + 0.5, 0.0, 1.0)).x, 0.0, 1.0), 32.0);\n"
	"\n"
	"        inputVertex = vec4(inputVertex.x, \n"
	"                           inputVertex.y, \n"
	"                           mix(inputVertex.z, sin(osg_FrameTime), height),\n"
	"                           inputVertex.w);\n"
	"\n"
	"        gl_Position = gl_ModelViewProjectionMatrix * inputVertex;\n"
	"    }\n"
	"\n"
	"    // -----------------------------------------------------------\n"
	"\n"
	"    // In object space\n"
	"    vVertex = inputVertex;\n"
	"    vLightDir = normalize( vec3( gl_ModelViewMatrixInverse * ( gl_LightSource[osgOcean_LightID].position ) ) );\n"
	"    vViewerDir = gl_ModelViewMatrixInverse[3].xyz - inputVertex.xyz;\n"
	"    vNormal = normalize(gl_Normal);\n"
	"\n"
	"    vec4 waveColorDiff = osgOcean_WaveTop-osgOcean_WaveBot;\n"
	"\n"
	"    gl_FrontColor = waveColorDiff *\n"
	"        clamp((inputVertex.z + osgOcean_Eye.z) * 0.1111111 + vNormal.z - 0.4666667, 0.0, 1.0) + osgOcean_WaveBot;\n"
	"\n"
	"    // -------------------------------------------------------------\n"
	"\n"
	"    mat4 modelMatrix = osg_ViewMatrixInverse * gl_ModelViewMatrix;\n"
	"    mat3 modelMatrix3x3 = get3x3Matrix( modelMatrix );\n"
	"\n"
	"    // world space\n"
	"    vWorldVertex = modelMatrix * inputVertex;\n"
	"    vWorldNormal = modelMatrix3x3 * gl_Normal;\n"
	"    vWorldViewDir = vWorldVertex.xyz - osgOcean_Eye.xyz;\n"
	"\n"
	"    // ------------- Texture Coords ---------------------------------\n"
	"\n"
	"    // Normal Map Coords\n"
	"    gl_TexCoord[0].xy = ( inputVertex.xy * osgOcean_NoiseCoords0.z + osgOcean_NoiseCoords0.xy );\n"
	"    gl_TexCoord[0].zw = ( inputVertex.xy * osgOcean_NoiseCoords1.z + osgOcean_NoiseCoords1.xy );\n"
	"    gl_TexCoord[0].y = -gl_TexCoord[0].y;\n"
	"    gl_TexCoord[0].w = -gl_TexCoord[0].w;\n"
	"\n"
	"    // Foam coords\n"
	"    gl_TexCoord[1].st = inputVertex.xy * osgOcean_FoamScale;\n"
	"\n"
	"    // Fog coords\n"
	"    gl_FogFragCoord = gl_Position.z;\n"
	"\n"
	"    if (osgOcean_EnableUnderwaterScattering)\n"
	"        computeScattering( osgOcean_Eye, vWorldVertex.xyz, vExtinction, vInScattering);\n"
	"}\n";
