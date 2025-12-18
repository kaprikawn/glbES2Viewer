#ifndef TYPES_HPP
#define TYPES_HPP

#include <stdint.h>
#include <cstddef>

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;
typedef s32       bool32;
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef size_t    memory_index;
typedef float     f32;
typedef double    f64;

typedef unsigned char uchar;

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

// define the size of a vertex for upload into gl buffer
const u32 VERTEX_BYTES =
  ( sizeof( f32 ) * 4 ) // four float for position ... x, y, z, w
+ ( sizeof( f32 ) * 3 ) // three values for normals
+ ( sizeof( f32 ) * 2 ) // two uv texture coordinates
+ ( sizeof( f32 ) * 4 ) // four floats for colour ... r, g, b, a
;

enum AccessorType { ACCESSOR_VEC3, ACCESSOR_VEC4, ACCESSOR_VEC2, ACCESSOR_SCALAR };

enum GL_COMPONENT_TYPE {
    GL_COMPONENT_TYPE_SIGNED_BYTE    = 5120 // 8 bits
  , GL_COMPONENT_TYPE_UNSIGNED_BYTE  = 5121 // 8 bits
  , GL_COMPONENT_TYPE_SIGNED_SHORT   = 5122 // 16 bits
  , GL_COMPONENT_TYPE_UNSIGNED_SHORT = 5123 // 16 bits
  , GL_COMPONENT_TYPE_UNSIGNED_INT   = 5125 // 32 bits
  , GL_COMPONENT_TYPE_FLOAT          = 5126 // Signed 32 bits
};

#endif //TYPES_HPP
