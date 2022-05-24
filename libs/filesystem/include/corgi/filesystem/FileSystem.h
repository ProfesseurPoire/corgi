#pragma once

#include <corgi/SimpleString.h>
#include <corgi/containers/Vector.h>

// TODO :	Instead of using my SimpleString class I could also work
//			with char*

/*
 *  The filesystem namespace contains functions and structures meant
 *  to help with file operations. At the moment it's mostly a wrapper for 
 *  std::filesystem. But if a platform doesn't support C++ 17, I'll "just"
 *  have to add a compilation flag and use system calls for this specific
 *  platform
 */
namespace corgi
{
    namespace filesystem
    {
        class FileInfo;

        /*!
	     * @brief Returns the file's size in bytes
	     */
        [[nodiscard]] int file_size(const SimpleString& path);

        /*!
	     * @brief   Checks if the file at the given path is a directory
	     *
	     * @param   path Path to the file we want to check
	     *
	     * @return  Returns true if the file is a directory, false otherwise
	     */
        [[nodiscard]] bool is_directory(const SimpleString& path);

        /*!
	     *  @brief  Creates and returns a FileInfo object for a given file
	     *
	     *  @param  path    Path to the file or directory we want to retrieve 
	     *                  information
	     *
	     *  @return Returns a @ref File object storing information about the file
	     */
        [[nodiscard]] FileInfo file_info(const SimpleString& path);

        /*!
		 * @brief   List the files inside the given directory.
		 *          
		 *          Creates a @ref FileInfo object for each file that the function
		 *          finds and adds it to a Vector that will be returned. 
		 * 
		 * 			We skip files that contains .sys because they are
		 *			system files that we can't really interact with
		 *			and causes exceptions when using functions like is_directory
		 *              
		 * @param   directory   Path to the directory to be scanned
		 *
		 * @return  Returns a vector of FileInfo object. Each file found by
		 *          the function will add a new FileInfo object to the vector.
		 *          Empty if the directory doesn't exist or can't be opened
		 */
        [[nodiscard]] Vector<FileInfo> list_directory(const SimpleString& directory,
                                                      bool recursive = false);

        /*!
	 	 * @brief   Renames or moves a file or directory 
	 	 *
	 	 *          The function will move or rename the file or directory located
	 	 *          at @ref oldPath to @ref newPath.
	 	 *          The function will fail if newPath already exist
	 	 *
	 	 * @param   oldPath     Path to the file or folder that will be renamed
	 	 *                      or move
	 	 * @param   newPath     Path where the file or directory located at oldPath 
	 	 *                      will be move, or its new name
	 	 *
	 	 * @return  Returns true if the operation succeeded 
	 	 */
        bool rename(const SimpleString& oldPath, const SimpleString& newPath);

        /*!
	 	 * @brief   Returns the file's extension
	 	 * 
	 	 *          extension("textures/frog.png"); will returns "png". 
	 	 *
	 	 * @param   path    Path to the file we want to extract the extension
	 	 * 
	 	 * @return  Returns a std::string storing the file's extension. 
	 	 *          Returns an empty string if the file had no extension
	 	 */
        [[nodiscard]] SimpleString extension(const SimpleString& path);

        /*!
	 	 * @brief   Returns the filename part of the given path
	 	 * 
	 	 *          filename("textures/frog.png"); will returns "frog.png"
	 	 *
	 	 * @param   path    Path to the file we want to extract the filename
	 	 *
	 	 * @return  Returns a SimpleString with the file's name
	 	 */
        [[nodiscard]] SimpleString filename(const SimpleString& path);

        /*!
	 	 * @brief   Returns the directory part of the given path
	 	 *
	 	 *          directory("textures/frog.png"); will return "textures"
	 	 *
	 	 * @param   path    Path to be worked on
	 	 *
	 	 * @return  Returns a SimpleString with he directory path
	 	 */
        [[nodiscard]] SimpleString directory(const SimpleString& path);

        /*!
	     * @brief   Returns the path without the file's name
	     * 
	     *          pathWithoutName("textures/frog.png"); will returns "textures/"
	     *
	     * @param   path    Path to the file we want to extract the data
	     *
	     * @return  Returns a SimpleString with the path without the name
	     *
	     */
        [[nodiscard]] SimpleString path_without_name(const SimpleString& path);

        /*!
	     * @brief   Remove the file or empty directory located at the given path
	     *
	     *          If the directory isn't empty, the operation will fail. You
	     *          should use removeAll instead.
	     *
	     * @param   path    Path to the file or empty directory to be removed
	     *
	     * @return  Returns true the operation succeeded, false otherwise
	     */
        bool remove(const SimpleString& path);

        /*!
	     * @brief   Remove the file or directory located at the given path
	     *
	     *          removeAll will work even if the file isn't an empty
	     *          directory
	     *
	     * @param   path    Path to the file or directory to be removed
	     *
	     * @return  Returns true if the operation succeeded, false otherwise
	     */
        void remove_all(const SimpleString& path);

        /*!
	     * @brief  Check if a file exist a given path
	     * 
	     *  @param      path    Path to the file to be checked for existence
	     *
	     *  @return     Returns true if the file exist, false otherwise
	     */
        [[nodiscard]] bool file_exist(const SimpleString& path);

        /*!
	     * @brief   Creates a new directory a given path
	     * 
	     * @param   path    Path where the new directory must be created
	     *
	     * @return  Returns true if a directory was created, false otherwise
	     */
        bool create_directory(const SimpleString& path);

        /**
		 * @brief Gets the parent folder of the given path
		 * 
		 * @param folder 
		 * @return std::string 
		 */
        std::string getParentFolder(const std::string& path);

        /*!
	 	 * @brief	Returns the file's size
	 	 */
        [[nodiscard]] long long size(const SimpleString& path);

        /*!
	     * @brief   Stores information about a File
	     *          
	     *          A FileInfo object is only a snapshot of a file at a given
	     *          moment. It won't track any changes made to the file it refers
	     *          to.
	     *
	     *          The file is also read only. And can only be created by 
	     *          filesystem functions
	     */
        class FileInfo
        {
        public:
            friend FileInfo         file_info(const SimpleString& path);
            friend Vector<FileInfo> list_directory(const SimpleString& directory);

            [[nodiscard]] bool is_valid() const;
            [[nodiscard]] bool is_folder() const;

            [[nodiscard]] SimpleString extension() const;
            [[nodiscard]] SimpleString name() const;

            [[nodiscard]] const SimpleString& path() const;

            /**
			 * @brief Sorting function
			 * 
			 * @param rhs 
			 * @return true 
			 * @return false 
			 */
            [[nodiscard]] bool operator<(const FileInfo& rhs) const noexcept
            {
                if(is_folder() != rhs.is_folder())
                {
                    return (is_folder());
                }
                return _path < rhs._path;
            }

        protected:
            FileInfo() = default;

            bool _is_valid  = false;
            bool _is_folder = false;

            SimpleString _path;
        };
    }    // namespace filesystem
}    // namespace corgi