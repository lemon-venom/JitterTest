#include "..\headers\Renderer.hpp"
#pragma comment( lib, "opengl32.lib" )
Renderer::Renderer()
{
	vertexBufferId_ = 0;
	indexBufferId_ = 0;

	vao_ = 0;

	// The initial buffer size will be set to 500, so won't need to re-allocate unless the new size is > 500.
	vertexBufferSize_ = 500;
	indexBufferSize_ = vertexBufferSize_ * 2;

	useVsync_ = false;
}

Renderer::~Renderer()
{
}


void Renderer::drawPolygon(std::vector<ColorVertex3D> vertices)
{
	// Require at least 2 verticies
	if (vertices.size() > 1)
	{
		int firstPolygonIndex = vertexData_.size();

		// Render a polygon to the screen.
		for (size_t i = 0; i < vertices.size(); i++)
		{
			vertexData_.push_back(vertices[i]);

			if (i > 0)
			{
				indexData_.push_back(vertexData_.size() - 2);
				indexData_.push_back(vertexData_.size() - 1);
			}
		}

		// No more vertices to add. Connect the last vertex added to the first in the index data.
		indexData_.push_back(vertexData_.size() - 1);
		indexData_.push_back(firstPolygonIndex);
	}
}

bool Renderer::getVSync()
{
	return useVsync_;
}

bool Renderer::initialize()
{
	std::cout << "Initializing SDL..." << std::endl;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL initialization failed with error " << SDL_GetError() << std::endl;

		return 0;
	}

	// Create the window
	window_ = SDL_CreateWindow("Smooth Motion Test",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	if (window_ == NULL)
	{
		std::cout << "Window creation failed with error " << SDL_GetError() << std::endl;

		return 0;
	}

	screen_ = SDL_GetWindowSurface(window_);

	// Create the renderer.
	sdlRenderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

	if (sdlRenderer_ == nullptr)
	{
		std::cout << "Renderer creation failed with error " << SDL_GetError() << std::endl;

		return false;
	}

	std::cout << "Initializing OpenGL..." << std::endl;

	if (initializeOpenGl() == false)
	{
		return false;
	}

	std::cout << "Initializing Shader..." << std::endl;

	if (initializeShader() == false)
	{
		return false;
	}

	return true;
}

bool Renderer::initializeOpenGl()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	openGlContext_ = SDL_GL_CreateContext(window_);

	if (openGlContext_ == NULL)
	{
		std::cout << "OpenGL context creation failed with error " << SDL_GetError() << std::endl;

		return false;
	}

	//Initialize GLEW
	GLenum glewError = glewInit();

	if (glewError != GLEW_OK)
	{
		std::cout << "GLEW initialization failed with error " << glewGetErrorString(glewError) << std::endl;
		return false;
	}

	// Use vsync by default.
	if (SDL_GL_SetSwapInterval(1) == -1)
	{
		std::cout << "Failed to set Swap Interval with error " << SDL_GetError() << std::endl;

		return false;
	}

	useVsync_ = true;

	//Make sure OpenGL 2.1 is supported
	if (!GLEW_VERSION_2_1)
	{
		std::cout << "OpenGL 2.1 not supported" << std::endl;
		return false;
	}

	std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

	//Set the viewport
	glViewport(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//Enable texturing
	glEnable(GL_TEXTURE_2D);

	//Set blending
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Check for error
	GLenum error = glGetError();

	if (error != GL_NO_ERROR)
	{
		std::cout << "OpenGL renderer initialization failed with error " << gluErrorString(error) << std::endl;

		return false;
	}

	std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	return true;
}

bool Renderer::initializeShader()
{
	const char* vertexShaderCode = R"(#version 460 core
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

in vec3 position_in;
in vec4 color_in;

out VS_OUT
{
	vec4 color;
} vs_out;

void main() 
{  
	vs_out.color = color_in;
	gl_Position = projectionMatrix * modelViewMatrix * vec4(position_in.x, position_in.y, position_in.z, 1.0);
})";


	const char* fragmentShaderCode = R"(#version 460 core

out vec4 fragColor;

in VS_OUT
{
	vec4 color;
} fs_in;

void main() 
{ 
	fragColor = fs_in.color;
})";


	GLenum error;

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Compile Vertex Shader
	glShaderSource(vertexShaderId, 1, &vertexShaderCode, NULL);

	glCompileShader(vertexShaderId);

	GLint Result = GL_FALSE;

	int InfoLogLength;

	// Check Vertex Shader
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &Result);

	glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0)
	{
		std::vector<char> vertexShaderErrorMessage(InfoLogLength);

		glGetShaderInfoLog(vertexShaderId, InfoLogLength, NULL, &vertexShaderErrorMessage[0]);

		std::cout << &vertexShaderErrorMessage[0] << std::endl;
	}

	// Compile Fragment Shader
	glShaderSource(fragmentShaderId, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShaderId);

	// Check Fragment Shader
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0)
	{
		std::vector<char> fragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(fragmentShaderId, InfoLogLength, NULL, &fragmentShaderErrorMessage[0]);
		std::cout << &fragmentShaderErrorMessage[0] << std::endl;
	}

	// Link
	programId_ = glCreateProgram();
	glAttachShader(programId_, vertexShaderId);
	glAttachShader(programId_, fragmentShaderId);
	glLinkProgram(programId_);

	// Check the program
	glGetProgramiv(programId_, GL_LINK_STATUS, &Result);
	glGetProgramiv(programId_, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0)
	{
		std::vector<char> programErrorMessage(std::max(InfoLogLength, int(1)));
		glGetProgramInfoLog(programId_, InfoLogLength, NULL, &programErrorMessage[0]);
		std::cout << &programErrorMessage[0] << std::endl;
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);


	glUseProgram(programId_);

	positionLocation_ = glGetAttribLocation(programId_, "position_in");
	colorLocation_ = glGetAttribLocation(programId_, "color_in");
	projectionMatrixLocation_ = glGetUniformLocation(programId_, "projectionMatrix");
	modelViewMatrixLocation_ = glGetUniformLocation(programId_, "modelViewMatrix");

	projectionMatrix_ = glm::ortho<GLfloat>(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0);
	glUniformMatrix4fv(projectionMatrixLocation_, 1, GL_FALSE, glm::value_ptr(projectionMatrix_));

	modelViewMatrix_ = glm::mat4();
	glUniformMatrix4fv(modelViewMatrixLocation_, 1, GL_FALSE, glm::value_ptr(modelViewMatrix_));

	bool vboInitOk = initVbo();

	if (vboInitOk == false)
	{
		return false;
	}

	//Generate VAO
	glGenVertexArrays(1, &vao_);

	//Bind vertex array
	glBindVertexArray(vao_);

	// Enable vertex attributes.
	glEnableVertexAttribArray(positionLocation_);

	glEnableVertexAttribArray(colorLocation_);

	//Set vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId_);

	//Check for error
	error = glGetError();

	if (error != GL_NO_ERROR)
	{
		std::cout << "Error initializing shader: " << gluErrorString(error) << std::endl;
		return false;
	}

	glVertexAttribPointer(positionLocation_,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(ColorVertex3D),
		(GLvoid*)offsetof(ColorVertex3D, position));


	//Check for error
	error = glGetError();

	if (error != GL_NO_ERROR)
	{
		std::cout << "Error initializing shader: " << gluErrorString(error) << std::endl;
		return false;
	}

	glVertexAttribPointer(colorLocation_,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(ColorVertex3D),
		(GLvoid*)offsetof(ColorVertex3D, rgba));

	//Check for error
	error = glGetError();

	if (error != GL_NO_ERROR)
	{
		std::cout << "Error initializing shader: " << gluErrorString(error) << std::endl;
		return false;
	}

	glVertexPointer(2, GL_FLOAT, 0, NULL);

	//Check for error
	error = glGetError();

	if (error != GL_NO_ERROR)
	{
		std::cout << "Error initializing shader: " << gluErrorString(error) << std::endl;
		return false;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId_);

	//Unbind VAO
	glBindVertexArray(NULL);

	//Check for error
	error = glGetError();

	if (error != GL_NO_ERROR)
	{
		std::cout << "Error initializing shader: " << gluErrorString(error) << std::endl;
		return false;
	}

	return true;
}

bool Renderer::initVbo()
{
	if (vertexBufferId_ == 0)
	{
		// Start with a buffer size of 500. Re-allocate a larger buffer if it becomes necessary later.
		ColorVertex3D vData[500];
		GLuint iData[500];

		//Create VBO
		glGenBuffers(1, &vertexBufferId_);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId_);
		glBufferData(GL_ARRAY_BUFFER, 500 * sizeof(ColorVertex3D), vData, GL_DYNAMIC_DRAW);

		GLenum error = glGetError();

		if (error != GL_NO_ERROR)
		{
			std::cout << "Error creating buffer: " << gluErrorString(error) << std::endl;
			return false;
		}

		//Create IBO
		glGenBuffers(1, &indexBufferId_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 500 * sizeof(GLuint), iData, GL_DYNAMIC_DRAW);

		//Check for error
		error = glGetError();

		if (error != GL_NO_ERROR)
		{
			std::cout << "Error creating index buffer: " << gluErrorString(error) << std::endl;
			return false;
		}

		//Unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}

	return true;
}

void Renderer::sceneBegin()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	vertexData_.clear();
	indexData_.clear();
}

void Renderer::sceneComplete()
{
	GLuint vertexCount = vertexData_.size();
	GLuint indexCount = indexData_.size();

	if (vertexCount > 0)
	{
		glUseProgram(programId_);

		updateVbo();

		glBindVertexArray(vao_);
		glDrawElements(GL_POLYGON, indexCount, GL_UNSIGNED_INT, NULL);
		
		glBindVertexArray(NULL);
	}

	SDL_GL_SwapWindow(window_);

	glFlush();
}


void Renderer::toggleVSync()
{
	useVsync_ = !useVsync_;

	if (useVsync_) {
		if (SDL_GL_SetSwapInterval(1) == -1) {
			std::cout << "Failed to set Swap Interval with error " << SDL_GetError() << std::endl;
		}
	} else {
		if (SDL_GL_SetSwapInterval(0) == -1) {
			std::cout << "Failed to set Swap Interval with error " << SDL_GetError() << std::endl;
		}
	}
}

void Renderer::updateVbo()
{
	// Update the VBO contents. If the size of the array has increased, allocate a new VBO.
	// Otherwise update the current VBO with the vertex data for this frame.
	int size = vertexData_.size();
	int indexSize = indexData_.size();

	if (indexSize > 0)
	{
		ColorVertex3D* vData = &vertexData_[0];
		GLuint* iData = &indexData_[0];

		if (indexSize > vertexBufferSize_)
		{
			// Allocate a new VBO and IBO to fit the new data size.
			vertexBufferSize_ = size;
			indexBufferSize_ = indexSize;

			// Destroy the old VBO and IBO
			freeVbo();

			//Create new VBO
			glGenBuffers(1, &vertexBufferId_);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId_);
			glBufferData(GL_ARRAY_BUFFER, size * sizeof(ColorVertex3D), vData, GL_DYNAMIC_DRAW);

			//Create new IBO
			glGenBuffers(1, &indexBufferId_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(GLuint), iData, GL_DYNAMIC_DRAW);

			// Bind the new VBO and IBO to the VAO.
			glBindVertexArray(vao_);

			//Set vertex data
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId_);

			glVertexAttribPointer(positionLocation_,
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(ColorVertex3D),
				(GLvoid*)offsetof(ColorVertex3D, position));

			glVertexAttribPointer(colorLocation_,
				4,
				GL_FLOAT,
				GL_FALSE,
				sizeof(ColorVertex3D),
				(GLvoid*)offsetof(ColorVertex3D, rgba));

			glVertexPointer(2, GL_FLOAT, 0, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId_);

			//Unbind VAO
			glBindVertexArray(NULL);

			//Unbind buffers
			glBindBuffer(GL_ARRAY_BUFFER, NULL);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
		}
		else
		{
			glBindVertexArray(vao_);

			// Bind vertex buffer.
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId_);

			// Update vertex buffer data.
			glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(ColorVertex3D), vData);

			// Bind index buffer.
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId_);

			// Update index buffer.		
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexSize * sizeof(GLuint), iData);

			//Unbind VAO
			glBindVertexArray(NULL);
		}
	}
}


void Renderer::freeVao()
{
	if (vao_ != 0)
	{
		glDeleteVertexArrays(1, &vao_);

		vao_ = 0;
	}
}

void Renderer::freeVbo()
{
	//Free VBO and IBO
	if (vertexBufferId_ != 0)
	{
		glDeleteBuffers(1, &vertexBufferId_);
		glDeleteBuffers(1, &indexBufferId_);

		vertexBufferId_ = 0;
		indexBufferId_ = 0;
	}
}

void Renderer::shutdown()
{
	freeVbo();
	freeVao();

	glDeleteProgram(programId_);

	SDL_GL_DeleteContext(openGlContext_);
	SDL_DestroyWindow(window_);
}