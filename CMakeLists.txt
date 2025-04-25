# CMakeLists.txt
cmake_minimum_required(VERSION 3.15) # FetchContent needs a reasonably modern CMake

project(MyWindowsApp LANGUAGES CXX) # Replace MyWindowsApp with your actual app name

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
                 -DBUILD_SHARED_LIBS=OFF  # <<< Force static linking for libusb
)

# Make libusb available (downloads/updates and adds it as a sub-project)
FetchContent_MakeAvailable(libusb)
# NOTE: FetchContent_MakeAvailable brings the targets from libusb's CMakeLists.txt
#       into scope. The primary library target is typically 'usb-1.0'.

# --- Define Your Application Executable ---
add_executable(MyWindowsApp src/main.cpp) # Replace MyWindowsApp with your actual app name

# --- Explicitly state build order dependency ---
# Although target_link_libraries implies a dependency, add_dependencies
# makes it explicit: MyWindowsApp cannot start building until the
# 'usb-1.0' target (the libusb static library) is complete.
# This solves link errors where the library hasn't been built yet.
add_dependencies(MyWindowsApp usb-1.0) 

# --- Link Your Application to libusb ---
# This tells the linker *what* to link with MyWindowsApp.
# The dependency created by add_dependencies ensures usb-1.0 exists when linking occurs.
target_link_libraries(MyWindowsApp PRIVATE usb-1.0)

# --- Include Directories ---
# Add libusb's include directory to your application's include path
target_include_directories(MyWindowsApp PRIVATE ${libusb_SOURCE_DIR}/libusb/libusb)

# Optional: Set output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Build Configuration Specifics (Optional but Recommended) ---
if(MSVC)
  # Improve Windows build settings if needed
  target_compile_options(MyWindowsApp PRIVATE /W4) # Example: Higher warning level
  # Force static runtime library (/MT or /MTd) to avoid needing VC++ Redistributable
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_POLICY_DEFAULT_CMP0091 NEW) # Handle MSVC runtime library selection correctly
endif()