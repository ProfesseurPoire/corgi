#pragma once

namespace corgi
{
struct Resource
{
    virtual ~Resource() {}

    [[nodiscard]] virtual long long memory_usage() const { return 0; }
};
}    // namespace corgi
