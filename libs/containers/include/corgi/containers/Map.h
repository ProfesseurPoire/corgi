#pragma once

#include <corgi/containers/binarytree.h>

namespace corgi
{
	// @brief   A map stores pair of objects associated by key/value
	//  Uses a binary search tree under the hood
	template<class Key, class Value>
	class Map
	{
		/*!
			* @brief Associate a Key with a value
			*/
		class KeyValue
		{
		public:

			// Constructors

			explicit KeyValue() = default;

			explicit KeyValue(Key k)
				:_key(k) {}

			explicit  KeyValue(Key k, Value v) 
				: _key(k), _value(v) {}

			// Comparison operators

			bool operator==(const KeyValue& rh) const 
			{
				// Maybe this is cheating, but this allow the Binary tree to only
				// look for the key
				return (_key == rh._key);
			}

			bool operator>(const KeyValue& rh)const 
			{
				return _key > rh._key;
			}

			bool operator<(const KeyValue& rh)const 
			{
				return _key < rh._key;
			}

			// Getters

			Key key()const 
			{
				return _key;
			}

			Value value() const 
			{
				return _value;
			}

		private:

			Key     _key;
			Value   _value;
		};

	public:

		// Constructors

		Map() = default;
		~Map() = default;
		Map(Map&& m) = default;
		Map(const Map& m) = default;
		Map& operator=(Map&& m) = default;
		Map& operator=(const Map& m) = default;

		// Methods

		auto begin()
		{
			return _binary_tree.begin();
		}

		auto end()
		{
			return _binary_tree.end();
		}

		/*!
			* @brief Try to find the value associated to the @ref key
			* If the key isn't associated to any value, return a default object
			* /!\ Warning, if the key doesn't exist, an exception will be sent
			* or undefined behavior
			*/
		Value value(Key key)
		{
			return _binary_tree.find(KeyValue(key))->value().value();
		}

		/*!
			* @brief Add a key/value pair to the map
			*/
		void add(Key key, Value value)
		{
			_binary_tree.insert(KeyValue(key, value));
		}

		/*!
			* @brief Returns true if the map doesn't store any item
			*/
		bool empty()const 
		{
			return _binary_tree.size() == 0;
		}

		/*!
			* @brief Removes a key-value pair from the map
			*/
		void remove(Key key) 
		{
			_binary_tree.remove(KeyValue(key));
		}

		/*!
			* @brief Check if @ref key exist inside the map
			* @returns Returns true if key exist, false otherwise
			*/
		bool key_exist(Key key) 
		{
			return _binary_tree.find(KeyValue(key)) != nullptr;
		}

		/*!
			* @brief Returns how many items are stored inside the map
			*/
		int size() const 
		{
			return _binary_tree.size();
		}

	private:

		BinaryTree<KeyValue> _binary_tree;
	};
}