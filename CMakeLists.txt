# CMakeLists.txt
cmake_minimum_required(VERSION 3.15) # FetchContent needs a reasonably modern CMake

project(MyWindowsApp LANGUAGES CXX) # Replace MyWindowsApp with your actual app name

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Fetch and Configure libusb ---
include(FetchContent)

# Declare libusb content
# Using the official libusb mirror on GitHub. Check for the latest stable tag.
FetchContent_Declare(
  libusb
  GIT_REPOSITORY https://github.com/libusb/libusb.git
  GIT_TAG        v1.0.27 # Use the desired stable version tag
  GIT_SHALLOW    TRUE    # Faster clone
  CMAKE_ARGS     -DINSTALL_UDEV_RULES=OFF # Not needed on Windows
                 -DENABLE_EXAMPLES=OFF    # Don't build examples
                 -DENABLE_TESTS=OFF       # Don't build tests
                 -DBUILD_SHARED_LIBS=OFF  # <<< Force static linking for libusb
                 # Add other libusb CMake options if needed
)

# Make libusb available (downloads/updates and adds it as a sub-project)
# This needs to happen *before* defining your executable that depends on it.
FetchContent_MakeAvailable(libusb)

# --- Define Your Application Executable ---
add_executable(MyWindowsApp src/main.cpp) # Replace MyWindowsApp with your actual app name

# --- Link Your Application to libusb ---

# The target name provided by libusb's CMake build is typically 'usb-1.0'
# We link it privately, meaning only MyWindowsApp needs it directly.
target_link_libraries(MyWindowsApp PRIVATE usb-1.0)

# Add libusb's include directory to your application's include path
# FetchContent typically sets up variables like <depname>_SOURCE_DIR and <depname>_BINARY_DIR
# We need the include directory from the source tree.
target_include_directories(MyWindowsApp PRIVATE ${libusb_SOURCE_DIR}/libusb)

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

# --- Installation (Optional) ---
# If you want to create an installer or package later
# install(TARGETS MyWindowsApp DESTINATION bin)