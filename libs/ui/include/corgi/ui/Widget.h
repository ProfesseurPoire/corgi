#pragma once

#include <corgi/SimpleString.h>
#include <corgi/containers/Node.h>
#include <corgi/inputs/Inputs.h>
#include <corgi/rendering/Material.h>
#include <corgi/utils/Event.h>

#include <optional>

namespace corgi
{
    class UISystem;
    class Renderer;
}    // namespace corgi

namespace corgi::ui
{
    /**
     * @brief Base class for every related widget
     *
     *        Handles events and dimensioning 
     */
    class Widget
    {
    public:
        friend class corgi::UISystem;

        enum class ResizeEnum
        {
            N,
            S,
            E,
            W,
            NE,
            NW,
            SE,
            SW,
            None
        };

        ResizeEnum currentResize_ = ResizeEnum::None;

        float resizeStartX_ = 0.0f;
        float resizeStartY_ = 0.0f;

        /*
         * Shortening the Children type
         *
         * We're using std::unique_ptr<Node etc because we want to be able to keep
         * references, and the second one with widget is because we want to be able
         * to use polymorphism
         */
        using Children = std::vector<std::unique_ptr<Node<std::unique_ptr<Widget>>>>;

        /**
         * @brief Values sets for positioning and dimensioning can be set with different Unit type
         */
        enum class UnitType
        {
            None,
            Float,        // Value won't change depending on the parent
            Percentage    // Percentage means the distance is relative to the parent's dimension. Like 10% left would means
            // that our widget is at 0.1*parent.width
        };

        /**
         * @brief Adds a tooltip that gets shown after a second the mouse entered the widget
         * 
         * @param tooltip 
         */
        void setToolTip(const SimpleString& tooltip);

        /**
         * @brief Encapsulate a UnitType and a value together
         *        Mostly here for convenience
         */
        struct Unit
        {
            float    value = 0.f;
            UnitType type  = UnitType::Float;
        };

        struct Margin
        {
            Unit left;
            Unit right;
            Unit top;
            Unit down;
        };

        friend class corgi::UISystem;

        // Lifecycle

        Widget()          = default;
        virtual ~Widget() = default;

        /**
         * @brief   Gets the widget active state
         * 
         *          When unactive, the widget is still displayed in grey.
         *          The difference with disable is that a disable widget will totally
         *          mask the widget
         */
        [[nodiscard]] bool isActive() const noexcept;

        /**
         * @brief   Activate the widget 
         * 
         *          If the widget was greyed out, turned it on
         */
        void activate(bool rec = true);

        /**
         * @brief   Deactive the widget
         * 
         *          Grey out the widget
         */
        void deactivate(bool rec = true);

        /**
         * @brief Called after construction
         *
         *        For some Widgets, the constructor is not enough. So, in the
         *        emplace_back function, we call the virtual init function to make sure
         *        that things are correctly constructed
         */
        virtual void init() {}

        /**
         * @brief   Virtual function that is called at the end of the emplace_back function
         * 
         *          This function is mostly useful for layouts that potentially
         *          needs to set the dimensions of their child widgets
         * 
         * @param w 
         */
        virtual void handleNewChild(Widget* w) {};

        /*
         * @brief Construct a new Widget of type U and adds it as a child of current widget
         */
        template<class U, class... Args>
        U& emplace_back(Args&&... args)
        {
            Node<std::unique_ptr<Widget>> nn;
            nn.mValue.reset(new U(std::forward<Args>(args)...));

            auto& n = mNode->children_.emplace_back(
                std::make_unique<Node<std::unique_ptr<Widget>>>(std::move(nn)));
            n->mValue->mNode                    = n.get();
            n->parent_                          = mNode;
            n->mValue.get()->parentWidthCache_  = width();
            n->mValue.get()->parentX_           = real_x();
            n->mValue.get()->parentHeightCache_ = height();
            n->mValue.get()->parentY_           = real_y();
            n->mValue->uiSystem_                = uiSystem_;
            n->mValue->init();
            handleNewChild(n->mValue.get());
            return *reinterpret_cast<Node<std::unique_ptr<U>>&>(*n.get())->get();
        }

        /*
         * @brief Construct a new Widget of type U and adds it as a child of current widget
         */
        template<class U, class... Args>
        U* emplaceBack(Args&&... args)
        {
            Node<std::unique_ptr<Widget>> nn;
            nn.mValue.reset(new U(std::forward<Args>(args)...));

            auto& n = mNode->children_.emplace_back(
                std::make_unique<Node<std::unique_ptr<Widget>>>(std::move(nn)));
            n->mValue->mNode                    = n.get();
            n->parent_                          = mNode;
            n->mValue.get()->parentWidthCache_  = width();
            n->mValue.get()->parentX_           = real_x();
            n->mValue.get()->parentHeightCache_ = height();
            n->mValue.get()->parentY_           = real_y();
            n->mValue->uiSystem_                = uiSystem_;
            n->mValue->init();
            handleNewChild(n->mValue.get());
            return reinterpret_cast<Node<std::unique_ptr<U>>&>(*n.get())->get();
        }

        void remove(Widget* child);

        // Events

        /**
         * @brief   Event called whenever the widget gets activated
         * 
         *          Called by activate() function
         * 
         * @return Event<>& 
         */
        [[nodiscard]] Event<>& onActivate() noexcept { return onActivate_; }

        /**
         * @brief    Event called whenever the widget gets deactivated
         * 
         *           Called by the deactivate function
         * 
         * @return Event<>& 
         */
        [[nodiscard]] Event<>& onDeactivate() noexcept { return onDeactivate_; }

        /*!
         * @brief   Event called the first time the mouse hover the widget
         */
        [[nodiscard]] Event<int, int>& onMouseEnter() noexcept { return onMouseEnter_; }

        /*!
         * @brief   Event called the first time the mouse exit the current widget
         */
        [[nodiscard]] Event<int, int>& onMouseExit() noexcept { return onMouseExit_; }

        [[nodiscard]] Event<>& focusInEvent() { return mFocusInEvent; }

        [[nodiscard]] Event<int>& mouseWheelEvent() { return mMouseWheelEvent; }

        [[nodiscard]] Event<>& focusOutEvent() { return mFocusOutEvent; }

        [[nodiscard]] Event<int, int>& on_mouse_over() { return on_mouse_over_; }

        [[nodiscard]] Event<int, int>& on_mouse_drag() { return on_mouse_drag_; }

        [[nodiscard]] Event<int, int>& on_mouse_drag_end() { return on_mouse_drag_end_; }

        [[nodiscard]] Event<int, int>& on_mouse_drag_begin()
        {
            return on_mouse_drag_begin_;
        }

        [[nodiscard]] Event<int, int>& onMouseClick() { return mOnMouseClick; }

        [[nodiscard]] Event<int, int>& resizeEvent() { return mResizeEvent; }

        [[nodiscard]] Event<int, int>& mouseDoubleClickEvent()
        {
            return mMouseDoubleClickEvent;
        }

        [[nodiscard]] Event<const Mouse&>& mouseDownEvent() { return mouseDownEvent_; }

        [[nodiscard]] Event<const Mouse&>& mouseUpEvent() { return mouseUpEvent_; }

        Event<const Inputs&> on_key_pressed;

        // Accessors

        /**
         * @brief   
         * 
         * @return true 
         * @return false 
         */
        bool resizeEventTriggered() const;

        void setFocusIn();
        void setFocusOut();

        void clearChildren();

        enum class VerticalAlign
        {
            Center,
            Top,
            Bottom
        };

        void alignVertical(VerticalAlign align);

        /**
         * @brief   Gets the widget's width
         * @return  Returns a float copy of the mWidth member variable
         */
        [[nodiscard]] virtual float width() const noexcept;

        /**
         * @brief   Gets the widget's height
         * @return  Returns a float copy of the mHeight member variable
         */
        [[nodiscard]] virtual float height() const noexcept;

        [[nodiscard]] float x() const noexcept;
        [[nodiscard]] float y() const noexcept;

        [[nodiscard]] float getLeft() const noexcept;
        [[nodiscard]] float getTop() const noexcept;

        [[nodiscard]] float real_x() const;
        [[nodiscard]] float real_y() const;

        /**
         * @brief Gets the left margin size
         * @return Returns a float copy of the mMargin.left member variable
         */
        [[nodiscard]] float marginLeft() const noexcept;

        /**
         * @brief Returns the right margin's size
         * @return float
         */
        [[nodiscard]] float marginRight() const;

        /**
         * @brief Returns the top margin's size
         * @return float
         */
        [[nodiscard]] float marginTop() const;

        /**
         * @brief Returns the bottom margin's size
         * @return float
         */
        [[nodiscard]] float marginBottom() const;

        /**
         * @brief The widget's anchors will match the paren'ts one
         */
        void setAnchorsToFillParentSpace();

        /**
         * @brief Destroy the current widget
         * 
         *        The widget won't instantly destroyed. It'll be deactivated
         *        and marked for destruction. Actual destruction will occur at
         *        the end of the frame inside the UISystem object.
         */
        void destroy();

        /**
         * @brief   Checks if the widget is marked for destruction
         */
        [[nodiscard]] bool isDestroyed() const noexcept;

        // Mutators

        /**
         * @brief Set the left margin's value
         *        If isPercentage equals true, it means we're going to use
         *        a % of the parent's width
         * @param value
         * @param isPercentage
         */
        void setMarginLeft(float value, bool isPercentage = false);

        /**
         * @brief Set the right margin's value
         *        If isPercentage equals true, it means we're going to use
         *        a % of the parent's width
         * @param value
         * @param isPercentage
         */
        void setMarginRight(float value, bool isPercentage = false);

        /**
         * @brief Sets the current widget relative to the right's parent border instead of the left
         * 
         *          WARNING : this is truly a hack and ideally we would prefer use
         *          or create a better mecanism, but it's really usefull for things like
         *          Icons on the right side of a window or Item
         * 
         * @param width 
         * @param offset 
         */
        void setRelativeRightBorder(float width, float offset);

        /**
         * @brief Set the left margin's value
         *        If isPercentage equals true, it means we're going to use a
         *        % of the parent's height
         * @param value
         * @param isPercentage
         */
        void setMarginTop(float value, bool isPercentage = false);

        /**
         * @brief Set the bottom margin's value
         *        If isPercentage equals true, it means we're going to use a
         *        % of the parent's height
         * @param value
         * @param isPercentage
         */
        void setMarginBottom(float value, bool isPercentage = false);

        /**
         * @brief Sets the width and height of the widget 
         * 
         * @param width 
         * @param height 
         * @param widthRelative 
         * @param heightRelative 
         */
        void setDimensions(float width,
                           float height,
                           bool  widthRelative  = false,
                           bool  heightRelative = false);

        /*!
         * @brief	Sets the widget's width
         *
         *			When you set the relative value to true, the width will
         *be a % of the parent's width
         */
        void setWidth(float val, bool relative = false);

        /*!
         * @brief	Sets the widget's width
         *
         *			When you set the relative value to true, the width will
         *be a % of the parent's width
         */
        void setHeight(float val, bool relative = false);

        /*!
         * @brief	Sets the widget's x coordinate
         *
         *			When you set the relative parameter to true, the x
         *          coordinate will be a % of the parent's width
         */
        void x(float val, bool relative = false);

        [[nodiscard]] bool isEnabled() const noexcept;

        void setTop(float val, bool isRelative = false);
        void setRight(float val, bool isRelative = false);
        void setLeft(float val, bool isRelative = false);
        void setBottom(float val, bool isRelative = false);

        /*!
         * @brief	Sets the widget's y coordinate
         *
         *			When you set the relative parameter to true, the y
         *          coordinate will be a % of the parent's height
         */
        void y(float val, bool relative = false);

        // Virtual functions

        /*!
         * @brief   Gets the resize left value
         *
         *          If true, we can resize the widget holding the left mouse button using the left edge
         */
        [[nodiscard]] bool getResizeLeft() const noexcept;

        /*!
         * @brief   Gets the resize right value
         *
         *          If true, it means we can resize the widget by holding the left mouse button
         *          using the widget's right edge
         */
        [[nodiscard]] bool getResizeRight() const noexcept;

        /*!
         * @brief   Gets the resize top value
         *
         *          If true, it means we can resize the widget by holding the left mouse button
         *          using the widget's top edge
         */
        [[nodiscard]] bool getResizeTop() const noexcept;

        /**
         * @brief   Gets the resize bottom value
         *          If true, it means we can resize the widget by holding the left mouse button
         *          using the widget's bottom edge
         */
        [[nodiscard]] bool getResizeBottom() const noexcept;

        /**
         * @brief   Sets the resize bottom value
         *          If true, it means we can resize the widget by holding the left mouse button
         *          using the widget's bottom edge
         */
        void setResizeBottom(bool value, bool inPercentage = false);

        /**
         * @brief   Sets the resize top value
         *
         *          If true we can resize the widget using the top edge
         */
        void setResizeTop(bool value, bool inPercentage = false);

        /**
         * @brief   Sets the resize left value
         *
         *          If true we can resize the widget using the left edge
         */
        void setResizeLeft(bool value, bool inPercentage = false);

        /**
         * @brief   Sets the resize right value
         *
         *          If true we can resize the widget using the right edge
         */
        void setResizeRight(bool value, bool inPercentage = false);

        virtual void update(float elapsed_time);

        /*!
         *  @brief Returns a pointer to the parent node (if any)
         */
        Widget* parent();

        /*!
         * @brief Returns a const pointer to the parent node
         */
        const Widget* parent() const;

        /*!
         * @brief   Gets the children list
         * @return  Returns a Children reference to the mNode->mChildren member variable
         */
        [[nodiscard]] Children& getChildren();

        /*!
         * @brief   Gets the children list
         * @return  Returns a Children const reference to the mNode->mChildren member variable
         */
        [[nodiscard]] const Children& getChildren() const;

        /*!
         * @brief   Constructs a children list with Widget* instead of std::unique_ptr<Node<std::unique_ptr<Widget>>
         * @return  Returns a std::vector<Widget*> constructed by the function
         */
        [[nodiscard]] std::vector<Widget*> getChildrenAsWidget() const;

        /*!
         * @brief Widgets must implement this so the engine knows how to
         *        display the current widget
         */
        virtual void paint(Renderer& renderer);

        // Functions

        /**
         * @brief   Searches for a the first child with the same name as @a name
         * 
         *          If no child could be found, returns nullptr
         * @param name 
         * @return Widget* 
         */
        Widget* find(const SimpleString& name) const noexcept;

        // FIXME Probably should use another function, like something in corgi::math
        // cause I'm pretty sure this already exist and I'm repeating it again and
        // again
        [[nodiscard]] bool contains(int point_x, int point_y) const;

        void update_event(const Inputs& inputs, float elapsed_time);

        /*!
         * @brief Simply calls the paint function then loop through the children
         */
        virtual void actual_paint(Renderer& renderer);

        void disable() noexcept;

        /**
         * @brief Gets the widget's name
         * 
         * @return const SimpleString& 
         */
        [[nodiscard]] const SimpleString& name() const noexcept;

        /**
         * @brief Sets 
         * 
         * @param name 
         */
        void setName(const SimpleString& name);

        /**
         * @brief   Enable the current widget
         * 
         *          By default, rec is equal to true, which means
         *          all children of the current widget will also get enabled.
         *          If you wish only to enable the current widget, call the function with false 
         * 
         * @param rec 
         */
        void enable(bool rec = true) noexcept;

        /**
         * @brief Finds the viewport the widget currently is
         *        Actually helps when for instance, a widget is out of view
         *        inside a scrollview, to reject the input events
         */
        const Widget* findViewport() const;

        void position(float x, float y, bool relative = false);
        void dimension(float w, float h, bool relative = false);

        // TODO temporary
        float r {0.0f};
        float g {0.0f};
        float b {0.0f};
        float a {0.0f};

        bool isViewport = false;
        bool relative {false};
        bool mManualFocusIn  = false;
        bool mManualFocusOut = false;

        Material mMaterial;
        Event<>  resizingEvent;

        bool mProcessEvent = true;
        bool isResizing    = false;

        void setPropagateEvent(bool value) { mPropagateEvent = value; }

        UISystem* uiSystem();

        void setParentWidthCache(float parentWidth) { parentWidthCache_ = parentWidth; }
        void setParentHeightCache(float parentHeight)
        {
            parentHeightCache_ = parentHeight;
        }

        [[nodiscard]] Event<>& onEnable();
        [[nodiscard]] Event<>& onDisable();

        [[nodiscard]] bool eventsDisabled() const { return eventsDisabled_; }
        void               disableEvents() { eventsDisabled_ = true; }

    protected:
        bool eventsDisabled_ = false;
        /**
         * @brief   Checks if a resize event should happen
         */
        void checkForResizeEvent();

        // Variables

        // The viewport is simply the parent widget where the current widget "resided" if that makes sense?
        // Like if there's a scrollview, the scrollview will be a new viewport
        Widget* mViewport = nullptr;

        /*
         * @brief Reference to the Node Object containing the current Widget
         */
        Node<std::unique_ptr<Widget>>* mNode;

        Event<int, int> onMouseEnter_;
        Event<int, int> onMouseExit_;
        Event<>         onActivate_;
        Event<>         onDeactivate_;

        Event<> onEnable_;
        Event<> onDisable_;

        Event<const Mouse&> mouseDownEvent_;
        Event<const Mouse&> mouseUpEvent_;

        Event<int, int> on_mouse_over_;
        Event<int, int> mOnMouseClick;
        Event<int, int> on_mouse_drag_begin_;
        Event<int, int> on_mouse_drag_;
        Event<int, int> on_mouse_drag_end_;

        Event<int, int> mResizeEvent;
        Event<int, int> mMouseDoubleClickEvent;

        /**
         * @brief   Event triggered when a mouse wheel event happens
         *
         *          The argument will be the distance travelled by the mouse wheel
         */
        Event<int> mMouseWheelEvent;

        Event<> mFocusOutEvent;
        Event<> mFocusInEvent;

        float width_ {0.0f};
        float height_ {0.0f};

        float x_ {0.0f};
        float y_ {0.0f};

        std::optional<Unit> mRight;
        std::optional<Unit> mBottom;

        float mDown {-1.f};

        Margin mMargin;
        bool   mFocus = false;

        bool dragging_ {false};
        bool is_enabled_ {true};

        int previous_x_position_;
        int previous_y_position_;

        float offset_x {0.0f};
        float offset_y {0.0f};

        bool width_relative_ {false};
        bool height_relative_ {false};

        bool x_relative_ {false};
        bool y_relative_ {false};

        float parentWidthCache_;
        float parentHeightCache_;

        float parentX_ = 0.0f;
        float parentY_ = 0.0f;

        /*
         * @brief Tells the UISystem if the event processed should
         *        be propagated or not
         */
        bool mPropagateEvent = false;

        /**
         * @brief   We store the previous widget's width to compare it
         *          with the current width to know if we need to trigger
         *          the resizeEvent
         */
        float mPreviousWidth {-1.0f};

        /**
         * @brief   We store the previous widget's height to compare it
         *          with the current height to know if we need to trigger
         *          the resizeEvent
         */
        float mPreviousHeight {-1.0f};

        bool mIsDestroyed {false};

        /**
         *  If on side is true, meant we can resize the widget by using the mouse
         *  The percentage variable is here to tell the UISystem if we want
         *  the resize event to be made in % and thus allowing the widget
         *  to be relative to its parent
         */
        struct Resize
        {
            bool left               = false;
            bool leftInPercentage   = false;
            bool right              = false;
            bool rightInPercentage  = false;
            bool top                = false;
            bool topInPercentage    = false;
            bool bottom             = false;
            bool bottomInPercentage = false;
        } resize_;

        bool mouseHasEntered_    = false;
        bool mouseHasExited_     = false;
        bool triggerResizeEvent_ = false;
        bool hasTooltip_         = false;

        SimpleString tooltip_;

        float        startTooltipTimer_ = -1.0f;
        float        tooltipMaxTimer_   = 1.0f;
        SimpleString name_;
        bool         activity_ = true;

        UISystem* uiSystem_ = nullptr;

        void updateDimension(Widget* w);
    };
}    // namespace corgi::ui
