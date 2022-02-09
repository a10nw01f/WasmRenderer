#pragma once

#include <Meta/MPCore.h>
#include <GLES3/gl3.h>

template<class T, class U, U T::* Member>
constexpr int OffsetOf()
{
    return (char*)&((T*)(nullptr)->*Member) - (char*)nullptr;
}

template<class T>
struct ToDataType
{
    using Type = std::decay_t<decltype(std::declval<T>()[0])>;
};

template<class T>
struct ToGLType;

#define TO_GL_TYPE(type, gltype) template<> struct ToGLType<type> { const static GLuint kValue = gltype; };

TO_GL_TYPE(float, GL_FLOAT)

template<class T, class U, U T::* Member, bool Normalize = false> 
struct VertexAttribute
{
    using ParentType = T;
    using Type = U;
    using DataType = typename ToDataType<Type>::Type;
    const static int kOffset = OffsetOf<T,U,Member>();
    const static GLuint kGLType = ToGLType<DataType>::kValue;
    const static bool kNormalize = Normalize;
};

#define VERTEX_ATTRIBUTE(s,m) VertexAttribute<s, decltype(std::declval<s>().m), &s::m>
#define VERTEX_ATTRIBUTE_N(s,m,n) VertexAttribute<s, decltype(std::declval<s>().m), &s::m, n>

template<class T> 
void EnableAttribute(int attribute, T)
{
    glEnableVertexAttribArray(attribute);
    glVertexAttribPointer(
        attribute, 
        sizeof(typename T::Type) / sizeof(typename T::DataType), 
        T::kGLType, 
        T::kNormalize, 
        sizeof(typename T::ParentType), 
        (void*)T::kOffset);
}

template<class... Ts>
void EnableAttributes(TypeList<Ts...>)
{
    int attribute = 0;
    ForEach([&attribute](auto t){
        EnableAttribute(attribute, t);
        attribute++;
    }, Ts{}...);
}

template<class... Ts>
void DisableAttributes(TypeList<Ts...>)
{
    for(int i = 0; i < sizeof...(Ts); ++i)
    {
        glDisableVertexAttribArray(i);
    }
}