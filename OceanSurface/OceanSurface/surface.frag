#version 330 core
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
out vec4 color;
  
uniform vec3 viewPos;
uniform Light light;

uniform float heightMax = 0;
uniform float heightMin = 0;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos); 
	vec3 viewDir = normalize(viewPos - FragPos);
	
	vec3 ambientFactor = vec3(0.0);
	vec3 diffuseFactor = vec3(1.0);
	
	vec3 skyColor = vec3(0.65, 0.80, 0.95);
	
	if (dot(norm, viewDir) < 0) norm = -norm;
	
    // Ambient
    vec3 ambient = light.ambient * ambientFactor;
	
	// Height Color
	vec3 shallowColor = vec3(0.0, 0.64, 0.68);
	vec3 deepColor = vec3(0.02, 0.05, 0.10);
	
	float relativeHeight;	// from 0 to 1
	relativeHeight = (FragPos.y - heightMin) / (heightMax - heightMin);
	vec3 heightColor = relativeHeight * shallowColor + (1 - relativeHeight) * deepColor;
	// heightColor = vec3(s);	// Black and white
	
	// Spray
	float sprayThresholdUpper = 1.0;
	float sprayThresholdLower = 0.9;
	float sprayRatio = 0;
	if (relativeHeight > sprayThresholdLower) sprayRatio = (relativeHeight - sprayThresholdLower) / (sprayThresholdUpper - sprayThresholdLower);
	vec3 sprayBaseColor = vec3(1.0);
	vec3 sprayColor = sprayRatio * sprayBaseColor;	
	
    // Diffuse  	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseFactor * light.diffuse * diff;
	diffuse = vec3(0.0);
	
	// Psudo reflect
	float refCoeff = pow(max(dot(norm, viewDir), 0.0), 0.3);	// Smaller power will have more concentrated reflect.
	vec3 reflectColor = (1 - refCoeff) * skyColor;
	
    // Specular
	//vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 64) * 3;	// Over exposure
    vec3 specular = light.specular * specCoeff;
        
	vec3 combinedColor = ambient + diffuse + heightColor + reflectColor;    
	
	//sprayRatio = clamp(sprayRatio, 0, 1);
	//combinedColor *= (1 - sprayRatio);
	//combinedColor += sprayColor;
	
	specCoeff = clamp(specCoeff, 0, 1);
	combinedColor *= (1 - specCoeff);
	combinedColor += specular;	
	color = vec4(combinedColor, 1.0f); 
	
	//color = vec4(sprayColor, 1.0f); 
	//color = vec4(heightColor, 1.0f); 
	//color = vec4(specular, 1.0f); 
	//color = vec4(1.0, 1.0, 1.0, 1.0);
} 