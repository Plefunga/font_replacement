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

Next, you 














# Windows
https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-msbuild?pivots=shell-cmd

but your installing opencv.

That should do it
To add files, get all files from src and lib and take them out and put them in the root project dir.
Then right click on the header files, and add existing item, and add all the header (and hpp) files.

Do likewise with the source files.

```bash
.\vcpkg install opencv[core,jpeg,png,webp,quirc,default-features,dnn,ffmpeg,freetype,ipp,tbb,tiff] --featurepackages
```

Quick note: it would be interesting to add Vulcan support for dnn. However, I haven't tried this yet, and have also not tested this yet as at the time of writing, xz has a massive security vulnerability that means that doing the above doesn't work as it fails to find xz 5.4.4 (which does not have the vulnerability, and is in pretty much all linux distros.)

I may need to do this:
https://github.com/microsoft/vcpkg/issues/37839#issuecomment-2028011285

In a nutshell, modify `vcpkg\ports\liblzma\portfile.cmake` to be 
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
However, I found that when I tried to compile my project, it didn't use my installed opencv. When I tried to build it, I had to modify the vcpkg.json to this:
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

and when it tries to install, it would fail on liblzma due to backdoor thing, and I had to do the above modification to the cached portfile.cmake, which I found the path to in the output. After that, it decided to completely rebuild it.

If this doesn't work, I am going to make it a cmake project.

Also note, I included vulkan and opengl in the above json file. This is a bit of an experiment, to make it do the dnn bits quicker on all platforms. However, at the moment it appears that Nvidia has just wrapped vulkan in opengl, so I don't expect huge benefits from using it on nvidia based systems.

# DONT DO THE FOLLOWING - THIS IS JUST A RECORD OF MY PREVIOUS ATTEMPTS

Follow the guides at
a) https://github.com/roxlu/cmake-freetype-harfbuzz
b) https://gist.github.com/demid5111/6faa590e4fc5813550dd95cc1c538893

Note: there are a few unclear steps. You will be building from source, and for the most part, the guide is very clear.

When installing the harfbuzz and freetype:
- you will need to make a toolchain file like this
```cmake
SET(FREETYPE_FOUND 1)
SET(FREETYPE_LIBRARIES C:\\freetype-harfbuzz\\freetype\\lib)
SET(FREETYPE_INCLUDE_DIRS C:\\freetype-harfbuzz\\freetype\\include)
SET(HARFBUZZ_FOUND 1)
SET(HARFBUZZ_LIBRARIES C:\\freetype-harfbuzz\\harfbuzz\\lib\\harfbuzz)
SET(HARFBUZZ_INCLUDE_DIRS C:\\freetype-harfbuzz\\harfbuzz\\include)
```
as it does not do it by itself.
In this case, I have taken the useful files out and put them into a directory. I may eventually create a link to this directory so you don't even need to build them.

For now, the build is failing for freetype in the linking step. I'll get to this later. For now, just untick the option to use freetype, and comment out the USE_FREETYPE macro in config.h


In the second one:
- when specifying `OPENCV_EXTRA_MODULES_PATH`, you need to do include "/modules/" at the end, or it won't work.
- You will need to link the toolchain file. This is doen with the radio selector in the initial configuration in cmake gui, and you select the one that allows you to specify a toolchain file (it also does say for cross compilation, which is fine.)
- when doing `BUILD_ALL` and `INSTALL` bits, have a random folder selected in the sidebar on the right, and go to build->build->BUILD_ALL and likewise with INSTALL.
- remember to actually restart after doing the steps with path.
- There are 'd's after each dll in the example. This is because they compiled a debug version. If you want to, you can do that, but otherwise just get rid of them
- I need to figure out why the linking is not working. It can't actually find the libs.










https://sourceforge.net/projects/opencvlibrary/files/
get latest windows (I used 4.9.0)

extract into C:\opencv

run:
setx OpenCV_DIR C:\opencv\build\x64\vc16