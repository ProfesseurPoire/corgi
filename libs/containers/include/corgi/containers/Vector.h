#pragma once

#include <utility>

namespace corgi
{
    using SizeType = int;

    /*!
        @brief Stores index accessible items in a contiguous dynamic array
        Use this class if you need to store an indeterminate amount of items.
        Every time a new item is added to the array, a check is made to know if
        there is enough space to directly add the item to the original, by
        comparing the _size and _item_count variables. If there's not enough
        space for the item to be added to the array, a new one is created with
        the desired size, the old array content is copied back to the new array,
        and the new item is added.
    */
    template<class ValueType>
    class Vector
    {
        using Reference      = ValueType&;
        using ConstReference = const ValueType&;
        using Pointer        = ValueType*;
        using ConstPointer   = const ValueType*;

    public:
        // Constructors

        void destroy()
        {
            // We manually call the destructor for every real item in our array
            for(int i = 0; i < _size; i++)
                (&_items[i])->~ValueType();

            // We deallocate the memory
            if(_capacity > 0 && _items != nullptr)
                ::operator delete[](_items, _capacity * sizeof(ValueType));
        }

        /*!
         * @brief Default constructor
         */
        Vector() = default;

        // Warning : this takes ownership of values
        Vector(ValueType* values, int count)
        {
            _items    = values;
            _capacity = count;
            _size     = count;
        }

        Vector(int size, const ValueType& val) { resize(size, val); }

        ~Vector() { destroy(); }

        void from_array(const ValueType* const values, int count)
        {
            resize(count);

            for(int i = 0; i < count; ++i)
                _items[i] = values[i];
        }

        /*!
        * @brief Initialize the list with a certain @ref size
        * @details Usefull when you know in advance how many item you have to
        * store in the list. This will reduce the need to resize the array,
        * which is a very costly operation
        */
        explicit Vector(const int size) { resize(size); }

        /*  Vector(Vector&& l)
              {
              move(l);
          }*/

        Vector(const Vector& other)
        {
            _capacity = other._capacity;
            _size     = other._size;

            if(_capacity == 0)
                return;

            _items = (ValueType*)new unsigned char[sizeof(ValueType) * _capacity];

            // Copy constructing the things
            // We need to use the special new operator because technically
            // the objects hasn't been constructed
            for(int i = 0; i < _size; i++)
                new(&_items[i]) ValueType(other._items[i]);
        }

        // Assignment Operators

        Vector& operator=(const Vector& other)
        {
            destroy();

            _capacity = other._capacity;
            _size     = other._size;
            _items    = nullptr;

            if(_capacity == 0)
                return *this;

            _items = (ValueType*)new unsigned char[sizeof(ValueType) * _capacity];

            for(int i = 0; i < _size; i++)
                new(&_items[i]) ValueType(other._items[i]);

            return *this;
        }

        Vector& operator=(Vector&& other) noexcept
        {
            destroy();

            _items    = other._items;
            _size     = other._size;
            _capacity = other._capacity;

            other._items    = nullptr;
            other._size     = 0;
            other._capacity = 0;

            return *this;
        }

        Vector(Vector&& other) noexcept
        {
            _items    = other._items;
            _size     = other._size;
            _capacity = other._capacity;

            other._items    = nullptr;
            other._size     = 0;
            other._capacity = 0;
        }

        // Access

        /*!
         * @brief Returns a const reference to the item stored at @ref index position
         * @pre @ref index must in the list range
         */
        ConstReference operator[](const int index) const { return _items[index]; }

        /*!
        * @brief Returns a reference to the item stored at @ref index
        * position
        * @pre @ref index must be in the list range
        */
        Reference operator[](const int index) { return _items[index]; }

        // Comparison operators

        // @brief Returns true if the two list stores the same items in the same
        // order
        bool operator==(const Vector& other) const noexcept
        {
            if(_size != other._size)
            {
                return false;
            }

            // From my experience, it looks like it's worse to use memcmp, which is weird
            //return memcmp(_items, other._items, _size * sizeof ValueType);
            const auto* a = other._items;

            for(SizeType i = 0; i < _size; ++i)
            {
                if(_items[i] != a[i])
                {
                    return false;
                }
            }
            return true;
        }

        int operator<=>(const Vector& other) const noexcept
        {
            if(_size < other._size)
                return -1;

            if(_size > other._size)
                return 1;

            const auto* a = other._items;

            for(SizeType i = 0; i < _size; ++i)
            {
                if(_items[i] < a[i])
                    return -1;

                if(a[i] < _items[i])
                    return 1;
            }

            return 0;
        }

        bool operator<(const Vector& other) const noexcept
        {
            if(_size != other._size)
            {
                return _size < other._size;
            }

            // From my experience, it looks like it's worse to use memcmp, which is weird
            //return memcmp(_items, other._items, _size * sizeof ValueType);
            const auto* a = other._items;

            for(unsigned i = 0; i < _size; ++i)
            {
                if(_items[i] < a[i])
                    return true;

                if(a[i] < _items[i])
                    return false;
            }
            return false;
        }

        // @brief Returns false if the two list stores the same items in the
        // same order
        bool operator!=(const Vector& l) const { return !(*this == l); }

        // Methods

        /*!
            * @brief  Returns a reference to the item stored at @ref index position
            * No checks are made to know if @ref index is within bounds
            */
        Reference at(const SizeType index)
        {
            //if (!in_range(index))
            //throw std::out_of_range("index argument is out of the list range");
            return _items[index];
        }

        /*!
         * @brief Clear the list from its elements
         *
         * Set @ref _item_count to 0, nothing is deleted here until the list
         * goes out of scope
         */
        void clear() { _size = 0; }

        /*!
        * @brief Adds an item to the current original Resize original if no
        * space is available to add @item to the original
        */
        void add(const ValueType& item) { emplace_back(item); }
        /*
                template<class ... Args>
                void emplace_back(Args ... args)
                {
                    push_back(ValueType(args...));
                }*/

        void push_back(const ValueType& item) { add(item); }

        void resize(const SizeType new_size, const ValueType& default_value)
        {
            // Create a new array
            auto* values = (ValueType*)new unsigned char[sizeof(ValueType) * new_size];

            for(int i = 0; i < _size; i++)
            {
                new(&values[i]) ValueType(default_value);
                (&_items[i])->~ValueType();
            }

            // We use operator delete to deallocate memory without calling the destructors
            ::operator delete(_items);

            // And we switch the arrays
            _items    = values;
            _capacity = new_size;
            _size     = new_size;
        }

        /*!
         * @brief  Resize the array to be able to store @ref new_size items
         * @param new_size How many items the array can store
         */
        void resize(const SizeType new_size)
        {
            // Create a new array
            auto* values = (ValueType*)new unsigned char[sizeof(ValueType) * new_size];

            // We move copy the previous items in the array

            if(_size != 0)
            {
                for(int i = 0; i < _size; i++)
                {
                    new(&values[i]) ValueType(_items[i]);
                    (&_items[i])->~ValueType();
                }
            }

            // We use operator delete to deallocate memory without calling the destructors
            ::operator delete(_items);

            // And we switch the arrays
            _items    = values;
            _capacity = new_size;
            _size     = new_size;
        }

        template<class... Args>
        ValueType& emplace_back(Args&&... args)
        {
            _size++;

            if(_capacity < _size)
            {
                // Allocating memory so we have enough place to insert the new item
                auto* values = (ValueType*)new unsigned char[sizeof(ValueType) * _size];

                // Constructing the new item at the end of the array
                new(&values[_size - 1]) ValueType(std::forward<Args>(args)...);

                // We move copy the previous items in the array
                if(_size != 0)
                {
                    for(int i = 0; i < _size - 1; i++)
                    {
                        // TODO : Not too sure about this but it seems std::vector copy and doesn't move
                        new(&values[i]) ValueType(std::move(_items[i]));
                        (&_items[i])->~ValueType();
                    }
                }

                // Deallocating memory of the previous array without calling the destructors
                ::operator delete[](_items);

                // Switch new array with the previous one

                _items    = values;
                _capacity = _size;
            }
            else
            {
                new(&_items[_size - 1]) ValueType(std::forward<Args>(args)...);
            }

            return back();
        }

        // Push back the item n times
        void push_back(int n, const ValueType& item)
        {
            for(int i = 0; i < n; ++i)
                push_back(n);
        }

        void add(const Vector& list)
        {
            const int last_item_count = _size;
            _size += list.size();

            if(_capacity < _size)
                resize(_size);

            for(int i = 0; i < list.size(); ++i)
                _items[last_item_count + i] = list._items[i];
        }

        /*!
        * @brief  Returns a const reference to the item stored at @ref index
        * position No checks are made to know if @ref index is within bounds
        */
        [[nodiscard]] const ValueType& at(const SizeType index) const
        {
            return _items[index];
        }

        /*
         *  @brief Returns how many items the list can actually hold
         */
        [[nodiscard]] SizeType capacity() const { return _capacity; }

        /*
         *   @brief Returns the size of the list in bytes
         */
        [[nodiscard]] SizeType memorySize() const
        {
            return _capacity * sizeof(ValueType);
        }

        /*!
         * @brief Needed for the ranged loop
         *
         * The ranged loop need to have @ref begin() and @ref end() functions.
         * Thoses functions needs to returns something that can be iterated on
         * and that can be dereferenced to get a value
         */
        Pointer      begin() { return _items; }
        ConstPointer begin() const { return _items; }

        /*!
         * @brief Needed for the ranged loop The ranged loop needs to have @ref
         * begin() and @ref end() functions. Thoses functions needs to returns
         * something that can be iterated on and that can be derefenced to get a
         * value
         */
        Pointer      end() { return &_items[_size]; }
        ConstPointer end() const { return &_items[_size]; }

        /*
         * @brief Force the list to have has much "real" space as items
         */
        void fit()
        {
            _size = _capacity;
            resize(_size);
        }

        /*!
         * @brief Returns the index where @ref value is stored Returns -1 if
         * value couldn't be found inside the list
         */
        SizeType index(const ValueType& value) const
        {
            for(SizeType i = 0; i < size(); ++i)
            {
                if(_items[i] == value)
                {
                    return i;
                }
            }

            return -1;
        }

        /*!
         * @brief Returns how many item are stored by the list
         */
        [[nodiscard]] SizeType size() const { return _size; }

        /*!
        * @brief Checks if @ref value is present inside the list
        * @return Returns true the value has been found
        */
        bool exist(const ValueType& value) const
        {
            for(int i = 0; i < _size; ++i)
                if(_items[i] == value)
                    return true;
            return false;
        }

        /*!
         * @brief Returns the first element in the list
         * @pre The list must no be empty or UB
         * @returns Returns a reference to the first element in the list
         */
        ConstReference front() const { return _items[0]; }
        Reference      front() { return _items[0]; }

        /*!
         * @brief Returns the last element in the list
         * @pre The list must not be empty or UB
         * @returns Returns a reference to the last element in the list
         */
        Reference      back() { return _items[_size - 1]; }
        ConstReference back() const { return _items[_size - 1]; }

        // Returns the actual pointer owned by the list
        Pointer      data() { return _items; }
        ConstPointer data() const { return _items; }

        /*!
         * @brief Removes the item stores at @ref index
         * @pre @ref index must be inside the list range
         */
        void remove_at(const SizeType index)
        {
            for(int i = index; i < _size - 1; ++i)
                _items[i] = std::move(_items[i + 1]);

            // We manually call the destructor of the deleted item
            // since we keep the memory
            (&_items[--_size])->~ValueType();
        }

        /*!
         * @brief Removes the item equals to @ref value from the list
         */
        void remove(const ValueType& value) { remove_at(index(value)); }

        /*!
        * @brief Checks if the list is empty
        * @return Returns true if the list is empty, false otherwise
        */
        [[nodiscard]] bool empty() const { return _size == 0; }

        /*!
         * @brief Makes the list larger by @ref growth
         */
        void grow(const SizeType growth) { resize(size() + growth); }

        /*!
         * Resize the vector. if new_size > current size, new elements will be initialize
         *  with @val
         */
        //void resize(const SizeType new_size, ValueType val)
        //{
        //    // When we resize, we re allocate a bunch of memory of memcpy the previous
        //    // array into the new
        //    ValueType* values = (ValueType*)(new char[new_size * sizeof(ValueType)]);
        //    memcpy(values, _items, _capacity * sizeof(ValueType));
        //    operator delete(_items);

        //    // I'm only using memcpy here to once again avoid constructors/destructors
        //    for (int j = _capacity; j < new_size; j++)
        //        memcpy(&values[j], &val, sizeof(ValueType));

        //    _items      = values;
        //    _capacity   = new_size;
        //    _size = new_size;
        //}

        void reserve(const SizeType new_capacity)
        {
            if(new_capacity <= _capacity)
                return;

            // Create a new array
            auto* values =
                (ValueType*)new unsigned char[sizeof(ValueType) * new_capacity];

            // We move copy the previous items in the array

            if(_size != 0)
            {
                for(int i = 0; i < _size; i++)
                {
                    new(&values[i]) ValueType(_items[i]);
                    (&_items[i])->~ValueType();
                }
            }

            // We use operator delete to deallocate memory without calling the destructors
            ::operator delete(_items);

            // And we switch the arrays
            _items    = values;
            _capacity = new_capacity;
        }

        /*!
         * @brief   Simply checks if the vector contains an item of the same
         *          "value"
         */
        [[nodiscard]] bool contains(const ValueType& v) const noexcept
        {
            for(int i = 0; i < _size; i++)
            {
                if(_items[i] == v)
                {
                    return true;
                }
            }
            return false;
        }

        /*!
         * @brief Swap item at index a with item at index b if a or b is out of
         * bound, the swap operation is canceled
         */
        void swap(const SizeType a, const SizeType b)
        {
            if(!in_range(a))
            {
                //! TODO Use something else
                //throw std::out_of_range("Argument a of Vector::size is out of
                //range");
            }

            if(!in_range(b))
            {
                //throw std::out_of_range("Argument b of Vector::size is out of
                //range");
            }

            ValueType temp = _items[a];
            _items[a]      = _items[b];
            _items[b]      = temp;
        }

        /*!
         * @brief Returns true if index is within the range of the list
         */
        bool in_range(const SizeType index) const { return index >= 0 && index < size(); }

    private:
        // Underlying array used to store the items
        ValueType* _items = nullptr;

        // How many items the list can hold before having to reallocate
        SizeType _capacity = 0;

        // How many items the list actually holds
        SizeType _size = 0;
    };
}    // namespace corgi

// I consider making my own vector class  that wraps
// the std::vector  to add some shortcut functions, because some of the stl
