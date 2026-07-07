#==============================================================================
# nfx::silicon::sample::common - Samples
#==============================================================================

#----------------------------------------------
# Khronos extension headers
#----------------------------------------------

if(WIN32)
    set(NFX_SILICON_3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/3rdParty")

    set(_khronos_headers
        "GL/glext.h|https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/main/api/GL/glext.h"
        "GL/wglext.h|https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/main/api/GL/wglext.h"
        "KHR/khrplatform.h|https://raw.githubusercontent.com/KhronosGroup/EGL-Registry/main/api/KHR/khrplatform.h"
    )

    foreach(_entry ${_khronos_headers})
        string(REPLACE "|" ";" _parts "${_entry}")
        list(GET _parts 0 _rel)
        list(GET _parts 1 _url)

        set(_dest "${NFX_SILICON_3RDPARTY_DIR}/${_rel}")
        if(NOT EXISTS "${_dest}")
            get_filename_component(_dest_dir "${_dest}" DIRECTORY)
            file(MAKE_DIRECTORY "${_dest_dir}")
            message(STATUS "[nfx] Downloading ${_rel} ...")
            file(DOWNLOAD "${_url}" "${_dest}" SHOW_PROGRESS STATUS _dl_status)
            list(GET _dl_status 0 _dl_code)
            if(NOT _dl_code EQUAL 0)
                list(GET _dl_status 1 _dl_err)
                message(FATAL_ERROR "[nfx] Failed to download ${_rel}: ${_dl_err}")
            endif()
        endif()
    endforeach()
endif()

#----------------------------------------------
# Platform sources
#----------------------------------------------

if(UNIX AND NOT APPLE)
    set(NFX_SILICON_SAMPLE_COMMON_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/App_linux.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GlRenderer.cpp)
elseif(WIN32)
    set(NFX_SILICON_SAMPLE_COMMON_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/App_win32.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GlRenderer.cpp)
else()
    message(STATUS "Samples: skipped (unsupported platform)")
    return()
endif()

add_library(nfx-silicon-sample-common SHARED
    ${NFX_SILICON_SAMPLE_COMMON_SOURCES}
)
add_library(nfx::silicon::sample::common ALIAS nfx-silicon-sample-common)

if(WIN32)
    set_target_properties(nfx-silicon-sample-common PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

target_include_directories(nfx-silicon-sample-common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(OpenGL REQUIRED)
target_link_libraries(nfx-silicon-sample-common
    PUBLIC
        nfx::silicon
        nfx::silicon::warnings
        OpenGL::GL
        $<$<PLATFORM_ID:Linux>:X11;GL;GLX>
        $<$<PLATFORM_ID:Windows>:user32;gdi32>
)

target_include_directories(nfx-silicon-sample-common
    PUBLIC
        $<$<PLATFORM_ID:Windows>:${CMAKE_SOURCE_DIR}/3rdParty>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
