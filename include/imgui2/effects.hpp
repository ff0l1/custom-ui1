#pragma once

class CGraphics;

namespace imgui2 {
namespace effects {

void bind( CGraphics* graphics );
void sweep( );

void compose( );
void set_background( int index );
int background( );
const char* const* background_names( int& count );

void draw( float left, float top, float width, float height, float rounding );
void draw_screen( float width, float height );

}
}
