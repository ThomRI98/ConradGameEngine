#include "AbstractMaterial.h"

AbstractMaterial::AbstractMaterial()
{
    //ctor
}

AbstractMaterial::AbstractMaterial(RGB ambientColor, RGB diffuseColor, RGB specularColor, RGB emitColor, float specularExponent, float alpha, float ambientStrength, float diffuseStrength, float specularStrength, float emitStrength) :
    m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_emit(emit), m_specularExponent(specularExponent), m_alpha(alpha), m_ambientStrength(ambientStrength), m_diffuseStrength(diffuseStrength), m_specularStrength(specularStrength), m_emitStrength(emitStrength)
{

}

/* #### GETTERS #### */

RGB AbstractMaterial::getAmbientColor()
{
    return m_ambient;
}

RGB AbstractMaterial::getDiffuseColor()
{
    return m_diffuse;
}

RGB AbstractMaterial::getSpecularColor()
{
    return m_specular;
}

RGB AbstractMaterial::getEmitColor()
{
    return m_emit;
}

float AbstractMaterial::getAmbientStrength()
{
    return m_ambientStrength;
}

float AbstractMaterial::getDiffuseStrength()
{
    return m_diffuseStrength;
}

float AbstractMaterial::getSpecularStrength()
{
    return m_specularStrength;
}

float AbstractMaterial::getEmitStrength()
{
    return m_emitStrength;
}

float AbstractMaterial::getSpecularExponent()
{
    return m_specularExponent;
}

float AbstractMaterial::getAlpha()
{
    return m_alpha;
}

AbstractTexture *AbstractMaterial::getDiffuseTexture()
{
    return m_diffuseTexture;
}

AbstractTexture *AbstractMaterial::getSpecularTexture()
{
    return m_specularTexture;
}

/* #### SETTERS #### */
void AbstractMaterial::setAmbientColor(RGB color)
{
    m_ambient = color;
}

void AbstractMaterial::setAmbientColor(float r, float g, float b)
{
    setAmbient({r, g, b});
}

void AbstractMaterial::setDiffuseColor(RGB color)
{
    m_diffuse = color;
}

void AbstractMaterial::setDiffuseColor(float r, float g, float b)
{
    setDiffuse({r, g, b});
}

void AbstractMaterial::setSpecularColor(RGB color)
{
    m_specular = color;
}

void AbstractMaterial::setSpecularColor(float r, float g, float b)
{
    setSpecular({r, g, b});
}

void AbstractMaterial::setEmitColor(RGB color)
{
    m_emit = color;
}

void AbstractMaterial::setEmitColor(float r, float g, float b)
{
    setEmit({r, g, b});
}

void AbstractMaterial::setAmbientStrength(float strength)
{
    m_ambientStrength = strength;
}

void AbstractMaterial::setDiffuseStrength(float strength)
{
    m_diffuseStrength = strength;
}

void AbstractMaterial::setSpecularStrength(float strength)
{
    m_specularStrength = strength;
}

void AbstractMaterial::setEmitStrength(float strength)
{
    m_emitStrength = strength;
}

void AbstractMaterial::setSpecularExponent(float exponent)
{
    m_specularExponent = exponent;
}

void AbstractMaterial::setAlpha(float alpha)
{
    m_alpha = alpha;
}

void AbstractMaterial::setDiffuseTexture(AbstractTexture *texture)
{
    m_diffuseTexture = texture;
}

void AbstractMaterial::setSpecularTexture(AbstractTexture *texture)
{
    m_specularTexture = texture;
}

AbstractMaterial::~AbstractMaterial()
{
    //dtor
}
