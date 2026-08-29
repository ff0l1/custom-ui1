cmake_minimum_required( VERSION 3.20 )

project( custom-ui-1 LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

if( NOT WIN32 )
    message( FATAL_ERROR "custom-ui-1 builds on Windows." )
endif( )

set( Engine ${CMAKE_CURRENT_SOURCE_DIR}/src/engine )

set( Imgui2Sources
    ${Engine}/Core/Arena/Arena.cpp
    ${Engine}/Core/Context/Context.cpp
    ${Engine}/Core/Input/Input.cpp
    ${Engine}/Text/Font/Font.cpp
    ${Engine}/Text/Format/Format.cpp
    ${Engine}/Text/Sheet/Sheet.cpp
    ${Engine}/Interface/Style/Style.cpp
    ${Engine}/Ports/Canvas/Canvas.cpp
    ${Engine}/Ports/Shaders/Shaders.cpp
    ${Engine}/Ports/Platform/Native/Native.cpp
    ${Engine}/Ports/Graphics/DirectX11/DirectX11.cpp
    src/host/ElevenHost.cpp
    src/app/engine.cpp
    src/app/app.cpp
    src/overlay/overlay.cpp
    src/resources/assets.cpp
    src/resources/fonts.cpp
    src/effects/effects.cpp
    src/ui/interact.cpp
    src/ui/motion.cpp
    src/ui/state.cpp
    src/ui/theme.cpp
    src/ui/sidebar.cpp
    src/ui/controls.cpp
    src/ui/window.cpp
)

add_library( imgui2 STATIC ${Imgui2Sources} )
add_library( imgui2::imgui2 ALIAS imgui2 )

set( Imgui2Includes
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/host
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${Engine}
    ${Engine}/Core
    ${Engine}/Core/Arena
    ${Engine}/Core/Context
    ${Engine}/Core/Input
    ${Engine}/Text/Font
    ${Engine}/Text/Format
    ${Engine}/Text/Sheet
    ${Engine}/Interface/Style
    ${Engine}/Ports/Canvas
    ${Engine}/Ports/Shaders
    ${Engine}/Ports/Platform/Native
    ${Engine}/Ports/Graphics/DirectX11
)

target_include_directories( imgui2 PUBLIC ${Imgui2Includes} )

if( MSVC )
    target_compile_options( imgui2 PRIVATE /W4 /utf-8 /EHsc )
endif( )

target_link_libraries( imgui2 PUBLIC
    user32
    gdi32
    dwmapi
    d3d11
    d3dcompiler
    dxgi
    winmm
)

function( imgui2_add_app Target )
    add_executable( ${Target} WIN32 ${ARGN} )
    target_link_libraries( ${Target} PRIVATE imgui2 )
    if( MSVC )
        target_compile_options( ${Target} PRIVATE /W4 /utf-8 /EHsc )
    endif( )
    set_target_properties( ${Target} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_custom_command( TARGET ${Target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${Target}>/assets
    )
endfunction( )

option( IMGUI2_BUILD_PREVIEW "Build the custom-ui-1 preview overlay" ON )

if( IMGUI2_BUILD_PREVIEW )
    imgui2_add_app( imgui2_preview demos/preview/Main.cpp )
    set_target_properties( imgui2_preview PROPERTIES OUTPUT_NAME custom-ui-1 )
endif( )
