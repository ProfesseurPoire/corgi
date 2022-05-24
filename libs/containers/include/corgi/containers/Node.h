#pragma once

#include <iterator>
#include <optional>
#include <queue>
#include <vector>

namespace corgi
{
    /*!
     * @brief Represent a Node from a Tree like structure
     * 
     *        Make your own class inherits from this if you want them to be
     *        stored in a tree like fashion
     */

    template<class T>
    class Node
    {
    public:
        // Typedefs

        using Children = std::vector<Node<T>>;

        class Iterator
        {
        public:
            // Typedefs

            // Needed by stl algorithms

            enum class Mode
            {
                BreadthFirst,
                DepthFirst
            };

            using iterator_category = std::output_iterator_tag;
            using value_type = Node<T>&;
            using difference_type = int;
            using pointer = Node<T>*;
            using reference = Node<T>&;

            // Lifecycle

            Iterator() = default;

            explicit Iterator(Node<T>* node, Mode mode = Mode::DepthFirst)
                : mode_(mode)
            {
                std::transform(node->children_.begin(), node->children_.end(), std::back_inserter(queue_),
                               [](std::unique_ptr<Node<T>>& n) -> Node<T>*
                               {
                                   return n.get();
                               });
                operator++();
            }

            // Operators

            Iterator operator+(int index)
            {
                for(int i = 0; i < index; ++i)
                {
                    operator++();
                }
                return *this;
            }

            // Functions

            [[nodiscard]] Node<T>& operator*() noexcept
            {
                return *current_node_;
            }

            void increment_breadth_first()
            {
                current_node_ = queue_.front();
                queue_.pop_front();

                std::transform(current_node_->children_.begin(), current_node_->children_.end(),
                               std::back_inserter(queue_), [](std::unique_ptr<Node<T>>& n) -> Node<T>*
                               {
                                   return n.get();
                               });
            }

            void increment_depth_first()
            {
                current_node_ = queue_.front();
                queue_.pop_front();

                std::transform(current_node_->children_.rbegin(), current_node_->children_.rend(),
                               std::front_inserter(queue_), [](std::unique_ptr<Node<T>>& n) -> Node*
                               {
                                   return n.get();
                               });
            }

            Iterator& operator++() noexcept
            {
                if(queue_.empty())
                {
                    current_node_ = nullptr;
                    return *this;
                }
                switch(mode_)
                {
                    case Mode::BreadthFirst :
                        increment_breadth_first();
                        break;

                    case Mode::DepthFirst :
                        increment_depth_first();
                        break;
                }
                return *this;
            }

            [[nodiscard]] bool operator!=(const Iterator& iterator) const noexcept
            {
                return current_node_ != iterator.current_node_;
            }

        private:
            std::deque<Node<T>*> queue_;
            Mode                 mode_ {Mode::DepthFirst};
            //bool recursive_         {true};
            Node<T>* current_node_ {nullptr};
        };

        // Lifecycle

        Node() = default;

        Node(const T& t)
            : mValue(t)
        { }

        Node(T&& t)
            : mValue(std::move(t))
        {}

        ~Node() = default;

        Node(const Node<T>& other)
            : parent_(other.parent_)
              , mode_(other.mode_)
              , mValue(other.mValue)
        {
            children_.reserve(other.children_.size());

            for(const auto& child : other.children_)
            {
                auto& c   = children_.emplace_back(child);
                c.parent_ = this;
            }
        }

        Node(Node<T>&& other) noexcept
            : mValue(std::move(other.mValue))
        {
            parent_ = std::move(other.parent_);

            for(auto& child : other.children_)
            {
                auto& c    = children_.emplace_back(std::move(child));
                c->parent_ = this;
            }

            mode_ = other.mode_;
        }

        Node& operator=(const Node<T>& other)
        {
            parent_ = other.parent_;
            mode_   = other.mode_;
            mValue  = other.mValue;

            children_.clear();
            children_.reserve(other.children_.size());

            for(const auto& child : children_)
            {
                auto& c   = children_.emplace_back(std::move(child));
                c.parent_ = this;
            }

            return *this;
        }

        Node& operator=(Node<T>&& other) noexcept
        {
            parent_ = other.parent_;
            mode_   = other.mode_;
            mValue  = std::move(other.mValue);

            children_.clear();

            for(auto& child : other.children_)
            {
                auto& c   = children_.emplace_back(std::move(child));
                c.parent_ = this;
            }

            return *this;
        }

        // Accessors

        [[nodiscard]] Children& children()
        {
            return children_;
        }

        [[nodiscard]] const Children& children() const
        {
            return children_;
        }

        [[nodiscard]] Node<T>* parent()
        {
            return parent_;
        }

        [[nodiscard]] Iterator::Mode mode() const noexcept
        {
            return mode_;
        }

        [[nodiscard]] T* operator->()
        {
            return &mValue;
        }

        // Mutators

        void mode(Iterator::Mode mode)
        {
            mode_ = mode;
        }

        // Functions

        /**
         * @brief Returns the depth of the current Node
         * @param currentDepth 
         * @return size_t 
         */
        [[nodiscard]] int depth(int currentDepth = 0) const
        {
            if(parent_)
            {
                return parent_->depth(++currentDepth);
            }
            return currentDepth;
        }

        /*!
         * @brief   Returns how many child are stored by the node
         */
        [[nodiscard]] size_t size() const noexcept
        {
            return children_.size();
        }

        /*!
         * @brief   Checks if the node is empty
         * 
         * @return  Returns true if empty, false otherwise
         */
        [[nodiscard]] bool empty() const noexcept
        {
            return children_.empty();
        }

        /*!
     * @brief   Erases the child at @a index, if any
     * 
     * @return  Returns true if the child has been removed, false otherwise
     */
        bool remove_child(int index)
        {
            if(!in_boundaries(index))
                return false;

            children_.erase(children_.begin() + index);
            return true;
        }

        bool remove_child(Node* node)
        {
            children_.erase(std::find_if(children_.begin(), children_.end(),
                                         [&](Node<T>& n)
                                         {
                                             return &n == node;
                                         }));
            return false;
        }

        /*!
     * @brief   Erases every child
     */
        void clear() noexcept
        {
            children_.clear();
        }

        /*!
     * @brief   Erases the last child
     */
        void pop_back()
        {
            children_.pop_back();
        }

        Node<T>& at(int index)
        {
            return children_[index];
        }

        /**
         * \brief Warning : Stored as value so emplacing new things
         *        breaks references
         * \tparam Args 
         * \param args 
         * \return 
         */
        template<class... Args>
        Node<T>& emplace_back(Args&&...args)
        {
            auto& up   = children_.emplace_back(T(std::forward<Args>(args)...));
            up.parent_ = this;
            return up;
        }

        /*!
     * @brief   Returns the node index in the children list (if any)
     * 
     * @returns Returns the node's index in the children list, -1 if the node hasn't
     *          been found
     */
        [[nodiscard]] int child_index(Node* n)
        {
            for(size_t i = 0; i < children_.size(); ++i)
            {
                if(children_[i].get() == n)
                {
                    return i;
                }
            }
            return -1;
        }

        void detach_node(Node* n)
        {
            // TODO : Not sure if I should actually be doing the release inside
            // the std::find_if but well, it's kinda convenient
            children_.erase(std::find_if(children_.begin(), children_.end(),
                                         [&](Node<T>& node)
                                         {
                                             if(&node == n)
                                             {
                                                 return true;
                                             }
                                             return false;
                                         }));
        }

        /*
         * @brief   Copies the parameter and stores it as a children of
         *          the current node
         */
        Node<T>& attach(const Node<T>& node)
        {
            auto& child   = children_.emplace_back(node);
            child.parent_ = this;
            return child;
        }

        /*
         * @brief   Moves the parameter and stores it as a children of the current
         *          Node
         */
        void attach(Node<T>&& node)
        {
            auto  parent = node.parent_;
            auto& child  = children_.emplace_back(std::make_unique<Node<T>>(std::move(node)));

            parent->remove_child(&node);

            child.parent_ = this;
        }

        /*!
         * @brief   Checks whether the node is a leaf
         * 
         *          A node is considered a leaf if it has no children
         * 
         * @return  Returns true if the node is a leaf, false otherwise
         */
        [[nodiscard]] bool is_leaf() const
        {
            return children().empty();
        }

        // Iterators

        [[nodiscard]] auto begin()
        {
            return Iterator(this, mode_);
        }

        [[nodiscard]] auto end()
        {
            return Iterator();
        }

    public:
        bool in_boundaries(const int index)
        {
            // TODO : maybe directly use size_t inside in_boundaries ?
            const auto i = static_cast<size_t>(index);

            return (index >= 0 && i < children_.size());
        }

        Iterator::Mode                        mode_ = Iterator::Mode::DepthFirst;
        Node<T>*                              parent_ {nullptr};
        std::vector<std::unique_ptr<Node<T>>> children_;
        T                                     mValue;

    private:
    };
}    // namespace corgi
