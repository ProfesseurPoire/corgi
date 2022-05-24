#pragma once

#include <vector>
#include <optional>
#include <memory>
#include <functional>
#include <queue>
#include <iterator>

template<class T>
using Vector = std::vector<T>;

template<class T>
using Optional = std::optional<T>;

template<class T>
using UniquePtr = std::unique_ptr<T>;

template<class T>
using Reference = std::reference_wrapper<T>;

namespace corgi
{

class AbstractNode
{
public:

// Lifecycle

	AbstractNode(AbstractNode* parent) : parent_(parent){}

// Functions

	// The sfinae thing here should disable the function when T doesn't inherit from AbstractNode
	template<class T,  std::enable_if_t<std::is_base_of<AbstractNode, T>::value>>	
	void push_back(const T& node)
	{
		children_.push_back(std::make_unique<T>(node));
	}


protected:	

	AbstractNode* parent_;
	std::vector<std::unique_ptr<AbstractNode>> children_;

	private:
};

// TODO :   I couldn't put this inside Iterator because compiler complained
//          that it couldn't find the type when I tried to initialized
//          it inside the Tree class so 🤦‍♀️ 🤷‍♂️
enum class IteratorMode :  char
{
	DepthFirst,
	BreadthFirst
};

template<class T>
class Node
{
public:

// Iterators

class ConstNodeIterator
{
public:

	using iterator_category = std::output_iterator_tag;
	using value_type        = Node&;
	using difference_type   = int;
	using pointer           = Node*;
	using reference         = Node&;

// Lifecycle

	ConstNodeIterator()=default;

	explicit ConstNodeIterator(const Node& node, IteratorMode mode = IteratorMode::DepthFirst):
		mode_(mode)
	{
		for(auto& n : node.children_)
		{
			queue_.push_back(*n);
		}
		operator++();
	}

// Functions

	[[nodiscard]] const Node<T>& operator*() noexcept{return *current_node_;}

	void increment_breadth_first()
	{
		current_node_ = &queue_.front().get();
		queue_.pop_front();
		std::transform(current_node_->children_.begin(), current_node_->children_.end(), std::back_inserter(queue_),
			[](std::unique_ptr<Node>& n)->Node&
			{
				return *n;
			});
	}

	void increment_depth_first()
	{
		current_node_ = &queue_.front().get();
		queue_.pop_front();
		std::transform(current_node_->children_.rbegin(), current_node_->children_.rend(), std::front_inserter(queue_),
			[](std::unique_ptr<Node>& n) ->Node&
			{
				return *n;
			});
	}

	ConstNodeIterator& operator++() noexcept
	{
		if(queue_.empty())
		{
			current_node_=nullptr;
			return *this;
		}

		switch(mode_)
		{
			case IteratorMode::BreadthFirst:
				increment_breadth_first();
			break;

			case IteratorMode::DepthFirst:
				increment_depth_first();
			break;
		}
		return *this;
	}

	[[nodiscard]] bool operator!=(const ConstNodeIterator& iterator)const noexcept
	{
		return current_node_ != iterator.current_node_;
	}

private:

	std::deque<Reference<const Node<T>>> queue_;
	IteratorMode mode_      {IteratorMode::DepthFirst};
	bool recursive_         {true};
	const Node<T>* current_node_  {nullptr};
};

class NodeIterator
{
public:

	using iterator_category = std::output_iterator_tag;
	using value_type        = Node&;
	using difference_type   = int;
	using pointer           = Node*;
	using reference         = Node&;

	NodeIterator()=default;

	explicit NodeIterator(Node& node, IteratorMode mode = IteratorMode::DepthFirst):
		mode_(mode)
	{
		std::transform(node.children_.begin(), node.children_.end(), std::back_inserter(queue_),
			[](std::unique_ptr<Node>& n)->Node& { return *n; });
		operator++();
	}

	[[nodiscard]] Node& operator*() noexcept{return *current_node_;}

	void increment_breadth_first()
	{
		current_node_ = &queue_.front().get();
		queue_.pop_front();

		std::transform
		(
			current_node_->children_.begin(),
			current_node_->children_.end(),  
			std::back_inserter(queue_),
			[](std::unique_ptr<Node>& n)->Node&{return *n;}
		);
	}

	void increment_depth_first() 
	{
		current_node_ = &queue_.front().get();
		queue_.pop_front();

		std::transform
		(
			current_node_->children_.rbegin(), 
			current_node_->children_.rend(), 
			std::front_inserter(queue_),
			[](std::unique_ptr<Node>& n) ->Node&{return *n;}
		);
	}

	NodeIterator& operator++() noexcept
	{
		if(queue_.empty())
		{
			current_node_=nullptr;
			return *this;
		}
		switch(mode_)
		{
			case IteratorMode::BreadthFirst:
				increment_breadth_first();
			break;

			case IteratorMode::DepthFirst:
				increment_depth_first();
			break;
		}
		return *this;
	}

	[[nodiscard]] bool operator!=(const NodeIterator& iterator)const noexcept
	{
		return current_node_ != iterator.current_node_;
	}

private:

	std::deque<Reference<Node<T>>> queue_;
	IteratorMode mode_      {IteratorMode::DepthFirst};
	bool recursive_         {true};
	Node* current_node_		{nullptr};
};

// Lifecycle

public:

	~Node() = default;
	
	Node(): parent_(nullptr){}
	
	template<class ... Args>
	Node(Node* parent, Args&& ... args):
		value_(std::forward<Args>(args)...),
		parent_(parent)
	{}

	Node(Node* parent, T&& value)
		: parent_(parent), value_(std::move(value)){}

	Node(const Node& node):
		value_(node.value_)
	{
		for(auto& child : node.children_)
		{
			auto* new_node = new Node(*child);
			new_node->parent_ = this;
			children_.push_back(std::unique_ptr<Node>( new_node));
		}
	}

	Node(Node&& node):
		value_(std::move(node.value_)),
		children_(std::move(node.children_))
		
	{
		for (auto& child : children_)
		{
			child->parent_ = this;
		}
	}

	//Node(Node&& node);

	Node& operator=(const Node& node)
	{
		return *this;
	}
	
	Node& operator=(Node&& node) noexcept
	{
		return *this;
	}


// Functions

	void insert(int index, const T& value)
	{
		children_.insert(children_.begin()+index, std::make_unique<Node>(value));
	}

	Node& push_back(const Node& node)
	{
		children_.push_back(std::make_unique<Node>(node));
		children_.back()->parent_ = this;
		return *children_.back();
	}

	Node& push_back(Node&& node)
	{
		children_.push_back(std::make_unique<Node>(std::move(node)));
		children_.back()->parent_ = this;
		return *children_.back();
	}

	constexpr Node& push_back(const T& value)
	{
		children_.push_back(std::make_unique<Node>(value));
		children_.back()->parent_ = this;
		return *children_.back();
	}

	constexpr Node& push_back(T&& value)
	{
		children_.push_back(std::make_unique<Node>(std::move(value)));
		children_.back()->parent_ = this;
		return *children_.back();
	}
	
	// TODO : 	If I ever become a SFINAE wizard, it would be nice to 
	// 			disable the Node's variadic template constructor, because,
	// 			currently, if you send a Node object to emplace_back, it 
	// 			won't route it to the copy constructor, since the 
	// 			copy constructor is a const reference, and is not picked
	//			first 

	/*!
	 * @brief   Constructs and adds a new Node into the children's vector
	 */
	template<class ... Args>
	Node& emplace_back(Args&&... args)
	{
		return *children_.emplace_back(std::make_unique<Node>(this, std::forward<Args>(args)...));
	}
	

	[[nodiscard]] auto max_size()const noexcept { return  children_.max_size(); }
	[[nodiscard]] auto capacity()const noexcept { return  children_.capacity(); }

	void shrink_to_fit() noexcept 				{ children_.capacity(); }

	void clear() noexcept{ children_.clear();}
	
	auto& data() 			{ return children_; }
	const auto& data()const { return children_;}

	[[nodiscard]] Node& 		operator[](int index) 		{ return *children_[index]; }
	[[nodiscard]] const Node& 	operator[](int index)const 	{ return *children_[index]; }

	[[nodiscard]] bool empty() 	{ return children_.empty();}
	[[nodiscard]] auto size()	{ return children_.size();}

	[[nodiscard]] bool is_parent(const Node& node) noexcept;

	[[nodiscard]] auto begin(){return NodeIterator(*this, mode_);}
	[[nodiscard]] auto begin()const noexcept{return ConstNodeIterator(*this, mode_);}

	[[nodiscard]] auto end()noexcept{return NodeIterator();}
	[[nodiscard]] auto end()const noexcept{return ConstNodeIterator();}

	[[nodiscard]] bool is_leaf()const noexcept{return children_.empty();}

	[[nodiscard]] Node* parent() noexcept{ return parent_;}
	[[nodiscard]] const Node* parent()const noexcept{return parent_;}

	[[nodiscard]] T& get()noexcept{return value_;}
	[[nodiscard]] const T& get()const noexcept{return value_;}

	[[nodiscard]] T* operator->()noexcept{return &value_;}
	[[nodiscard]] const T* operator->()const noexcept{return &value_;}

	/*!
	 * @brief 	Sets the Iterator's behavior
	 */
	void iterator_mode(IteratorMode mode)noexcept{mode_=mode;}

	/*!
	 * @brief 	Detach the current node from its parent
	 */
	std::unique_ptr<Node> detach()noexcept;

private:

	T                       value_;
	std::vector<std::unique_ptr<Node<T>>>  children_;
	Node<T>*                parent_ {nullptr};

	// This part is used to iterate the children nodes
	IteratorMode            mode_                   {IteratorMode::DepthFirst};
	bool                    recursive_iterator_     {true};
};

// Implementation

template<class T>
[[nodiscard]] bool Node<T>::is_parent(const Node<T>& node) noexcept
{
	if(!node.parent())
		return false;

	if(node.parent() == this)
		return true;

	return is_parent(*node.parent());
}

//template<class T>
//Node<T>::Node(const Node<T>& node):
//	children_(node.children_, *this),
//	value_(node.value_)
//{}

template<class T>
std::unique_ptr<Node<T>> Node<T>::detach()noexcept
{
	if(!parent_)		// If the current node has no parents
		return nullptr;	// nothing happens and we exist the function

	// Otherwise, we need to remove the node from it's parent's children vector
	auto& c =  parent()->children_ ;

	auto it = std::find_if(c.begin(), c.end(), [&](const auto& a){return a.get()==this;});

	parent_= nullptr;

	if(it->operator bool())	// If he found the node
	{
		auto index = it - c.begin() ;
		auto* p = c[index].release();
		c.erase(c.begin()+index);
		return std::make_unique<Node<T>>(p);
	}
	
	return nullptr;
}


template<class T>
using Tree = Node<T>;

}