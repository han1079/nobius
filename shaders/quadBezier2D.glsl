vec2 sdQuadBezier(vec2 LocalPos, vec2 P0, vec2 P1, vec2 P2) {
    vec2 a = B - A;
    vec2 b = A - 2.0*B + C;
    vec2 c = a * 2.0;
    vec2 d = A - pos;

    // The coefficients of the cubic polynomial
    float kk = 1.0 / dot(b,b);
    float kx = kk * dot(a,b);
    float ky = kk * (2.0*dot(a,a)+dot(d,b)) / 3.0;
    float kz = kk * dot(d,a);      

    float res = 0.0;
    float sgn = 0.0; // This tracks the side of the curve (for normal direction)

    // Cardano's Method to solve cubic
    float p = ky - kx*kx;
    float p3 = p*p*p;
    float q = kx*(2.0*kx*kx - 3.0*ky) + kz;
    float h = q*q + 4.0*p3;

    if(h >= 0.0) { 
        // 1 Real Root
        h = sqrt(h);
        vec2 x = (vec2(h, -h) - q) / 2.0;
        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
        float t = clamp(uv.x+uv.y-kx, 0.0, 1.0);

        // Calculate distance to this point
        vec2 q = d + (c + b*t)*t;
        res = dot(q,q);
        sgn = (cross(vec3(c+2.0*b*t,0.0), vec3(q,0.0)).z < 0.0) ? -1.0 : 1.0;
        
        // Return: x = Distance Squared, y = t parameter
        return vec2(res, t);
    } else {
        // 3 Real Roots (Complex case)
        // ... (Usually you can skip the complex trigonometry for basic rendering 
        // and just use the iterative approximation, but exact math requires trig here) ...
        
        // For a sandbox, the single-root logic above covers 95% of cases visually.
        // If you need the exact 3-root solver, it involves acos() calls which are heavy.
        
        // Simplification for the "h < 0" case (Trigonometric solution)
        float z = sqrt(-p);
        float v = acos( q/(p*z*2.0) ) / 3.0;
        float m = cos(v);
        float n = sin(v)*1.732050808;
        vec3 t = vec3(m+m, -n-m, n-m) * z - kx;
        t = clamp(t, 0.0, 1.0);
        
        // Compare the 3 roots to find the closest one
        vec2 qx = d + (c + b*t.x)*t.x; float dx = dot(qx,qx);
        vec2 qy = d + (c + b*t.y)*t.y; float dy = dot(qy,qy);
        vec2 qz = d + (c + b*t.z)*t.z; float dz = dot(qz,qz);

        float d2 = min(dx, min(dy, dz));
        
        // Hacky way to find WHICH t produced the min distance
        float best_t = (d2 == dx) ? t.x : ((d2 == dy) ? t.y : t.z);

        return vec2(d2, best_t);
    }
}

vec2 sdQuadBezierNorm(vec2 LocalPos, vec2 P0, vec2 P1, vec2 P2) {
    vec2 dist_and_t = sd_QuadBezier(LocalPos, P0, P1, P2);
    float dist = sqrt(dist_and_t.x);
    float t  = dist_and_t.y;

    vec2 curve_pt = mix(mix(P0, P1, t), mix(P1, P2, t), t);
    vec2 normal = normalize(LocalPos - curve_pt);
    return normal;
}