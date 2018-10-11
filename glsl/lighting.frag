uniform sampler2D shadowMap;
uniform sampler2D world;


const vec3 gamboge = vec3(0.89, 0.6, 0.06);
const vec3 ultramarine = vec3(0.1, 0.0, 1.0);


const float lighten = 1.3;
const float darken = 0.88;


void main() {
    vec4 values = texture2D(shadowMap, gl_TexCoord[0].xy);
    vec4 color = texture2D(world, gl_TexCoord[0].xy);
    vec3 colorRGB = vec3(color.x, color.y, color.z);

    if (values.r > 0.4) {
        // push shadow areas darker and cooler
        vec3 cooled = mix(colorRGB, ultramarine, 0.3);
        gl_FragColor = vec4(cooled * darken, 1.0);
    } else {
        // push lit areas lighter and warmer
        vec3 warmed = mix(colorRGB, gamboge, 0.19);
        gl_FragColor = vec4(warmed * lighten, 1.0);
    }
}
