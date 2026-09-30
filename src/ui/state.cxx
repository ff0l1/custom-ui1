#include "state.hxx"

#include "Style.hxx"

namespace imgui2 {
namespace ui {

HostState& host( ) {
    static HostState State;
    return State;
}

void gap( Slot Kind ) {
    HostState& State = host( );
    if ( !State.open )
        return;

    if ( State.last != Slot::None && State.last != Kind )
        State.pen.Vertical += Style->Spacing * 0.75f;

    State.last = Kind;
}

}
}
