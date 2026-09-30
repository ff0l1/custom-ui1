#include "imgui2/effects.hxx"

#include "Canvas.hxx"
#include "Geometry.hxx"
#include "Shaders.hxx"
#include "Style.hxx"

namespace imgui2 {
namespace effects {

static int Background = 0;
static CGraphics* Gfx = nullptr;
static unsigned long long Plate = 0;
static unsigned int Effects[ 3 ] = { };
static bool Ready = false;

static const char* Names[ ] = {
    "Lightning",
    "Galaxy",
    "Plasma"
};

static const char* Bodies[ ] = {
    R"(
Float2 Uv = Local / max( Extent, Float2( 1.0, 1.0 ) );
float Y = Uv.y;
float X = Uv.x + 0.18 * sin( Y * 14.0 + Moment * 6.0 ) + 0.08 * sin( Y * 37.0 - Moment * 11.0 );
float Bolt = 0.012 / max( abs( X ), 0.0015 );
float Flash = pow( Saturate( sin( Moment * 9.0 ) ), 18.0 );
Final.rgb = Float3( 0.95, 0.55, 0.68 ) * Bolt * ( 0.25 + Flash ) + Float3( 0.09, 0.04, 0.06 );
)",
    R"(
Float2 Uv = Local / max( Extent.y, 1.0 );
float T = Moment * 0.04;
Float2 P = Uv;
float Arm = P.x * 3.1 + P.y * 2.2 + length( P ) * 2.4 - T * 1.6;
float Spiral = 0.5 + 0.5 * sin( Arm * 3.0 );
float Dust = exp( -length( P ) * 1.15 );
Float3 Nebula = Float3( 0.18, 0.08, 0.32 ) * Spiral * Dust;
Nebula += Float3( 0.06, 0.12, 0.28 ) * ( 1.0 - Spiral ) * Dust;
Float3 Color = Float3( 0.015, 0.018, 0.04 ) + Nebula;
for ( int Layer = 0; Layer < 5; Layer++ )
{
    float Depth = 0.55 + float( Layer ) * 0.28;
    Float2 Q = Uv * Depth + Float2( T * ( 0.08 + float( Layer ) * 0.03 ), T * 0.04 );
    Float2 Cell = floor( Q * 18.0 );
    Float2 Fr = Fract( Q * 18.0 ) - Float2( 0.5, 0.5 );
    float N = Fract( sin( dot( Cell, Float2( 127.1, 311.7 ) ) ) * 43758.5453 );
    float Core = 0.006 / max( length( Fr ), 0.0018 );
    float Star = Core * step( 0.965 - float( Layer ) * 0.012, N );
    float Twinkle = 0.45 + 0.55 * abs( sin( Moment * ( 1.2 + N * 4.0 ) + N * 20.0 ) );
    Color += Float3( 0.92, 0.94, 1.0 ) * Star * Twinkle;
}
float Glow = exp( -length( P ) * 2.4 ) * ( 0.18 + 0.08 * sin( Moment * 0.7 ) );
Color += Float3( 0.35, 0.22, 0.55 ) * Glow;
Final.rgb = Saturate( Color );
)",
    R"(
Float2 Uv = Local / max( Extent, Float2( 1.0, 1.0 ) );
float T = Moment * 0.45;
float A = sin( Uv.x * 7.0 + T ) + sin( Uv.y * 6.2 - T * 1.15 );
float B = sin( ( Uv.x + Uv.y ) * 4.4 + T * 0.8 );
float Wave = A + B;
Float3 Tone = Float3( 0.16 + 0.28 * sin( Wave ), 0.14 + 0.22 * sin( Wave + 1.9 ), 0.32 + 0.30 * sin( Wave + 3.4 ) );
Final.rgb = Tone;
)"
};

void bind( CGraphics* Graphics ) {
    if ( Gfx && Plate ) {
        Gfx->DestroyImage( Plate );
        Plate = 0;
    }

    Gfx = Graphics;
    unsigned char White[ 16 ] = {
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255
    };

    if ( Gfx )
        Plate = Gfx->CreateImage( White, 2, 2 );
}

void sweep( ) {
    if ( Gfx && Plate )
        Gfx->DestroyImage( Plate );

    Plate = 0;
    Gfx = nullptr;
}

void compose( ) {
    if ( Ready )
        return;

    for ( int Index = 0; Index < 3; Index++ )
        Effects[ Index ] = Shaders->Compose( Names[ Index ], Bodies[ Index ] );

    Ready = true;
}

void set_background( int Index ) {
    if ( Index < 0 )
        Index = 0;

    if ( Index > 2 )
        Index = 2;

    Background = Index;
}

int background( ) {
    return Background;
}

const char* const* background_names( int& Count ) {
    Count = 3;
    return Names;
}

void draw( float Left, float Top, float Width, float Height, float Rounding ) {
    if ( !Plate )
        return;

    compose( );

    unsigned int Former = Canvas->Effect( Effects[ Background ] );
    Canvas->Image( CRectangle( Left, Top, Width, Height ), Plate, CRectangle( 0.0f, 0.0f, 1.0f, 1.0f ), Style->AccentSoft, Rounding );
    Canvas->Effect( Former );
}

void draw_screen( float Width, float Height ) {
    draw( 0.0f, 0.0f, Width, Height, 0.0f );
}

}
}
