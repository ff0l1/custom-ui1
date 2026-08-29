#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "imgui2/imgui2.hpp"

#include <Windows.h>

int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int ) {
    imgui2::app::Config Config;
    Config.title = "ImgU2";
    Config.vsync = true;
    Config.topmost = true;
    Config.atmosphere = 0;

    return imgui2::app::run( Config, [ ] {
        static bool Enabled = false;
        static bool Team = true;
        static bool Visible = true;
        static float Field = 8.0f;
        static float Smooth = 6.0f;
        static bool BonesOn[ 5 ] = { true, false, true, false, false };
        static int AimKey = VK_RBUTTON;

        static bool EspOn = false;
        static bool EspGlobal = true;
        static bool EspBoxes = true;
        static bool EspSkeletons = false;
        static bool EspHealth = true;
        static bool EspName = true;
        static bool EspTeam = true;

        static bool VSync = true;
        static bool LimitFps = false;
        static float Cap = 144.0f;
        static float Dpi = 1.0f;
        static int Theme = 0;
        static int Atmosphere = 0;

        static const char* Bones[ ] = { "Head", "Neck", "Chest", "Pelvis", "Closest" };
        static const char* Themes[ ] = { "Rose", "Mocha", "Forest" };

        int BackgroundCount = 0;
        const char* const* Backgrounds = imgui2::effects::background_names( BackgroundCount );

        imgui2::app::set_vsync( VSync );
        imgui2::app::set_ui_scale( Dpi );
        imgui2::app::set_fps_cap( LimitFps ? ( int )( Cap + 0.5f ) : 0 );
        imgui2::style::apply( Theme );
        imgui2::effects::set_background( Atmosphere );

        if ( imgui2::ui::window Window( "ImgU2" ); Window ) {
            if ( imgui2::ui::page( ) == imgui2::ui::Page::Aimbot ) {
                imgui2::ui::label( "Aimbot" );
                imgui2::ui::check( "Enable", Enabled, AimKey );
                if ( Enabled ) {
                    imgui2::ui::check( "Team check", Team );
                    imgui2::ui::check( "Visible check", Visible );
                    imgui2::ui::slider( "FOV", Field, 1.0f, 30.0f );
                    imgui2::ui::slider( "Smooth", Smooth, 1.0f, 20.0f );
                    imgui2::ui::combo( "Bones", BonesOn, Bones, 5 );
                }
            } else if ( imgui2::ui::page( ) == imgui2::ui::Page::Esp ) {
                imgui2::ui::label( "ESP" );
                imgui2::ui::check( "Enable", EspOn );
                if ( EspOn ) {
                    imgui2::ui::check( "Global", EspGlobal );
                    imgui2::ui::check( "Boxes", EspBoxes );
                    imgui2::ui::check( "Skeletons", EspSkeletons );
                    imgui2::ui::check( "Health", EspHealth );
                    imgui2::ui::check( "Name", EspName );
                    imgui2::ui::check( "Team check", EspTeam );
                }
            } else if ( imgui2::ui::page( ) == imgui2::ui::Page::Settings ) {
                imgui2::ui::label( "Settings" );
                imgui2::ui::check( "VSync", VSync );
                imgui2::ui::check( "FPS limiter", LimitFps );
                if ( LimitFps )
                    imgui2::ui::slider( "FPS cap", Cap, 30.0f, 360.0f, "%.0f" );
                imgui2::ui::slider( "Scale", Dpi, 0.85f, 1.25f, "%.2f" );
                imgui2::ui::combo( "Theme", Theme, Themes, 3 );
                if ( BackgroundCount > 0 )
                    imgui2::ui::combo( "Background", Atmosphere, Backgrounds, BackgroundCount );
            }
        }
    } );
}
