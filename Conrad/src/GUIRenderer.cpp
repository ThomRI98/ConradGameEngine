#include "GUIRenderer.h"

using namespace std;

GUIRenderer::GUIRenderer()
{
    //ctor
}

void GUIRenderer::load()
{
    /* Setting up VBO */
    if(glIsBuffer(m_vboID) == GL_TRUE) {
        glDeleteBuffers(1, &m_vboID);
    }

    glGenBuffers(1, &m_vboID);

    /* Setting up VAO */
    if(glIsVertexArray(m_vaoID) == GL_TRUE) {
        glDeleteVertexArrays(1, &m_vaoID);
    }

    glGenVertexArrays(1, &m_vaoID);

    glBindVertexArray(m_vaoID);

    /* Linking VAO and VBO */
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

        // [stride start] VERTEX1_X VERTEX1_Y TEX1_X TEX1_Y [stride end] VERTEX2_X VERTEX2_Y TEX2_X TEX2_Y....

            glVertexAttribPointer(VERTEX_BUFFER, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, BUFFER_OFFSET(0));
            glEnableVertexAttribArray(VERTEX_BUFFER);

            glVertexAttribPointer(TEX_BUFFER, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, BUFFER_OFFSET(sizeof(GLfloat) * 2)); // Begins after the two first vertices
            glEnableVertexAttribArray(TEX_BUFFER);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void GUIRenderer::addGUIObject(AbstractGUIObject *object)
{
    size_t size = object->getCoordsCount() * 2 * sizeof(float); // size of one array (only vertices for example, total size is 2*size to take into account texture coords)

    float *datas;
    datas = (float *) malloc(2 * size);

    float *vertices = object->getVertices();
    float *tex      = object->getTexCoords();

    // VERTEX1_X VERTEX1_Y TEX1_X TEX1_Y VERTEX2_X VERTEX2_Y TEX2_X TEX2_Y....
    for(int i = 0;i < object->getCoordsCount();i++) {
        cout << "DATA (" << 4*i << ") " << vertices[2*i] << " " << vertices[2*i+1] << " " << tex[2*i] << " " << tex[2*i+1] << endl;
        datas[4*i]      = vertices[2*i];
        datas[4*i + 1]  = vertices[2*i+1];
        datas[4*i + 2]  = tex[2*i];
        datas[4*i + 3]  = tex[2*i+1];
    }

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

        glBufferData(GL_ARRAY_BUFFER, 2*size, datas, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //free(datas);

    m_guiObjects.push_back(object);
}

void GUIRenderer::setShader(Shader shader)
{
    m_shader = shader;
    if(!m_shader.load()) {
        cout << "Error loading the GUI shader. App will most likely crash." << endl;
    }

    m_uniformLocations.modelview = m_shader.getUniformLocation("modelview");
}

void GUIRenderer::render()
{
    // NOTE (IMPORTANT) : GUI FACES MUST BE CORRECTLY ORIENTED SO THAT CULLING DON'T INTERFERE
    // FOR NOW, CULLING IS JUST DISABLED DURING GUI RENDER TIME
    glDisable(GL_CULL_FACE);

    m_shader.bind();
    glBindVertexArray(m_vaoID); // Using the VAO
    // Each draw call will bind the associated texture if necessary.

        for(auto it = m_guiObjects.begin();it != m_guiObjects.end();it++) {
            m_shader.sendMatrix(m_uniformLocations.modelview, (*it)->getModelview());
            (*it)->draw();
        }

    glBindVertexArray(0);
    m_shader.unbind();

    glEnable(GL_CULL_FACE);
}

GUIRenderer::~GUIRenderer()
{
    //dtor
}
