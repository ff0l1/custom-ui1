#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "imgui2/resources.hxx"

#include <Windows.h>
#include <cstring>

namespace imgui2 {
namespace resources {

static bool IsDirectory( const char* Path ) {
    DWORD Attributes = GetFileAttributesA( Path );
    return Attributes != INVALID_FILE_ATTRIBUTES && ( Attributes & FILE_ATTRIBUTE_DIRECTORY );
}

static bool Join( char* Out, int OutSize, const char* Left, const char* Right ) {
    if ( !Out || OutSize <= 0 || !Left || !Right )
        return false;

    int Written = 0;
    for ( ; Left[ Written ] && Written + 1 < OutSize; Written++ )
        Out[ Written ] = Left[ Written ];

    if ( Written > 0 && Out[ Written - 1 ] != '\\' && Out[ Written - 1 ] != '/' && Written + 1 < OutSize )
        Out[ Written++ ] = '\\';

    for ( int Index = 0; Right[ Index ] && Written + 1 < OutSize; Index++, Written++ )
        Out[ Written ] = Right[ Index ];

    if ( Written >= OutSize )
        return false;

    Out[ Written ] = 0;
    return true;
}

bool locate_root( char* Path, int PathSize ) {
    if ( !Path || PathSize <= 0 )
        return false;

    char Probe[ MAX_PATH ];
    char Candidate[ MAX_PATH ];

    DWORD Length = GetModuleFileNameA( nullptr, Probe, MAX_PATH );
    if ( Length > 0 && Length < MAX_PATH ) {
        for ( int Index = ( int )Length - 1; Index >= 0; Index-- ) {
            if ( Probe[ Index ] == '\\' || Probe[ Index ] == '/' ) {
                Probe[ Index ] = 0;
                break;
            }
        }

        if ( Join( Candidate, MAX_PATH, Probe, "assets" ) && IsDirectory( Candidate ) ) {
            if ( strcpy_s( Path, ( size_t )PathSize, Candidate ) == 0 )
                return true;
        }
    }

    if ( GetCurrentDirectoryA( MAX_PATH, Probe ) ) {
        if ( Join( Candidate, MAX_PATH, Probe, "assets" ) && IsDirectory( Candidate ) ) {
            if ( strcpy_s( Path, ( size_t )PathSize, Candidate ) == 0 )
                return true;
        }
    }

    Path[ 0 ] = 0;
    return false;
}

}
}
