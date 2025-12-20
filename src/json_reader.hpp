#ifndef JSON_HPP
#define JSON_HPP

#include "types.hpp"
#include "strings.hpp"

#ifdef _WIN32

#include "..\ext\json.hpp"

#pragma pack(1)
struct  GltfHeader {
  u32 magic;
  u32 version;
  u32 length;
  u32 json_chunk_length;
  u32 json_chunk_type;
};

#elif __linux__

#include "../ext/json.hpp"

struct __attribute__ ((__packed__)) GltfHeader {
  u32 magic;
  u32 version;
  u32 length;
  u32 json_chunk_length;
  u32 json_chunk_type;
};

#endif

enum GltfContentType { GLTF_VERTICES, GLTF_INDICES, GLTF_NORMALS, GLTF_TEX_COORD0, GLTF_COLOR0, GLTF_IMAGE };

struct MeshData {
  bool32  is_collider = false;
  u32     vertex_data_in_bytes;
  u32     vertex_count;
  u32     index_data_raw_in_bytes;
  u32     index_data_in_bytes;
  u32     index_count;
  u32     normal_data_in_bytes;
  u32     normal_count;
  u32     tex_coord0_data_in_bytes;
  u32     tex_coord0_count;
  u32     colour0_data_in_bytes;
  u32     colour0_count;
  u32     image_data_in_bytes;
  f32*    vertex_data;
  f32*    normal_data;
  u16*    index_data_raw;
  u32*    index_data;
  f32*    tex_coord0_data;
  f32*    colour0_data;
  u8*     image_data;
  u32     gl_vertex_offset;
  u32     gl_normal_offset;
  u32     gl_tex_coord0_offset;
  u32     gl_index_offset;
};

struct JsonString {
  u32   json_char_count;
  char* json_string;
};

struct GltfBufferViewInfo {
  u32 buffer_view;
  u32 component_type;
  u32 gltf_count; // count as defined by the JSON
  u32 buffer;
  u32 byte_length;
  u32 byte_offset;
  u32 count; // count of values
  u32 offset;
  s32 type;
};

struct MeshPositionIndices {
  u32   vertices;
  u32   normals;
  u32   texcoord_0;
  u32   colour_0;
  u32   indices;
  u32   material;
  bool  colour_0_populated    = false;
  bool  texcoord_0_populated  = false;
  bool  material_populated    = false;
};

struct AccessorData {
  u32   buffer_view;
  u32   component_type;
  u32   count;
  s32   accessor_type;
  char  type[ 7 ];
};

struct BufferViewData {
  u32 buffer;
  u32 byte_length;
  u32 byte_offset;
  u32 gl_buffer_byte_offset;
};

inline u32 json_size_in_bytes ( ReadFileResult* gltf_file ) {
  GltfHeader* gltf_header = ( GltfHeader* )gltf_file -> contents;
  return gltf_header -> json_chunk_length;
}

inline u32 get_bin_start_offset (  GltfHeader* gltf_header ) {
  u32 result;
  u32 gltf_header_size_in_bytes = 12; // magic + version + length
  u32 json_header_in_bytes      = 8; // 4 bytes for chunk length, 4 bytes for chunk type
  u32 json_string_in_bytes      = gltf_header -> json_chunk_length;
  u32 bin_header_in_bytes       = 8; // 4 bytes for chunk length, 4 bytes for chunk type
  result                        = gltf_header_size_in_bytes + json_header_in_bytes + json_string_in_bytes + bin_header_in_bytes;
  return result;
}

inline void pull_out_json_string ( ReadFileResult* gltf_file, char* json_string, u32 json_string_length ) {
  
  GltfHeader* gltf_header               = ( GltfHeader* )gltf_file -> contents;
  
  u8          file_header_size_in_bytes = 12; // magic + version + length
  u8          json_header_size_in_bytes = 8; // 4 bytes for chunk length, 4 bytes for chunk type
  u32         jsonCurrentIndex          = 0;
  const char* json_data = ( char* )( ( u8* )gltf_file -> contents + file_header_size_in_bytes + json_header_size_in_bytes );
  for( u32 i = 0; i < json_string_length; i++ ) {
    json_string[ jsonCurrentIndex++ ] = json_data[ i ];
  }
  json_string[ jsonCurrentIndex ] = '\0';
  for( u32 i = json_string_length + 1; i > json_string_length - 10; i-- ) {
    if( json_string[ i ] == 0x7D || json_string[ i ] == 0x5D )//   } or ]
      break;
    json_string[ i ] = '\0'; // make sure there's no erroneous padding bits at the end of the json, set to \0 if so
  }
}

u32 count_meshes ( nlohmann::json  parsed_json ) {
  u32             result  = 0;
  nlohmann::json  nodes   = parsed_json [ "nodes" ];
  
  for( nlohmann::json::iterator it1 = nodes.begin(); it1 != nodes.end(); ++it1 ) {
    nlohmann::json node = *it1;
    if( node.contains( "mesh" ) && node.contains( "name" ) ) {
      result++;
    }
  }
  
  return result;
}


void populate_mesh_name ( u32 target_mesh_index, nlohmann::json parsed_json, char* mesh_name ) {
  return; // @TODO
}

// s32 get_image_buffer_view_index ( , nlohmann::json parsed_json ) {
//   s32 result;
  
//   return result;
// }

MeshPositionIndices get_mesh_position_indices ( u32 target_mesh_index, nlohmann::json parsed_json ) {
  
  MeshPositionIndices result;
  
  result.vertices   = parsed_json[ "meshes" ][ target_mesh_index ][ "primitives" ][ 0 ][ "attributes" ][ "POSITION" ];
  result.normals    = parsed_json[ "meshes" ][ target_mesh_index ][ "primitives" ][ 0 ][ "attributes" ][ "NORMAL" ];
  result.indices    = parsed_json[ "meshes" ][ target_mesh_index ][ "primitives" ][ 0 ][ "indices" ];
  
  nlohmann::json  attributes   = parsed_json [ "meshes" ][ target_mesh_index ][ "primitives" ][ 0 ][ "attributes" ];
  if ( attributes.contains( "COLOR_0" ) ) {
    result.colour_0 = parsed_json[ "meshes" ][ target_mesh_index ][ "primitives" ][ 0 ][ "attributes" ][ "COLOR_0" ];
    result.colour_0_populated = true;
  }
  
  if ( attributes.contains( "TEXCOORD_0" ) ) {
    result.texcoord_0 = parsed_json[ "meshes" ][ target_mesh_index ][ "primitives" ][ 0 ][ "attributes" ][ "TEXCOORD_0" ];
    result.texcoord_0_populated = true;
  }
  
  nlohmann::json  primitives   = parsed_json [ "meshes" ][ target_mesh_index ][ "primitives" ][ 0 ];
  if ( attributes.contains( "material" ) ) {
    result.material   = parsed_json[ "meshes" ][ target_mesh_index ][ "primitives" ][ 0 ][ "material" ];
    result.material_populated = true;
  }
  
  return result;
}

AccessorData get_accessor_data ( u32 target_accessor_index, nlohmann::json parsed_json ) {
  
  AccessorData result = {};
  
  nlohmann::json bufferView = parsed_json [ "accessors" ][ target_accessor_index ];
  
  result.buffer_view    = parsed_json [ "accessors" ][ target_accessor_index ][ "bufferView" ];
  result.component_type = parsed_json [ "accessors" ][ target_accessor_index ][ "componentType" ];
  result.count          = parsed_json [ "accessors" ][ target_accessor_index ][ "count" ];
  
  std::string this_type = parsed_json [ "accessors" ][ target_accessor_index ][ "type" ];
  u32 type_length       = this_type.length();
  
  for ( u32 i = 0; i < this_type.length(); i++ ) {
    char this_char = this_type[ i ];
    result.type[ i ] = this_char;
  }
  
  return result;
}

BufferViewData get_buffer_view_data ( u32 target_buffer_view_index, nlohmann::json parsed_json ) {
  
  BufferViewData result = {};
  
  result.buffer       = parsed_json[ "bufferViews" ][ target_buffer_view_index ][ "buffer" ];
  result.byte_length  = parsed_json[ "bufferViews" ][ target_buffer_view_index ][ "byteLength" ];
  result.byte_offset  = parsed_json[ "bufferViews" ][ target_buffer_view_index ][ "byteOffset" ];
  
  return result;
}

// MeshData populate_mesh_data ( u32 target_mesh_index, const char* json_string, ReadFileResult* gltf_file, f32 scale ) {
  
//   MeshData result;
  
//   // TODO : get the mesh name????
  
//   MeshPositionIndices mesh_position_indices = get_mesh_position_indices( target_mesh_index, json_string, json_char_count );
  
  
//   return result;
// }

u32 get_image_buffer_view_index ( nlohmann::json parsed_json ) {
  u32 result = 0xFFFFFFFF;
  
  if ( parsed_json.contains( "images" ) ) {
    result = parsed_json [ "images" ][ 0 ][ "bufferView" ];
  }
  
  return result;
}

#endif //JSON_HPP
