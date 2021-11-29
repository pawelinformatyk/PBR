/*
* Copyright (C) 2010 Josh A. Beam
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*   1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Shader.h"

FShader::FShader( const char* vertex_path, const char* fragment_path)
{
	this->vertex_path = vertex_path;
	this->fragment_path = fragment_path;
}

void FShader::Init()
{
	ID = LoadShaders(vertex_path, fragment_path, NULL);
}

void FShader::ShaderAttachFromFile( GLuint program, GLenum type, const char* file_path )
{
	/* compile the shader */
	GLuint shader = ShaderCompileFromFile( type, file_path );
	if( shader != 0 )
	{
        /* attach the shader to the program */
		glAttachShader( program, shader );

		/* delete the shader - it won't actually be
		* destroyed until the program that it's attached
		* to has been destroyed */
		glDeleteShader( shader );
	}
}

GLuint FShader::ShaderCompileFromFile( GLenum type, const char* file_path )
{
	char* source;
	GLuint shader;
	GLint length, result;

	/* get shader source */
	source = ShaderLoadSource( file_path );
	if( !source )
		return 0;

	/* create shader object, set the source, and compile */
	shader = glCreateShader( type );
	length = strlen( source );
	glShaderSource( shader, 1, (const char**)&source, &length );
	glCompileShader( shader );
	free( source );

	/* make sure the compilation was successful */
	glGetShaderiv( shader, GL_COMPILE_STATUS, &result );
	if( result == GL_FALSE )
	{
		char* log;

		/* get the shader info log */
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
		log = (char*)malloc( length );
		glGetShaderInfoLog( shader, length, &result, log );

		/* print an error message and the info log */
		fprintf( stderr, "shaderCompileFromFile(): Unable to compile %s: %s\n", file_path, log );
		free( log );

		glDeleteShader( shader );
		return 0;
	}

	return shader;
}

char* FShader::ShaderLoadSource( const char* file_path )
{
	const size_t blockSize = 512;
	FILE* fp;
	char buf[ 512 ];
	char* source = NULL;
	size_t tmp, sourceLength = 0;

	/* open file */
	fopen_s( &fp, file_path, "r" );
	if( !fp )
	{
		fprintf( stderr, "shaderLoadSource(): Unable to open %s for reading\n", file_path );
		return NULL;
	}

	/* read the entire file into a string */
	while( (tmp = fread( buf, 1, blockSize, fp )) > 0 )
	{
		char* newSource = (char*)malloc( sourceLength + tmp + 1 );
		if( !newSource )
		{
			fprintf( stderr, "shaderLoadSource(): malloc failed\n" );
			if( source )
				free( source );
			return NULL;
		}

		if( source )
		{
			memcpy( newSource, source, sourceLength );
			free( source );
		}
		memcpy( newSource + sourceLength, buf, tmp );

		source = newSource;
		sourceLength += tmp;
	}

	/* close the file and null terminate the string */
	fclose( fp );
	if( source )
		source[ sourceLength ] = '\0';

	return source;
}

int FShader::LoadShaders( const char* vertex_path, const char* fragment_path, const char* geometry_path )
{
	GLint result;
    /* create program object and attach shaders */
	GLint g_program = glCreateProgram();
	ShaderAttachFromFile( g_program, GL_VERTEX_SHADER, vertex_path );
	if(geometry_path) ShaderAttachFromFile( g_program, GL_GEOMETRY_SHADER, geometry_path );
	ShaderAttachFromFile( g_program, GL_FRAGMENT_SHADER, fragment_path );

	/* link the program and make sure that there were no errors */
	glLinkProgram( g_program );
	glGetProgramiv( g_program, GL_LINK_STATUS, &result );
	if( result == GL_FALSE )
	{
		GLint length;
		char* log;

		/* get the program info log */
		glGetProgramiv( g_program, GL_INFO_LOG_LENGTH, &length );
		log = (char*)malloc( length );
		glGetProgramInfoLog( g_program, length, &result, log );

		/* print an error message and the info log */
		fprintf( stderr, "sceneInit(): Program linking failed: %s\n", log );
		free( log );

		/* delete the program */
		glDeleteProgram( g_program );
		g_program = 0;
	}
	return g_program;
}

void FShaderTexture::Init()
{
	FShader::Init();

	Use();

	SetInt("AlbedoMap", 0);
	SetInt("NormalMap", 1);
	SetInt("MetallicMap", 2);
	SetInt("RoughnessMap", 3);
	SetInt("AOMap", 4);
}

void FShaderBase::Init()
{
	FShader::Init();

	Use();

	SetVec3("Albedo", 0.5f, 0.0f, 0.0f);
	SetFloat("AO", 1.0f);
	SetFloat("Metallic", 1.0f);
	SetFloat("Roughness", 1.0f);
}
