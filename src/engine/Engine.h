#pragma once

#include <memory>

#include "Arena.h"
#include "Canvas.h"
#include "Context.h"
#include "Font.h"
#include "Format.h"
#include "Geometry.h"
#include "Input.h"
#include "Native.h"
#include "Sheet.h"
#include "Style.h"

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
