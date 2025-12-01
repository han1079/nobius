vec2 sdCircleEdge(vec2 LocalPos, vec2 center, float radius) {
    vec2 offset = localpos - center;
    float dist = length(offset);

    vec2 pointonedge;

    if (dist > 0.0) {
        // normal projection
        pointonedge = center + (offset / dist) * radius;
    } else {
        // fallback: if we are exactly at the center, pick arbitrary point on edge
        // (e.g., to the right). it doesn't matter visually because it's inside.
        pointonedge = center + vec2(radius, 0.0);
    }

    return pointonedge;
}

