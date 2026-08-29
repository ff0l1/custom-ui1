#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "imgui2/app.hpp"
#include "imgui2/effects.hpp"
#include "imgui2/resources.hpp"

#include "ElevenHost.h"
#include "Engine.h"

#include "state.hpp"

#include <Windows.h>
#include <timeapi.h>

namespace imgui2 {
namespace app {

static Config Active;
static CGraphics* Gfx = nullptr;
static float LastScale = 1.0f;
static float UserScale = 1.0f;
static int FpsCap = 0;

static void SyncScale( ) {
    float Scale = Native->Scale( ) * UserScale;
    if ( Scale < 0.75f )
        Scale = 0.75f;
    if ( Scale > 2.75f )
        Scale = 2.75f;

    float Delta = Scale - LastScale;
    if ( Delta < 0.0f )
        Delta = -Delta;
    if ( Delta < 0.008f )
        return;

    LastScale = Scale;
    Engine->Rescale( Scale );
}

static void TickFrame( const std::function< void( ) >& User ) {
    int Across = 0;
    int Down = 0;

    if ( overlay::consume_resize( Across, Down ) )
        ElevenHost->Resize( Across, Down );

    SyncScale( );

    overlay::poll_pointer( );

    Engine->Begin( CVector( ( float )overlay::width( ), ( float )overlay::height( ) ) );
    User( );

    ui::HostState& State = ui::host( );
    if ( State.panel.Width > 0.0f && State.panel.Height > 0.0f ) {
        float Left = State.panel.Left;
        float Top = State.panel.Top;
        float Right = State.panel.Left + State.panel.Width;
        float Bottom = State.panel.Top + State.panel.Height;

        if ( State.combo.open && State.combo.list.Width > 0.0f ) {
            if ( State.combo.list.Left < Left )
                Left = State.combo.list.Left;
            if ( State.combo.list.Top < Top )
                Top = State.combo.list.Top;
            if ( State.combo.list.Right( ) > Right )
                Right = State.combo.list.Right( );
            if ( State.combo.list.Bottom( ) > Bottom )
                Bottom = State.combo.list.Bottom( );
        }

        overlay::set_hit_bounds( Left, Top, Right - Left, Bottom - Top );
    }

    Engine->End( );

    ElevenHost->Begin( Style->Backdrop );
    ElevenHost->Graphics( )->Render( Engine->Data( ), ElevenHost->Stream( ) );
    ElevenHost->End( Active.vsync );

    if ( FpsCap > 0 ) {
        static LARGE_INTEGER Frequency = { };
        static LARGE_INTEGER Previous = { };
        if ( Frequency.QuadPart == 0 )
            QueryPerformanceFrequency( &Frequency );

        LARGE_INTEGER Now = { };
        QueryPerformanceCounter( &Now );

        if ( Previous.QuadPart != 0 ) {
            double Elapsed = ( double )( Now.QuadPart - Previous.QuadPart ) / ( double )Frequency.QuadPart;
            double Budget = 1.0 / ( double )FpsCap;
            if ( Elapsed < Budget ) {
                DWORD Wait = ( DWORD )( ( Budget - Elapsed ) * 1000.0 );
                if ( Wait > 0 )
                    Sleep( Wait );
            }
        }

        QueryPerformanceCounter( &Previous );
    }
}

void quit( ) {
    overlay::request_quit( );
}

int width( ) {
    return overlay::width( );
}

int height( ) {
    return overlay::height( );
}

void set_vsync( bool Enabled ) {
    Active.vsync = Enabled;
}

void set_fps_cap( int Frames ) {
    FpsCap = Frames > 0 ? Frames : 0;
}

void set_ui_scale( float Scale ) {
    if ( Scale < 0.75f )
        Scale = 0.75f;
    if ( Scale > 1.50f )
        Scale = 1.50f;
    UserScale = Scale;
}

const Config& config( ) {
    return Active;
}

int run( const Config& Wanted, std::function< void( ) > User ) {
    Active = Wanted;
    LastScale = 1.0f;
    UserScale = 1.0f;
    FpsCap = 0;
    Gfx = nullptr;

    timeBeginPeriod( 1 );

    overlay::Options Overlay;
    Overlay.title = Active.title;
    Overlay.topmost = Active.topmost;

    if ( !overlay::create( Overlay ) ) {
        timeEndPeriod( 1 );
        return 1;
    }

    if ( !resources::install_fonts( ) ) {
        overlay::destroy( );
        timeEndPeriod( 1 );
        return 1;
    }

    if ( !Engine->Create( Active.font_size ) ) {
        resources::remove_fonts( );
        overlay::destroy( );
        timeEndPeriod( 1 );
        return 1;
    }

    LastScale = Native->Scale( );
    Engine->Rescale( LastScale );

    if ( !ElevenHost->Create( overlay::native( ), overlay::width( ), overlay::height( ) ) ) {
        Engine->Destroy( );
        resources::remove_fonts( );
        overlay::destroy( );
        timeEndPeriod( 1 );
        return 1;
    }

    Gfx = ElevenHost->Graphics( );
    effects::bind( Gfx );
    effects::compose( );
    effects::set_background( Active.atmosphere );

    while ( !overlay::quit_requested( ) ) {
        overlay::pump( );
        if ( overlay::quit_requested( ) )
            break;

        TickFrame( User );
    }

    effects::sweep( );
    Gfx = nullptr;

    ElevenHost->Destroy( );
    Engine->Destroy( );
    resources::remove_fonts( );
    overlay::destroy( );

    timeEndPeriod( 1 );
    return 0;
}

}
}
