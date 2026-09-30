#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "imgui2/motion.hxx"
#include "imgui2/resources.hxx"
#include "imgui2/ui.hxx"

#include "interact.hxx"
#include "state.hxx"

#include "Canvas.hxx"
#include "Context.hxx"
#include "Font.hxx"
#include "Format.hxx"
#include "Input.hxx"
#include "Style.hxx"

#include <Windows.h>

namespace imgui2 {
namespace ui {

static const char* KeyName( int Key ) {
    if ( Key <= 0 )
        return "None";
    if ( Key == VK_LBUTTON )
        return "Mouse 1";
    if ( Key == VK_RBUTTON )
        return "Mouse 2";
    if ( Key == VK_MBUTTON )
        return "Mouse 3";
    if ( Key == VK_XBUTTON1 )
        return "Mouse 4";
    if ( Key == VK_XBUTTON2 )
        return "Mouse 5";

    UINT Scan = MapVirtualKeyW( ( UINT )Key, MAPVK_VK_TO_VSC );
    LONG Packed = ( LONG )( Scan << 16 );
    if ( Key == VK_LEFT || Key == VK_RIGHT || Key == VK_UP || Key == VK_DOWN || Key == VK_INSERT || Key == VK_DELETE || Key == VK_HOME || Key == VK_END || Key == VK_PRIOR || Key == VK_NEXT || Key == VK_NUMLOCK || Key == VK_DIVIDE )
        Packed |= 1 << 24;

    static char Label[ 32 ];
    wchar_t Wide[ 32 ] = { };
    if ( GetKeyNameTextW( Packed, Wide, 32 ) > 0 ) {
        WideCharToMultiByte( CP_UTF8, 0, Wide, -1, Label, 32, nullptr, nullptr );
        return Label;
    }

    return Format->Print( "Key %d", Key );
}

static bool ListenBind( unsigned int Identifier, CRectangle, int& Key, bool Hovered, bool Clicked ) {
    HostState& State = host( );
    bool Listening = State.bind_id == Identifier;
    if ( Clicked ) {
        State.bind_id = Identifier;
        State.bind_armed = false;
        Listening = true;
    }

    if ( Listening && !State.bind_armed )
        State.bind_armed = true;
    else if ( Listening && State.bind_armed ) {
        if ( Input->KeyPressed( KeyEscape ) ) {
            State.bind_id = 0;
            State.bind_armed = false;
            Listening = false;
        } else {
            for ( int Code = 1; Code < 256; Code++ ) {
                if ( Code == KeyEscape || Code == KeyShift || Code == KeyControl || Code == KeyAlt )
                    continue;
                if ( Input->KeyPressed( Code ) ) {
                    Key = Code;
                    State.bind_id = 0;
                    State.bind_armed = false;
                    Listening = false;
                    break;
                }
            }

            if ( Listening ) {
                if ( Input->MousePressed( 0 ) && !Hovered ) {
                    Key = VK_LBUTTON;
                    State.bind_id = 0;
                    State.bind_armed = false;
                    Listening = false;
                } else if ( Input->MousePressed( 1 ) ) {
                    Key = VK_RBUTTON;
                    State.bind_id = 0;
                    State.bind_armed = false;
                    Listening = false;
                } else if ( Input->MousePressed( 2 ) ) {
                    Key = VK_MBUTTON;
                    State.bind_id = 0;
                    State.bind_armed = false;
                    Listening = false;
                }
            }
        }
    }

    return Listening;
}

static void DrawBindField( unsigned int Identifier, CRectangle Field, bool Hovered, bool Listening, int Key ) {
    float Glow = motion::hover( Identifier ^ 0x51u, Hovered || Listening, 22.0f );
    Canvas->Rectangle( Field, Style->Control.Blend( Style->Hovered, Glow * 0.4f ), Style->ControlRounding );
    if ( Style->Borders )
        Canvas->Border( Field, Style->Outline.Blend( Style->Accent, Listening ? 0.7f : 0.12f + Glow * 0.4f ), Style->ControlRounding, Style->Thickness );

    const char* Reading = Listening ? "..." : KeyName( Key );
    CVector Size = Font->Measure( Reading );
    Canvas->Write(
        Font.get( ),
        CVector( Field.Left + ( Field.Width - Size.Horizontal ) * 0.5f, Field.Top + ( Field.Height - Font->LineSpan ) * 0.5f ),
        Listening ? Style->AccentSoft : Style->Text,
        Reading
    );
}

Page page( ) {
    return host( ).page;
}

void label( const char* Text ) {
    HostState& State = host( );
    if ( !State.open || !Text )
        return;

    State.last = Slot::None;
    Canvas->Write( Font.get( ), State.pen, Style->Faint, Text );
    State.pen.Vertical += Font->LineSpan + Style->Spacing * 0.55f;
}

bool check( const char* Text, bool& Value, int& Key ) {
    HostState& State = host( );
    if ( !State.open || !Text )
        return false;

    gap( Slot::Check );

    float BindWidth = 108.0f * Style->Scale;
    CRectangle Bounds( State.pen.Horizontal, State.pen.Vertical, State.content.Width, Style->ControlHeight );
    CRectangle BindField( Bounds.Right( ) - BindWidth, Bounds.Top, BindWidth, Bounds.Height );
    CRectangle HitBox( Bounds.Left, Bounds.Top, Bounds.Width - BindWidth - 8.0f * Style->Scale, Bounds.Height );
    unsigned int Identifier = Context->Hash( Text );
    unsigned int BindId = Identifier ^ 0xB1u;

    bool BindHovered = false;
    bool BindHeld = false;
    bool BindClicked = hit( BindId, BindField, BindHovered, BindHeld );
    bool Listening = ListenBind( BindId, BindField, Key, BindHovered, BindClicked );
    if ( BindHovered )
        Input->Pointer = PointerHand;
    DrawBindField( BindId, BindField, BindHovered, Listening, Key );

    bool Hovered = false;
    bool Held = false;
    bool Clicked = hit( Identifier, HitBox, Hovered, Held );
    if ( Clicked )
        Value = !Value;

    if ( Hovered )
        Input->Pointer = PointerHand;

    float Glow = motion::hover( Identifier, Hovered, 22.0f );
    float Slide = motion::toward( Identifier ^ 1u, Value ? 1.0f : 0.0f, 20.0f );
    float Press = motion::toward( Identifier ^ 2u, Held && Hovered ? 1.0f : 0.0f, 26.0f );

    float Box = Style->CheckSize;
    CRectangle Mark( Bounds.Left, Bounds.Top + ( Bounds.Height - Box ) * 0.5f, Box, Box );
    CColor Fill = Style->Control.Blend( Style->Hovered, Glow * 0.35f ).Blend( Style->Accent, Slide );
    Fill = Fill.Blend( Style->Pressed, Press * 0.35f );

    Canvas->Rectangle( Mark, Fill, Style->ControlRounding * 0.55f );
    if ( Style->Borders )
        Canvas->Border( Mark, Style->Outline.Blend( Style->Accent, Slide * 0.7f + Glow * 0.2f ), Style->ControlRounding * 0.55f, Style->Thickness );

    if ( Slide > 0.02f ) {
        CRectangle Inner = Mark.Shrink( Box * 0.26f );
        CColor Tick = CColor( 255, 255, 255 ).Fade( Slide );
        Canvas->Stroke( Inner, CVector( 0.05f, 0.56f ), CVector( 0.38f, 0.86f ), Tick, Style->IconStroke );
        Canvas->Stroke( Inner, CVector( 0.38f, 0.86f ), CVector( 0.97f, 0.18f ), Tick, Style->IconStroke );
    }

    const char* Shown = Context->Caption( Text );
    Canvas->Write( Font.get( ), CVector( Mark.Right( ) + Style->Spacing, Bounds.Top + ( Bounds.Height - Font->LineSpan ) * 0.5f ), Style->Text.Blend( Style->Faint, 0.12f - Glow * 0.12f ), Shown );

    State.pen.Vertical += Bounds.Height + Style->Spacing * 0.32f;
    return Clicked;
}

bool check( const char* Text, bool& Value ) {
    HostState& State = host( );
    if ( !State.open || !Text )
        return false;

    gap( Slot::Check );

    CRectangle Bounds( State.pen.Horizontal, State.pen.Vertical, State.content.Width, Style->ControlHeight );
    unsigned int Identifier = Context->Hash( Text );

    bool Hovered = false;
    bool Held = false;
    bool Clicked = hit( Identifier, Bounds, Hovered, Held );
    if ( Clicked )
        Value = !Value;

    if ( Hovered )
        Input->Pointer = PointerHand;

    float Glow = motion::hover( Identifier, Hovered, 22.0f );
    float Slide = motion::toward( Identifier ^ 1u, Value ? 1.0f : 0.0f, 20.0f );
    float Press = motion::toward( Identifier ^ 2u, Held && Hovered ? 1.0f : 0.0f, 26.0f );

    float Box = Style->CheckSize;
    CRectangle Mark( Bounds.Left, Bounds.Top + ( Bounds.Height - Box ) * 0.5f, Box, Box );
    CColor Fill = Style->Control.Blend( Style->Hovered, Glow * 0.35f ).Blend( Style->Accent, Slide );
    Fill = Fill.Blend( Style->Pressed, Press * 0.35f );

    Canvas->Rectangle( Mark, Fill, Style->ControlRounding * 0.55f );
    if ( Style->Borders )
        Canvas->Border( Mark, Style->Outline.Blend( Style->Accent, Slide * 0.7f + Glow * 0.2f ), Style->ControlRounding * 0.55f, Style->Thickness );

    if ( Slide > 0.02f ) {
        CRectangle Inner = Mark.Shrink( Box * 0.26f );
        CColor Tick = CColor( 255, 255, 255 ).Fade( Slide );
        Canvas->Stroke( Inner, CVector( 0.05f, 0.56f ), CVector( 0.38f, 0.86f ), Tick, Style->IconStroke );
        Canvas->Stroke( Inner, CVector( 0.38f, 0.86f ), CVector( 0.97f, 0.18f ), Tick, Style->IconStroke );
    }

    const char* Shown = Context->Caption( Text );
    Canvas->Write( Font.get( ), CVector( Mark.Right( ) + Style->Spacing, Bounds.Top + ( Bounds.Height - Font->LineSpan ) * 0.5f ), Style->Text.Blend( Style->Faint, 0.12f - Glow * 0.12f ), Shown );

    State.pen.Vertical += Bounds.Height + Style->Spacing * 0.32f;
    return Clicked;
}

bool slider( const char* Text, float& Value, float Minimum, float Maximum, const char* FormatHint ) {
    HostState& State = host( );
    if ( !State.open || !Text )
        return false;

    gap( Slot::Slider );

    if ( Maximum < Minimum )
        Maximum = Minimum;

    if ( !FormatHint )
        FormatHint = "%.1f";

    float Depth = Font->LineSpan + Style->KnobSize * 2.0f + 10.0f * Style->Scale;
    CRectangle Bounds( State.pen.Horizontal, State.pen.Vertical, State.content.Width, Depth );
    unsigned int Identifier = Context->Hash( Text );

    bool Hovered = false;
    bool Held = false;
    hit( Identifier, Bounds, Hovered, Held );

    float Before = Value;
    if ( Held && Maximum > Minimum && Bounds.Width > 0.0f ) {
        float Ratio = ( Input->MousePosition.Horizontal - Bounds.Left ) / Bounds.Width;
        if ( Ratio < 0.0f )
            Ratio = 0.0f;
        if ( Ratio > 1.0f )
            Ratio = 1.0f;

        Value = Minimum + ( Maximum - Minimum ) * Ratio;
        Input->Pointer = PointerAcross;
    } else if ( Hovered ) {
        Input->Pointer = PointerHand;
    }

    if ( Value < Minimum )
        Value = Minimum;
    if ( Value > Maximum )
        Value = Maximum;

    float Glow = motion::hover( Identifier, Hovered || Held, 22.0f );
    float Press = motion::toward( Identifier ^ 3u, Held ? 1.0f : 0.0f, 26.0f );

    const char* Shown = Context->Caption( Text );
    char* Reading = Format->Print( FormatHint, ( double )Value );
    CVector TitleSize = Font->Measure( Shown );

    Canvas->Write( Font.get( ), Bounds.Origin( ), Style->Text, Shown );
    Canvas->Write( Font.get( ), CVector( Bounds.Left + TitleSize.Horizontal + 8.0f * Style->Scale, Bounds.Top ), Style->Faint.Blend( Style->Text, Glow * 0.35f ), Reading );

    float Line = Style->GrooveWidth;
    float Middle = Bounds.Bottom( ) - Style->KnobSize - 1.0f * Style->Scale;
    CRectangle Groove( Bounds.Left, Middle - Line * 0.5f, Bounds.Width, Line );
    Canvas->Rectangle( Groove, Style->Control.Blend( Style->Hovered, Glow * 0.25f ), Line * 0.5f );

    float Portion = Maximum > Minimum ? ( Value - Minimum ) / ( Maximum - Minimum ) : 0.0f;
    float Reach = Groove.Width * Portion;
    if ( Reach > 0.0f )
        Canvas->Rectangle( CRectangle( Groove.Left, Groove.Top, Reach, Line ), Style->Accent.Blend( Style->AccentSoft, Glow * 0.2f ), Line * 0.5f );

    float Radius = Style->KnobSize + ( 0.6f * Glow + 1.1f * Press ) * Style->Scale;
    float Center = Groove.Left + Groove.Width * Portion;
    if ( Center < Groove.Left + Radius )
        Center = Groove.Left + Radius;
    if ( Center > Groove.Right( ) - Radius )
        Center = Groove.Right( ) - Radius;

    Canvas->Circle( CVector( Center, Middle ), Radius, Style->AccentSoft.Blend( Style->Text, Press * 0.12f ) );
    if ( Style->Borders ) {
        CRectangle Ring( Center - Radius, Middle - Radius, Radius * 2.0f, Radius * 2.0f );
        Canvas->Border( Ring, Style->Outline.Blend( Style->Accent, 0.15f + Glow * 0.45f ), Radius, Style->Thickness );
    }

    State.pen.Vertical += Bounds.Height + Style->Spacing * 0.38f;
    return Value != Before;
}

static CRectangle PlaceComboList( const CRectangle& Field, int Count ) {
    HostState& State = host( );
    float Row = Style->ControlHeight;
    float Height = Row * ( float )Count + 8.0f * Style->Scale;
    float Below = State.panel.Bottom( ) - Field.Bottom( ) - 8.0f * Style->Scale;
    float Above = Field.Top - State.panel.Top - 8.0f * Style->Scale;
    bool Up = Height > Below && Above > Below;

    if ( Up ) {
        if ( Height > Above )
            Height = Above;
        if ( Height < Row )
            Height = Row;
        return CRectangle( Field.Left, Field.Top - Height - 4.0f * Style->Scale, Field.Width, Height );
    }

    if ( Height > Below )
        Height = Below;
    if ( Height < Row )
        Height = Row;
    return CRectangle( Field.Left, Field.Bottom( ) + 4.0f * Style->Scale, Field.Width, Height );
}

static const char* ComboSummary( const ComboPopup& Popup ) {
    if ( Popup.many && Popup.picked ) {
        int Taken = 0;
        const char* First = nullptr;
        for ( int Item = 0; Item < Popup.count; Item++ ) {
            if ( !Popup.picked[ Item ] )
                continue;
            if ( !First )
                First = Popup.items[ Item ];
            Taken++;
        }

        if ( Taken <= 0 )
            return "None";
        if ( Taken == 1 )
            return First;
        return Format->Print( "%s +%d", First, Taken - 1 );
    }

    if ( Popup.index && *Popup.index >= 0 && *Popup.index < Popup.count )
        return Popup.items[ *Popup.index ];

    return Popup.items[ 0 ];
}

static bool OpenCombo( unsigned int Identifier, CRectangle Field, const char* const* Items, int Count, int* Index, bool* Picked ) {
    HostState& State = host( );
    bool Hovered = false;
    bool Held = false;
    bool Clicked = hit( Identifier, Field, Hovered, Held );
    if ( Hovered )
        Input->Pointer = PointerHand;

    if ( Clicked ) {
        if ( State.combo.open && State.combo.id == Identifier )
            State.combo.open = false;
        else {
            State.combo.open = true;
            State.combo.id = Identifier;
            State.combo.many = Picked != nullptr;
            State.combo.ignore = true;
        }
    }

    if ( State.combo.open && State.combo.id == Identifier ) {
        State.combo.index = Index;
        State.combo.picked = Picked;
        State.combo.items = Items;
        State.combo.count = Count;
        State.combo.field = Field;
        State.combo.list = PlaceComboList( Field, Count );
        State.combo.alive = Context->FrameIndex;
    }

    return Hovered || ( State.combo.open && State.combo.id == Identifier );
}

static void DrawComboField( unsigned int Identifier, CRectangle Field, bool Active, const char* Reading ) {
    float Glow = motion::hover( Identifier, Active, 22.0f );
    Canvas->Rectangle( Field, Style->Control.Blend( Style->Hovered, Glow * 0.4f ), Style->ControlRounding );
    if ( Style->Borders )
        Canvas->Border( Field, Style->Outline.Blend( Style->Accent, 0.12f + Glow * 0.4f ), Style->ControlRounding, Style->Thickness );

    Canvas->Write(
        Font.get( ),
        CVector( Field.Left + 10.0f * Style->Scale, Field.Top + ( Field.Height - Font->LineSpan ) * 0.5f ),
        Style->Text,
        Reading
    );

    char Mark[ 8 ] = { };
    resources::encode_utf8( resources::chevron_codepoint( ), Mark, 8 );
    CVector Glyph = Icons->Measure( Mark );
    Canvas->Write(
        Icons.get( ),
        CVector( Field.Right( ) - Glyph.Horizontal - 10.0f * Style->Scale, Field.Top + ( Field.Height - Icons->LineSpan ) * 0.5f ),
        Style->Faint.Blend( Style->AccentSoft, Glow ),
        Mark
    );
}

bool combo( const char* Text, int& Index, const char* const* Items, int Count ) {
    HostState& State = host( );
    if ( !State.open || !Text || !Items || Count <= 0 )
        return false;

    gap( Slot::Combo );

    if ( Index < 0 )
        Index = 0;
    if ( Index >= Count )
        Index = Count - 1;

    float Depth = Font->LineSpan + Style->ControlHeight + 6.0f * Style->Scale;
    CRectangle Bounds( State.pen.Horizontal, State.pen.Vertical, State.content.Width, Depth );
    unsigned int Identifier = Context->Hash( Text );

    Canvas->Write( Font.get( ), Bounds.Origin( ), Style->Text, Context->Caption( Text ) );

    CRectangle Field(
        Bounds.Left,
        Bounds.Top + Font->LineSpan + 4.0f * Style->Scale,
        Bounds.Width,
        Style->ControlHeight
    );

    bool Active = OpenCombo( Identifier, Field, Items, Count, &Index, nullptr );
    DrawComboField( Identifier, Field, Active, Items[ Index ] );

    State.pen.Vertical += Bounds.Height + Style->Spacing * 0.38f;
    return false;
}

bool combo( const char* Text, bool* Picked, const char* const* Items, int Count ) {
    HostState& State = host( );
    if ( !State.open || !Text || !Picked || !Items || Count <= 0 )
        return false;

    gap( Slot::Combo );

    float Depth = Font->LineSpan + Style->ControlHeight + 6.0f * Style->Scale;
    CRectangle Bounds( State.pen.Horizontal, State.pen.Vertical, State.content.Width, Depth );
    unsigned int Identifier = Context->Hash( Text );

    Canvas->Write( Font.get( ), Bounds.Origin( ), Style->Text, Context->Caption( Text ) );

    CRectangle Field(
        Bounds.Left,
        Bounds.Top + Font->LineSpan + 4.0f * Style->Scale,
        Bounds.Width,
        Style->ControlHeight
    );

    ComboPopup Preview = { };
    Preview.picked = Picked;
    Preview.items = Items;
    Preview.count = Count;
    Preview.many = true;

    bool Active = OpenCombo( Identifier, Field, Items, Count, nullptr, Picked );
    DrawComboField( Identifier, Field, Active, ComboSummary( Preview ) );

    State.pen.Vertical += Bounds.Height + Style->Spacing * 0.38f;
    return false;
}

bool bind( const char* Text, int& Key ) {
    HostState& State = host( );
    if ( !State.open || !Text )
        return false;

    gap( Slot::Bind );

    float Depth = Font->LineSpan + Style->ControlHeight + 6.0f * Style->Scale;
    CRectangle Bounds( State.pen.Horizontal, State.pen.Vertical, State.content.Width, Depth );
    unsigned int Identifier = Context->Hash( Text );

    const char* Shown = Context->Caption( Text );
    Canvas->Write( Font.get( ), Bounds.Origin( ), Style->Text, Shown );

    CRectangle Field(
        Bounds.Left,
        Bounds.Top + Font->LineSpan + 4.0f * Style->Scale,
        Bounds.Width,
        Style->ControlHeight
    );

    bool Hovered = false;
    bool Held = false;
    bool Clicked = hit( Identifier, Field, Hovered, Held );
    if ( Hovered )
        Input->Pointer = PointerHand;

    bool Listening = ListenBind( Identifier, Field, Key, Hovered, Clicked );
    DrawBindField( Identifier, Field, Hovered, Listening, Key );

    State.pen.Vertical += Bounds.Height + Style->Spacing * 0.38f;
    return false;
}

void paint_overlays( ) {
    HostState& State = host( );
    ComboPopup& Popup = State.combo;
    if ( !Popup.open || !Popup.items || Popup.count <= 0 || Popup.alive != Context->FrameIndex ) {
        Popup.open = false;
        return;
    }

    if ( !Popup.many && !Popup.index )
        return;
    if ( Popup.many && !Popup.picked )
        return;

    Popup.list = PlaceComboList( Popup.field, Popup.count );
    CRectangle List = Popup.list;
    CVector Point = Input->MousePosition;
    bool OverList = List.Contains( Point );
    bool OverField = Popup.field.Contains( Point );

    if ( Style->Shadows )
        Canvas->Shadow( List, Style->Shade, Style->ControlRounding, Style->Softness * 0.45f );

    Canvas->Rectangle( List, Style->Elevated.Fade( 0.94f ), Style->ControlRounding );
    if ( Style->Borders )
        Canvas->Border( List, Style->Outline.Blend( Style->Accent, 0.2f ), Style->ControlRounding, Style->Thickness );

    float Row = Style->ControlHeight;
    int Visible = ( int )( ( List.Height - 8.0f * Style->Scale ) / Row );
    if ( Visible < 1 )
        Visible = 1;
    if ( Visible > Popup.count )
        Visible = Popup.count;

    bool Chosen = false;
    for ( int Item = 0; Item < Visible; Item++ ) {
        CRectangle RowBox( List.Left + 4.0f * Style->Scale, List.Top + 4.0f * Style->Scale + Row * ( float )Item, List.Width - 8.0f * Style->Scale, Row );
        unsigned int RowId = Popup.id ^ ( unsigned int )( Item + 1 ) * 0x9E3779B9u;
        bool Hovered = RowBox.Contains( Point );
        if ( Hovered )
            Input->Pointer = PointerHand;

        bool On = Popup.many ? Popup.picked[ Item ] : Item == *Popup.index;
        float Glow = motion::hover( RowId, Hovered || On, 24.0f );
        if ( Glow > 0.02f )
            Canvas->Rectangle( RowBox, Style->Accent.Fade( On ? 0.22f : 0.12f * Glow ), Style->ControlRounding * 0.7f );

        if ( Popup.many ) {
            float Box = Style->CheckSize * 0.85f;
            CRectangle Mark( RowBox.Left + 8.0f * Style->Scale, RowBox.Top + ( RowBox.Height - Box ) * 0.5f, Box, Box );
            Canvas->Rectangle( Mark, Style->Control.Blend( Style->Accent, On ? 1.0f : 0.0f ), Style->ControlRounding * 0.45f );
            if ( On ) {
                CRectangle Inner = Mark.Shrink( Box * 0.26f );
                Canvas->Stroke( Inner, CVector( 0.05f, 0.56f ), CVector( 0.38f, 0.86f ), CColor( 255, 255, 255 ), Style->IconStroke );
                Canvas->Stroke( Inner, CVector( 0.38f, 0.86f ), CVector( 0.97f, 0.18f ), CColor( 255, 255, 255 ), Style->IconStroke );
            }

            Canvas->Write(
                Font.get( ),
                CVector( Mark.Right( ) + 8.0f * Style->Scale, RowBox.Top + ( RowBox.Height - Font->LineSpan ) * 0.5f ),
                On ? Style->AccentSoft : Style->Text,
                Popup.items[ Item ]
            );
        } else {
            Canvas->Write(
                Font.get( ),
                CVector( RowBox.Left + 8.0f * Style->Scale, RowBox.Top + ( RowBox.Height - Font->LineSpan ) * 0.5f ),
                On ? Style->AccentSoft : Style->Text,
                Popup.items[ Item ]
            );
        }

        if ( !Popup.ignore && Hovered && Input->MouseReleased( 0 ) ) {
            if ( Popup.many )
                Popup.picked[ Item ] = !Popup.picked[ Item ];
            else {
                *Popup.index = Item;
                Chosen = true;
            }
        }
    }

    if ( Popup.ignore ) {
        if ( !Input->MouseDown( 0 ) )
            Popup.ignore = false;
        return;
    }

    if ( Chosen || ( Input->MousePressed( 0 ) && !OverList && !OverField ) )
        Popup.open = false;
}

}
}
