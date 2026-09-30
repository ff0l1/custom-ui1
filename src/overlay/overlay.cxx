#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "imgui2/overlay.hxx"

#include "Input.hxx"
#include "Native.hxx"

#include <string>

#include <Windows.h>
#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWCP_DONOTROUND
#define DWMWCP_DONOTROUND 1
#endif

#ifndef DWMWA_TRANSITIONS_FORCEDISABLED
#define DWMWA_TRANSITIONS_FORCEDISABLED 3
#endif

namespace imgui2 {
namespace overlay {

static const wchar_t* ClassName = L"imgui2.Overlay";

static HWND Handle = nullptr;
static bool Quit = false;
static bool Resized = false;
static bool Interactive = true;

static int ClientWidth = 0;
static int ClientHeight = 0;

static RECT Hit = { };
static bool HaveHit = false;

static void CoverMonitor( HWND Window ) {
    HMONITOR Monitor = MonitorFromWindow( Window, MONITOR_DEFAULTTOPRIMARY );
    MONITORINFO Info = { };
    Info.cbSize = sizeof( Info );

    if ( !GetMonitorInfoW( Monitor, &Info ) )
        return;

    int Across = Info.rcMonitor.right - Info.rcMonitor.left;
    int Down = Info.rcMonitor.bottom - Info.rcMonitor.top;

    SetWindowPos( Window, nullptr, Info.rcMonitor.left, Info.rcMonitor.top, Across, Down, SWP_NOZORDER | SWP_NOACTIVATE );

    ClientWidth = Across;
    ClientHeight = Down;
    Resized = true;
}

static void AttachTransparency( HWND Window ) {
    MARGINS Margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea( Window, &Margins );

    BOOL Dark = TRUE;
    if ( FAILED( DwmSetWindowAttribute( Window, DWMWA_USE_IMMERSIVE_DARK_MODE, &Dark, sizeof( Dark ) ) ) )
        DwmSetWindowAttribute( Window, 19, &Dark, sizeof( Dark ) );

    int Corners = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute( Window, DWMWA_WINDOW_CORNER_PREFERENCE, &Corners, sizeof( Corners ) );

    BOOL NoTransition = TRUE;
    DwmSetWindowAttribute( Window, DWMWA_TRANSITIONS_FORCEDISABLED, &NoTransition, sizeof( NoTransition ) );

    SetLayeredWindowAttributes( Window, 0, 255, LWA_ALPHA );
}

static LRESULT CALLBACK ProcessMessage( HWND Origin, UINT Message, WPARAM Primary, LPARAM Secondary ) {
    if ( Native->Translate( Origin, Message, Primary, Secondary ) )
        return Message == WM_SETCURSOR ? TRUE : 0;

    switch ( Message ) {
    case WM_CLOSE:
    case WM_DESTROY:
        Quit = true;
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT Paint = { };
        BeginPaint( Origin, &Paint );
        EndPaint( Origin, &Paint );
        return 0;
    }

    case WM_SIZE: {
        int Across = ( int )( unsigned short )LOWORD( Secondary );
        int Down = ( int )( unsigned short )HIWORD( Secondary );

        if ( Across > 0 && Down > 0 ) {
            ClientWidth = Across;
            ClientHeight = Down;
            Resized = true;
        }

        return 0;
    }

    case WM_DISPLAYCHANGE:
        CoverMonitor( Origin );
        return 0;

    case WM_DPICHANGED:
        CoverMonitor( Origin );
        return 0;
    }

    return DefWindowProcW( Origin, Message, Primary, Secondary );
}

bool create( const Options& Options ) {
    destroy( );

    SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 );

    WNDCLASSEXW Description = { };
    Description.cbSize = sizeof( WNDCLASSEXW );
    Description.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    Description.lpfnWndProc = ProcessMessage;
    Description.hInstance = GetModuleHandleW( nullptr );
    Description.hCursor = LoadCursorW( nullptr, ( LPCWSTR )IDC_ARROW );
    Description.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
    Description.lpszClassName = ClassName;

    if ( !RegisterClassExW( &Description ) )
        return false;

    const char* TitleUtf8 = Options.title ? Options.title : "custom-ui-1";
    int TitleCount = MultiByteToWideChar( CP_UTF8, 0, TitleUtf8, -1, nullptr, 0 );
    std::wstring Title( TitleCount > 0 ? ( size_t )TitleCount : 1, 0 );
    if ( TitleCount > 0 )
        MultiByteToWideChar( CP_UTF8, 0, TitleUtf8, -1, Title.data( ), TitleCount );

    DWORD Extra = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
    if ( Options.topmost )
        Extra |= WS_EX_TOPMOST;

    Handle = CreateWindowExW( Extra, ClassName, Title.c_str( ), WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, Description.hInstance, nullptr );
    if ( !Handle ) {
        UnregisterClassW( ClassName, Description.hInstance );
        return false;
    }

    CoverMonitor( Handle );
    AttachTransparency( Handle );
    Native->Create( Handle );

    ShowWindow( Handle, SW_SHOW );
    UpdateWindow( Handle );
    return true;
}

void destroy( ) {
    if ( Handle ) {
        Native->Destroy( );
        DestroyWindow( Handle );
        Handle = nullptr;
    }

    UnregisterClassW( ClassName, GetModuleHandleW( nullptr ) );

    Quit = false;
    Resized = false;
    Interactive = true;
    HaveHit = false;
    ClientWidth = 0;
    ClientHeight = 0;
}

void* native( ) {
    return Handle;
}

int width( ) {
    return ClientWidth;
}

int height( ) {
    return ClientHeight;
}

void pump( ) {
    MSG Message;
    while ( PeekMessageW( &Message, nullptr, 0, 0, PM_REMOVE ) ) {
        if ( Message.message == WM_QUIT )
            Quit = true;

        TranslateMessage( &Message );
        DispatchMessageW( &Message );
    }
}

void poll_pointer( ) {
    if ( !Handle )
        return;

    POINT Cursor = { };
    GetCursorPos( &Cursor );
    ScreenToClient( Handle, &Cursor );
    Input->ApplyPosition( ( float )Cursor.x, ( float )Cursor.y );

    if ( !HaveHit )
        return;

    bool Inside = Cursor.x >= Hit.left && Cursor.x < Hit.right && Cursor.y >= Hit.top && Cursor.y < Hit.bottom;
    set_interactive( Inside || Input->MouseDown( 0 ) );
}

void set_interactive( bool Capture ) {
    if ( !Handle || Interactive == Capture )
        return;

    Interactive = Capture;

    LONG Extra = GetWindowLongW( Handle, GWL_EXSTYLE );
    if ( Capture )
        Extra &= ~WS_EX_TRANSPARENT;
    else
        Extra |= WS_EX_TRANSPARENT;

    SetWindowLongW( Handle, GWL_EXSTYLE, Extra );
    SetWindowPos( Handle, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED );
}

void set_hit_bounds( float Left, float Top, float Width, float Height ) {
    Hit.left = ( LONG )Left;
    Hit.top = ( LONG )Top;
    Hit.right = ( LONG )( Left + Width );
    Hit.bottom = ( LONG )( Top + Height );
    HaveHit = Width > 0.0f && Height > 0.0f;
}

bool quit_requested( ) {
    return Quit;
}

void request_quit( ) {
    Quit = true;
}

bool consume_resize( int& Across, int& Down ) {
    if ( !Resized )
        return false;

    Resized = false;
    Across = ClientWidth;
    Down = ClientHeight;
    return Across > 0 && Down > 0;
}

}
}
