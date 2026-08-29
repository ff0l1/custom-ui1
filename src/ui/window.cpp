#include "imgui2/app.hpp"
#include "imgui2/effects.hpp"
#include "imgui2/motion.hpp"
#include "imgui2/resources.hpp"
#include "imgui2/ui.hpp"

#include "interact.hpp"
#include "sidebar.hpp"
#include "state.hpp"

#include "Canvas.h"
#include "Context.h"
#include "Font.h"
#include "Input.h"
#include "Style.h"

namespace imgui2 {
namespace ui {

static void PlaceIfNeeded( HostState& State ) {
    if ( State.placed )
        return;

    State.origin = CVector(
        ( Context->Display.Horizontal - Style->PanelWidth ) * 0.5f,
        ( Context->Display.Vertical - Style->PanelHeight ) * 0.5f
    );
    State.placed = true;
}

static void KeepOnScreen( HostState& State ) {
    float Margin = 24.0f * Style->Scale;
    float MostLeft = Context->Display.Horizontal - Style->PanelWidth - Margin;
    float MostTop = Context->Display.Vertical - Style->PanelHeight - Margin;

    if ( MostLeft < Margin )
        MostLeft = Margin;

    if ( MostTop < Margin )
        MostTop = Margin;

    if ( State.origin.Horizontal < Margin )
        State.origin.Horizontal = Margin;

    if ( State.origin.Vertical < Margin )
        State.origin.Vertical = Margin;

    if ( State.origin.Horizontal > MostLeft )
        State.origin.Horizontal = MostLeft;

    if ( State.origin.Vertical > MostTop )
        State.origin.Vertical = MostTop;
}

window::window( const char* Title ) {
    HostState& State = host( );
    PlaceIfNeeded( State );
    KeepOnScreen( State );

    State.reveal = motion::toward( "##imgui2.reveal", 1.0f, 16.0f );
    float Lift = ( 1.0f - State.reveal ) * 8.0f * Style->Scale;
    float WasOpacity = Canvas->Opacity;
    Canvas->Opacity = State.reveal;

    CRectangle Bounds( CVector( State.origin.Horizontal, State.origin.Vertical + Lift ), CVector( Style->PanelWidth, Style->PanelHeight ) );

    if ( State.combo.open && State.combo.list.Width > 0.0f && State.combo.list.Height > 0.0f ) {
        Context->Shielding = true;
        Context->Shield = State.combo.list;
        Context->ShieldStamp = Context->FrameIndex;
    }

    CRectangle Close(
        Bounds.Right( ) - Style->CloseInset - Style->CloseSize,
        Bounds.Top + ( Style->TitleHeight - Style->CloseSize ) * 0.5f,
        Style->CloseSize,
        Style->CloseSize
    );

    unsigned int WindowId = Context->Hash( Title ? Title : "ImgU2##window" );
    unsigned int CloseId = Context->Hash( "##imgui2.close" );
    unsigned int DragId = Context->Hash( "##imgui2.drag" );

    bool CloseHovered = false;
    bool CloseHeld = false;
    bool CloseClicked = hit( CloseId, Close, CloseHovered, CloseHeld );

    float WellLeft = Bounds.Left + Style->RailInset + Style->RailWidth + 16.0f * Style->Scale;
    CRectangle DragRegion( WellLeft, Bounds.Top, Close.Left - WellLeft, Style->TitleHeight );
    bool DragHovered = false;
    bool DragHeld = false;
    hit( DragId, DragRegion, DragHovered, DragHeld );

    if ( DragHeld ) {
        if ( !State.dragging ) {
            State.dragging = true;
            State.grab = Input->MousePosition - State.origin;
        }

        State.origin = Input->MousePosition - State.grab;
        KeepOnScreen( State );
        Bounds = CRectangle( CVector( State.origin.Horizontal, State.origin.Vertical + Lift ), CVector( Style->PanelWidth, Style->PanelHeight ) );
        Close.Left = Bounds.Right( ) - Style->CloseInset - Style->CloseSize;
        Close.Top = Bounds.Top + ( Style->TitleHeight - Style->CloseSize ) * 0.5f;
        WellLeft = Bounds.Left + Style->RailInset + Style->RailWidth + 16.0f * Style->Scale;
    } else {
        State.dragging = false;
    }

    if ( CloseHovered || DragHovered )
        Input->Pointer = CloseHovered ? PointerHand : PointerMove;

    State.close_glow = motion::hover( CloseId, CloseHovered, 28.0f );

    if ( Style->Shadows )
        Canvas->Shadow( Bounds, Style->Shade, Style->Rounding, Style->Softness );

    Canvas->Gradient( Bounds, Style->Header.Fade( 0.52f ), Style->Surface.Fade( 0.38f ), Style->Rounding, false );

    float Foot = Style->RailInset;
    CRectangle Well(
        WellLeft,
        Bounds.Top + Style->TitleHeight,
        Bounds.Right( ) - Style->PaddingWide - WellLeft,
        Bounds.Height - Style->TitleHeight - Foot
    );

    if ( Well.Width < 0.0f )
        Well.Width = 0.0f;
    if ( Well.Height < 0.0f )
        Well.Height = 0.0f;

    Canvas->Gradient( Well, Style->Header.Fade( 0.22f ), Style->Surface.Fade( 0.08f ), Style->ControlRounding, false );
    effects::draw( Bounds.Left, Bounds.Top, Bounds.Width, Bounds.Height, Style->Rounding );
    Canvas->Gradient( Bounds, Style->Header.Fade( 0.16f ), Style->Surface.Fade( 0.06f ), Style->Rounding, false );
    draw_rail( Bounds );
    State.well = Well;

    if ( Style->Borders )
        Canvas->Border( Bounds, Style->Outline, Style->Rounding, Style->Thickness );

    char Mark[ 8 ] = { };
    resources::encode_utf8( resources::close_codepoint( ), Mark, 8 );

    CVector Glyph = Icons->Measure( Mark );
    CColor Ink = Style->Text.Blend( Style->AccentSoft, State.close_glow );
    Canvas->Write(
        Icons.get( ),
        CVector(
            Close.Left + ( Close.Width - Glyph.Horizontal ) * 0.5f,
            Close.Top + ( Close.Height - Icons->LineSpan ) * 0.5f
        ),
        Ink,
        Mark
    );

    const char* Shown = Title ? Title : "ImgU2";
    float TitleLeft = WellLeft;
    float TitleTop = Bounds.Top + ( Style->TitleHeight - Heading->LineSpan - Heading->Leading ) * 0.5f + 1.0f * Style->Scale;
    Canvas->Write( Heading.get( ), CVector( TitleLeft, TitleTop ), Style->Text, Shown );

    if ( CloseClicked ) {
        Canvas->Opacity = WasOpacity;
        app::quit( );
        State.open = false;
        open_ = false;
        return;
    }

    State.panel = Bounds;
    State.content = CRectangle(
        Well.Left + 14.0f * Style->Scale,
        Well.Top + 14.0f * Style->Scale,
        Well.Width - 28.0f * Style->Scale,
        Well.Height - 28.0f * Style->Scale
    );
    State.pen = State.content.Origin( );
    State.last = Slot::None;
    State.open = true;
    open_ = true;

    Canvas->PushClip( State.content );
    Context->PushOwner( WindowId );
}

window::~window( ) {
    if ( !open_ )
        return;

    Canvas->PopClip( );
    Context->PopOwner( );
    paint_overlays( );
    Canvas->Opacity = 1.0f;
    host( ).open = false;
}

window::operator bool( ) const {
    return open_;
}

}
}
