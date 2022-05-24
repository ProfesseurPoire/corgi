#pragma once

#include <memory>

namespace corgi
{
	/*!
	 * @brief Represents a node of the binary tree
	 */
	template<class T>
	class Node
	{
		using NodePtr = std::shared_ptr<Node>;

	public:

		// Constructors

		explicit Node() = default;

		explicit Node(T val) 
			: _value(std::move(val)) {}

		explicit Node(Node&& node) 
			: _parent(node._parent),
			_left(node._left),
			_right(node._right) {}

		// Copy would need to make the whole subtree
		explicit Node(const Node& node) 
		{
			recursive_copy(node);
		}

		~Node() = default;

		// Assigment Operators

		Node& operator=(const Node& node) 
		{
			recursive_copy(node);
			return *this;
		}

		Node& operator=(Node&& node) 
		{
			_parent = node._parent;
			_left = node._left;
			_right = node._right;
			return *this;
		}

		// Getters

		auto& value()  { return _value; }
		const auto& value()const  { return _value; }

		std::shared_ptr<Node>& left() { return _left; }
		std::shared_ptr<const Node>& left()const  { return  _left; }

		std::shared_ptr<Node>& right() { return _right; }
		std::shared_ptr<const Node>&right()const  { return _right; }

		std::weak_ptr<Node>& parent() { return _parent; }
		std::weak_ptr<const Node>&& parent()const  { return _parent; }

		// Methods

		bool has_no_son()const 
		{
			return _left == nullptr && _right == nullptr;
		}

		bool has_two_son()const 
		{
			return (_left != nullptr && _right != nullptr);
		}

		bool has_one_son()const 
		{
			if (_left != nullptr && _right == nullptr) return true;
			if (_left == nullptr && _right != nullptr) return true;
			return false;
		}

	private:

		/*
			* @brief Copy a Node
			* When copying a node, we want to copy his left and right node, but also
			* what thoses node might be storing, thus the use of this recursive function
			* This function will effectivly creates a new subtree using the first @ref node
			* as a root
			*/
		void recursive_copy(const Node& n) 
		{
			_parent.reset();

			if (n._left != nullptr)
			{
				_left = std::make_unique<Node<T>>(n._left.val);
				recursive_copy(_left);
			}

			if (n._right != nullptr)
			{
				_right = std::make_unique<Node<T>>(n._right.val);
				recursive_copy(_right);
			}
		}

		T _value;

		std::weak_ptr<Node> _parent;
		NodePtr _left;
		NodePtr _right;
	};

	/*!
		* @brief A binary tree is a graph where each node only have 2 childs
		*
		* The numbers correspond to the index of a node inside the tree
		*                      1
		*                   /     \
		*                 2           3
		*               /   \        /   \
		*              4     5       6      7
		*            /  \   / \     / \     / \
		*           8    9 10  11  12 13    14  15
		* Warning : this binary tree only sort things in desc order
		* In a Binary seach tree, the left son's value is < to the parent
		* and the right son's value is > to his parent
		*/
	template<class T>
	class BinaryTree
	{
		/*!
		* @brief Recursivly finds the next node
		* This probably needs some works but it seems to works
		*/
		static std::shared_ptr<Node<T>> traversal(std::shared_ptr<Node<T>> node,
			std::shared_ptr<Node<T>> from)
		{
			if (from != nullptr && from != node->parent().lock())
			{
				if (from == node->left())
				{
					if (node->right() != nullptr)
					{
						return node->right();
					}

					if (node->parent().lock() == nullptr)
					{
						return std::shared_ptr<Node<T>>();
					}

					return traversal(node->parent().lock(), node);
				}
				if (node->parent().lock() == nullptr)
				{
					return std::shared_ptr<Node<T>>();
				}

				return traversal(node->parent().lock(), node);
			}

			if (node->left() != nullptr)
			{
				return node->left();
			}

			if (node->right() != nullptr)
			{
				return node->right();
			}

			if (node->parent().lock() == nullptr)
			{
				return std::shared_ptr<Node<T>>();
			}

			return traversal(node->parent().lock(), node);
		}


	public:

		class Iterator
		{
		public:

			// Constructors 

			Iterator() {}

			explicit Iterator(std::shared_ptr<Node<T>> n) 
				: _node(std::move(n)) {}

			// Comparison operator

				// Returns what's inside the iterator
			auto operator->()const  { return _node.get(); }

			/*!
				* @brief Derefencing the iterator should returns the item's value
				*/
			auto operator*()const 
			{
				return _node.get();
			}

			//Navigate through the binary tree in left order
			void operator ++() 
			{
				std::shared_ptr<Node<T>> ptr;
				_node = traversal(_node, ptr);  // Since I use a reference here
			}

			bool operator!=(const Iterator& it)const 
			{
				return _node != it._node;
			}

		private:

			std::shared_ptr<Node<T>> _node;

		};

		class ConstIterator
		{
		public:

			// Constructors 

			ConstIterator() : _node(nullptr) {}

			explicit ConstIterator(std::shared_ptr<const Node<T>> n) : _node(n) {}

			// Comparison operators

			bool operator!=(const Iterator& it)const 
			{
				return _node != it._node;
			}

			// Operators

				// Returns what's inside the iterator
			auto operator->() const { return _node.get(); }
			auto operator*()  const { return _node.get(); }

			//Navigate through the binary tree in left order
			void operator++()
			{
				_node = traversal(_node);
			}

		private:

			// Variables

			std::shared_ptr<const Node<T>> _node;
		};

		// Usings

		using SizeType = int;
		using Reference = T & ;
		using ConstReference = const T&;

		// Constructors 

		BinaryTree() = default;

		~BinaryTree() = default;
		BinaryTree(const BinaryTree&) = default;
		BinaryTree(BinaryTree&&) = default;

		// Assigment Operators

		BinaryTree& operator=(const BinaryTree&) = default;
		BinaryTree& operator=(BinaryTree&&) = default;

		// Iterators

		Iterator begin() { return Iterator(_root); }
		Iterator end() { return Iterator(nullptr); }

		ConstIterator begin()   const { return Iterator(_root); }
		ConstIterator end()     const { return Iterator(nullptr); }

		ConstIterator cbegin()  const { return ConstIterator(_root); }
		ConstIterator cend()    const { return ConstIterator(nullptr); }

		// Methods

		SizeType size()const 
		{
			return _item_count;
		}

		bool empty()const 
		{
			return _item_count == 0;
		}

		// We must manage 3 case scenario : 
		// * a is parent of b, 
		// * b is parent of a
		// * Neither are relatives
		//
		// Also, if a parent is equals to nullptr, we need to update the root node
		void swap_nodes(std::shared_ptr<Node<T>> a, std::shared_ptr<Node<T>>b)
		{
			if (b->parent().lock() == a) // swap b and a so they this edge case is managed by the next if
			{
				swap_nodes(b, a);
			}

			auto left = a->left();
			auto right = a->right();

			if (b->parent().lock() != nullptr)
			{
				auto p = b->parent().lock();

				if (p->left() == b)
				{
					p->left() = a;
				}

				if (p->right() == b)
				{
					p->right() = a;
				}
			}

			if (a->parent().lock() == b) // Manage case where b is parent of a
			{
				// We update the parent of the parent
				a->parent() = b->parent();
				b->parent() = a;

				// Checking for circular dependencies 
				b->right() == a ? a->right() = b : a->right() = b->right();
				b->left() == a ? a->left() = b : a->left() = b->left();
			}
			else
			{
				// a and b have no parent relationship
				if (a->parent().lock() != nullptr)
				{
					auto p = a->parent().lock();

					if (p->left() == a)
					{
						p->left() = b;
					}

					if (p->right() == a)
					{
						p->right() = b;
					}
				}

				auto parent = a->parent();

				a->parent() = b->parent();
				b->parent() = parent;

				a->left() = b->left();
				a->right() = b->right();
			}

			b->left() = left;
			b->right() = right;

			if (a->parent().lock() == nullptr) _root = a;
			if (b->parent().lock() == nullptr) _root = b;
		}

		void delete_node(std::shared_ptr<Node<T>> node)
		{
			if (node->parent().lock() == nullptr)
			{
				_root.reset();
			}
			else
			{
				if (node->parent().lock()->left() == node)
				{
					node->parent().lock()->left().reset();
				}
				else
				{
					node->parent().lock()->right().reset();
				}
			}
			_item_count--;
		}

		void remove(std::shared_ptr<Node<T>> node)
		{
			if (node != nullptr)
			{
				if (node->has_no_son())
				{
					delete_node(node);
				}
				else if (node->has_one_son())
				{
					std::shared_ptr<Node<T>> son;
					node->left() ? son = node->left() : son = node->right();
					swap_nodes(son, node);
					delete_node(node);
				}
				else if (node->has_two_son())
				{
					// swap the node in the most left side
					// of the right side of the tree
					swap_nodes(find_minimum(node->right()), node);
					remove(node);
				}
			}
		}

		/*
			* Removes a node by it's value
			*/
		void remove(const T& val)
		{
			remove(find(val));
		}

		auto root()
		{
			return _root;
		}

		/*!
			* @brief Insert a new value inside the binary search tree
			*/
		auto insert(T val) 
		{
			return recursive_insert(val, _root);
		}

		/*!
			* @brief Find something
			*/
		auto find(const T& val)
		{
			return recursive_find(val, _root);
		}

	private:

		// Methods

		/*!
		* @brief Recursivly find an item
		*/
		auto recursive_find(const T& val, std::shared_ptr<Node<T>> node)
		{
			if (node == nullptr)
			{
				return std::shared_ptr<Node<T>>();
			}

			if (node->value() == val)
			{
				return node;
			}

			if (node->value() < val)
			{
				return recursive_find(val, node->right());
			}

			return recursive_find(val, node->left());
		}

		/*!
		* @brief Insert a new value inside the binary search tree
		*/
		std::shared_ptr<Node<T>> recursive_insert(T val,
			std::shared_ptr<Node<T>> parent = std::shared_ptr<Node<T>>()) 
		{
			if (_root == nullptr)
			{
				_root = std::make_shared<Node<T>>(val);
				_item_count++;
				return _root;
			}

			if (parent == nullptr)
			{
				parent = std::make_shared<Node<T>>(val);
				_item_count++;
				return parent;
			}

			// if it's the same value we do nothing
			if (val == parent->value())
			{
				return parent;
			}

			if (val < parent->value())
			{
				parent->left() = recursive_insert(val, parent->left());
				parent->left()->parent() = parent;
				return parent;
			}

			parent->right() = recursive_insert(val, parent->right());
			parent->right()->parent() = parent;
			return parent;
		}

		std::shared_ptr<Node<T>> find_minimum(std::shared_ptr<Node<T>> n)
		{
			while (n->left() != nullptr)
			{
				n = n->left()->left();
			}
			return n;
		}

		// Variables

		std::shared_ptr<Node<T>> _root;
		int _item_count = 0;
	};
}