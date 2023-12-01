#version 330 core

out vec4 FragColor;

struct Material 
{
   
    sampler2D diffuse1;
    sampler2D specular1;    
    float shininess;
}; 



struct DirLight 
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};


struct sLight
{
    vec3 position;			
	vec3 diffuse;	
	vec3 specular;	
	//vec4 atten;	
	vec3 direction;	
	int lightType;	
	        
                   
   float constant;
    float linear;
    float quadratic;	
    
   
					
    vec3 ambient;

     float cutOff;
    float outerCutOff;
    vec3 color;

};


in vec3 FragPosition;  
in vec3 Normal;  
in vec2 TextureCoordinates;
  
   in vec4 ourColor;
uniform vec3 viewPos;
uniform Material material;
//uniform Light light; // similar to direction light

//Light Types
uniform DirLight dirLight;
uniform PointLight pointLights[4];
uniform SpotLight spotLight;

uniform int DIRECTION_LIGHT_ID =0;
uniform int POINT_LIGHT_ID =1;
uniform int SPOTLIGHT_ID =2;
const int LIGHTCOUNT = 10;
uniform sLight lights[LIGHTCOUNT];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);



void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPosition);

    

    vec3 result;
  

    for( int index = 0; index < LIGHTCOUNT; index++)
    {


       int LightType = lights[index].lightType;

       if(LightType == DIRECTION_LIGHT_ID)
       {
          vec3 lightDir = normalize(-lights[index].direction);
          float diff = max(dot(norm, lightDir), 0.0);
         vec3 reflectDir = reflect(-lightDir, norm);
         float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
         vec3 ambient = lights[index].ambient *  ourColor.rgb;
         vec3 diffuse =  lights[index].diffuse * diff *  ourColor.rgb;
         vec3 specular =  lights[index].specular * spec *  ourColor.rgb;
         vec3 finalValueforDir =(ambient+diffuse+specular);
         result+=finalValueforDir;
        
       }

       if(LightType ==POINT_LIGHT_ID)
       {
        vec3 lightDir = normalize(lights[index].position - FragPosition);
        // diffuse shading
        float diff = max(dot(norm, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        // attenuation
        float distance = length(lights[index].position - FragPosition);
        float attenuation = 1.0 / (lights[index].constant + lights[index].linear * distance + lights[index].quadratic * (distance * distance));    
        // combine results
        vec3 ambient = lights[index].ambient *  ourColor.rgb;
        vec3 diffuse = lights[index].diffuse * diff *  ourColor.rgb;
        vec3 specular = lights[index].specular * spec *  ourColor.rgb;
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
         vec3 finalValueforDir= (ambient + diffuse + specular);
          result+= finalValueforDir * lights[index].color.rgb;
       }

       if(LightType == SPOTLIGHT_ID)
       {
                vec3 lightDir = normalize(lights[index].position - FragPosition);
         // diffuse shading
         float diff = max(dot(norm, lightDir), 0.0);
         // specular shading
         vec3 reflectDir = reflect(-lightDir, norm);
         float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
         // attenuation
         float distance = length(lights[index].position - FragPosition);
         float attenuation = 1.0 / (lights[index].constant + lights[index].linear * distance + lights[index].quadratic * (distance * distance));    
         // spotlight intensity
         float theta = dot(lightDir, normalize(-lights[index].direction)); 
         float epsilon = lights[index].cutOff - lights[index].outerCutOff;
         float intensity = clamp((theta - lights[index].outerCutOff) / epsilon, 0.0, 1.0);
         // combine results
         vec3 ambient = lights[index].ambient *  ourColor.rgb;
         vec3 diffuse = lights[index].diffuse * diff *  ourColor.rgb;
         vec3 specular = lights[index].specular * spec *  ourColor.rgb;
         ambient *= attenuation * intensity;
         diffuse *= attenuation * intensity;
         specular *= attenuation * intensity;
         vec3 finalValueforDir= (ambient + diffuse + specular);
          result+=finalValueforDir * lights[index].color.rgb;
       }

    }




    // phase 2: point lights
  // for(int i = 0; i < 4; i++)

    
    FragColor = vec4(result  , 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient *  ourColor.rgb;
    vec3 diffuse = light.diffuse * diff *  ourColor.rgb;
    vec3 specular = light.specular * spec *  ourColor.rgb;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient *  ourColor.rgb;
    vec3 diffuse = light.diffuse * diff *  ourColor.rgb;
    vec3 specular = light.specular * spec *  ourColor.rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}



vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient *  ourColor.rgb;
    vec3 diffuse = light.diffuse * diff *  ourColor.rgb;
    vec3 specular = light.specular * spec *  ourColor.rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}