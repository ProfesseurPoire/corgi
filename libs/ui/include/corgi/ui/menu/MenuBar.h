#pragma once

#include <corgi/SimpleString.h>
#include <corgi/containers/Vector.h>
#include <corgi/ui/Image.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/ui/StyleSheet.h>
#include <corgi/ui/Text.h>
#include <corgi/ui/Widget.h>
#include <corgi/utils/Event.h>

namespace corgi::ui
{
    class MenuItem : public Widget
    {
    public:
        MenuItem(const SimpleString& name);

        void init() override;

        [[nodiscard]] const SimpleString& name() const noexcept;

    private:
        ui::Rectangle* background_;
        ui::Text*      text_;
        SimpleString   name_;
    };

    class Menu : public Widget
    {
    public:
        friend class MenuBar;
        Menu(const SimpleString& name, StyleSheet* styleSheet = &defaultStyleSheet);

        void init() override;

        MenuItem* addMenuItem(const SimpleString& name);
        void      closeMenu();

        Vector<MenuItem*>& items();

        [[nodiscard]] const SimpleString& name() const noexcept;

    private:
        Vector<MenuItem*> items_;
        SimpleString      name_;
        ui::Rectangle*    background_;
        ui::Text*         text_;
        StyleSheet*       styleSheet_;
        Widget*           menuItemContainers_;
    };

    /**
     * @brief There can be only 1 menu bar 
     */
    class MenuBar : public Widget
    {
    public:
        MenuBar(StyleSheet* styleSheet = &defaultStyleSheet);

        void init() override;

        Menu* addMenu(const SimpleString& menuName);

        /**
         * @brief Returns the width of every combined menu on the left of the menu bar
         */
        [[nodiscard]] int menusWidth() const noexcept;

        /**
         * @brief Returns the width of every combined buttons on the right of the menu bar
         * 
         * @return int 
         */
        [[nodiscard]] int buttonsWidth() const noexcept;

    private:
        /**
         * @brief   Resize all menus 
         * 
         *          Called when a menu change means that their dimensions are updated
         * 
         */
        void resizeMenus();

        void initializeButtons();
        void initializeCloseButton();
        void initializeRestoreButton();
        void initializeMaximizeButton();
        void initializeMinimizeButton();

        Vector<Menu*> menus_;

        Rectangle*  background_;
        StyleSheet* styleSheet_;
        Image*      closeButtonImage_    = nullptr;
        Image*      restoreButtonImage_  = nullptr;
        Image*      maximizeButtonImage_ = nullptr;
        Image*      minimizeButtonImage_ = nullptr;

        Rectangle* closeButton_    = nullptr;
        Rectangle* restoreButton_  = nullptr;
        Rectangle* maximizeButton_ = nullptr;
        Rectangle* minimizeButton_ = nullptr;

        int previousMousePositionX_;
        int previousMousePositionY_;

        /**
         * @brief If true, we move the window when dragging the menu
         */
        bool isMainMenuBar_ = true;
    };
}    // namespace corgi::ui