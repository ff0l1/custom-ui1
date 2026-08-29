#pragma once

namespace imgui2 {
namespace overlay {

struct Options {
    const char* title = "custom-ui-1";
    bool topmost = true;
};

bool create( const Options& options );
void destroy( );

void* native( );
int width( );
int height( );

void pump( );
void poll_pointer( );

void set_interactive( bool capture );
void set_hit_bounds( float left, float top, float width, float height );

bool quit_requested( );
void request_quit( );

bool consume_resize( int& width, int& height );

}
}
