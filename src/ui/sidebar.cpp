#include "sidebar.hpp"

#include "imgui2/motion.hpp"
#include "imgui2/resources.hpp"

#include "interact.hpp"
#include "state.hpp"

#include "Canvas.h"
#include "Context.h"
#include "Font.h"
#include "Input.h"
#include "Style.h"

namespace imgui2 {
namespace ui {

struct Tab {
    Page page;
    const char* id;
    unsigned code;
    bool overlay;
};

static const Tab Tabs[ ] = {
    { Page::Aimbot, "##nav.aimbot", resources::crosshair_codepoint( ), false },
    { Page::Esp, "##nav.esp", resources::eye_codepoint( ), false },
    { Page::Settings, "##nav.settings", resources::gear_codepoint( ), false }
};

static constexpr int TabCount = ( int )( sizeof( Tabs ) / sizeof( Tabs[ 0 ] ) );

static void WriteMark( const Tab& Item, const CRectangle& Box, const CColor& Ink ) {
    char Mark[ 8 ] = { };
    CFont* Face = Item.overlay && Marks->LineSpan > 0.0f ? Marks.get( ) : Icons.get( );
    resources::encode_utf8( Item.code, Mark, 8 );

    CVector Glyph = Face->Measure( Mark );
    if ( Glyph.Horizontal < 2.0f ) {
        Face = Icons.get( );
        resources::encode_utf8( Item.code, Mark, 8 );
        Glyph = Face->Measure( Mark );
    }

    Canvas->Write(
        Face,
        CVector(
            Box.Left + ( Box.Width - Glyph.Horizontal ) * 0.5f,
            Box.Top + ( Box.Height - Face->LineSpan ) * 0.5f
        ),
        Ink,
        Mark
    );
}

void draw_rail( const CRectangle& Panel ) {
    HostState& State = host( );

    CRectangle Rail(
        Panel.Left + Style->RailInset,
        Panel.Top + Style->RailInset,
        Style->RailWidth,
        Panel.Height - Style->RailInset * 2.0f
    );

    Canvas->Rectangle( Rail, Style->Header.Fade( 0.18f ), Style->ControlRounding );
    State.rail = Rail;

    float Slot = Style->PillSize;
    float Gap = 10.0f * Style->Scale;
    float Left = Rail.Left + ( Rail.Width - Slot ) * 0.5f;
    float Top = Rail.Top + 10.0f * Style->Scale;

    float TargetX = Left;
    float TargetY = Top;
    if ( State.tab < 0 || State.tab >= TabCount )
        State.tab = 0;

    for ( int Index = 0; Index < TabCount; Index++ ) {
        CRectangle HitBox( Left, Top + ( Slot + Gap ) * ( float )Index, Slot, Slot );
        if ( Index == State.tab ) {
            TargetX = HitBox.Left;
            TargetY = HitBox.Top;
        }

        bool Hovered = false;
        bool Held = false;
        if ( hit( Context->Hash( Tabs[ Index ].id ), HitBox, Hovered, Held ) ) {
            State.tab = Index;
            State.page = Tabs[ Index ].page;
            State.combo.open = false;
            TargetX = HitBox.Left;
            TargetY = HitBox.Top;
        }

        if ( Hovered )
            Input->Pointer = PointerHand;

        float Glow = motion::hover( Context->Hash( Tabs[ Index ].id ) ^ 0xA11u, Hovered && Index != State.tab, 22.0f );
        if ( Glow > 0.01f )
            Canvas->Rectangle( HitBox, Style->Hovered.Fade( 0.45f * Glow ), Style->ControlRounding );
    }

    if ( !State.pill_ready ) {
        State.pill_x = TargetX;
        State.pill_y = TargetY;
        State.pill_ready = true;
    } else {
        float Pace = 20.0f * Context->DeltaTime;
        if ( Pace > 1.0f )
            Pace = 1.0f;
        State.pill_x += ( TargetX - State.pill_x ) * Pace;
        State.pill_y += ( TargetY - State.pill_y ) * Pace;
    }

    CRectangle Pill( State.pill_x, State.pill_y, Slot, Slot );
    Canvas->Rectangle( Pill, Style->Accent.Fade( 0.24f ), Style->ControlRounding );

    for ( int Index = 0; Index < TabCount; Index++ ) {
        CRectangle SlotBox( Left, Top + ( Slot + Gap ) * ( float )Index, Slot, Slot );
        CRectangle Box = Index == State.tab ? Pill : SlotBox;
        bool Active = Index == State.tab;
        bool Hovered = Context->HoveredItem == Context->Hash( Tabs[ Index ].id );
        float Mix = motion::toward( Tabs[ Index ].id, Active ? 1.0f : ( Hovered ? 0.45f : 0.0f ), 20.0f );
        WriteMark( Tabs[ Index ], Box, Style->Faint.Blend( Style->AccentSoft, Mix ) );
    }
}

}
}
