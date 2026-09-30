#include "Style.hxx"

void CStyle::Glass( ) {
    Preset = 0;
    Backdrop = CColor( 0, 0, 0, 0 );

    Surface = CColor( 22, 22, 24, 214 );
    Elevated = CColor( 40, 40, 43, 230 );
    Header = CColor( 36, 36, 38, 226 );

    Outline = CColor( 255, 255, 255, 30 );
    Highlight = CColor( 255, 255, 255, 46 );

    Text = CColor( 244, 244, 247 );
    Faint = CColor( 154, 154, 160 );

    Accent = CColor( 10, 132, 255 );
    AccentSoft = CColor( 90, 168, 255 );

    Control = CColor( 44, 44, 48, 180 );
    Selected = CColor( 52, 52, 58, 200 );

    Hovered = CColor( 255, 255, 255, 22 );
    Pressed = CColor( 255, 255, 255, 14 );

    Shade = CColor( 0, 0, 0, 88 );
    Focus = CColor( 10, 132, 255 );

    Danger = CColor( 255, 69, 58 );
    Success = CColor( 48, 209, 88 );
    Warning = CColor( 255, 214, 10 );

    Grain = CColor( 255, 255, 255, 16 );

    Apply( );
}

void CStyle::Rose( ) {
    Preset = 5;

    Backdrop = CColor( 0, 0, 0, 0 );
    Surface = CColor( 34, 22, 28 );

    Elevated = CColor( 48, 30, 38 );
    Header = CColor( 40, 24, 32 );

    Outline = CColor( 244, 180, 196, 32 );
    Highlight = CColor( 255, 214, 224, 16 );

    Text = CColor( 250, 236, 240 );
    Faint = CColor( 196, 156, 168 );

    Accent = CColor( 235, 111, 146 );
    AccentSoft = CColor( 246, 160, 180 );

    Control = CColor( 48, 30, 38 );
    Selected = CColor( 72, 40, 52 );

    Hovered = CColor( 64, 38, 48 );
    Pressed = CColor( 28, 16, 22 );

    Shade = CColor( 16, 8, 12, 120 );
    Focus = CColor( 235, 111, 146 );

    Danger = CColor( 244, 92, 110 );
    Success = CColor( 134, 239, 172 );
    Warning = CColor( 251, 191, 36 );

    Grain = CColor( 255, 214, 224, 18 );

    Apply( );
}

void CStyle::Mocha( ) {
    Preset = 2;

    Backdrop = CColor( 0, 0, 0, 0 );
    Surface = CColor( 30, 30, 46 );
    Elevated = CColor( 49, 50, 68 );
    Header = CColor( 24, 24, 37 );

    Outline = CColor( 108, 112, 134, 48 );
    Highlight = CColor( 205, 214, 244, 18 );

    Text = CColor( 205, 214, 244 );
    Faint = CColor( 166, 173, 200 );

    Accent = CColor( 203, 166, 247 );
    AccentSoft = CColor( 180, 190, 254 );

    Control = CColor( 49, 50, 68 );
    Selected = CColor( 69, 71, 90 );

    Hovered = CColor( 69, 71, 90 );
    Pressed = CColor( 24, 24, 37 );

    Shade = CColor( 17, 17, 27, 160 );
    Focus = CColor( 203, 166, 247 );

    Danger = CColor( 243, 139, 168 );
    Success = CColor( 166, 227, 161 );
    Warning = CColor( 249, 226, 175 );

    Grain = CColor( 205, 214, 244, 16 );

    Apply( );
}

void CStyle::Forest( ) {
    Preset = 9;

    Backdrop = CColor( 0, 0, 0, 0 );
    Surface = CColor( 61, 72, 77 );
    Elevated = CColor( 79, 93, 99 );
    Header = CColor( 52, 63, 68 );

    Outline = CColor( 133, 146, 137, 44 );
    Highlight = CColor( 211, 198, 170, 14 );

    Text = CColor( 211, 198, 170 );
    Faint = CColor( 133, 146, 137 );

    Accent = CColor( 167, 192, 128 );
    AccentSoft = CColor( 127, 187, 179 );

    Control = CColor( 61, 72, 77 );
    Selected = CColor( 79, 93, 99 );

    Hovered = CColor( 79, 93, 99 );
    Pressed = CColor( 45, 53, 59 );

    Shade = CColor( 23, 28, 31, 160 );
    Focus = CColor( 167, 192, 128 );

    Danger = CColor( 230, 126, 128 );
    Success = CColor( 135, 169, 107 );
    Warning = CColor( 230, 180, 80 );

    Grain = CColor( 211, 198, 170, 16 );

    Apply( );
}

void CStyle::Apply( ) {
    Rounding = 18.0f * Scale;
    ControlRounding = 10.0f * Scale;

    PaddingWide = 24.0f * Scale;
    PaddingTall = 20.0f * Scale;

    Spacing = 12.0f * Scale;
    Thickness = 1.0f * Scale;

    Softness = 36.0f * Scale;
    TitleHeight = 52.0f * Scale;

    ControlHeight = 32.0f * Scale;
    FadeSpeed = 18.0f;

    CheckSize = 18.0f * Scale;
    GrooveWidth = 4.0f * Scale;
    KnobSize = 6.0f * Scale;
    IconStroke = 1.6f * Scale;

    PanelWidth = 540.0f * Scale;
    PanelHeight = 428.0f * Scale;

    CloseSize = 28.0f * Scale;
    CloseInset = 14.0f * Scale;

    RailWidth = 56.0f * Scale;
    RailInset = 12.0f * Scale;
    PillSize = 40.0f * Scale;
}

void CStyle::Rescale( float Factor ) {
    if ( Factor <= 0.0f )
        Factor = 1.0f;

    Scale = Factor;

    if ( Preset == 9 )
        Forest( );
    else if ( Preset == 2 )
        Mocha( );
    else if ( Preset == 5 )
        Rose( );
    else
        Glass( );
}
