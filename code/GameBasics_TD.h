#pragma once
#include "Renderer_TD.h"

enum cursor_state
{
    cursorState_Arrow,
    cursorState_Drag,
};

struct timer
{
    u32 Count;
    i64 Start;
    i64 LastSnap;
};

struct game_state
{
    string_c DataPath;
    //bucket_allocator Bucket;
    arena_allocator FixArena;     // Never gets reset (except if a bucket is emptied again)
    arena_allocator ScratchArena; // Gets reset every frame.
    
    
    input_info Input;
    
    // Render Stuff
    direct_3d_info D3D11Info;
    
    // Time management
    time_management Time;
    
    renderer Renderer;
    
    drag_list DragableList;
    cursor_state CursorState;
    
    // Threading stuff
    
    // Entity Stuff
    u32 EntityCount;
};


internal loaded_bitmap LoadImage_STB(u8 *Path);
inline void FreeImage_STB(loaded_bitmap Bitmap);

#if DEBUG_TD
inline timer StartTimer();
inline void SnapTimer(timer *Timer, string_c *Identification = 0);
#else
#define StartTimer() {}
#define SnapTimer(f) 
#endif


struct entity
{
    u32 ID;
    
    d3d11_render_object *RenderObject;
    
    i32 Health;
    
    v3  Velocity;
    r32 Mass;
    r32 Power;
    
    r32 RotVelocity;
};

#define MAX_ENEMY_COUNT 50
struct enemies
{
    entity Entity[MAX_ENEMY_COUNT];
    u32 Count;
};

struct base_room
{
    obj_data Model;
    loaded_bitmap Bitmap;
    
    vertex_array VerticeData;
    index_array  IndiceData;
    texture_info TexInfo;
};

struct room
{
    base_room *Base;
    d3d11_render_object *RenderObject;
};

inline entity CreateEntity(u32 Health, r32 Mass, r32 Power);