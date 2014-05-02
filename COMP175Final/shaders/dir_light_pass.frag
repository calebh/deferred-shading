#version 330

uniform sampler2D colorMap;
uniform sampler2D normalMap;

uniform vec3 lightDir;
uniform float lightIntensity;
uniform float diffuseConstant;
uniform float ambientConstant;
uniform vec2 screenSize;
uniform vec3 lightColor;

out vec4 fragColor;

vec2 calcTexCoord() {
    return gl_FragCoord.xy / screenSize;
}

void main() {
	vec2 texCoord = calcTexCoord();
	vec3 color = texture(colorMap, texCoord).xyz;
	vec3 normal = texture(normalMap, texCoord).xyz;

	if (dot(normal, normal) != 0.0) {
		vec3 lightVec = -1.0 * normalize(lightDir);
		vec3 diffuseComponent = (lightIntensity * diffuseConstant * dot(normal, -1.0 * lightDir)) * (color * lightColor);
		vec3 ambientComponent = color * ambientConstant;
		fragColor = vec4(diffuseComponent + ambientComponent, 1.0);
	} else {
		discard;
	}
}