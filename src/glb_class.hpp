#ifndef GLB_CLASS_HPP
#define GLB_CLASS_HPP

#include "strings.hpp"
#include "json_reader.hpp"

class Glb_Mesh_Data {
  
  private : 
    
  public : 
    
    
};

class Glb_imported_object {
  
  private :
  
    GltfHeader*     gltf_header;
    char*           glb_file_binary_data_pointer;
    u32             bin_start_offset;
    ReadFileResult  glb_file;
    
  public :
    
    void read_file_offsets () {
      
      gltf_header = ( GltfHeader* ) glb_file.contents;
      
      u32 gltf_header_size_in_bytes = 12; // magic + version + length
      u32 json_header_in_bytes      = 8; // 4 bytes for chunk length, 4 bytes for chunk type
      u32 json_string_in_bytes      = gltf_header -> json_chunk_length;
      u32 bin_header_in_bytes       = 8; // 4 bytes for chunk length, 4 bytes for chunk type
      bin_start_offset              = gltf_header_size_in_bytes + json_header_in_bytes + json_string_in_bytes + bin_header_in_bytes;
      
      glb_file_binary_data_pointer  = ( char* )( ( char* ) glb_file.contents + bin_start_offset );
      
    }
    
    // constructor
    Glb_imported_object ( String filepath ) {
      
      if ( fileExists ( filepath.get_string_data() ) ) {
        SDL_LogInfo ( SDL_LOG_CATEGORY_APPLICATION, "File exists : %s\n", filepath.get_string_data() );
        
        glb_file = read_entire_file ( filepath.get_string_data() );
        
        
        
        free_read_file_result ( &glb_file );
      } else {
        SDL_LogInfo ( SDL_LOG_CATEGORY_APPLICATION, "File does not exist : %s\n", filepath.get_string_data() );
      }
    }
    
    // destructor
    ~Glb_imported_object () {
      
    }
    
      
};

#endif //GLB_CLASS_HPP
