#pragma once

#include "imgui2/ui.hpp"

#include "Geometry.h"

namespace imgui2 {
namespace ui {

enum class Slot {
    None,
    Check,
    Slider,
    Combo,
    Bind
};

struct ComboPopup {
    unsigned int id = 0;
    int* index = nullptr;
    bool* picked = nullptr;
    const char* const* items = nullptr;
    int count = 0;
    CRectangle field;
    CRectangle list;
    bool open = false;
    bool many = false;
    bool ignore = false;
    unsigned int alive = 0;
};

struct HostState {
    CVector origin;
    bool placed = false;
    bool dragging = false;
    CVector grab;

    float close_glow = 0.0f;
    float pill_x = 0.0f;
    float pill_y = 0.0f;
    bool pill_ready = false;
    float reveal = 0.0f;

    Page page = Page::Aimbot;
    int tab = 0;
    Slot last = Slot::None;

    ComboPopup combo;
    unsigned int bind_id = 0;
    bool bind_armed = false;

    CRectangle panel;
    CRectangle rail;
    CRectangle well;
    CRectangle content;
    CVector pen;

    bool open = false;
};

HostState& host( );

void gap( Slot kind );

}
}
