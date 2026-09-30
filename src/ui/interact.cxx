#include "interact.hxx"

#include "Canvas.hxx"
#include "Context.hxx"
#include "Input.hxx"

namespace imgui2 {
namespace ui {

bool hit( unsigned int Identifier, CRectangle Bounds, bool& Hovered, bool& Held ) {
    CVector Point = Input->MousePosition;

    Hovered = Bounds.Contains( Point ) && Canvas->Visible( Point ) && !Context->Covered( Point );
    if ( Hovered )
        Context->HoveredItem = Identifier;

    if ( Hovered && Input->MousePressed( 0 ) && Context->ActiveItem == 0 )
        Context->ActiveItem = Identifier;

    Held = Context->ActiveItem == Identifier;
    bool Clicked = Held && Hovered && Input->MouseReleased( 0 );

    if ( Held && Input->MouseReleased( 0 ) )
        Context->ActiveItem = 0;

    return Clicked;
}

}
}
