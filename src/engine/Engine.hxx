#pragma once

#include <memory>

#include "Arena.hxx"
#include "Canvas.hxx"
#include "Context.hxx"
#include "Font.hxx"
#include "Format.hxx"
#include "Geometry.hxx"
#include "Input.hxx"
#include "Native.hxx"
#include "Sheet.hxx"
#include "Style.hxx"

class CEngine {
public:
    bool Create( float BodySize );
    void Destroy( );

    void Rescale( float Factor );

    void Begin( CVector Display );
    void End( );

    const CDrawData& Data( ) const;
};

inline auto Engine = std::make_unique< CEngine >( );
