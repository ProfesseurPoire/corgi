#include <corgi/utils/Utils.h>

#include <corgi/resources/image.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include <corgi/SimpleString.h>

#include "stb_image.h"
//#include "stb_image_write.h"

corgi::Image* Editor::Utils::LoadImageForReal(const corgi::SimpleString& imageFile)
{
    int x, y, channels;
    // Images are horizontal on OpenGL otherwise
    stbi_set_flip_vertically_on_load(true);

    stbi_uc* imageData = stbi_load(  imageFile.c_str(), &x, &y, &channels
        , STBI_rgb_alpha);

    return new corgi::Image(x, y, channels, imageData);
}

// void Editor::Utils::WriteImage(const corgi::String& image_file, const corgi::String& output_folder)
// {
//     int x, y, channels;
    
//     // Images are horizontal on OpenGL otherwise
//     //stbi_set_flip_vertically_on_load(true);
    
//     stbi_uc* image = stbi_load(image_file.cstr(), &x, &y, &channels,
//         STBI_rgb_alpha);
//     std::string f = image_file.cstr();
//     std::string ff = output_folder.cstr();
//     std::string filename = corgi::filesystem::filename(f);

//     filename = filename.substr(0, filename.size() - 4);
    
//     corgi::filesystem::create_directory(ff);

//     std::string fff = (ff+ "/" + filename + ".dat");
//     const char* s = fff.c_str();
//     std::ofstream file(s, std::ofstream::trunc | std::ofstream::binary);
    
//     WriteBinaryImage(file, x, y, channels, (unsigned char*)image);
    
//     stbi_image_free(image);
// }