#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

// NOTE:: internal is for procedures, to let the compiler know that it is only for this compilation unit
#define internal        static
#define local_persist   static 
#define global_variable static

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef i32     b32;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  r32;
typedef double r64;

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

// NOTE:: For loop simplification macro. first param: Count until, second param(optional): Iterater name prefix ...It
#define For(until, ...) for(u32 (__VA_ARGS__##It) = 0; (__VA_ARGS__##It) < (until); ++(__VA_ARGS__##It))

// NOTE:: Counts the size of a fixed array.
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#ifndef __CD // Combine both params to a single name
#define __CD2(X, Y) X##Y
#define __CD(X, Y) __CD2(X, Y)
#endif

// NOTE:: General macro for removing an entry/item from an arbitrary array. Moving all following items one up.
#define RemoveItem(Array, ArrayCount, RemovePos, ArrayType) { \
u32 MoveSize = (ArrayCount-RemovePos)*sizeof(ArrayType); \
u8 *Goal = ((u8 *)Array) + (RemovePos*sizeof(ArrayType)); \
u8 *Start = Goal + sizeof(ArrayType); \
memmove_s(Goal, MoveSize, Start, MoveSize); \
} 

// NOTE:: Simple DebugLog that simplifies just printing stuff to the debug output.
#define DebugLog(Count, Text, ...) { \
char B[Count]; \
sprintf_s(B, Text, __VA_ARGS__);\
OutputDebugStringA(B); \
}

#if DEBUG_TD
#define Assert(Expression)  {            \
if(!(Expression))                        \
{                                        \
DebugLog(1000, "Assert fired at:\nLine: %i\nFile: %s\n", __LINE__, __FILE__); \
*(int *)0 = 0;                       \
} }                                       
#else
#define Assert(Expression)
#endif
#define InvalidCodePath    Assert(!"InvalidCodePath")
#define NotImplemented     Assert(!"NotImplementedYet")
#define InvalidDefaultCase default: {Assert(false)}


#define MINIMP3_IMPLEMENTATION
#include "Libraries\\MiniMP3.h"

// TODO:: Implement STBI_MALLOC, STBI_REALLOC and STBI_FREE!
#define STB_IMAGE_IMPLEMENTATION
#include "Libraries\\STB_Image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "Libraries\\STB_Truetype.h"

#if DEBUG_TD
#define STBI_ASSERT(x) if(!(x)) {*(int *)0 = 0;}
#else
#define STBI_ASSERT(x)
#endif

#include "Math_TD.h"
#include "Allocator_TD.c"
struct time_management
{
    i32 GameHz;
    r32 GoalFrameRate;
    r32 dTime;
    r64 GameTime;
    r32 CurrentTimeSpeed;
    
    i64 PerfCountFrequency;
    b32 SleepIsGranular;
};
internal b32 TryGetClipboardText(struct string_compound *String);
internal void ApplyWindowResize(HWND Window, i32 NewWidth, i32 NewHeight, b32 ForceResize = false);
inline v2i GetWindowSize();

#include "Direct3D_11_TD.c"
#include "Input_TD.c"
#include "String_TD.h"
#include "StandardUtilities_TD.c"
#include "Threading_TD.c"
#include "Math_TD.c"
#include "FileUtilities_TD.c"
#include "3DModels.c"
#include "Renderer_TD.c"

global_variable game_state GlobalGameState;
global_variable i32 GlobalMinWindowWidth  = 344;
global_variable i32 GlobalMinWindowHeight = 190;

#include "UI_TD.c"
#include "GameBasics_TD.c"
#include "Room.c"

global_variable b32 IsRunning;
HCURSOR ArrowCursor = 0;
HCURSOR DragCursor = 0;


internal void
WindowGotResized(game_state *GameState)
{
    if(GameState->Renderer.Window.GotResized)
    {
        renderer *Renderer = &GameState->Renderer;
        
        PerformScreenTransform(Renderer);
        
        Renderer->Window.GotResized = false;
    }
    
}

internal void
ApplyWindowResize(HWND Window, i32 NewWidth, i32 NewHeight, b32 ForceResize)
{
    window_info *WinInfo = &GlobalGameState.Renderer.Window;
    
    RECT WRect = {};
    GetWindowRect(Window, &WRect);
    if(NewWidth < GlobalMinWindowWidth || NewHeight < GlobalMinWindowHeight || ForceResize)
    {
        NewWidth  = (NewWidth < GlobalMinWindowWidth) ? GlobalMinWindowWidth : NewWidth;
        NewHeight = (NewHeight < GlobalMinWindowHeight) ? GlobalMinWindowHeight : NewHeight;
        
        if(SetWindowPos(Window, HWND_TOP, WRect.left, WRect.top, NewWidth, NewHeight, 0))
        {
            // Stopped from making window too small
        }
    }
    
    RECT Rect = {};
    if(GetClientRect(Window, &Rect)) // The client has the window borders included. 
    {
        WinInfo->CurrentDim.Width  = Rect.right - Rect.left;
        WinInfo->CurrentDim.Height = Rect.bottom - Rect.top;
    }
    
    WinInfo->CurrentAspect = (r32)WinInfo->CurrentDim.Width/(r32)WinInfo->CurrentDim.Height;
    WinInfo->CurrentReverseAspect = (r32)WinInfo->CurrentDim.Height/(r32)WinInfo->CurrentDim.Width;
    
    GlobalGameState.Renderer.Window.GotResized = true;
    GlobalGameState.Renderer.Rerender = true;
    //ReshapeGLWindow(&GlobalGameState.Renderer);
    WindowGotResized(&GlobalGameState);
    
    HDC DeviceContext = GetDC(Window);
    //DisplayBufferInWindow(DeviceContext, &GlobalGameState.Renderer);
    
    ReleaseDC(Window, DeviceContext);
}

inline v2i
GetWindowSize()
{
    v2i Result = {};
    RECT Rect = {};
    GetWindowRect(GlobalGameState.Renderer.Window.WindowHandle, &Rect);
    
    Result.x = Rect.right - Rect.left;
    Result.y = Rect.bottom - Rect.top;
    
    return Result;
}

internal LRESULT CALLBACK
WindowCallback(HWND Window,
               UINT Message,
               WPARAM WParam,
               LPARAM LParam)
{
    LRESULT Result = 0;
    
    switch(Message)
    {
        case WM_SIZE: 
        {
            if(IsRunning) 
            {
                switch(WParam)
                {
                    case SIZE_MAXHIDE:
                    {
                        DebugLog(255, "Message is sent to all pop-up windows when some other window is maximized.\n"); 
                    } break;
                    case SIZE_MAXSHOW:
                    {
                        DebugLog(255, "Message is sent to all pop-up windows when some other window has been restored to its former size.\n"); 
                    } break;
                    case SIZE_MINIMIZED:
                    {
                        GlobalGameState.Renderer.Minimized = true;
                    } break;
                    case SIZE_MAXIMIZED:
                    case SIZE_RESTORED:
                    {
                        GlobalGameState.Renderer.Minimized = false;
                        
                        RECT WRect = {};
                        GetWindowRect(Window, &WRect);
                        i32 WWidth  = WRect.right - WRect.left;
                        i32 WHeight = WRect.bottom - WRect.top;
                        
                        ApplyWindowResize(Window, WWidth, WHeight);
                        
                    } break;
                }
            }
            
        } break;
        case WM_DESTROY:
        case WM_CLOSE: 
        { 
            IsRunning = false;
        } break;
        case WM_ACTIVATEAPP: {} break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {} break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            //DisplayBufferInWindow(DeviceContext, &GlobalGameState.Renderer);
            EndPaint(Window, &Paint);
            ReleaseDC(Window, DeviceContext);
        } break;
        case WM_SETCURSOR: 
        {
            if (GlobalGameState.CursorState == cursorState_Arrow) 
                Result = DefWindowProcA(Window, Message, WParam, LParam);
            else if(GlobalGameState.CursorState == cursorState_Drag) 
                SetCursor(DragCursor);
        } break;
        case WM_SETFOCUS:
        {
            HandleFocusRegain(&GlobalGameState.Input);
        } break;
        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }
    
    return Result;
}

internal void
ProcessPendingMessages(input_info *Input, HWND Window)
{
    b32 PrevMouseMove = Input->_MouseMoved;
    ResetKeys(Input);
    
    MSG Message = {};
    while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT: { IsRunning = false; } break;
            case WM_HOTKEY:
            {
                HandleHotkey(Input, (i32)Message.wParam, Message.lParam);
            } break;
            case WM_CHAR: 
            {
                UpdateTypedCharacters(Input, (u8)Message.wParam);
            } break;
            case WM_SYSKEYDOWN: 
            case WM_SYSKEYUP: 
            case WM_KEYDOWN: 
            case WM_KEYUP:
            {
                u32 Scancode = (Message.lParam & 0x00ff0000) >> 16;
                i32 Extended  = (Message.lParam & 0x01000000) != 0;
                
                switch (Message.wParam) {
                    case VK_SHIFT:
                    {
                        u32 NewKeyCode = MapVirtualKey(Scancode, MAPVK_VSC_TO_VK_EX);
                        UpdateKeyChange(Input, NewKeyCode, Message.lParam);
                    } break;
                    
                    case VK_CONTROL:
                    {
                        u32 NewKeyCode = Extended ? VK_RCONTROL : VK_LCONTROL;
                        UpdateKeyChange(Input, NewKeyCode, Message.lParam);
                    } break;
                    
                    case VK_MENU:
                    {
                        u32 NewKeyCode = Extended ? VK_RMENU : VK_LMENU;
                        UpdateKeyChange(Input, NewKeyCode, Message.lParam);
                    } break;
                    
                    default:
                    {
                        if(!UpdateKeyChange(Input, (u32)Message.wParam, Message.lParam))
                        {
                            // If we don't process this message, try translating it to WM_CHAR
                            TranslateMessage(&Message);
                            DispatchMessage(&Message);
                        }
                    } break;    
                }
            } break;
            case WM_MOUSEMOVE:
            {
                Input->MouseP.x = (u16) Message.lParam; 
                Input->MouseP.y = (u16)(Message.lParam >> 16);
                Input->MouseP.y = GlobalGameState.Renderer.Window.CurrentDim.Height - Input->MouseP.y;
                
                if(!PrevMouseMove) // Checks if mouse buttons are still pressed after leaving and re-entering the window
                {
                    if(Input->Pressed[KEY_LMB] && GetAsyncKeyState(VK_LBUTTON) == 0)
                        UpdateSingleKey(Input, KEY_LMB, true, false);
                    if(Input->Pressed[KEY_RMB] && GetAsyncKeyState(VK_RBUTTON) == 0) 
                        UpdateSingleKey(Input, KEY_RMB, true, false);
                    if(Input->Pressed[KEY_MMB] && GetAsyncKeyState(VK_MBUTTON) == 0) 
                        UpdateSingleKey(Input, KEY_MMB, true, false);
                }
                Input->_MouseMoved = true;
            } break;
            case WM_MOUSEWHEEL:
            {
                i32 Modifiers = GET_KEYSTATE_WPARAM(Message.wParam)/4;
                i32 ZDelta    = GET_WHEEL_DELTA_WPARAM(Message.wParam);
                
                i32 WheelPlus = (Modifiers == 2) ? 1000 : 1 + Modifiers*3;
                Input->WheelAmount += ((ZDelta/30)*-WheelPlus)*10;
            } break;
            case WM_LBUTTONDOWN:
            {
                UpdateSingleKey(Input, KEY_LMB, false, true); 
            } break;
            case WM_LBUTTONUP:
            {
                UpdateSingleKey(Input, KEY_LMB, true, false); 
            } break;
            case WM_RBUTTONDOWN:
            {
                UpdateSingleKey(Input, KEY_RMB, false, true); 
            } break;
            case WM_RBUTTONUP:
            {
                UpdateSingleKey(Input, KEY_RMB, false, true); 
            } break;
            case WM_MBUTTONDOWN:
            {
                UpdateSingleKey(Input, KEY_MMB, false, true); 
            } break;
            case WM_MBUTTONUP:
            {
                UpdateSingleKey(Input, KEY_MMB, false, true); 
            } break;
            
            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            } break;
        }
    }
}

internal b32
TryGetClipboardText(string_c *String)
{
    b32 Result = false;
    Assert(String);
    
    // Try opening the clipboard
    if (OpenClipboard(0))
    {
        // Get handle of clipboard object for ANSI text
        HANDLE Data = GetClipboardData(CF_TEXT);
        if (Data)
        {
            // Lock the handle to get the actual text pointer
            u8 *Text = (u8 *)GlobalLock(Data);
            if (Text)
            {
                Result = true;
                
                if(StringLength(Text) >= String->Length) Text[String->Length-1] = 0;
                else AppendStringToCompound(String, Text);
                
                // Release the lock
                GlobalUnlock(Data);
                
                // Release the clipboard
                CloseClipboard();
            }
        }
    }
    return Result;
}

inline void
GetWindowDimensions(HWND Window, v2i *Dim)
{
    PAINTSTRUCT Paint;
    HDC DeviceContext = BeginPaint(Window, &Paint);
    Dim->x = Paint.rcPaint.right - Paint.rcPaint.left; 
    Dim->y = Paint.rcPaint.bottom - Paint.rcPaint.top;
    EndPaint(Window, &Paint);
    ReleaseDC(Window, DeviceContext);
}

inline LONGLONG
GetWallClock()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return(Result.QuadPart);
}

inline r32
GetSecondsElapsed(i64 PerfCountFrequency, LONGLONG Start, LONGLONG End)
{
    r32 Result = ((r32)(End - Start) / (r32)PerfCountFrequency);
    return(Result);
}

#ifdef CONSOLE_APP
int main(int argv, char** arcs) 
#else
i32 CALLBACK 
WinMain(HINSTANCE Instance, 
        HINSTANCE PrevInstance, 
        LPSTR CmdLine, 
        i32 ShowCmd)
#endif
{
    // Hey, baby! Check out the nil-value _I'm_ dereferencing.
    
    ArrowCursor = LoadCursor(0, IDC_ARROW);
    DragCursor = LoadCursor(0, IDC_SIZEWE);
    
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = WindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = ArrowCursor;
    WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WindowClass.lpszClassName = "TetroidClassName";
    
    if(RegisterClassA(&WindowClass))
    {
        // Initializing GameState
        game_state *GameState = &GlobalGameState;
        *GameState = {};
        GameState->DataPath = NewStaticStringCompound("..\\data\\");
        time_management *Time = &GameState->Time;
        Time->GameHz           = 60; // TODO:: Get monitor refresh rate!?
        Time->GoalFrameRate    = 1.0f/Time->GameHz;
        Time->dTime            = 0.0f;
        Time->GameTime         = 0.0f;
        Time->CurrentTimeSpeed = 1.0f;
        Time->SleepIsGranular = (timeBeginPeriod(1) == TIMERR_NOERROR); 
        GameState->Input                 = {};
        input_info *Input = &GameState->Input;
        
        // Initializing clock
        LARGE_INTEGER PerfCountFrequencyResult;
        QueryPerformanceFrequency(&PerfCountFrequencyResult);
        Time->PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
        LONGLONG PrevCycleCount = GetWallClock();
        LONGLONG FlipWallClock  = GetWallClock();
        
        SetRandomSeed(FlipWallClock);
        
        // Initializing Allocator
        GameState->FixArena.MaxEmptyArenaCount = 2;
        GameState->ScratchArena = {arenaFlags_IsTransient, Megabytes(16)};
        
        u32 InitialWindowWidth  = 2200;
        u32 InitialWindowHeight = (u32)(InitialWindowWidth*(9.0f/16.0f));
        HWND Window = CreateWindowExA(0, WindowClass.lpszClassName, "Tetroid", 
                                      WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, 
                                      CW_USEDEFAULT, InitialWindowWidth, -1, 
                                      0, 0, Instance, 0);
        if(Window)
        {
            SetWindowPos(Window, 0, 0, 0, InitialWindowWidth, InitialWindowHeight, SWP_NOMOVE); // NOTE:: AMD Win7 driver bug fix
            HDC DeviceContext = GetDC(Window);
            GameState->D3D11Info = InitializeDirect3D(GameState->FixArena, GameState->ScratchArena, Window);
            direct_3d_info *D3D11Info = &GameState->D3D11Info;
            
            AddHotKey(Window, Input, KEY_PLAY_PAUSE);
            AddHotKey(Window, Input, KEY_STOP);
            AddHotKey(Window, Input, KEY_NEXT);
            AddHotKey(Window, Input, KEY_PREVIOUS);
            // ********************************************
            // Threading***********************************
            // ********************************************
            
            
            renderer *Renderer = &GameState->Renderer;
            *Renderer = InitializeRenderer(GameState, Window);
            
            //ReshapeGLWindow(&GameState->Renderer);
            b32 *ReRender = &Renderer->Rerender;
            
            
            // ********************************************
            // UI rendering stuff   ***********************
            // ********************************************
            r32 WWidth = (r32)Renderer->Window.FixedDim.Width;
            r32 WMid    = WWidth*0.5f;
            r32 WHeight = (r32)Renderer->Window.FixedDim.Height;
            
            //Renderer->FontInfo.BigFont    = InitSTBBakeFont(GameState, FontSizeToPixel(font_Big));
            //Renderer->FontInfo.MediumFont = InitSTBBakeFont(GameState, FontSizeToPixel(font_Medium));
            //Renderer->FontInfo.SmallFont  = InitSTBBakeFont(GameState, FontSizeToPixel(font_Small));
            
            
            
            
            string_c RoomPath = NewStaticStringCompound("../Data/Models/Room_001.obj");
            string_c BitmapPath = NewStaticStringCompound("../Data/Models/576.png");
            //string_c BitmapPath = NewStaticStringCompound("../Data/Models/pattern.png");
            
            
            // ROOMs ***********************************************************
            base_room BaseRoom = {};
            CreateBaseRoom(&BaseRoom, RoomPath, BitmapPath);
            
            r32 RoomBaseOffset = 0.315f;
            const i32 RoomCount = 9;
            u32 RoomRootCount = (i32)sqrt(RoomCount);
            room Rooms[RoomCount];
            
            r32 RoomZOffset = -RoomBaseOffset*(RoomRootCount - (RoomRootCount%2));
            For(RoomRootCount, Z)
            {
                r32 RoomXOffset = -RoomBaseOffset*(RoomRootCount - (RoomRootCount%2));
                For(RoomRootCount, X)
                {
                    i32 It = XIt + ZIt*RoomRootCount;
                    Rooms[It] = {&BaseRoom, 0};
                    Rooms[It].RenderObject = CreateRenderObject(D3D11Info, BaseRoom.VerticeData, 
                                                                BaseRoom.IndiceData, BaseRoom.TexInfo);
                    
                    SetTransform(Rooms[It].RenderObject, {RoomXOffset, 0, RoomZOffset}, {}, {1, 1, 1});
                    RoomXOffset += RoomBaseOffset;
                }
                RoomZOffset += RoomBaseOffset;
            }
            
            // Player **********************************************************
            d3d11_render_object Camera = {};
            bool CamP1 = false;
            
            entity Player = CreateEntity(25, 88, 200);
            Player.RenderObject = &Camera;
            //SetTransform(Player.RenderObject, {0, 0.5f, -0.6f}, {-Pi32/6.0f, 0, 0}, {1, 1, 1});
            SetTransform(Player.RenderObject, {0, -0.05f, 0}, {-0.2f, 0, 0}, {1, 1, 1});
            
            r32 Drag = 0.75f;
            entity Ship = CreateEntity(8, 1.0f, 100);
            Ship.RenderObject = CreateRenderObject(D3D11Info, 
                                                   {RectVertexData, ArrayCount(RectVertexData)}, 
                                                   {RectIndexData, ArrayCount(RectIndexData)}, 
                                                   {2,2,ThingTextureData});
            
            const u32 BulletCount = 20;
            u32 NextBullet = 0;
            entity Bullets[BulletCount];
            For(BulletCount)
            {
                Bullets[It] = CreateEntity(100, 0.25f, 150);
                Bullets[It].RenderObject = CreateRenderObject(D3D11Info, 
                                                              {RectVertexData, ArrayCount(RectVertexData)}, 
                                                              {RectIndexData, ArrayCount(RectIndexData)}, 
                                                              {2,2,ThingTextureData});
                SetScale(Bullets[It].RenderObject, V3(0.5f));
                Bullets[It].RenderObject->Render = false;
            }
            
            enemies Enemies = {};
            For(MAX_ENEMY_COUNT)
            {
                Enemies.Entity[It]              = CreateEntity(12, 1.0f, 100);
                Enemies.Entity[It].RenderObject = CreateRenderObject(D3D11Info, 
                                                                     {RectVertexData, ArrayCount(RectVertexData)}, 
                                                                     {RectIndexData, ArrayCount(RectIndexData)}, 
                                                                     {2,2,ThingTextureData});
                Enemies.Entity[It].RenderObject->Render = false;
            }
            
            r32 dSpeed = 100;
            
            // ********************************************
            // FPS ****************************************
            // ********************************************
            
#if DEBUG_TD 
            r32 FPSList[100] = {};
            u32 FPSCount = 0;
            v3 NOP = {};
            entry_id *FPSParent = CreateRenderRect(Renderer, {{},{}}, -0.9f, 0, &NOP);
            SetPosition(FPSParent, V2(Renderer->Window.CurrentDim.Dim) - V2(60, 11));
            TranslateWithScreen(&Renderer->TransformList, FPSParent, fixedTo_TopRight);
            render_text FPSText = {};
            r32 dUpdateRate = 0.0f;
#endif
            
            b32 SongChangedIsCurrentlyDecoding      = true;
            b32 SongChangedIsCurrentlyFullyDecoding = true;
            IsRunning = true;
            while(IsRunning)
            {
                ResetMemoryArena(&GameState->ScratchArena);
                
                LONGLONG CurrentCycleCount = GetWallClock();
                Time->dTime = GetSecondsElapsed(Time->PerfCountFrequency, 
                                                PrevCycleCount, CurrentCycleCount);
                PrevCycleCount = CurrentCycleCount;
                Time->GameTime += Time->dTime;
                
#if false //DEBUG_TD
                if(!Renderer->Minimized) {
                    FPSList[FPSCount] = (1.0f/Time->dTime);
                    r32 CurrentFPS = 0;
                    For(100) CurrentFPS += FPSList[It];
                    CurrentFPS /= 100.0f;
                    
                    if(dUpdateRate >= 1.0f)
                    {
                        dUpdateRate = 0.0f;
                        char FPSString[10];
                        sprintf_s(FPSString, "%.2f", CurrentFPS);
                        string_c FPSComp = NewStringCompound(&GameState->ScratchArena, 10);
                        AppendStringToCompound(&FPSComp, (u8 *)FPSString);
                        RemoveRenderText(Renderer, &FPSText);
                        CreateRenderText(Renderer, &GameState->FixArena, font_Small, &FPSComp,
                                         &DisplayInfo->ColorPalette.ForegroundText, &FPSText, -0.9999f, FPSParent);
                        DeleteStringCompound(&GameState->ScratchArena, &FPSComp);
                    }
                    else dUpdateRate += Time->dTime/0.1f;
                    FPSCount = (FPSCount+1)%100;
                }
#endif
                
                // *******************************************
                // Input handling ****************************
                // *******************************************
                ProcessPendingMessages(Input, Window);
                
                if(Input->KeyChange[KEY_ESCAPE] == KeyDown)
                {
                    IsRunning = false;
                }
                if(Input->Pressed[KEY_ALT_LEFT] && Input->KeyChange[KEY_F4] == KeyDown) 
                {
                }
                
                if(Input->KeyChange[KEY_SPACE] == KeyDown) 
                {
                    if(CamP1) SetTransform(Player.RenderObject, {0, 0.5f, -0.6f}, {-Pi32/6.0f, 0, 0}, {1, 1, 1});
                    else      SetTransform(Player.RenderObject, {0, -0.05f, 0}, {-0.2f, 0, 0}, {1, 1, 1});
                    CamP1 = !CamP1;
                }
                if(Input->Pressed[KEY_PLUS]) dSpeed += 10;
                if(Input->Pressed[KEY_MINUS]) dSpeed -= 10;
                
                
                r32 RForce = 0;
                if(Input->Pressed[KEY_A])  RForce = 100;
                if(Input->Pressed[KEY_D]) RForce = -100;
                v3 Force = {};
                if(Input->Pressed[KEY_Q]) Force.x = -1;
                if(Input->Pressed[KEY_E]) Force.x =  1;
                if(Input->Pressed[KEY_W]) Force.y =  1;
                if(Input->Pressed[KEY_S]) Force.y = -1;
                CalculateShipMovement(&Ship, Time->dTime, Force, RForce);
                CalculateShipMovement(&Player, Time->dTime, {Force.x, Force.z, Force.y}, RForce);
                
                if(Input->KeyChange[KEY_UP] == KeyDown) 
                {
                    SpawnBullet(Bullets+NextBullet, &Ship);
                    NextBullet = (NextBullet+1)%BulletCount;
                }
                For(BulletCount)
                {
                    if(!Bullets[It].RenderObject->Render) continue;
                    
                    CalculateShipMovement(Bullets+It, Time->dTime, V3(0), 0);
                    UpdateTransform(Bullets[It].RenderObject);
                }
                
                if(Input->KeyChange[KEY_ENTER] == KeyDown)
                {
                    if(Time->GameHz == 60) Time->GameHz = 30;
                    else if(Time->GameHz == 30) Time->GameHz = 10;
                    else if(Time->GameHz == 10) Time->GameHz = 60;
                    
                    Time->GoalFrameRate = 1.0f/Time->GameHz;
                }
                
                // *******************************************
                // Rendering *********************************
                // *******************************************
                *ReRender = true;
                //if(!Renderer->Minimized) DisplayBufferInWindow(DeviceContext, Renderer);
                
                UpdateTransform(Player.RenderObject);
                For(RoomCount) UpdateTransform(Rooms[It].RenderObject);
                UpdateTransform(Ship.RenderObject);
                
                
                Direct3DDraw(D3D11Info, Player.RenderObject);
                
                
                // Allocation debug stuff
#if DEBUG_TD
                CollectArenaDebugFrameData(&GameState->FixArena.DebugData);
                CollectArenaDebugFrameData(&GameState->ScratchArena.DebugData);
#endif
                
                // ****************************************************************
                // Sleep if we are faster than targeted framerate. ****************
                // ****************************************************************
                r32 dFrameWorkTime = GetSecondsElapsed(Time->PerfCountFrequency, CurrentCycleCount, GetWallClock());
                if(dFrameWorkTime < Time->GoalFrameRate)
                {
                    DWORD SleepMS = (DWORD)(1000.0f * (Time->GoalFrameRate - dFrameWorkTime));
                    if(Time->SleepIsGranular)
                    {
                        if(SleepMS > 0) Sleep(SleepMS);
                    }
                    else
                    {
                        // NOTE:: Just guessing that 10 ms might be enough for the scheduling granularity 
                        // to not oversleep.
                        if(SleepMS > 10) Sleep(SleepMS); 
                    }
                }
            }
            
            DebugLog(25, "Settings saved.\n");
        }
        else
        {
            DebugLog(255, "%i\n", GetLastError());
        }
    }
    
    return 0;
}
