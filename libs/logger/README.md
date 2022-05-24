# CorgiLogger

## Table of Contents
 1. [Introduction](#Introduction)
 2. [Platforms](#Platforms)
 3. [Installation](#Installation)
 4. [Usage](#Usage) 
 
 ## Introduction
 
 CorgiLogger provide functions to log information about the application. 
 
 It can be used by developers to ensure that their applications are working as intended by logging useful information about the application inside the console or files.

 ## Platforms
The supported platforms so far are (afaik)
 * Linux ( GCC 10+)
 * Windows (MSVC 2019+)
 * macos (Clang 12+)

## Installation

### As a Static lib

Simply clone the repo on your computer, and use the cmake install command.
If you're using cmake on your project, you can then directly use the find_package
command.

Again, if you're using cmake, you can simply throw the repo in your project,
use add_subdirectory, and then add the CorgiLogger target in your project dependencies
 
 ## Features
  * Channels used to sort logs
  * User defined types
  * 6 different level of logging information
  * Write in files or console
  * Use preprocessor to disable or enable logging
 ## Usage

 Include the <b><corgi/logger/log.h></b> header where you want to log stuff and then simply use the following functions inside your code, depending on the type of information you wish to log

 Logs are written in 3 locations by default : 
 * Inside the console
 * Inside a file with their tagged channel
 * Inside a std::vector associated with their tagged channel

### Basic Example
```cpp
#include <corgi/test/test.h>

int main()
{
    log_info("Log information")
    log_trace("Tracing information")
    log_debug("Log Debug information")
    log_warning("Log Warning information")
    log_error("Log Error information")
    log_fatal_error("Log Fatal Error")
}
```

### Configuration
The <b>CORGI_VERBOSITY</b> preprocessor macro is used to specify which logs will be actually registered by the application. 

Its value ranged from 0 to 6
  
  0. Everything will be logged by the application
  1. Info won't be logged
  2. Info + Trace won't be logged
  3. Info + Trace + Debug won't be logged
  4. Info + Trace + Debug + Warning won't be logged
  5. Info + Trace + Debug + Warning + Error won't be logged
  6. Nothing will be logged
  
  To set the <b>CORGI_VERBOSITY</b> variable, add this line inside the CMakeLists.txt

```cmake
  target_compile_definitions(target PUBLIC CORGI_VERBOSITY=3)
```

### Channels
Logs can be sorted by channels.

By default, all log functions are associated with the "default" channel. But maybe you don't want to mix every log in the same output.

Say for instance that we're working on a renderer. You probably don't want the console to display logs about the audio or logic.

By tagging the renderer logs with a "renderer" channel, you'll have 
the ability to only watch "renderer" logs inside the console
by using the set_current_channel method, and logs will also be rerouted into a file with the channel name.