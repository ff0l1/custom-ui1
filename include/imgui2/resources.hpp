#pragma once

namespace imgui2 {
namespace resources {

bool locate_root( char* path, int path_size );
bool install_fonts( );
void remove_fonts( );
bool have_inter( );

unsigned close_codepoint( );
unsigned crosshair_codepoint( );
unsigned eye_codepoint( );
unsigned gear_codepoint( );
unsigned chevron_codepoint( );
void encode_utf8( unsigned codepoint, char* out, int out_size );

}
}
