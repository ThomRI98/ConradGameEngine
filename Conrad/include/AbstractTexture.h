#ifndef ABSTRACTTEXTURE_H
#define ABSTRACTTEXTURE_H

/* Cross-plateform includes */
#ifdef WIN32
    #include <GL/glew.h>

#elif __APPLE__
    #define GL3_PROTOTYPES 1
    #include <OpenGL/gl3.h>

#else // UNIX / Linux
    #define GL3_PROTOTYPES 1
    #include <GL3/gl3.h>

#endif

#include "scope.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>

#define INVALID_MODE    0
#define FILE_MODE       1
#define SDL_MODE        2
#define FBO_MODE        3

class AbstractTexture
{
    public:
        AbstractTexture();
        AbstractTexture(std::string filepath);
        virtual ~AbstractTexture();

        bool load();
        bool loadFromSDL(SDL_Surface *surface, GLvoid* &data_ptr, GLenum &internalFormat, GLenum &format, bool reverse = true);

        inline void linkToFBO(GLuint fbo, int index = 0) {
            /* index is the attachment index within the FBO */
            glBindRenderbuffer(GL_RENDERBUFFER, fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, m_id, 0);

            // TODO : Unbind the FBO ? Warning, if used with a bind before, unbinding here could be unexpected
        }

        bool update(SDL_Surface *sdl_image, bool reverse = true); // reverse parameter is used to reverse the sdl_image in order to use the correct coordinates system
        // The new sdl_image must have the same format than the current texture ! (For performance issue, we can't afford to recalculate everything)

        /* OpenGL */
        void bind();
        void unbind();

        /* Setters */
        void setID(GLuint id);
        void setPath(std::string filepath);
        void setSize(GLsizei width, GLsizei height);

        /* Getters */
        GLuint getID();
        GLsizei getWidth();
        GLsizei getHeight();
        bool isLoaded();

    protected:
        static SDL_Surface *reverse_SDL_surface(SDL_Surface *source);

    private:
        std::string m_filepath;
        GLsizei m_width     = 0,
                m_height    = 0;
        GLenum  m_internalFormat,
                m_format;

        /* OpenGL */
        GLuint m_id = 0; // 0 is always unused

        bool m_mode = INVALID_MODE;
        bool m_loaded = false;
};

#endif // ABSTRACTTEXTURE_H
