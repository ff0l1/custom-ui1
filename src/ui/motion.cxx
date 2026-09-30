#include "imgui2/motion.hxx"

#include "Context.hxx"
#include "Style.hxx"

#include <unordered_map>

namespace imgui2 {
namespace motion {

static std::unordered_map< unsigned int, float > Values;

float toward( unsigned int Id, float Target, float Speed ) {
    float& Current = Values[ Id ];
    float Rate = Speed > 0.0f ? Speed : Style->FadeSpeed;
    float Step = Rate * Context->DeltaTime;
    if ( Step > 1.0f )
        Step = 1.0f;

    Current += ( Target - Current ) * Step;
    return Current;
}

float toward( const char* Id, float Target, float Speed ) {
    return toward( Context->Hash( Id ? Id : "" ), Target, Speed );
}

float hover( unsigned int Id, bool On, float Speed ) {
    return toward( Id, On ? 1.0f : 0.0f, Speed );
}

void clear( ) {
    Values.clear( );
}

}
}
