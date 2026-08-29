#pragma once

#include <functional>

#include "imgui2/overlay.hpp"

namespace imgui2 {
namespace app {

struct Config {
    const char* title = "ImgU2";
    bool vsync = true;
    bool topmost = true;
    float font_size = 15.0f;
    int atmosphere = 0;
};

int run( const Config& config, std::function< void( ) > tick );

void quit( );
int width( );
int height( );

void set_vsync( bool enabled );
void set_fps_cap( int frames );
void set_ui_scale( float scale );

const Config& config( );

}
}
