#version 330 core
// --- INPUTS (Attributes are still vec3 in your VBO layout) ---
layout (location = 0) in vec3 abound;      
layout (location = 1) in vec3 astart;      
layout (location = 2) in vec3 aend;        
layout (location = 3) in vec3 actrl_1;     
layout (location = 4) in vec3 actrl_2;     
layout (location = 5) in vec2 atex_coord;  
layout (location = 6) in float thickness;
layout (location = 7) in int filled;

uniform mat4 u_ViewProj;
uniform vec4 u_baseColor;

// --- OUTPUTS (Optimized for 2D) ---

// Interpolated Canvas Position
out vec2 vLocalPos; 

// Flat Shape Definitions (Swizzled to vec2)
flat out vec2 vStart;
flat out vec2 vEnd;
flat out vec2 vCtrl1;
flat out vec2 vCtrl2;

flat out float  vThickness; // 1D, no swizzle
flat out int  vFilled;    // 1D, no swizzle

void main(){
    // 1. TRANSFORM (Still needs 3D for depth/layering)
    gl_Position = u_ViewProj * vec4(abound, 1.0);

    // 2. SWIZZLE TO 2D
    // We drop the Z component here so the Fragment Shader 
    // only deals with pure 2D vectors.
    vLocalPos = abound.xy; 
    vStart    = astart.xy;
    vEnd      = aend.xy;
    vCtrl1    = actrl_1.xy;
    vCtrl2    = actrl_2.xy;

    // 3. PASS INTEGERS
    vThickness = thickness;
    vFilled    = filled;
}