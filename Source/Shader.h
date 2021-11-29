#pragma once

#include <iostream>
#include <string.h>

#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp"

class FShader
{
public:

	FShader( const char* vertex_path, const char* fragment_path );
	
    virtual void Init();
    const char* vertex_path;
    const char* fragment_path;

    unsigned int GetID()const{ return ID; };
    void Use()
    {
        glUseProgram( ID );
    }
    void SetBool( const std::string& name, bool value ) const
    {
        glUniform1i( glGetUniformLocation( ID, name.c_str() ), (int)value );
    }
    void SetInt( const std::string& name, int value ) const
    {
        glUniform1i( glGetUniformLocation( ID, name.c_str() ), value );
    }
    void SetFloat( const std::string& name, float value ) const
    {
        glUniform1f( glGetUniformLocation( ID, name.c_str() ), value );
    }
    void SetVec2( const std::string& name, const glm::vec2& value ) const
    {
        glUniform2fv( glGetUniformLocation( ID, name.c_str() ), 1, &value[ 0 ] );
    }
    void SetVec2( const std::string& name, float x, float y ) const
    {
        glUniform2f( glGetUniformLocation( ID, name.c_str() ), x, y );
    }
    void SetVec3( const std::string& name, const glm::vec3& value ) const
    {
        glUniform3fv( glGetUniformLocation( ID, name.c_str() ), 1, &value[ 0 ] );
    }
    void SetVec3( const std::string& name, float x, float y, float z ) const
    {
        glUniform3f( glGetUniformLocation( ID, name.c_str() ), x, y, z );
    }
    void SetVec4( const std::string& name, const glm::vec4& value ) const
    {
        glUniform4fv( glGetUniformLocation( ID, name.c_str() ), 1, &value[ 0 ] );
    }
    void SetVec4( const std::string& name, float x, float y, float z, float w )
    {
        glUniform4f( glGetUniformLocation( ID, name.c_str() ), x, y, z, w );
    }
    void SetMat2( const std::string& name, const glm::mat2& mat ) const
    {
        glUniformMatrix2fv( glGetUniformLocation( ID, name.c_str() ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
    }
    void SetMat3( const std::string& name, const glm::mat3& mat ) const
    {
        glUniformMatrix3fv( glGetUniformLocation( ID, name.c_str() ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
    }
    void SetMat4( const std::string& name, const glm::mat4& mat ) const
    {
        glUniformMatrix4fv( glGetUniformLocation( ID, name.c_str() ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
    }

private:
    unsigned int ID;
	/*
	* Returns a string containing the text in
	* a vertex/fragment shader source file.
	*/
	static char* ShaderLoadSource( const char* file_path );
	/*
	* Returns a shader object containing a shader
	* compiled from the given GLSL shader file.
	*/
	static GLuint ShaderCompileFromFile( GLenum type, const char* file_path );
	/*
	* Compiles and attaches a shader of the
	* given type to the given program object.
	*/
	void ShaderAttachFromFile( GLuint program, GLenum type, const char* file_path );
	int LoadShaders( const char* vertex_path, const char* fragment_path, const char* geometry_path = NULL );
};

class FShaderTexture : public FShader
{
public:

    FShaderTexture(const char* vertex_path, const char* fragment_path) :FShader(vertex_path, fragment_path) {}

    virtual void Init()override;
};

class FShaderBase : public FShader
{
public:

    FShaderBase(const char* vertex_path, const char* fragment_path) :FShader(vertex_path, fragment_path) {}

	virtual void Init()override;
};