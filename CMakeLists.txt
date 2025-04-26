# CMakeLists.txt
cmake_minimum_required(VERSION 3.15) # FetchContent needs a reasonably modern CMake

project(DialLog LANGUAGES CXX) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Fetch and Configure libusb ---
include(FetchContent)
 
# Declare libusb content
FetchContent_Declare(
  libusb
  GIT_REPOSITORY https://github.com/libusb/libusb-cmake.git
  GIT_TAG        v1.0.28-0 # Use the desired stable version tag
  GIT_SHALLOW    TRUE    # Faster clone
  CMAKE_ARGS     -DINSTALL_UDEV_RULES=OFF # Not needed on Windows
                 -DENABLE_EXAMPLES=OFF    # Don't build examples
                 -DENABLE_TESTS=OFF       # Don't build tests
                 -DBUILD_SHARED_LIBS=OFF  # Force static linking for libusb
)
FetchContent_MakeAvailable(libusb)

FetchContent_Declare(
  cmdparser
  GIT_REPOSITORY https://github.com/FlorianRappl/CmdParser.git
  GIT_TAG        v1.1.0
  PATCH_COMMAND git apply --ignore-space-change --ignore-whitespace ${CMAKE_CURRENT_SOURCE_DIR}/patches/cmdparser/CMakeLists.txt.patch

)
FetchContent_MakeAvailable(cmdparser)
 

# --- Define Your Application Executable ---
add_executable(DialLog src/main.cpp) 

# --- Explicitly state build order dependency ---
# Although target_link_libraries implies a dependency, add_dependencies
# makes it explicit: DialLog cannot start building until the
# 'usb-1.0' target (the libusb static library) is complete.
# This solves link errors where the library hasn't been built yet.
add_dependencies(DialLog usb-1.0) 
 
# --- Link Your Application to libusb ---
# This tells the linker *what* to link with DialLog.
# The dependency created by add_dependencies ensures usb-1.0 exists when linking occurs.
target_link_libraries(DialLog PRIVATE usb-1.0)

# --- Include Directories ---
# Add libusb's include directory to your application's include path
target_include_directories(DialLog PRIVATE ${libusb_SOURCE_DIR}/libusb/libusb)
target_include_directories(DialLog PRIVATE ${cmdparser_SOURCE_DIR}/)

# --- Build Configuration Specifics (Optional but Recommended) ---
if(MSVC)
  # Improve Windows build settings if needed
  target_compile_options(DialLog PRIVATE /W4) # Example: Higher warning level
  # Force static runtime library (/MT or /MTd) to avoid needing VC++ Redistributable
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_POLICY_DEFAULT_CMP0091 NEW) # Handle MSVC runtime library selection correctly
endif()