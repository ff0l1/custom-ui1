#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "imgui2/resources.hxx"

#include <Windows.h>
#include <vector>
#include <string>

namespace imgui2 {
namespace resources {

static std::vector< std::wstring > PrivateFonts;
static bool HaveInter = false;

static bool AppendPath( std::wstring& Out, const char* Utf8 ) {
    if ( !Utf8 )
        return false;

    int Count = MultiByteToWideChar( CP_UTF8, 0, Utf8, -1, nullptr, 0 );
    if ( Count <= 1 )
        return false;

    Out.assign( ( size_t )Count, 0 );
    MultiByteToWideChar( CP_UTF8, 0, Utf8, -1, Out.data( ), Count );
    if ( !Out.empty( ) && Out.back( ) == 0 )
        Out.pop_back( );

    return !Out.empty( );
}

static bool AddPrivate( const char* Root, const char* Relative ) {
    char Combined[ MAX_PATH ];
    int Written = 0;

    for ( ; Root && Root[ Written ] && Written + 1 < MAX_PATH; Written++ )
        Combined[ Written ] = Root[ Written ];

    if ( Written > 0 && Combined[ Written - 1 ] != '\\' && Combined[ Written - 1 ] != '/' && Written + 1 < MAX_PATH )
        Combined[ Written++ ] = '\\';

    for ( int Index = 0; Relative && Relative[ Index ] && Written + 1 < MAX_PATH; Index++, Written++ )
        Combined[ Written ] = Relative[ Index ];

    Combined[ Written ] = 0;

    std::wstring Wide;
    if ( !AppendPath( Wide, Combined ) )
        return false;

    if ( AddFontResourceExW( Wide.c_str( ), FR_PRIVATE, nullptr ) <= 0 )
        return false;

    PrivateFonts.push_back( Wide );
    return true;
}

bool install_fonts( ) {
    remove_fonts( );

    char Root[ MAX_PATH ];
    if ( !locate_root( Root, MAX_PATH ) )
        return false;

    HaveInter = AddPrivate( Root, "fonts\\Inter-Regular.ttf" )
        && AddPrivate( Root, "fonts\\Inter-SemiBold.ttf" );

    bool Body = AddPrivate( Root, "fonts\\Montserrat-Regular.ttf" );
    bool Medium = AddPrivate( Root, "fonts\\Montserrat-Medium.ttf" );
    bool Title = AddPrivate( Root, "fonts\\LeagueSpartan-SemiBold.ttf" );
    bool Icon = AddPrivate( Root, "fonts\\FontAwesome7Free-Solid-900.otf" );
    bool Overlay = AddPrivate( Root, "fonts\\CustomIconPackDOPAMINA.ttf" );

    return HaveInter && Body && Medium && Title && Icon && Overlay;
}

bool have_inter( ) {
    return HaveInter;
}

void remove_fonts( ) {
    for ( const std::wstring& Path : PrivateFonts )
        RemoveFontResourceExW( Path.c_str( ), FR_PRIVATE, nullptr );

    PrivateFonts.clear( );
    HaveInter = false;
}

unsigned close_codepoint( ) {
    return 0xF00D;
}

unsigned crosshair_codepoint( ) {
    return 0xF05B;
}

unsigned eye_codepoint( ) {
    return 0xF06E;
}

unsigned gear_codepoint( ) {
    return 0xF013;
}

unsigned chevron_codepoint( ) {
    return 0xF078;
}

void encode_utf8( unsigned Codepoint, char* Out, int OutSize ) {
    if ( !Out || OutSize <= 0 )
        return;

    Out[ 0 ] = 0;

    wchar_t Wide[ 3 ] = { };
    int Units = 0;

    if ( Codepoint <= 0xFFFF ) {
        Wide[ 0 ] = ( wchar_t )Codepoint;
        Units = 1;
    } else if ( Codepoint <= 0x10FFFF ) {
        unsigned Adjusted = Codepoint - 0x10000;
        Wide[ 0 ] = ( wchar_t )( 0xD800 + ( Adjusted >> 10 ) );
        Wide[ 1 ] = ( wchar_t )( 0xDC00 + ( Adjusted & 0x3FF ) );
        Units = 2;
    } else {
        return;
    }

    WideCharToMultiByte( CP_UTF8, 0, Wide, Units, Out, OutSize, nullptr, nullptr );
    if ( OutSize > 0 )
        Out[ OutSize - 1 ] = 0;
}

}
}
