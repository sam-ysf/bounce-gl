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
    struct ProgramBuildException;

    //! struct ShaderBuildException
    /*! Thrown on shader creation error
     */
    struct ShaderBuildException;

    // dtor.
    virtual ~Program() = default;

    // ctor.
    Program();

    //! Sets program to be used by subsequent calls
    void use();

    //! Links program (use during creation phase)
    void link();

    //! @set
    void set_value(const char* name, const bool value);

    //! @set
    void set_value(const char* name, const int value);

    //! @set
    void set_value(const char* name, const float value);

    //! @set
    void set_value_vec3(const char* name, const float* value);

    //! @set
    void set_value_mat3x3(const char* name, const float* value);

    //! @set
    void set_value_vec4(const char* name, const float* value);

    //! @set
    void set_value_mat4x4(const char* name, const float* value);

    //! Adds shaders
    //! @param
    //!     first shader
    //! @param args...
    //!     additional shaders
    template <typename T, typename... Args>
    void add_shader(const T first, const Args... args)
    {
        create_shader(first);
        add(args...);
    }

    //! Adds shader base case
    //! @param
    //!     first shader
    template <typename T>
    void add_shader(const T first)
    {
        create_shader(first);
    }
private:
    // Handle to shader program
    int programHandle_;

    // Helper
    // @param
    //     fragment shader source
    void create_shader(const fragment_shader& s) const;

    // Helper
    // @param
    //     vertex shader source
    void create_shader(const vertex_shader& s) const;
};

//! struct ProgramBuildException
/*! Thrown on program creation failure
 */
class Program::ProgramBuildException {
public:
    //! @param
    //!     programHandle handle to program instance under construction
    explicit ProgramBuildException(int programHandle);

    //! @return
    //!     null-terminated error message
    const char* what() const;

    //! @param
    //!     len error message length
    //! @return
    //!     null-terminated error message
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
struct Program::ShaderBuildException {
public:
    //! Ctor.
    //! @param programHandle
    //!     handle to program instance under construction
    explicit ShaderBuildException(int shaderHandle);

    //! @return
    //!     null-terminated error message
    const char* what() const;

    //! @param len
    //!     error message length
    //! @return
    //!     null-terminated error message
    const char* what(std::size_t* len /* [out] */) const;
private:
    static constexpr int kBufflen = 1024;
    /* Exception message length */
    int len_;
    /* Exception message */
    char message_[kBufflen + 1];
};
