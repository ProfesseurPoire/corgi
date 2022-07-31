#include <corgi/main/Window.h>
#include <corgi/rendering/texture.h>
#include <corgi/ui/menu/MenuBar.h>
#include <corgi/utils/ResourcesCache.h>
#include <corgi/ui/Image.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/ui/Text.h>

using namespace corgi::ui;
using namespace corgi;

MenuItem* Menu::addMenuItem(const std::string& name)
{
    auto* ptr = emplaceBack<MenuItem>(name);
    items_.push_back(ptr);
    ptr->onMouseClick() += [&](int x, int y)
    {
        closeMenu();
    };
    return ptr;
}

void MenuItem::init()
{
    background_ = emplaceBack<ui::Rectangle>();
    background_->setAnchorsToFillParentSpace();
    background_->setColor(Color(30, 30, 30));

    background_->onMouseEnter() += [&](int, int)
    {
        background_->setColor(Color(120, 120, 120));
    };

    background_->onMouseExit() += [&](int, int)
    {
        background_->setColor(Color(30, 30, 30));
    };

    text_ = background_->emplaceBack<ui::Text>();
    text_->setFont(ResourcesCache::get<Font>("corgi/fonts/Roboto-Medium.fnt"));
    text_->setText(name_);
    text_->setHorizontalAlignment(HorizontalAlignment::Left);
    text_->setLeft(0);
    text_->setMarginLeft(20);
    text_->setRight(0);
    text_->setColor(Color(230, 230, 230));
    text_->setAnchorsToFillParentSpace();
    disable();
}

void MenuBar::initializeMinimizeButton()
{
    minimizeButton_ = background_->emplaceBack<ui::Rectangle>();
    minimizeButton_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    minimizeButton_->setTop(0);
    minimizeButton_->setLeft(100, true);
    minimizeButton_->setWidth(-64);
    minimizeButton_->setMarginLeft(-96);
    minimizeButton_->setBottom(0);
    minimizeButton_->set_name("MinimizeButton");

    minimizeButton_->onMouseEnter() += [=](int, int)
    {
        minimizeButton_->setColor(styleSheet_->menuBar().backgroundMenuMouseOverColor());
    };

    minimizeButton_->onMouseExit() += [=](int, int)
    {
        minimizeButton_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    };

    minimizeButton_->onMouseClick() += [=](int, int)
    {
        Window::current_window()->minimize();
    };

    minimizeButtonImage_ = minimizeButton_->emplaceBack<ui::Image>();
    minimizeButtonImage_->setImage(ResourcesCache::get<Texture>("MinimizeIcon.tex"));
    minimizeButtonImage_->setWidth(16);
    minimizeButtonImage_->setHeight(16);
    minimizeButtonImage_->setTop(((40 - minimizeButtonImage_->height()) / 2.0f));
    minimizeButtonImage_->setLeft(8);
}

void MenuBar::initializeMaximizeButton()
{
    maximizeButton_ = background_->emplaceBack<ui::Rectangle>();
    maximizeButton_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    maximizeButton_->setTop(0);
    maximizeButton_->setLeft(100, true);
    maximizeButton_->setWidth(-32);
    maximizeButton_->setMarginLeft(-64);
    maximizeButton_->setBottom(0);
    maximizeButton_->set_name("MaximizeButton");

    maximizeButton_->onMouseEnter() += [=](int, int)
    {
        maximizeButton_->setColor(styleSheet_->menuBar().backgroundMenuMouseOverColor());
    };

    maximizeButton_->onMouseExit() += [=](int, int)
    {
        maximizeButton_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    };

    maximizeButton_->onMouseClick() += [&](int, int)
    {
        Window::current_window()->maximize();
        maximizeButton_->disable();
        restoreButton_->enable();
    };

    maximizeButtonImage_ = maximizeButton_->emplaceBack<ui::Image>();
    maximizeButtonImage_->setImage(ResourcesCache::get<Texture>("MaximizeIcon.tex"));
    maximizeButtonImage_->setWidth(16);
    maximizeButtonImage_->setHeight(16);
    maximizeButtonImage_->setTop(((40 - maximizeButtonImage_->height()) / 2.0f));
    maximizeButtonImage_->setLeft(8);
}

void MenuBar::initializeCloseButton()
{
    closeButton_ = background_->emplaceBack<ui::Rectangle>();
    closeButton_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    closeButton_->setTop(0);
    closeButton_->setLeft(100, true);
    closeButton_->setWidth(32);
    closeButton_->setMarginLeft(-32);
    closeButton_->setBottom(0);
    closeButton_->set_name("CloseWindowButton");

    closeButton_->onMouseEnter() += [=](int, int)
    {
        closeButton_->setColor(Color(220, 0, 0));
    };

    closeButton_->onMouseExit() += [=](int, int)
    {
        closeButton_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    };

    closeButton_->onMouseClick() += [=](int, int)
    {
        //Game::instance().quit();
    };

    closeButtonImage_ = closeButton_->emplaceBack<ui::Image>();
    closeButtonImage_->setImage(ResourcesCache::get<Texture>("closeIcon.tex"));
    closeButtonImage_->setWidth(16);
    closeButtonImage_->setHeight(16);
    closeButtonImage_->setTop(((40 - closeButtonImage_->height()) / 2.0f));
    closeButtonImage_->setLeft(8);
}

void Menu::closeMenu()
{
    menuItemContainers_->disable();
    for(auto* item : items_)
    {
        item->disable();
    }
}

void MenuBar::initializeRestoreButton()
{
    restoreButton_ = background_->emplaceBack<ui::Rectangle>();
    restoreButton_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    restoreButton_->setTop(0);
    restoreButton_->setLeft(100, true);
    restoreButton_->setWidth(-32);
    restoreButton_->setMarginLeft(-64);
    restoreButton_->setBottom(0);
    restoreButton_->set_name("RestoreButton");

    restoreButton_->onMouseEnter() += [=](int, int)
    {
        restoreButton_->setColor(styleSheet_->menuBar().backgroundMenuMouseOverColor());
    };

    restoreButton_->onMouseExit() += [=](int, int)
    {
        restoreButton_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    };

    restoreButton_->onMouseClick() += [=](int, int)
    {
        Window::current_window()->restore();
        maximizeButton_->enable();
        restoreButton_->disable();
    };

    restoreButtonImage_ = restoreButton_->emplaceBack<ui::Image>();
    restoreButtonImage_->setImage(ResourcesCache::get<Texture>("RestoreIcon.tex"));
    restoreButtonImage_->setWidth(16);
    restoreButtonImage_->setHeight(16);
    restoreButtonImage_->setTop(((40 - restoreButtonImage_->height()) / 2.0f));
    restoreButtonImage_->setLeft(8);

    restoreButtonImage_->disable();
}

void MenuBar::initializeButtons()
{
    initializeCloseButton();
    initializeRestoreButton();
    initializeMinimizeButton();
    initializeMaximizeButton();
}

int MenuBar::menusWidth() const noexcept
{
    int menuSum = 0;
    for(auto* menu : menus_)
    {
        menuSum += menu->width();
    }
    return menuSum;
}

int MenuBar::buttonsWidth() const noexcept
{
    return closeButton_->width() * 3;
}

MenuItem::MenuItem(const std::string& name)
    : name_(name)
{}

const std::string& MenuItem::name() const noexcept
{
    return name_;
}

std::vector<MenuItem*>& Menu::items()
{
    return items_;
}

Menu::Menu(const std::string& name, StyleSheet* styleSheet)
    : name_(name)
      , styleSheet_(styleSheet)
{}

void Menu::init()
{
    background_ = emplaceBack<ui::Rectangle>();
    background_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    background_->setAnchorsToFillParentSpace();

    menuItemContainers_ = emplaceBack<ui::Widget>();

    menuItemContainers_->onMouseEnter() += [&](int x, int y)
        { };

    menuItemContainers_->onMouseExit() += [&](int x, int y)
    {
        if(contains(x, y))
        {
            return;
        }
        menuItemContainers_->disable();
        for(auto* item : items_)
        {
            item->disable();
        }
    };

    menuItemContainers_->disable();

    background_->onMouseEnter() += [&](int x, int y)
    {
        // Means we're already displaying the menu
        if(menuItemContainers_->isEnabled())
            return;

        background_->setColor(styleSheet_->menuBar().backgroundMenuMouseOverColor());

        const float heightItem    = 30.0f;
        float       currentHeight = 0.0f;

        menuItemContainers_->setWidth(200);
        menuItemContainers_->setTop(40);
        menuItemContainers_->setHeight(heightItem * items_.size());
        menuItemContainers_->enable();

        for(auto* item : items_)
        {
            item->enable();
            item->setHeight(heightItem);
            item->setWidth(200);
            item->setLeft(0);
            item->setTop(40 + currentHeight);
            currentHeight += heightItem;
        }
    };

    background_->onMouseExit() += [&](int x, int y)
    {
        background_->setColor(styleSheet_->menuBar().backgroundMenuColor());

        if(menuItemContainers_->contains(x, y))
        {
            return;
        }

        for(auto* item : items_)
        {
            item->disable();
        }
        menuItemContainers_->disable();
    };

    text_ = background_->emplaceBack<ui::Text>();
    text_->setAnchorsToFillParentSpace();
    text_->setText(name_);
    text_->setColor(styleSheet_->menuBar().menuTitleTextColor());

    // Probably need to use a stylesheet instead of this later on
    text_->setFont(ResourcesCache::get<Font>("corgi/fonts/Roboto-Medium.fnt"));
}

const std::string& Menu::name() const noexcept
{
    return name_;
}

Menu* MenuBar::addMenu(const std::string& menuName)
{
    menus_.push_back(emplaceBack<ui::Menu>(menuName));
    resizeMenus();
    return menus_.back();
}

MenuBar::MenuBar(StyleSheet* styleSheet)
    : styleSheet_(styleSheet)
{}

void MenuBar::init()
{
    background_ = emplaceBack<ui::Rectangle>();
    background_->setColor(styleSheet_->menuBar().backgroundMenuColor());
    background_->setAnchorsToFillParentSpace();

    auto* text = background_->emplaceBack<ui::Text>();
    text->setFont(ResourcesCache::get<Font>("corgi/fonts/Roboto-Medium.fnt"));
    text->setAnchorsToFillParentSpace();
    text->setText("Unnamed Project");
    text->setColor(Color(230, 230, 230));

    initializeButtons();
}

void MenuBar::resizeMenus()
{
    float       offset = 0.0f;
    const float margin = 20.0f;
    for(auto* menu : menus_)
    {
        auto textWidth = menu->text_->textWidth(menu->name_);
        menu->setWidth(textWidth + margin);
        menu->setLeft(offset);
        menu->setTop(0.0f);
        menu->setHeight(height());
        offset += textWidth + margin;
    }
}
