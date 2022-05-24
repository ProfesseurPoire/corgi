#pragma once

#include <corgi/memory/OwningPointer.h>

#include <map>
#include <type_traits>
#include <typeindex>

namespace corgi
{

/*!
 * @brief   A map associates a key and a mapped value.
 *
 *          Here, the mapped value is a pointer to an object that must
 *          inherits from the given template argument.
 *
 *          The key that is used to identity the mapped value is of the type
 *          of the derived object stored by the map
 */
template<class Base>
class PolymorphicMap
{
public:

    // typename helps the template to know whether iterator
    // will be a typename or a member. He can't know that at compile time
    // apparently
    using Iterator = typename std::map<std::type_index, OwningPointer<Base>>::iterator;

    // Lifecycle

    PolymorphicMap() = default;

    /*
     * @brief   Copy is delete because I can't copy with only a pointer
     *          to the base class. I could have a virtual "copy" function but
     *          that would be a bit cluncky
     */
    PolymorphicMap(const PolymorphicMap&) = delete;
    PolymorphicMap(PolymorphicMap&&) = delete;

    PolymorphicMap& operator=(PolymorphicMap&) = delete;
    PolymorphicMap& operator=(const PolymorphicMap&) = delete;
    ~PolymorphicMap() = default;

    // Methods

    Iterator begin() { return _map.begin(); }
    Iterator end() { return _map.end(); }

    /*
     * @brief   Directly creates a new Derived object. The arguments given
     *          by the user to the function will be forwarded to the Derived
     *          constructor
     *
     */
    template<typename Derived, typename... Args>
    void emplace(Args&& ... args)
    {
        check_template_argument<Derived>();
        if (!has<Derived>())
            _map.emplace(typeid(Derived), new Derived(std::forward<Args>(args)...));
    }

    /*
     * @brief Adds a new Derived object to the map using its type as the key
     */
    template<class Derived>
    void add()
    {
        check_template_argument<Derived>();
        static_assert(std::is_default_constructible<Derived>(),
                      "Template argument must have a default constructor.");

        if (!has<Derived>())
            _map.emplace(typeid(Derived), new Derived);
    }

    /*
     * @brief   Checks if the map already has a key that stores the given
     *          template argument's type
     */
    template<class Derived>
    bool has()
    {
        check_template_argument<Derived>();
        return _map.count(typeid(Derived)) > 0;
    }

    template<class Derived>
    void remove()
    {
        check_template_argument<Derived>();
        _map.erase(typeid(Derived));
    }

    template<class Derived>
    ObserverPointer<Derived> get()
    {
        check_template_argument<Derived>();
        if (has<Derived>())
            return _map[typeid(Derived)].create_derived_observer<Derived>();
    }

    /*!
     * @brief   Returns how many System have been registered
     */
    int size()const { return _map.size(); }

    /*!
     * @brief   Delete every system that has been added to the manager
     */
    void clear()
    {
        for (auto& pair : _map)
            pair.second.replace(nullptr);
        _map.clear();
    }

    /*
     * @brief   Returns true if the manager doesn't store any system
     */
    bool empty()const { return _map.size() == 0; }

private:

    template<class Derived>
    void check_template_argument()const
    {
        static_assert(std::is_base_of<Base, Derived>(),
                      "Template argument must inherit from Base.");
    }

    std::map<std::type_index, OwningPointer<Base>> _map;
};
}