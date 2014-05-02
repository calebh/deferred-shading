#version 330

uniform sampler2D positionMap;
uniform sampler2D colorMap;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform float lightIntensity;
uniform float diffuseConstant;
uniform vec2 screenSize;
uniform vec3 lightColor;

out vec4 fragColor;

vec2 calcTexCoord() {
    return gl_FragCoord.xy / screenSize;
}

void main() {
	vec2 texCoord = calcTexCoord();
	vec3 worldPos = texture(positionMap, texCoord).xyz;
	vec3 color = texture(colorMap, texCoord).xyz;
	vec3 normal = texture(normalMap, texCoord).xyz;

	vec3 lightDir = lightPos - worldPos;
	float attenuation = lightIntensity / dot(lightDir, lightDir);
	lightDir = normalize(lightDir);

	vec3 diffuseComponent = (diffuseConstant * dot(normal, lightDir)) * (color * lightColor);

	fragColor = vec4(attenuation * diffuseComponent, 1.0);
}