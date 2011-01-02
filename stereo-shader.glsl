/**

This fragment shader will generate a stereogram image from a pattern texture and
a depth map.

*/

/**
 * The strength of the stereogram effect.
 */
uniform float strength;

/**
 * The background pattern texture.
 */
uniform sampler2D pattern;

/**
 * The z-buffer containing the depth map to render.
 */
uniform sampler2DShadow zbuffer;

/**
 * The output is a texture colour value.
 */
out vec4 result;

void
main()
{
    /* This is the source x coordinate in the pattern */
    float x = gl_FragCoord.x;

    /* Store the dimensions of the pattern image */
    ivec2 pattern_dim = textureSize(pattern, 0);

    do {
        /* Get the depth value at the current location in the z-buffer */
        float d = texture(zbuffer, gl_FragCoord);

        /* If we are currently at the first pattern iteration, we need to scale
           the depth to make the resulting image smooth */
        if (x < pattern_dim[0]) {
            d *= gl_FragCoord.x / pattern_dim[0];
        }

        x -= d * strength;
    } while (gl_FragCoord.x > pattern_dim[0]);

    /* Write the result */
    result = texture(pattern, gl_FragCoord);
}
