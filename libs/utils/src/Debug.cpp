#include <corgi/logger/log.h>
#include <corgi/utils/Debug.h>

#include <string>

using namespace corgi;

void Debug::log(const Color& color)
{
    std::string str;

    str += "Color : { ";
    str += std::to_string(color.getRedInt());

    str += ", " + std::to_string(color.getGreenInt()) + ", " + std::to_string(color.getBlueInt()) + ", " +
           std::to_string(color.getAlphaInt()) + "}";

    log_debug(str);
}