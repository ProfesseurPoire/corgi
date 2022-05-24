#pragma once

#include <string>

#include <fstream>
#include <vector>
#include <corgi/SimpleString.h>
//#include <font.h>

namespace corgi
{
	class Image;
}

namespace Editor
{
    namespace Utils
    {
        inline void WriteBinaryImage(std::ofstream& file, int x, int y,
            int channels, unsigned char* pixels)
        {
            file.write(reinterpret_cast<const char*>(&x), sizeof(x));
            file.write(reinterpret_cast<const char*>(&y), sizeof(y));
            file.write(reinterpret_cast<const char*>(&channels), 
                sizeof(channels));

            for (int i = 0; i < x*y*channels; ++i)
            {
                file << pixels[i];
            }
        }

        corgi::Image* LoadImageForReal(const corgi::SimpleString& imageFile);

        //void WriteFont(const std::string& file,
        //    const std::string& outputFolder)
        //{
        //    //corgi::FontLoader font;
        //    //font.load(file.c_str());
        //    //
        //    //std::string filename = corgi::filesystem::filename(file);
        //    //filename = filename.substr(0, filename.length() - 4);
        //    //
        //    //corgi::filesystem::create_directory(outputFolder);
        //    //
        //    //std::ofstream fontImage(outputFolder + "/" + filename + ".dat", 
        //    //std::ofstream::trunc, std::ofstream::binary);
        //    //WriteBinaryImage(fontImage, font.width, font.height, 4,
        //    //font.image);
        //    //
        //    //std::ofstream fontAtlas(outputFolder+"/"+filename+"Atlas.dat",
        //    //std::ofstream::trunc, std::ofstream::binary);
        //    //fontAtlas.close();
        //    //
        //    ////fontAtlas.open(outputFolder + "/" + filename + "Atlas.dat",
        //    //std::ofstream::app, std::ofstream::binary);
        //    //    
        //    //std::string outputFile = outputFolder + "/" + filename + 
        //    //"Atlas.dat";
        //    //
        //    //FILE* fileOld; 
        //    //fileOld = fopen(outputFile.c_str(), "wb");  // r for read, b 
        //    //for binary
        //    //fwrite(font.characters, sizeof(corgi::character_info), 256,
        //    //fileOld); // read 10 bytes to our buffer
        //    //fclose(fileOld);
        //    //corgi::character_info infos[256];
        //    //
        //    //FILE* fileRead;
        //    //fileRead = fopen(outputFile.c_str(), "rb");  // r for read,
        //    //b for binary
        //    //fread(infos, sizeof(corgi::character_info), 256, fileRead); 
        //    // read 10 bytes to our buffer
        //    //fclose(fileRead);
        //}

        inline std::vector<char*> ConvertString(std::vector<std::string>& strings)
        {
            std::vector<char*> result;
            for (auto& string : strings)
            {
                result.push_back(&string.front());
            }
            return result;
        }
        void WriteImage(const std::string& image_file, const std::string& output_folder);
    }
}