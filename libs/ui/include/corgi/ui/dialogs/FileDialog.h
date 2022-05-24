#pragma once

#include <corgi/filesystem/FileSystem.h>
#include <corgi/ui/DialogWindow.h>
#include <corgi/ui/TextBox.h>
#include <corgi/ui/dialogs/FileDialog.h>
#include <corgi/ui/layouts/FlowLayout.h>

namespace corgi::ui
{
    class SimpleButton;
    class VerticalLayout;

    /**
     * @brief   Widget displaying a file in the FileDialog
     *          
     *          Contains a FileInfo object with informations about the
     *          file the item is displaying. Display an icon and the file's name
     */
    class FileItem : public Widget
    {
        friend class FileDialog;

    public:
        FileItem(corgi::filesystem::FileInfo fileInfo);

        /**
         * @brief   Initialize the fileItem and its widgets
         */
        void init() override;

        /**
         * @brief   Initialize the icon used
         * 
         *          We use different icons depending on the type of file
         *          (textual, folders, images etc)
         */
        void initIcon();

        /**
         * @brief   Returns the fileInfo displayed by the fileItem
         * 
         * @return  const corgi::filesystem::FileInfo& 
         */
        [[nodiscard]] const corgi::filesystem::FileInfo& fileInfo() const noexcept;

    private:
        /**
         * @brief   Rectangle used as a text background
         */
        ui::Rectangle* textBackground_ = nullptr;

        /**
         * @brief True if the item is selected
         */
        bool isItemSelected_ = false;

        /**
         * @brief Stores informations about the file
         */
        corgi::filesystem::FileInfo fileInfo_;
    };

    /**
     * @brief   Modal Dialog Window used to select a file from disk
     * 
     *          To get back the selected file after pressing ok, use the 
     *          onFileSelection event 
     */
    class FileDialog : public DialogWindow
    {
    public:
        /**
         * @brief Construct a new File Dialog object
         * 
         * @param folder        Display the content of this folder when the dialog is
         *                      first opened
         * 
         * @param styleSheet    Contains rules about the widget's appearance. 
         *                      Use the default StyleSheet if non is specified by the user
         */
        FileDialog(const SimpleString& folder,
                   StyleSheet&         styleSheet = defaultStyleSheet);

        /**
         * @brief The initialize function actually construct the widgets
         * 
         */
        void init() override;

        /**
         * @brief Initialize the fileItems
         * 
         * @param rootFolder 
         */
        void initFiles(const SimpleString& rootFolder);

        /**
         * @brief Returns the event triggered when clicking ok button after selecting a file
         * 
         * @return Event<SimpleString>& 
         */
        [[nodiscard]] Event<SimpleString>& onFileSelection();

    private:
        // Functions

        /**
         * @brief   Initialize the ok button
         * 
         *          Ok button is greyout if no fileItem is currently selected
         */
        void initOkButton();

        /**
         * @brief Initialize the buttons widgets
         */
        void initButtons();

        /**
         * @brief Initialize the cancel button
         */
        void initCancelButton();

        // Variables

        /**
         * @brief Exit the current fileDialog
         */
        SimpleButton* cancelButton_ = nullptr;

        /**
         * @brief Greyed out when no item is selected. Triggers onFileSelection event
         */
        SimpleButton* okButton_ = nullptr;

        /**
         * @brief   Layout positionning the FileItem
         */
        ui::VerticalLayout* flowLayout_ = nullptr;

        /**
         * @brief   Image representing the 
         */
        ui::Image* upFolderImage_ = nullptr;

        /**
         * @brief   Textbox displaying the current folder
         */
        ui::TextBox* currentFolderTextBox_ = nullptr;

        /**
         * @brief   Saves the folder we're currently in
         */
        SimpleString currentFolder;

        /**
         * @brief Pointer to the item currently selected
         */
        FileItem* selectedItem_ = nullptr;

        /**
         * @brief   Event called when the file dialog is closed that send the 
         *          file that has been selected
         */
        Event<SimpleString> onFileSelection_;
    };
}    // namespace corgi::ui