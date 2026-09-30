#include "Engine.hxx"

#include "imgui2/resources.hxx"

bool CEngine::Create( float BodySize ) {
    Style->Rose( );

    if ( !Sheet->Create( ) )
        return false;

    float Body = BodySize > 0.0f ? BodySize : 15.0f;
    const char* InterBody[ 3 ] = { "Inter", "Montserrat Medium", "Montserrat" };
    const char* InterTitle[ 3 ] = { "Inter SemiBold", "Inter", "League Spartan SemiBold" };
    const char* StaticBody[ 2 ] = { "Montserrat Medium", "Montserrat" };
    const char* StaticTitle[ 2 ] = { "League Spartan SemiBold", "Montserrat Medium" };
    const char* IconFaces[ 2 ] = { "Font Awesome 7 Free Solid", "Font Awesome 7 Free" };
    const char* MarkFaces[ 2 ] = { "Untitled", "Untitled Regular" };

    const bool Inter = imgui2::resources::have_inter( );
    const char* const* BodyFaces = Inter ? InterBody : StaticBody;
    const char* const* TitleFaces = Inter ? InterTitle : StaticTitle;
    int BodyCount = Inter ? 3 : 2;
    int TitleCount = Inter ? 3 : 2;

    if ( !Font->Create( BodyFaces, BodyCount, Body, 400 )
        || !Heading->Create( TitleFaces, TitleCount, Body + 4.0f, 600 )
        || !Icons->Create( IconFaces, 2, Body - 1.0f, 900 )
        || !Arena->Create( 1048576 )
        || !Context->Create( ) ) {
        Destroy( );
        return false;
    }

    Marks->Create( MarkFaces, 2, Body + 2.0f, 400 );
    return true;
}

void CEngine::Destroy( ) {
    Marks->Destroy( );
    Icons->Destroy( );
    Heading->Destroy( );
    Font->Destroy( );

    Sheet->Destroy( );
    Arena->Destroy( );
    Context->Destroy( );
}

void CEngine::Rescale( float Factor ) {
    Style->Rescale( Factor );

    Font->Rescale( Factor );
    Heading->Rescale( Factor );
    Icons->Rescale( Factor );
    Marks->Rescale( Factor );
}

void CEngine::Begin( CVector Display ) {
    Context->Begin( );
    Context->Display = Display;

    Input->Pointer = PointerArrow;
    Arena->Reset( );

    Canvas->Begin( Display );
}

void CEngine::End( ) {
    int Order[ 2 ] = { 0, OverlayRoute };
    Canvas->Arrange( Order, 2 );

    if ( !Input->MouseDown( 0 ) )
        Context->ActiveItem = 0;

    if ( Input->MousePressed( 0 ) && !Context->FocusClaimed )
        Context->FocusedItem = 0;

    Input->Settle( );
}

const CDrawData& CEngine::Data( ) const {
    return Canvas->Data( );
}
