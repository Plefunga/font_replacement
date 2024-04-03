# Linux

Assuming that you have all of the code and resources downloaded. They should have come packaged with this.

## Install Opencv
```bash
cd
sudo apt install pkg-config     
sudo apt install ffmpeg libavformat-dev libavcodec-dev libswscale-dev 
mkdir opencv-install
cd opencv-install
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
mkdir build
cd build
cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv
make -j8
sudo make install
```
This should install opencv and opencv-contrib, as well as whatever else is required.

## Compile
All you should need to do is
```bash
git clone https://github.com/Plefunga/font_replacement.git
cd font_replacement
cmake .
make
```

## Run
To do it from the camera:
`./fontreplacement`
You can select which camera by changing `camera_index` in `config.ini`.

To use a video or stream:
`./fontreplacement -i=/path/to/some/video.mp4`

It can also take still images.

# Windows
## Install packages
You will need to have Visual Studio with c++ installed.

Do<br />
https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-msbuild?pivots=shell-cmd

However, use this as `vcpkg.json`:
```
{
  "dependencies": [
    "opencv",
    {
      "name": "opencv",
      "default-features": true,
      "features": [
        "contrib","default-features","eigen","ffmpeg","freetype","ipp","jpeg","lapack","opengl","png","tbb","tiff","vulkan","webp"
      ]
    }
    
  ]
}
```

To add files in visual studio, get all files from src and lib and take them out and put them in the root project dir.
Then right click on the header files, and add existing item, and add all the header (and hpp) files.

Do likewise with the source files.

At the time of writing, there is a vulnerability in xz, meaning that the github repo is down. THis means that the installation fails, and you will need to do the things in this to the `portfile.cmake` that it is using:
https://github.com/microsoft/vcpkg/issues/37839#issuecomment-2028011285
except with `xz-mirror` as the mirror that is listed in the commend is also not available.

If that makes no sense, start compiling the code, and it will take forever, but it will also fail to compile liblzma. Take note of the portfile.cmake that it is using, and edit it to be
```cmake
# old one that is currently down due to backdoor
#vcpkg_from_github(
#    OUT_SOURCE_PATH SOURCE_PATH
#    REPO tukaani-project/xz
#    REF "v${VERSION}"
#    SHA512 #c28461123562564e030f3f733f078bc4c840e87598d9f4b718d4bca639120d8133f969c45d7bdc62f33f081d789ec0f14a1791fb7da185156#82bfe3c0c7362e0
#    HEAD_REF master
#    PATCHES
#        fix_config_include.patch
#        win_output_name.patch # Fix output name on Windows. Autotool build does not generate lib prefixed #libraries on windows. 
#        add_support_ios.patch # add install bundle info for support ios 
#        build-tools.patch
#)

# mirror
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO xz-mirror/xz
    REF "v${VERSION}"
    SHA512 c28461123562564e030f3f733f078bc4c840e87598d9f4b718d4bca639120d8133f969c45d7bdc62f33f081d789ec0f14a1791fb7da18515682bfe3c0c7362e0
    HEAD_REF master
    PATCHES
        fix_config_include.patch
        win_output_name.patch # Fix output name on Windows. Autotool build does not generate lib prefixed libraries on windows. 
        add_support_ios.patch # add install bundle info for support ios 
        build-tools.patch
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        tools BUILD_TOOLS
)

if(VCPKG_TARGET_ARCHITECTURE STREQUAL "wasm32")
    set(WASM_OPTIONS -DCMAKE_C_BYTE_ORDER=LITTLE_ENDIAN -DCMAKE_CXX_BYTE_ORDER=LITTLE_ENDIAN)
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
        ${WASM_OPTIONS}
        -DBUILD_TESTING=OFF
        -DCREATE_XZ_SYMLINKS=OFF
        -DCREATE_LZMA_SYMLINKS=OFF
        -DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=   # using flags from (vcpkg) toolchain
    MAYBE_UNUSED_VARIABLES
        CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        CREATE_XZ_SYMLINKS
        CREATE_LZMA_SYMLINKS
)
vcpkg_cmake_install()
vcpkg_copy_pdbs()

set(exec_prefix "\${prefix}")
set(libdir "\${prefix}/lib")
set(includedir "\${prefix}/include")
set(PACKAGE_URL https://tukaani.org/xz/)
set(PACKAGE_VERSION 5.4.3)
if(NOT VCPKG_TARGET_IS_WINDOWS)
    set(PTHREAD_CFLAGS -pthread)
endif()
set(prefix "${CURRENT_INSTALLED_DIR}")
configure_file("${SOURCE_PATH}/src/liblzma/liblzma.pc.in" "${CURRENT_PACKAGES_DIR}/lib/pkgconfig/liblzma.pc" @ONLY)
if (NOT VCPKG_BUILD_TYPE)
  set(prefix "${CURRENT_INSTALLED_DIR}/debug")
  configure_file("${SOURCE_PATH}/src/liblzma/liblzma.pc.in" "${CURRENT_PACKAGES_DIR}/debug/lib/pkgconfig/liblzma.pc" @ONLY)
endif()
vcpkg_fixup_pkgconfig()

vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/liblzma)

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/include/lzma.h" "defined(LZMA_API_STATIC)" "1")
else()
    vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/include/lzma.h" "defined(LZMA_API_STATIC)" "0")
endif()

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
    "${CURRENT_PACKAGES_DIR}/share/man"
)

set(TOOLS xz xzdec)
foreach(_tool IN LISTS TOOLS)
    if(NOT EXISTS "${CURRENT_PACKAGES_DIR}/bin/${_tool}${VCPKG_TARGET_EXECUTABLE_SUFFIX}")
        list(REMOVE_ITEM TOOLS ${_tool})
    endif()
endforeach()
if(TOOLS)
    vcpkg_copy_tools(TOOL_NAMES ${TOOLS} AUTO_CLEAN)
endif()

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()

file(COPY "${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(COPY "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/COPYING")
```

## Building
I find that the debug configuration is very slow, and so I don't use it. When you have compiled it, you will need to copy and paste either the .exe and .dll files out of `x64/Release` or `x64/Debug` (depending on which configuration you built), and place them into the same directory that has `config.ini`. Either that, or you can put config.ini and the resources folder into the Debug or Release folder.<br />

In a nutshell, you need to have `resources` and `config.ini` in the same folder as `fontreplacement.exe` and all of the `.dll`s.

## Run
To do it from the camera:
`fontreplacement` (or double click on the exe)
You can select which camera by changing `camera_index` in `config.ini`.

To use a video or stream:
`fontreplacement -i=/path/to/some/video.mp4`

It can also take still images.