#include <mbgl/gl/vertex_attribute_gl.hpp>

#include <mbgl/gl/defines.hpp>
#include <mbgl/platform/gl_functions.hpp>
#include <mbgl/shaders/gl/shader_program_gl.hpp>

#include <cstring>

namespace mbgl {
namespace gl {

class VertexAttributeArrayGL;

using namespace platform;

int VertexAttributeGL::getSize(GLenum glType) {
    switch (glType) {
        case GL_FLOAT:              return 4*1;
        case GL_FLOAT_VEC2:         return 4*2;
        case GL_FLOAT_VEC3:         return 4*3;
        case GL_FLOAT_VEC4:         return 4*4;
        case GL_FLOAT_MAT2:         return 4*4;
        case GL_INT:                return 4*1;
        case GL_UNSIGNED_INT:       return 4*1;
        case GL_INT_VEC2:           return 4*2;
        case GL_INT_VEC3:           return 4*3;
        case GL_INT_VEC4:           return 4*4;
        default:                    return 0;
    }
}

int VertexAttributeGL::getStride(GLenum glType) {
    switch (glType) {
        case GL_FLOAT:              return 4*1;
        case GL_FLOAT_VEC2:         return 4*2;
        case GL_FLOAT_VEC3:         return 4*4;
        case GL_FLOAT_VEC4:         return 4*4;
        case GL_FLOAT_MAT2:         return 4*4;
        case GL_INT:                return 4*1;
        case GL_UNSIGNED_INT:       return 4*1;
        case GL_INT_VEC2:           return 4*2;
        case GL_INT_VEC3:           return 4*4;
        case GL_INT_VEC4:           return 4*4;
        default:                    return 0;
    }
}

const void* VertexAttributeGL::getPtr(const gfx::VertexAttribute::ElementType& element, GLenum glType) {
    switch (glType) {
        case GL_FLOAT:
        case GL_FLOAT_VEC2:
        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC4:
        case GL_FLOAT_MAT2:         return std::get_if<float>(&element);
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:           return std::get_if<int>(&element);
        default:                    return nullptr;
    }
}

void VertexAttributeGL::setGLType(platform::GLenum value) {
    glType = value;
    stride = getStride(glType);
}

std::size_t VertexAttributeGL::getStride() const {
    return getStride(getGLType());
}

const std::vector<std::uint8_t>& VertexAttributeGL::getRaw() const {
    if (dirty) {
        const auto count = getCount();
        const auto size_ = getSize(getGLType());

        rawData.resize(stride * count);
        std::fill(rawData.begin(), rawData.end(), 0);

        std::uint8_t* outPtr = &rawData[0];
        for (std::size_t i = 0; i < count; ++i) {
            if (const auto rawPtr = getPtr(items[i], getGLType())) {
                std::memcpy(outPtr, rawPtr, size_);
            }
            outPtr += stride;
        }

        dirty = false;
    }
    return rawData;
}

namespace {
    template <typename T> void applyUniform(GLint, const T&);
    template <> void applyUniform(GLint, const std::int32_t&) { }
    template <> void applyUniform(GLint, const gfx::VertexAttribute::int2&) { }
    template <> void applyUniform(GLint, const gfx::VertexAttribute::int3&) { }
    template <> void applyUniform(GLint, const gfx::VertexAttribute::int4&) { }
    template <> void applyUniform(GLint, const float& ) { }
    template <> void applyUniform(GLint, const gfx::VertexAttribute::float2& ) { }
    template <> void applyUniform(GLint, const gfx::VertexAttribute::float3& ) { }
    template <> void applyUniform(GLint, const gfx::VertexAttribute::float4& ) { }
    template <> void applyUniform(GLint, const gfx::VertexAttribute::matf3& ) { }
    template <> void applyUniform(GLint location, const gfx::VertexAttribute::matf4& value) {
        MBGL_CHECK_ERROR(glUniformMatrix4fv(location, 1, GL_FALSE, &value[0]));
    }
}

struct ApplyUniform {
    GLint location;
    template <typename T> void operator()(const T& value) { applyUniform(location, value); }
};

void VertexAttributeArrayGL::applyUniforms(const gfx::ShaderProgramBase& shader) {
    const auto& glShader = static_cast<const ShaderProgramGL&>(shader);
    const auto program = glShader.getGLProgramID();

    for (auto& kv : attrs) {
        const auto& name = kv.first;
        auto& uniform = kv.second;

        if (uniform->getDirty()) {
            if (uniform->getIndex() < 0) {
                const auto index = MBGL_CHECK_ERROR(glGetUniformLocation(program, name.c_str()));
                uniform->setIndex(index);
            }
            std::visit(ApplyUniform { uniform->getIndex() }, uniform->get(0));
            uniform->clearDirty();
        }
    }
}

} // namespace gfx
} // namespace mbgl
