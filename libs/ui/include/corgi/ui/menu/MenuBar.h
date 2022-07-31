#pragma once

#include <corgi/ui/StyleSheet.h>
#include <corgi/ui/Widget.h>

#include <string>
#include <vector>

namespace corgi::ui
{
    class Rectangle;
    class Text;
    class Image;

    class MenuItem : public Widget
    {
    public:
        MenuItem(const std::string& name);

        void init() override;

        [[nodiscard]] const std::string& name() const noexcept;

    private:
        ui::Rectangle* background_;
        ui::Text*      text_;
        std::string   name_;
    };

    class Menu : public Widget
    {
    public:
        friend class MenuBar;
        Menu(const std::string& name, StyleSheet* styleSheet = &defaultStyleSheet);

        void init() override;

        MenuItem* addMenuItem(const std::string& name);
        void      closeMenu();

        std::vector<MenuItem*>& items();

        [[nodiscard]] const std::string& name() const noexcept;

    private:
        std::vector<MenuItem*> items_;
        std::string      name_;
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

        Menu* addMenu(const std::string& menuName);

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

        std::vector<Menu*> menus_;

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