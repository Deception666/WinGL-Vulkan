// local includes
#include "ShaderProgram.h"
#include "Shaders.h"

// std includes
#include <algorithm>

// defines an invalid uniform location
const GLint INVALID_UNIFORM_LOCATION = -1;

// defines an invalid value when obtaining a uniform value
const GLint ShaderProgram::UniformValueSelector< GLint >::INVALID_VALUE = 134679852;
const GLuint ShaderProgram::UniformValueSelector< GLuint >::INVALID_VALUE = 134679852;
const GLfloat ShaderProgram::UniformValueSelector< GLfloat >::INVALID_VALUE = 123456.648f;
const GLdouble ShaderProgram::UniformValueSelector< GLdouble >::INVALID_VALUE = 123456.648;

GLuint ShaderProgram::GetCurrentProgram( )
{
   GLint program = 0;
   glGetIntegerv(GL_CURRENT_PROGRAM, &program);

   return static_cast< GLuint >(program);
}

ShaderProgram::ShaderProgram( ) :
mShaderProg    ( 0 )
{
}

ShaderProgram::~ShaderProgram( )
{
   if (mShaderProg)
   {
      // must happen within a valid gl context
      WGL_ASSERT(wglGetCurrentContext());

      glDeleteProgram(mShaderProg);
   }
}

ShaderProgram::ShaderProgram( ShaderProgram && program ) :
mShaderProg( 0 )
{
   std::swap(mShaderProg, program.mShaderProg);
   std::swap(mUniformLocCtr, program.mUniformLocCtr);
}

ShaderProgram & ShaderProgram::operator = ( ShaderProgram && program )
{
   if (this != &program)
   {
      std::swap(mShaderProg, program.mShaderProg);
      std::swap(mUniformLocCtr, program.mUniformLocCtr);
   }

   return *this;
}

bool ShaderProgram::Attach( const GLenum shader, const GLuint shader_obj )
{
   bool attached = false;

   if ((mShaderProg || (mShaderProg = glCreateProgram())) && shader_obj)
   {
      // attach the shader to the program...
      glAttachShader(mShaderProg, shader_obj);

      attached = true;
   }

   return attached;
}

bool ShaderProgram::Attach( const GLenum shader, const std::string & src )
{
   bool attached = false;

   if (mShaderProg || (mShaderProg = glCreateProgram()))
   {
      if (const GLuint shader_obj = shader::LoadShaderSrc(shader, src))
      {
         // determine if the shader is already attached
         const auto attached_shaders = GetAttachedShaders(shader);

         if (attached_shaders.empty())
         {
            // attach the shader to the program...
            glAttachShader(mShaderProg, shader_obj);
            // release the reference from the shader...
            glDeleteShader(shader_obj);
         }
         else
         {
            // get the shader source for the currently attached shader
            const std::string attached_shader_source = GetShaderSource(attached_shaders[0]);

            // get the new shader source
            const std::string new_shader_source = GetShaderSource(shader_obj);

            // get the locations that should not be substringed
            // #version must be specified; otherwise it will not compile correctly (core profile)
            const size_t version_location_beg = new_shader_source.find("#version");
            const size_t version_location_end = new_shader_source.find("\n", version_location_beg);

            // substring out the #version
            const std::string new_shader_source_minus_version =
               new_shader_source.substr(0, version_location_beg) + new_shader_source.substr(version_location_end + 1);

            // remove the currently attached shader
            glDetachShader(mShaderProg, attached_shaders[0]);
            // release the created shader object
            glDeleteShader(shader_obj);

            // attach the new source
            Attach(shader, attached_shader_source + "\n\n" + new_shader_source_minus_version);
         }

         attached = true;
      }
   }

   return attached;
}

bool ShaderProgram::AttachFile( const GLenum shader, const std::string & file )
{
   bool attached = false;

   if (mShaderProg || (mShaderProg = glCreateProgram()))
   {
      if (const GLuint shader_obj = shader::LoadShaderFile(shader, file))
      {
         // determine if the shader is already attached
         const auto attached_shaders = GetAttachedShaders(shader);

         if (attached_shaders.empty())
         {
            // attach the shader to the program...
            glAttachShader(mShaderProg, shader_obj);
            // release the reference from the shader...
            glDeleteShader(shader_obj);
         }
         else
         {
            // get the shader source for the currently attached shader
            const std::string attached_shader_source = GetShaderSource(attached_shaders[0]);

            // get the new shader source
            const std::string new_shader_source = GetShaderSource(shader_obj);

            // get the locations that should not be substringed
            // #version must be specified; otherwise it will not compile correctly (core profile)
            const size_t version_location_beg = new_shader_source.find("#version");
            const size_t version_location_end = new_shader_source.find("\n", version_location_beg);

            // substring out the #version
            const std::string new_shader_source_minus_version =
               new_shader_source.substr(0, version_location_beg) + new_shader_source.substr(version_location_end + 1);

            // remove the currently attached shader
            glDetachShader(mShaderProg, attached_shaders[0]);
            // release the created shader object
            glDeleteShader(shader_obj);

            // attach the new source
            Attach(shader, attached_shader_source + "\n\n" + new_shader_source_minus_version);
         }

         attached = true;
      }
   }

   return attached;
}

std::vector< GLuint > ShaderProgram::GetAttachedShaders( const GLenum shader ) const
{
   std::vector< GLuint > shaders;

   if (mShaderProg)
   {
      // obtain the maximum number of attached shaders
      const GLint num_attached_shaders = [ this ] ( ) -> GLint
      {
         GLint num_attached_shaders = 0;
         glGetProgramiv(mShaderProg, GL_ATTACHED_SHADERS, &num_attached_shaders);

         return num_attached_shaders;
      }();

      if (num_attached_shaders)
      {
         // obtain all the shader names attached to this program
         shaders.resize(num_attached_shaders);
         glGetAttachedShaders(mShaderProg, num_attached_shaders, nullptr, &shaders[0]);

         // validate for the existance of the shader type requested
         const auto end = std::remove_if(shaders.begin(), shaders.end(),
         [ &shader ] ( const GLint shader_obj ) -> bool
         {
            // obtain the shader type
            GLint shader_type = 0;
            glGetShaderiv(shader_obj, GL_SHADER_TYPE, &shader_type);
            
            return shader != shader_type;
         });

         // resize to only include those types that match shader
         shaders.erase(end, shaders.end());
      }
   }

   return shaders;
}

std::string ShaderProgram::GetShaderSource( const GLenum shader ) const
{
   std::string shader_source;

   // get the size of the shader source
   const GLsizei source_length =
   [ shader ] ( ) -> GLsizei
   {
      GLsizei source_length = 0;
      glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &source_length);

      return source_length;
   }();

   if (source_length)
   {
      // resize the shader source
      shader_source.resize(source_length - 1);

      // fill in the shader source
      glGetShaderSource(shader, source_length, nullptr, &shader_source[0]);
   }

   return shader_source;
}

bool ShaderProgram::Link( )
{
   WGL_ASSERT(mShaderProg);

   return mShaderProg && shader::LinkShader(mShaderProg);
}

GLint ShaderProgram::GetUniformLocation( const std::string & uniform )
{
   WGL_ASSERT(mShaderProg);

   GLint uniform_loc = INVALID_UNIFORM_LOCATION;

   if (mShaderProg)
   {
      const auto cached_uniform = mUniformLocCtr.find(uniform);

      if (cached_uniform != mUniformLocCtr.end())
      {
         uniform_loc = cached_uniform->second;
      }

      if (INVALID_UNIFORM_LOCATION == uniform_loc)
      {
         if (INVALID_UNIFORM_LOCATION != (uniform_loc = glGetUniformLocation(mShaderProg, uniform.c_str())))
         {
            mUniformLocCtr.insert(UniformLocationCtr::value_type(uniform, uniform_loc));
         }
      }
   }

   return uniform_loc;
}

void ShaderProgram::Enable( )
{
   if (mShaderProg) glUseProgram(mShaderProg);
}

void ShaderProgram::Disable( )
{
   glUseProgram(0);
}

