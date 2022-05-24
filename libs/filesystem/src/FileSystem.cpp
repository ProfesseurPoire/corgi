#include <corgi/filesystem/FileSystem.h>
#include <stdarg.h>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace corgi::filesystem
{
    long long size(const SimpleString& path)
    {
        std::ifstream file(path.c_str(), std::ifstream::in | std::ifstream::binary |
                                             std::ifstream::ate);
        return file.tellg();
    }

    Vector<FileInfo> list_directory(const SimpleString& directory, bool recursive)
    {
        Vector<FileInfo> files;

        if(!std::filesystem::exists(directory.c_str()))
            return files;

        for(auto& p : std::filesystem::directory_iterator(directory.c_str()))
        {
            auto fileInfo = file_info(p.path().string().c_str());

            // We skip files that contains .sys because they are
            // system files that we can't really interact with
            // and causes exceptions when using functions like is_directory
            if(fileInfo.extension() == "sys")
                continue;

            // We also skip this file on Windows, same, it's locked and
            // triggers exceptions
            if(fileInfo.name() == "DumpStack.log.tmp")
                continue;

            files.push_back(file_info(p.path().string().c_str()));

            // If the file is a directory, and the function was called with
            // recursive set to true, we simply call list_directory again
            // on that directory, and append what the function returns
            if(recursive && p.is_directory())
            {
                auto f = list_directory(p.path().string().c_str(), recursive);

                // TODO : Make an insert/append function that takes a full Vector please
                //files.insert(files.end(), f.begin(), f.end());
                for(auto& file : f)
                {
                    files.emplace_back(file);
                }
            }
        }
        return files;
    }

    bool remove(const SimpleString& path)
    {
        return std::filesystem::remove(path.c_str());
    }

    bool create_directory(const SimpleString& path)
    {
        return std::filesystem::create_directory(path.c_str());
    }

    int find_last(const SimpleString& str, char c)
    {
        for(size_t i = str.size() - 1; i > 0; i--)
        {
            if(str[i] == c)
            {
                return i;
            }
        }
        return -1;
    }

    std::string getParentFolder(const std::string& folder)
    {
        auto sub = folder.substr(0, folder.size() - 1);
        return directory(sub.c_str()).c_str();
    }

    void remove_all(const SimpleString& path)
    {
        std::filesystem::remove_all(path.c_str());
    }

    bool file_exist(const SimpleString& path)
    {
        return std::filesystem::exists(path.c_str());
    }

    /*
	 * @brief  Find the last occurrence of character c in str
	 */
    int find_last(const SimpleString& str, const SimpleString& characters)
    {
        for(int i = str.size() - 1; i >= 0; i--)
        {
            for(const auto& character : characters)
            {
                if(str[i] == character)
                {
                    return i;
                }
            }
        }
        return -1;
    }

    SimpleString directory(const SimpleString& path)
    {
        int index_last = find_last(path, '/');
        int other      = find_last(path, '\\');

        int final_index = other;

        if(index_last > other)
        {
            final_index = index_last;
        }

        if(final_index == -1)
            return path;

        return path.substr(0, final_index);
    }

    bool rename(const SimpleString& path, const SimpleString& newPath)
    {
        std::error_code errorCode;
        std::filesystem::rename(path.c_str(), newPath.c_str(), errorCode);
        std::cout << errorCode.message() << std::endl;
        return true;
    }

    SimpleString path_without_name(const SimpleString& path)
    {
        // we look for the first "/" or "\" if the path from the end
        const int index = find_last(path, "/\\");

        // If index equals -1 it means there's nothing before the filename
        if(index == -1)
            return SimpleString();

        return path.substr(0, index + 1);
    }

    SimpleString filename(const SimpleString& path)
    {
        for(size_t i = path.size() - 1; i > 0; --i)
            if(path[i] == '/' || path[i] == '\\')
                return path.substr(
                    i + 1,
                    SimpleString::npos);    //npos means until the end of the string
        return SimpleString();
    }

    FileInfo file_info(const SimpleString& path)
    {
        FileInfo info;
        info._path = path;
        return info;
    }

    SimpleString extension(const SimpleString& path)
    {
        SimpleString str(path);
        SimpleString ext;

        bool start_ext = false;

        for(int i = str.size() - 1; i >= 0; --i)
        {
            if(str[i] == '.')
                return ext;
            ext.addFront(str[i]);
        }
        return ext;
    }

    int file_size(const SimpleString& path)
    {
        std::ifstream file(path.c_str(), std::ifstream::in | std::ifstream::binary);

        int fileSize = 0;

        if(file.is_open())
        {
            file.seekg(0, std::ios::end);
            return int(file.tellg());
        }
        return -1;
    }

    bool is_directory(const SimpleString& path)
    {
        return std::filesystem::is_directory(path.c_str());
    }

    bool FileInfo::is_valid() const { return _is_valid; }

    bool FileInfo::is_folder() const { return is_directory(_path); }

    SimpleString FileInfo::extension() const { return filesystem::extension(_path); }

    SimpleString FileInfo::name() const { return filename(_path); }

    const SimpleString& FileInfo::path() const { return _path; }
}    // namespace corgi::filesystem