#pragma once

#include <memory>

#include "Geometry.hxx"

class CStyle {
public:
    void Glass( );
    void Rose( );
    void Mocha( );
    void Forest( );
    void Rescale( float Factor );

    int Theme( ) const {
        return Preset;
    }

    CColor Backdrop;
    CColor Surface;

    CColor Elevated;
    CColor Header;

    CColor Outline;
    CColor Highlight;

    CColor Text;
    CColor Faint;

    CColor Accent;
    CColor AccentSoft;

    CColor Control;
    CColor Selected;

    CColor Hovered;
    CColor Pressed;

    CColor Shade;
    CColor Focus;

    CColor Danger;
    CColor Success;
    CColor Warning;

    CColor Grain;

    float Rounding = 18.0f;
    float ControlRounding = 10.0f;

    float PaddingWide = 24.0f;
    float PaddingTall = 20.0f;

    float Spacing = 12.0f;
    float Thickness = 1.0f;

    float Softness = 36.0f;
    float TitleHeight = 52.0f;

    float ControlHeight = 32.0f;
    float FadeSpeed = 18.0f;

    float CheckSize = 18.0f;
    float GrooveWidth = 4.0f;
    float KnobSize = 6.0f;
    float IconStroke = 1.6f;

    float Scale = 1.0f;

    float PanelWidth = 540.0f;
    float PanelHeight = 360.0f;

    float CloseSize = 28.0f;
    float CloseInset = 14.0f;

    float RailWidth = 56.0f;
    float RailInset = 12.0f;
    float PillSize = 40.0f;

    bool Shadows = true;
    bool Borders = true;

private:
    void Apply( );
    int Preset = 0;
};

inline auto Style = std::make_unique< CStyle >( );
