uniform sampler2D sampler;

// Global intensity of ambient light...
uniform float ambientLight = 0.1f;

// Defines surface properties of an object...
uniform struct Material
{
  vec3 emissive;    // do you glow?
  vec3 ambient;     // general background light scattering
  vec3 diffuse;     // when lit, how reflective is it?
  vec3 specular;    // if shiney, what colour of shiney?
  float shininess;  // exponent; bigger == sharper, tighter highlights
} material;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTextureCoord;

out vec4 outColour;

void main()
{
  vec4 surfaceColour = texture(sampler, fragTextureCoord);

  // Get the world position and normal of the point on the surface...
  vec3 worldFragPos = vec3(WorldMatrix * vec4(fragPosition, 1));
  vec3 worldNormal = normalize(mat3(NormalMatrix) * fragNormal);

  // Emissive light is just a value...
  vec3 emissive = material.emissive;
  // Ambient is based on global ambient light level...
  vec3 ambient = material.ambient * ambientLight;

  vec3 lighting = emissive + ambient;

  outColour.rgb = surfaceColour.rgb * lighting;
  outColour.a = 1;
}
