#pragma once

#include <corgi/ui/Image.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/ui/Widget.h>

namespace corgi::ui
{
    class Checkbox : public Widget
    {
    public:
        Checkbox();

        /**
         * \brief  I have to call the init function since the Node is not set by the constructor. And can't be
         */
        void init() override;

        void paint(Renderer& renderer) override;

        corgi::Event<bool>& onChanged();

        [[nodiscard]] bool isChecked() const;

        void check();
        void uncheck();
        bool isIndeterminated();
        void setIndeterminate(bool value);

    private:
        corgi::Event<bool> mOnChange;
        Image*             mImage            = nullptr;
        bool               mIsChecked        = false;
        bool               mIsIndeterminated = false;
    };
}    // namespace corgi::ui