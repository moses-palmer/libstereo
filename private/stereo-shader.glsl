/**

This fragment shader will generate a stereogram image from a pattern texture and
a depth map.

*/

#version 110

/**
 * The strength of the stereogram effect.
 */
uniform float strength;

/**
 * The width in texels of the left hand margin.
 */
uniform int left_margin;

/**
 * The background pattern texture.
 */
uniform sampler2D pattern;

/**
 * The z-buffer containing the depth map to render.
 */
uniform sampler2DShadow zbuffer;

void
main()
{
    /* This is the source x coordinate in the pattern */
    float x = gl_FragCoord.x;
#if 0
    do {
        /* Get the depth value at the current location in the z-buffer */
        float d = shadow2D(zbuffer, vec3(x, gl_FragCoord.y, 0.0))[0];

        /* If we are currently at the first pattern iteration, we need to scale
           the depth to make the resulting image smooth */
        if (x < float(left_margin)) {
            d *= x / float(left_margin);
        }

        x -= d * strength;
    } while (x > float(left_margin));
#endif
    /* Write the result */
    gl_FragColor = texture2D(pattern, vec2(gl_FragCoord.x, gl_FragCoord.y));
}
