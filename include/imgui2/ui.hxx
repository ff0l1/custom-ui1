#pragma once

namespace imgui2 {
namespace ui {

enum class Page {
    Aimbot,
    Esp,
    Settings
};

class window {
public:
    explicit window( const char* title );
    ~window( );

    window( const window& ) = delete;
    window& operator=( const window& ) = delete;

    explicit operator bool( ) const;

private:
    bool open_ = false;
};

Page page( );

void label( const char* text );
bool check( const char* text, bool& value );
bool check( const char* text, bool& value, int& key );
bool slider( const char* text, float& value, float minimum, float maximum, const char* format = "%.1f" );
bool combo( const char* text, int& index, const char* const* items, int count );
bool combo( const char* text, bool* picked, const char* const* items, int count );
bool bind( const char* text, int& key );
void paint_overlays( );

}
}
