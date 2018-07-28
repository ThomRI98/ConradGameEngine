#include "SunLight.h"

using namespace glm;

SunLight::SunLight(vec3 position, vec3 direction, vec3 color, float intensity, bool castShadow) :
    AbstractLight(position, color, intensity, castShadow), m_direction(direction)
{
    //ctor
}

void SunLight::sendUniforms(GLuint programID, size_t index)
{
    glUniform1i(glGetUniformLocation(programID, uniform_str(index, "type")), LIGHT_SUN);
    glUniform3f(glGetUniformLocation(programID, uniform_str(index, "color")), m_color.r, m_color.g, m_color.b);
    glUniform1f(glGetUniformLocation(programID, uniform_str(index, "intensity")), m_intensity);
    glUniform3f(glGetUniformLocation(programID, uniform_str(index, "direction")), m_direction[0], m_direction[1], m_direction[2]);

    sendShadowUniforms(programID, index);
}

SunLight::~SunLight()
{
    //dtor
}