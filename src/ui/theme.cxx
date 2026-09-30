#include "imgui2/style.hxx"

#include "Style.hxx"

namespace imgui2 {
namespace style {

void apply_glass( ) {
    Style->Glass( );
}

void apply_rose( ) {
    Style->Rose( );
}

void apply_mocha( ) {
    Style->Mocha( );
}

void apply_forest( ) {
    Style->Forest( );
}

void apply( int Index ) {
    if ( Index == 1 )
        Style->Mocha( );
    else if ( Index == 2 )
        Style->Forest( );
    else
        Style->Rose( );
}

}
}
