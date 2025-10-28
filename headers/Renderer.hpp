#pragma once

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
//#include <GL/freeglut.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <stdio.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

struct ColorRgba
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
};

struct Vertex3
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

struct ColorVertex3D
{
    Vertex3     position;
    ColorRgba	rgba;
};


class Renderer 
{
public:

   Renderer();
    virtual ~Renderer();

    void    drawPolygon(std::vector<ColorVertex3D> vertices);
    bool	getVSync();
    bool    initialize();
    void    sceneBegin();
    void    sceneComplete();
    void    shutdown();
    void	toggleVSync();

private:

    bool	initVbo();
    void	freeVao();
    void	freeVbo();
    bool    initializeDevIl();
    bool    initializeOpenGl();
    bool    initializeShader();
    void	updateVbo();

    GLuint						colorLocation_;
    GLuint						indexBufferId_;
    int							indexBufferSize_;
    std::vector<GLuint>         indexData_;
    glm::mat4					modelViewMatrix_;
    GLuint						modelViewMatrixLocation_;
    SDL_GLContext               openGlContext_;
    GLuint						positionLocation_;
    GLuint						programId_;
    glm::mat4					projectionMatrix_;
    GLuint						projectionMatrixLocation_;
    SDL_Surface*                screen_;
    SDL_Renderer*               sdlRenderer_;
    bool                        useVsync_;
    GLuint						vao_;
    GLuint						vertexBufferId_;
    int							vertexBufferSize_;
    std::vector<ColorVertex3D>  vertexData_;
    SDL_Window*                 window_;

};
