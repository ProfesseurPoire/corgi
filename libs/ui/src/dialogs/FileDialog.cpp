#include <corgi/filesystem/FileSystem.h>
#include <corgi/main/Window.h>
#include <corgi/rendering/texture.h>
#include <corgi/systems/UISystem.h>
#include <corgi/ui/Image.h>
#include <corgi/ui/ScrollView.h>
#include <corgi/ui/SimpleButton.h>
#include <corgi/ui/dialogs/FileDialog.h>
#include <corgi/ui/layouts/FlowLayout.h>
#include <corgi/ui/layouts/VerticalLayout.h>
#include <corgi/utils/ResourcesCache.h>

using namespace corgi::ui;

FileItem::FileItem(corgi::filesystem::FileInfo fileInfo)
    : fileInfo_(fileInfo)
{
}

const corgi::filesystem::FileInfo& FileItem::fileInfo() const noexcept
{
    return fileInfo_;
}

void FileItem::initIcon()
{
    auto icon = textBackground_->emplaceBack<ui::Image>();

    textBackground_->set_name("FileItemBackground");
    icon->setDimensions(16, 16);
    icon->setLeft(10);
    icon->setTop(7);

    if(fileInfo_.is_folder())
        icon->setImage(ResourcesCache::get<Texture>("Folder.tex"));
    else
        icon->setImage(ResourcesCache::get<Texture>("FileIcon.tex"));
}

void FileItem::init()
{
    textBackground_ = emplaceBack<ui::Rectangle>();

    // We add the folder Icon in case we're dealing with a folder
    try
    {
        initIcon();
    }
    catch(std::exception e)
    {
        std::cout << e.what() << std::endl;
    }

    textBackground_->setColor(Color(50, 50, 60));

    textBackground_->onMouseEnter() += [=, this](int, int)
    {
        if(!this->isItemSelected_)
            textBackground_->setColor(Color(70, 70, 80));
    };

    textBackground_->onMouseExit() += [=, this](int, int)
    {
        if(!this->isItemSelected_)
            textBackground_->setColor(Color(50, 50, 60));
    };

    textBackground_->setAnchorsToFillParentSpace();

    auto text = textBackground_->emplaceBack<ui::Text>();

    text->setAnchorsToFillParentSpace();
    text->setText(fileInfo_.name().c_str());
    text->setHorizontalAlignment(corgi::HorizontalAlignment::Left);
    text->setColor(Color(215, 215, 215));
    text->setLeft(36);
}

void FileDialog::initFiles(const std::string& rootFolder)
{
    currentFolder = rootFolder;
    currentFolderTextBox_->setText(currentFolder.c_str());

    // It would be nice to sort directories by folder, then alpha

    auto files = filesystem::list_directory(rootFolder);

    std::vector<filesystem::FileInfo> fileInfos;

    for(auto file : files)
    {
        fileInfos.push_back(file);
    }

    std::sort(fileInfos.begin(), fileInfos.end());

    for(auto file : fileInfos)
    {
        auto fileItem = flowLayout_->emplaceBack<FileItem>(file);
        fileItem->setHeight(30);
        fileItem->setLeft(0);
        fileItem->setRight(0);

        fileItem->onMouseClick() += [&, fileItem](int, int)
        {
            if(selectedItem_ != fileItem)
            {
                if(selectedItem_ != nullptr)
                {
                    selectedItem_->isItemSelected_ = false;
                    dynamic_cast<ui::Rectangle*>(
                        selectedItem_->find("FileItemBackground"))
                        ->setColor(Color(50, 50, 60));
                }

                okButton_->activate();
                selectedItem_ = fileItem;

                fileItem->isItemSelected_ = true;
                dynamic_cast<ui::Rectangle*>(fileItem->find("FileItemBackground"))
                    ->setColor(Color(150, 150, 200));
            }
        };

        fileItem->mouseDoubleClickEvent() += [=](int, int)
        {
            if(file.is_folder())
            {
                flowLayout_->clearChildren();
                initFiles(file.path() + "/");
                selectedItem_ = nullptr;
                okButton_->deactivate();
            }
        };
    }
}

FileDialog::FileDialog(const std::string& folder, StyleSheet& styleSheet)
    : DialogWindow(styleSheet)
    , currentFolder(folder)
{
}

void FileDialog::initButtons()
{
    initCancelButton();
    initOkButton();
}

void FileDialog::initCancelButton()
{
    cancelButton_ = emplaceBack<ui::SimpleButton>();
    cancelButton_->setText("Cancel");
    cancelButton_->setLeft(100, true);
    cancelButton_->setWidth(-10);
    cancelButton_->setMarginLeft(-100);

    cancelButton_->setTop(100, true);
    cancelButton_->setHeight(-10);
    cancelButton_->setMarginTop(-50);

    cancelButton_->onMouseClick() += [&](int x, int y)
    {
        parent()->destroy();
    };
}

void FileDialog::initOkButton()
{
    okButton_ = emplaceBack<ui::SimpleButton>();
    okButton_->setText("Ok");
    okButton_->setLeft(100, true);
    okButton_->setWidth(-110);
    okButton_->setMarginLeft(-200);

    okButton_->setTop(100, true);
    okButton_->setHeight(-10);
    okButton_->setMarginTop(-50);

    okButton_->onMouseClick() += [&](int x, int y)
    {
        if(selectedItem_ != nullptr)
        {
            onFileSelection_(selectedItem_->fileInfo().path());
        }
        parent()->destroy();
    };

    okButton_->deactivate();
}

corgi::Event<std::string>& FileDialog::onFileSelection()
{
    return onFileSelection_;
}

void FileDialog::init()
{
    DialogWindow::init();

    float topBarHeight = 50;

    currentFolderTextBox_ = contentWidget().emplaceBack<ui::TextBox>();

    currentFolderTextBox_->setLeft(100);
    currentFolderTextBox_->setWidth(300);
    currentFolderTextBox_->setHeight(30);
    currentFolderTextBox_->setTop((topBarHeight - currentFolderTextBox_->height()) /
                                  2.0f);

    currentFolderTextBox_->textWidget().setHorizontalAlignment(HorizontalAlignment::Left);
    currentFolderTextBox_->textWidget().setLeft(15);

    auto* upFolderBackground_ = contentWidget().emplaceBack<ui::Rectangle>();
    upFolderBackground_->setDimensions(32, 32);
    upFolderBackground_->setTop((topBarHeight - upFolderBackground_->height()) / 2.0f);
    upFolderBackground_->setLeft(15);
    upFolderBackground_->setColor(styleSheet_->dialog().contentBackgroundColor());

    upFolderBackground_->onMouseClick() += [&](int, int)
    {
        auto parentFolder = filesystem::getParentFolder(currentFolder.c_str());
        parentFolder += "/";
        selectedItem_ = nullptr;
        okButton_->deactivate();
        flowLayout_->clearChildren();

        initFiles(parentFolder.c_str());
    };

    upFolderBackground_->onMouseEnter() += [=](int, int)
    {
        upFolderBackground_->setColor(styleSheet_->dialog().contentBackgroundColor() -
                                      Color(10, 10, 20, 0));
    };

    upFolderBackground_->onMouseExit() += [=](int, int)
    {
        upFolderBackground_->setColor(styleSheet_->dialog().contentBackgroundColor());
    };

    upFolderImage_ = upFolderBackground_->emplaceBack<ui::Image>();
    upFolderImage_->setDimensions(24, 24);
    upFolderImage_->setImage(ResourcesCache::get<Texture>("UpFolder.tex"));
    upFolderImage_->setLeft(4);
    upFolderImage_->setTop(4);

    auto scrollViewBackground = contentWidget().emplaceBack<ui::Rectangle>();
    scrollViewBackground->setColor(Color(20, 20, 20));
    scrollViewBackground->setTop(topBarHeight);
    scrollViewBackground->setLeft(10);
    scrollViewBackground->setRight(10);
    scrollViewBackground->setBottom(50);

    auto scrollView = contentWidget().emplaceBack<ui::ScrollView>();
    scrollView->setTop(topBarHeight);
    scrollView->setLeft(10);
    scrollView->setRight(10);
    scrollView->setBottom(0);

    flowLayout_ = scrollView->content()->emplaceBack<ui::VerticalLayout>();
    flowLayout_->setAnchorsToFillParentSpace();

    initFiles(currentFolder);
    initButtons();
}
