#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform float time;  
uniform vec3 viewPos;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define MAX_DIR_LIGHTS 16  
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform int direct_lights;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define MAX_POINT_LIGHTS 16  
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int point_lights;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outer_cutoff;

    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define MAX_SPOT_LIGHTS 16  
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int spot_lights;

struct Texture {
    sampler2D id;
    int type;
    float offsetX;
    float offsetY;
    float scaleX;
    float scaleY;
    float rotation;
};
#define MAX_TEXTURES 16
uniform Texture textures[MAX_TEXTURES];
uniform int num_textures;

struct Material {
    float ambient;
    float diffuse;
    float specular;

    vec3 color;

    float shininess;
}; 
uniform Material material;

uniform vec3 highlight_color;
uniform float highlight_value;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir); 

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec2 CalcTexCoords(Texture t, vec2 inTexCoords);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    // phase 1: Directional lighting
    for(int i = 0; i < direct_lights; i++)
        result += CalcDirLight(dirLights[i], norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < point_lights; i++)
       result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // phase 3: Spot light
    for(int i = 0; i < spot_lights; i++)
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);    
    
    //Emission Light
    //*
    for(int i=0; i<num_textures; i++){
        vec2 tex_coords = CalcTexCoords(textures[i], TexCoords);
        if(textures[i].type == 2){ //Emission
            vec4 emission_texture = texture(textures[i].id, tex_coords);
            if(emission_texture.a > 0.1){
                result = vec3(emission_texture);
            }
        }
    }
    //*/
    
    FragColor = vec4(mix(result, highlight_color, highlight_value), 1.0);
}

vec2 CalcTexCoords(Texture t, vec2 inTexCoords){
    vec2 tex_coords = inTexCoords;
    tex_coords += vec2(-0.5, -0.5); //move to center
    tex_coords /= vec2(t.scaleX, t.scaleY); //scale
    mat2 rot_mat = mat2(cos(t.rotation),
                        sin(t.rotation),
                       -sin(t.rotation),
                        cos(t.rotation)); 
    tex_coords *= rot_mat; //rotate 
    tex_coords += vec2(0.5, 0.5); //move back
    tex_coords += vec2(-t.offsetX, t.offsetY) * rot_mat; //move
    return tex_coords;
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
    vec3 ambient  = light.ambient * material.color;
    vec3 diffuse  = light.diffuse * diff * material.color;
    vec3 specular = light.specular * spec * material.specular;
        
    for(int i=0; i<num_textures; i++){
        vec2 tex_coords = CalcTexCoords(textures[i], TexCoords);
        if(textures[i].type == 0){ //Diffuse
            vec4 diffuse_texture = texture(textures[i].id, tex_coords);
            if(diffuse_texture.a > 0.1){
                ambient = light.ambient * vec3(diffuse_texture);
                diffuse = light.diffuse * diff * vec3(diffuse_texture);
                specular = light.specular * spec * material.specular;
            }
        }
        if(textures[i].type == 1){ //Specular
            vec4 specular_texture = texture(textures[i].id, tex_coords);
            if(specular_texture.a > 0.1){
                specular = light.specular * spec * vec3(specular_texture);
            }
        }
    }

    ambient *= material.ambient;
    diffuse *= material.diffuse;

    return (ambient + diffuse + specular);
} 

//*
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * material.color;
    vec3 diffuse  = light.diffuse  * diff * material.color;
    vec3 specular = light.specular * spec * material.specular;
    
    for(int i=0; i<num_textures; i++){
        vec2 tex_coords = CalcTexCoords(textures[i], TexCoords);
        if(textures[i].type == 0){ //Diffuse
            vec4 diffuse_texture = texture(textures[i].id, tex_coords);
            if(diffuse_texture.a > 0.1){
                ambient = light.ambient * vec3(diffuse_texture);
                diffuse = light.diffuse * diff * vec3(diffuse_texture);
                specular = light.specular * spec * material.specular;
            }
        }
        if(textures[i].type == 1){ //Specular
            vec4 specular_texture = texture(textures[i].id, tex_coords);
            if(specular_texture.a > 0.1){
                specular = light.specular * spec * vec3(specular_texture);
            }
        }
    }
    
    ambient *= material.ambient;
    diffuse *= material.diffuse;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
} 
//*/

//*
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * material.color;
    vec3 diffuse  = light.diffuse  * diff * material.color;
    vec3 specular = light.specular * spec * material.specular;
    
    for(int i=0; i<num_textures; i++){
        vec2 tex_coords = CalcTexCoords(textures[i], TexCoords);
        if(textures[i].type == 0){ //Diffuse
            vec4 diffuse_texture = texture(textures[i].id, tex_coords);
            if(diffuse_texture.a > 0.1){
                ambient = light.ambient * vec3(diffuse_texture);
                diffuse = light.diffuse * diff * vec3(diffuse_texture);
                specular = light.specular * spec * material.specular;
            }
        }
        if(textures[i].type == 1){ //Specular
            vec4 specular_texture = texture(textures[i].id, tex_coords);
            if(specular_texture.a > 0.1){
                specular = light.specular * spec * vec3(specular_texture);
            }
        }
    }
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
    //float intensity = 1.0;
    diffuse *= intensity;
    specular *= intensity;

    vec3 result = ambient;
    if(theta > light.outer_cutoff){       
        result += diffuse + specular;
    }
    return result;
}
//*/