#version 330 core

in vec2 vLocalPos;      // The "Canvas" coordinate (interpolated)

flat in vec2 vStart;    
flat in vec2 vEnd;
flat in vec2 vCtrl1;

flat in float  vThickness;
flat in int  vFilled;   // 0 = Wireframe, 1 = Solid

uniform vec4 u_baseColor;
uniform mat4 u_ViewProj;

out vec4 FragColor;

vec4 blend(vec4 back, vec4 front) {
    float outAlpha = front.a + back.a * (1.0 - front.a);

    if (outAlpha == 0.0) return vec4(0.0);

    vec3 outRGB = (front.rgb * front.a + back.rgb * back.a * (1.0 - front.a)) / outAlpha;

    return vec4(outRGB, outAlpha);
}

void main() {

    float radius = vEnd.x; // Convention and kind of annoying, but allows me to reuse Bezier Vertices
    bool draw_bounding = (vCtrl1.x > 0.5);
    float pxSize = length(vec2(dFdx(vLocalPos.x), dFdy(vLocalPos.y)));

    vec2 center = vStart; 
    
    float dist = length(vLocalPos - center) - radius; // Positive dist is outside.

    float fillalpha = 0.001;

    if (vFilled == 1){
        // Negative distance high alpha. Positive distance low alpha.
        fillalpha = 1.0 - smoothstep(0.0, (vThickness/2), dist);
    }

    float edgealpha = 1.0 - smoothstep(0.0, (vThickness/2), abs(dist));

    float boxalpha = 0.0;
    float dist_x = abs(vLocalPos.x - center.x - radius);
    float dist_y = abs(vLocalPos.y - center.y - radius);

    if(draw_bounding) {
        boxalpha = smoothstep(0.0, 5.0*pxSize, min(dist_x, dist_y));
    }

    vec4 edgeColor = vec4(0, 0, 1.0, edgealpha);
    vec4 fillColor = vec4(u_baseColor.x, u_baseColor.y, u_baseColor.z, fillalpha);
    vec4 boxColor = vec4(0.0, 0.0, 0.0, boxalpha);

    vec4 finalColor = vec4(0.0);
    finalColor = blend(finalColor, fillColor);
    finalColor = blend(finalColor, edgeColor);
    finalColor = blend(finalColor, boxColor);
    FragColor = finalColor;
}