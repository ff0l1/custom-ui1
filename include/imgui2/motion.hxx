#pragma once

namespace imgui2 {
namespace motion {

float toward( unsigned int id, float target, float speed = 0.0f );
float toward( const char* id, float target, float speed = 0.0f );
float hover( unsigned int id, bool on, float speed = 0.0f );
void clear( );

}
}
