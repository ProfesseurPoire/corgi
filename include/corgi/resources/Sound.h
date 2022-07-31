#pragma once

#include <corgi/SimpleString.h>
#include <corgi/resources/Resource.h>

namespace corgi
{
class Sound : public Resource
{

public:
    friend class AudioPlayer;

    // Constructors

    Sound(const SimpleString& file, const SimpleString& identifier);

    ~Sound() override;

    // Functions

    [[nodiscard]] long long memory_usage() const override;
    bool                    is_wav() const;

private:
    bool         is_wav_     = false;
    void*        sound_file_ = nullptr;
    SimpleString name_;
};
}    // namespace corgi
