#pragma once

#include <cstddef>

//! struct vertex_shader
/*! Vertex shader source code
 */
struct vertex_shader {
    const char* src;
};

//! struct fragment_shader
/*! Fragment shader source code
 */
struct fragment_shader {
    const char* src;
};

//! class program
/*! Encapsulates an OpenGL program
 */
class Program {
public:
    //! struct ProgramBuildException
    /*! Thrown on program creation error
     */
    class ProgramBuildException;

    //! struct ShaderBuildException
    /*! Thrown on shader creation error
     */
    class ShaderBuildException;

    // dtor.
    virtual ~Program() = default;

    // ctor.
    Program();

    //! Sets program to be used by subsequent calls
    void use() const;

    //! Links program (use during creation phase)
    void link() const;

    //! @set
    void set_value(const char* name, bool value) const;

    //! @set
    void set_value(const char* name, int value) const;

    //! @set
    void set_value(const char* name, float value) const;

    //! @set
    void set_value_vec3(const char* name, const float* value) const;

    //! @set
    void set_value_mat3x3(const char* name, const float* value) const;

    //! @set
    void set_value_vec4(const char* name, const float* value) const;

    //! @set
    void set_value_mat4x4(const char* name, const float* value) const;

    //! Adds shaders
    //! @param
    //!     First shader
    //! @param args...
    //!     Additional shaders
    template <typename T, typename... Args>
    void add_shader(const T first, const Args... args)
    {
        create_shader(first);
        add(args...);
    }

    //! Adds shader base case
    //! @param
    //!     First shader
    template <typename T>
    void add_shader(const T first)
    {
        create_shader(first);
    }
private:
    // Handle to shader program
    unsigned programHandle_;

    //! Helper
    //! @param
    //!     Fragment shader source
    void create_shader(const fragment_shader& s) const;

    //! Helper
    //! @param
    //!     Vertex shader source
    void create_shader(const vertex_shader& s) const;
};

//! struct ProgramBuildException
/*! Thrown on program creation failure
 */
class Program::ProgramBuildException {
public:
    //! @param
    //!     programHandle handle to program instance under construction
    explicit ProgramBuildException(unsigned programHandle);

    //! @return
    //!     Null-terminated error message
    const char* what() const;

    //! @param len
    //!     Error message length
    //! @return
    //!     Null-terminated error message
    const char* what(std::size_t* len /* [out] */) const;
private:
    static constexpr int kBufflen = 1024;
    /* Exception message length */
    int len_;
    /* Exception message */
    char message_[kBufflen + 1];
};

//! struct ShaderBuildException
/*! Thrown on shader creation failure
 */
class Program::ShaderBuildException {
public:
    //! @param shaderHandle
    //!     Handle to instance of the program under construction
    explicit ShaderBuildException(unsigned shaderHandle);

    //! @return
    //!     Null-terminated error message
    const char* what() const;
private:
    static constexpr std::size_t kBufflen = 1024;
    /* Exception message length */
    int len_ = 0;
    /* Exception message */
    char message_[kBufflen + 1];
};
