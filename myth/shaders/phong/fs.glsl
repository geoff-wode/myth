uniform sampler2D sampler;

// Global intensity of ambient light...
uniform vec3 ambientLightColour = vec3(0.5f);

// Defines world-space position and channel intensity of a light source...
uniform struct Light
{
  vec3 position;
  vec3 colour;
} light;

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
  // Emissive light is just a value...
  vec3 brightness = material.emissive;

  // Ambient is based on global ambient light level...
  brightness += (material.ambient * ambientLightColour);

  // Diffuse relies on angle between surface normal and light...
  vec3 worldFragPos = WorldMatrix * vec4(fragPosition, 1);
  vec3 N = normalize(fragNormal);
  vec3 L = normalize(light.position - worldFragPos);
  float diffuseLight = clamp(dot(N,L), 0.0f, 1.0f);
  brightness += material.diffuse * light.colour * diffuseLight;

  // Only compute specular highlights if the surface is facing the light...
  if (diffuseLight > 0.0f)
  {
    // Specular relies on angle between viewer and surface...
    vec3 V = normalize(CameraPosition.xyz - worldFragPos);
    // Get the vector half-way betwwen the light and the viewer
    vec3 H = normalize(L + V);

    float specularLight = pow(max(dot(N, H), 0), material.shininess);

    brightness += material.specular * light.colour * specularLight;
  }

  vec3 surfaceColour = texture(sampler, fragTextureCoord);
  outColour = vec4(surfaceColour * brightness, 1);
}
