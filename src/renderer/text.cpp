/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  text.cpp
// Created:  5/12/2016
// Author:  K. Loux
// Description:  Helper object for building BufferInfo objects for text rendering.
// History:

// Standard C++ headers
#include <cassert>

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "renderer/text.h"
#include "renderer/renderWindow.h"

// Freetype headers
#include FT_MODULE_H

//==========================================================================
// Class:			Text
// Function:		Constant declarations
//
// Description:		Constant declarations for Text class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
unsigned int Text::program;
unsigned int Text::colorLocation;
unsigned int Text::modelviewMatrixLocation;
bool Text::initialized;
FT_Library Text::ft;
unsigned int Text::ftReferenceCount(0);

//==========================================================================
// Class:			Text
// Function:		vertexShader
//
// Description:		Text vertex shader.
//
// Input Arguments:
//		0	= vertex
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
const std::string Text::vertexShader(
	"#version 330\n"
	"\n"
	"uniform mat4 projectionMatrix;\n"
	"uniform mat4 modelviewMatrix;\n"
	"\n"
	"layout(location = 0) in vec4 vertex;// <vec2 pos, vec2 tex>\n"
	"\n"
	"out vec2 texCoords;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_Position = projectionMatrix * modelviewMatrix * vec4(vertex.xy, 0.0, 1.0);\n"
	"    texCoords = vertex.zw;\n"
	"}\n"
);

//==========================================================================
// Class:			Text
// Function:		fragmentShader
//
// Description:		Text fragment shader.
//
// Input Arguments:
//		0	= texCoords
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
const std::string Text::fragmentShader(
	"#version 330\n"
	"\n"
	"uniform sampler2D text;\n"
	"uniform vec3 textColor;\n"
	"\n"
	"in vec2 texCoords;\n"
	"\n"
	"out vec4 color;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoords).r);\n"
	"    color = vec4(textColor, 1.0) * sampled;\n"
	"}\n"
);

//==========================================================================
// Class:			Text
// Function:		Text
//
// Description:		Constructor for Text object.
//
// Input Arguments:
//		renderer	= RenderWindow&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Text::Text(RenderWindow& renderer) : renderer(renderer)
{
	color = Color::ColorBlack;
	scale = 1.0;
	initialized = false;
	glyphsGenerated = false;
	face = NULL;
	isOK = true;

	modelview.Resize(4, 4);
	SetOrientation(0.0);

	Initialize();
}

//==========================================================================
// Class:			Text
// Function:		~Text
//
// Description:		Destructor for Text object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Text::~Text()
{
	FreeFTResources();
}

//==========================================================================
// Class:			Text
// Function:		Initialize
//
// Description:		Initializes the Freetype back-end of this object.  Must
//					be called prior to any other post-creation method.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Text::Initialize()
{
	assert(!glyphsGenerated);
	assert(isOK);

	if (ftReferenceCount == 0)
	{
		if (FT_Init_FreeType(&ft))
			isOK = false;
	}

	ftReferenceCount++;
}

//==========================================================================
// Class:			Text
// Function:		Initialize
//
// Description:		Initializes the Freetype back-end of this object.  Must
//					be called prior to any other post-creation method.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
bool Text::SetFace(const std::string& fontFileName)
{
	assert(!glyphsGenerated);
	if (FT_New_Face(ft, fontFileName.c_str(), 0, &face))
		return false;

	return true;
}

//==========================================================================
// Class:			Text
// Function:		SetSize
//
// Description:		Sets the width and height to use for the font.  Width is
//					calculated to maintain proper aspect ratio.
//
// Input Arguments:
//		height	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Text::SetSize(const double& height)
{
	SetSize(0, height);
}

//==========================================================================
// Class:			Text
// Function:		SetSize
//
// Description:		Sets the width and height to use for the font.
//
// Input Arguments:
//		width	= const double&
//		height	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Text::SetSize(const double& width, const double& height)
{
	assert(!glyphsGenerated);
	FT_Set_Pixel_Sizes(face, width, height);
}

//==========================================================================
// Class:			Text
// Function:		GenerateGlyphs
//
// Description:		Generates the set of glyphs.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool Text::GenerateGlyphs()
{
	// TODO:  Better to use texture atlas?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	Glyph g;

	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			return false;

		glGenTextures(1, &g.id);
		glBindTexture(GL_TEXTURE_2D, g.id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,
			GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		g.xSize = face->glyph->bitmap.width;
		g.ySize = face->glyph->bitmap.rows;
		g.xBearing = face->glyph->bitmap_left;
		g.yBearing = face->glyph->bitmap_top;
		g.advance = face->glyph->advance.x;

		glyphs.insert(std::make_pair(c, g));
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glyphsGenerated = true;

	return glGetError() == GL_NO_ERROR;
}

//==========================================================================
// Class:			Text
// Function:		FreeFTResources
//
// Description:		Frees previously allocated Freetype resources (with
//					reference counting).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Text::FreeFTResources()
{
	FT_Done_Face(face);

	assert(ftReferenceCount > 0);
	ftReferenceCount--;
	if (ftReferenceCount == 0)
		FT_Done_FreeType(ft);
}

//==========================================================================
// Class:			Text
// Function:		BuildText
//
// Description:		Generates the vertex buffer information for this object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive::BufferInfo
//
//==========================================================================
Primitive::BufferInfo Text::BuildText()
{
	DoInternalInitialization();

	Primitive::BufferInfo bufferInfo;
	bufferInfo.GetOpenGLIndices();
	bufferInfo.vertexCount = 4 * text.length();
	bufferInfo.vertexBuffer = new float[bufferInfo.vertexCount * 4];

	glBindVertexArray(bufferInfo.vertexArrayIndex);
	double xStart(x);

	unsigned int i(0);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Glyph g = glyphs[*c];

		GLfloat xpos = xStart + g.xBearing * scale;
		GLfloat ypos = y - (g.ySize - g.yBearing) * scale;

		GLfloat w = g.xSize * scale;
		GLfloat h = g.ySize * scale;

		bufferInfo.vertexBuffer[i++] = xpos;
		bufferInfo.vertexBuffer[i++] = ypos;
		bufferInfo.vertexBuffer[i++] = 0.0;
		bufferInfo.vertexBuffer[i++] = 1.0;

		bufferInfo.vertexBuffer[i++] = xpos;
		bufferInfo.vertexBuffer[i++] = ypos + h;
		bufferInfo.vertexBuffer[i++] = 0.0;
		bufferInfo.vertexBuffer[i++] = 0.0;

		bufferInfo.vertexBuffer[i++] = xpos + w;
		bufferInfo.vertexBuffer[i++] = ypos + h;
		bufferInfo.vertexBuffer[i++] = 1.0;
		bufferInfo.vertexBuffer[i++] = 0.0;

		bufferInfo.vertexBuffer[i++] = xpos + w;
		bufferInfo.vertexBuffer[i++] = ypos;
		bufferInfo.vertexBuffer[i++] = 1.0;
		bufferInfo.vertexBuffer[i++] = 1.0;

		xStart += (g.advance >> 6) * scale;// Bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.vertexBufferIndex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * bufferInfo.vertexCount,
		bufferInfo.vertexBuffer, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

	delete[] bufferInfo.vertexBuffer;
	bufferInfo.vertexBuffer = NULL;

	return bufferInfo;
}

//==========================================================================
// Class:			Text
// Function:		BuildText
//
// Description:		Generates the vertex buffer information for this object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Text::RenderBufferedGlyph(const unsigned int& vertexCount)
{
	glUseProgram(program);

	// TODO:  Really, we don't want to access state here that isn't contained within BufferInfo
	// Are we making an exception for color and orientation?  Assume that this object will
	// be used only to render text having the same color and orientation (and size?).
	// Maybe put some assertions in the Set() methods then to ensure it hasn't yet been initialized?
	glUniform3f(colorLocation, color.GetRed(), color.GetGreen(), color.GetBlue());
	RenderWindow::SendUniformMatrix(modelview, modelviewMatrixLocation);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 66);// TODO:  Remove this and correctly implement it

	glDrawArrays(GL_QUADS, 0, vertexCount);

	renderer.UseDefaultProgram();
	glBindTexture(GL_TEXTURE_2D, 0);
}

//==========================================================================
// Class:			Text
// Function:		GetBoundingBox
//
// Description:		Returns the bounding box for the specified text string
//					given this object's font and size settings.
//
// Input Arguments:
//		s	= const std::string&
//
// Output Arguments:
//		None
//
// Return Value:
//		Bounding Box
//
//==========================================================================
Text::BoundingBox Text::GetBoundingBox(const std::string& s)
{
	DoInternalInitialization();

	BoundingBox b;
	b.xLeft = 0;
	b.xRight = 0;
	b.yUp = 0;
	b.yDown = 0;

	std::string::const_iterator c;
	for (c = s.begin(); c != s.end(); c++)
	{
		Glyph g = glyphs.find(*c)->second;

		//b.xLeft += 0;
		b.xRight += g.advance >> 6;
		b.yUp = std::max(b.yUp, g.yBearing);
		b.yDown = std::min(b.yDown, g.yBearing - g.ySize);
	}

	b.xLeft *= scale;
	b.xRight *= scale;
	b.yUp *= scale;
	b.yDown *= scale;

	return b;
}

//==========================================================================
// Class:			Text
// Function:		DoInternalInitialization
//
// Description:		Performs necessary static-state initialization.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Text::DoInternalInitialization()
{
	if (!glyphsGenerated)
	{
		if (!GenerateGlyphs())
		{
			isOK = false;
			return;
		}
	}

	if (!initialized)
	{
		std::vector<GLuint> shaderList;
		shaderList.push_back(renderer.CreateShader(GL_VERTEX_SHADER, vertexShader));
		shaderList.push_back(renderer.CreateShader(GL_FRAGMENT_SHADER, fragmentShader));

		program = renderer.CreateProgram(shaderList);
		colorLocation = glGetUniformLocation(program, "textColor");

		RenderWindow::ShaderInfo s;
		s.programId = program;
		s.needsModelview = false;
		s.needsProjection = true;
		s.projectionLocation = glGetUniformLocation(program, "projectionMatrix");
		modelviewMatrixLocation = glGetUniformLocation(program, "modelviewMatrix");
		renderer.AddShader(s);

		initialized = true;
	}
}

//==========================================================================
// Class:			Text
// Function:		SetOrientation
//
// Description:		Alters the modelview matrix according to the text rotation angle.
//
// Input Arguments:
//		angle	= const double& [rad]
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Text::SetOrientation(const double& angle)
{
	modelview.MakeIdentity();
	renderer.Rotate(modelview, angle, 0.0, 0.0, 1.0);
}