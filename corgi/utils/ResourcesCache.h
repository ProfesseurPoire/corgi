#pragma once

#include <corgi/SimpleString.h>
#include <corgi/containers/UniquePtr.h>
#include <corgi/containers/Vector.h>
#include <corgi/resources/Animation.h>
#include <corgi/resources/Resource.h>

#include <map>

namespace corgi
{

    // TODO : Maybe put that everywhere

    //template <class T>
    //concept InheritsFromResource = std::is_base_of<Resource, T>::value;

    /*!
     * @brief	Tries to load files into memory and store them into a cache for further reuse
     *
     *			// more detailed description
     */
    class ResourcesCache
    {

    public:
        using Resources = std::map<SimpleString, UniquePtr<Resource>>;

        // Lifecycle

        /**
		 * @brief	Construct the ResourcesCache with the resource directories to scan
	 	 */
        static void init(SimpleString directories)
        {
            directories_.emplace_back("./resources");
            directories_.emplace_back(directories);
        }

        // Functions

        /*!
		 * @brief	Tries to load a resource located @a identifier. 
		 * 
		 *			This function will first check if a resource called @a name is already stored
		 *			in memory. If the resource has already been loaded into the cache, it will
		 *			return a non owning pointer to the requested resource.
		 *			
		 *			Otherwise, it will try to load it in memory from the resources folder. If no
		 *			file corresponding to the id has been found, it will log a warning and return
		 *			nullptr. Otherwise it return a non owning pointer to the newly loaded resource.
		 *
		 *			Resources can throw exceptions when they are constructed
		 *
		 * @tparam	T	The type of the requested resource
		 * @param	id	The id to a resource is it's local path in one of the resource folder
		 *
		 * @return	Returns a non owning pointer to the requested resource. If the requested
		 *			texture could not be found, returns nullptr.
		 */
        template<class T>
        [[nodiscard]] static T* get(const SimpleString& id)
        {
            if(contains(id))
                return dynamic_cast<T*>(resources_.at(id).get());

            auto path = find(id);

            if(path == "")
            {
                //log_warning("ResourcesCache : No file found at : "+id);
                return nullptr;
            }

            return dynamic_cast<T*>(resources_.emplace(id, make_unique<T>(path, id)).first->second.get());
        }

        /**
		 * @brief 	Gets the container that stores all loaded resources 
		 * 
		 * @return Resources& 
		 */
        static const Resources& resources();

        /*!
	 	 * @brief	Creates a new resource of type T
	 	 *
	 	 *			Use this when you want to create a resource directly from code
	 	 *			so that the ResourcesCache handles it's lifetime
	 	 */
        template<class T, class... Args>
        static T* add(const SimpleString& name, Args... args)
        {
            if(contains(name))
                return dynamic_cast<T*>(resources_.at(name).get());

            return dynamic_cast<T*>(
                resources_.emplace(name, make_unique<T>(std::forward<Args>(args)...)).first->second.get());
        }

        /*!
	     * @brief	Returns true if the cache already loaded a resource with @a identifier
	     *
	     * @param	identifier The identifier to a resource is its local path in one of the resource folder
	     */
        [[nodiscard]] static bool is_cached(const SimpleString& identifier);

        /*!
	     * @brief 	Returns a reference to the directory list
	     */
        [[nodiscard]] static Vector<SimpleString>& directories() noexcept;

        /*!
	     * @brief	Returns the sum of every object's size stored by the cache
	     */
        [[nodiscard]] static int memory_usage();

        /*!
	     * @brief	Free the cache memory and every loaded resource
	     */
        static void clear() noexcept;

        /**
         * @brief   Load every resources that can be found in the resources folders
         */
        static void loadEverything();

        /*!
	     * @brief	Tries to find a file located in one of the resource directories
	     *			by using @a path as the relative part of the full path.
	     *
	     *			Basically loops through every stored resource directory, and
	     *			checks if a file exists at the "directory+"/"+path" location
	     *
	     * @param	relative_path Relative path to the file we want to find
	     *
	     * @return	If a file could be found in one of the resource directories, returns its
	     *			full path. Otherwise it returns ""
	     */
        [[nodiscard]] static SimpleString find(const SimpleString& relative_path);

        /*
	     * @brief	Tries to an animation file
	     */
        [[nodiscard]] static std::map<SimpleString, Animation> load_animations(const SimpleString& path);

        static bool contains(const SimpleString& str);

        static int index(const SimpleString& str);

    private:
        static inline Vector<SimpleString> directories_;
        //std::map<SimpleString, std::unique_ptr<Resource>> resources_;

        // TODO : It's actually slower to use a Vector here but well, at least I can get rid of std::map for now
        // Use my own map implementation one day for this
        //Vector<SimpleString>		_indexes_to_resources;
        static inline Resources resources_;
    };
}    // namespace corgi
