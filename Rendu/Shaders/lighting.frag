#version 330 core

struct Material 
{
    vec3 ambient;
    sampler2D diffuseMap;
    vec3 specular;
    float shininess;
};

struct DirectionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float exponent;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cosInnerCone;
	float cosOuterCone;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	bool on;

	float constant;
	float linear;
	float exponent;
};

  
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

#define MAX_POINT_LIGHTS 3

uniform DirectionalLight sunLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;

out vec4 frag_color;


// Calculer l'effet de la lumière directionnelle et retourner la somme des couleurs diffus et spéculaires résultantes
vec3 calcDirectionalLightColor(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);  // inversion => Doit être une direction du fragment vers la lumière

	// Diffus
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord));
    
    // Spéculaire (Blinn-Phong) 
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0f);
	vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess);

	return (diffuse + specular);
}


// Calculer l'effet de la lumière ponctuelle et retourner la somme des couleurs diffus et spéculaires résultantes
vec3 calcPointLightColor(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// Diffus 
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord));
    
    // Spéculaire (Blinn-Phong)
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0f);
	vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess);

	// Atténuation avec Kc, Kl, Kq
	float d = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * d + light.exponent * (d * d));

	diffuse = diffuse * attenuation;
	specular = specular * attenuation;
	
	return (diffuse + specular);
}


// Calculer l'effet de la lampe torche et retourner la somme des couleurs diffus et spéculaires résultantes
vec3 calcSpotLightColor(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 spotDir  = normalize(light.direction);

	float cosDir = dot(-lightDir, spotDir);  // angle entre le vecteur direction de la lumière et celui de la lampe torche
	float spotIntensity = smoothstep(light.cosOuterCone, light.cosInnerCone, cosDir);

	// Diffus 
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord));
    
    // Spéculaire (Blinn-Phong)
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0f);
	vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess);

	// Atténuation avec Kc, Kl, Kq 
	float d = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * d + light.exponent * (d * d));

	diffuse = diffuse * attenuation * spotIntensity;
	specular = specular * attenuation * spotIntensity;
	
	return (diffuse + specular);
}


void main()
{ 
	vec3 normal = normalize(Normal);  
	vec3 viewDir = normalize(viewPos - FragPos);

    // Ambiant
	vec3 ambient = spotLight.ambient * material.ambient * vec3(texture(material.diffuseMap, TexCoord));
	vec3 outColor = vec3(0.0f);	

	outColor = outColor + calcDirectionalLightColor(sunLight, normal, viewDir);

   for(int i = 0; i < MAX_POINT_LIGHTS; i = i + 1)
   {
		outColor = outColor + calcPointLightColor(pointLights[i], normal, FragPos, viewDir);
   }
          

	// Si la lumière n'est pas activée, retourner 0 pour les couleurs diffus et spéculaires
	if (spotLight.on)
	{
		outColor = outColor + calcSpotLightColor(spotLight, normal, FragPos, viewDir);
	}
		
	frag_color = vec4(ambient + outColor, 1.0f);
}
