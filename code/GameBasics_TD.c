#include "GameBasics_TD.h"

internal loaded_bitmap 
LoadImage_STB(u8 *Path)
{
    loaded_bitmap Result = {};
    
    // TODO:: Small image is strange: https://github.com/nothings/stb/issues/161
    i32 BG_X, BG_Y, BG_N;
    Result.Pixels = (u32 *)stbi_load((const char *)Path, &BG_X, &BG_Y, &BG_N, 0);
    Result.Width = BG_X;
    Result.Height = BG_Y;
    if(Result.Pixels) 
    {
        Result.WasLoaded = true;
        switch(BG_N)
        {
            case 3:
            {
                Result.ColorFormat = colorFormat_RGB;
            } break;
            case 4:
            {
                Result.ColorFormat = colorFormat_RGBA;
            } break;
            
            InvalidDefaultCase
        }
    }
    
    return Result;
}

inline void
FreeImage_STB(loaded_bitmap Bitmap)
{
    stbi_image_free(Bitmap.Pixels);
}


#if DEBUG_TD
inline timer
StartTimer()
{
    timer Timer = {};
    Timer.Start = Timer.LastSnap = GetWallClock();
    return Timer;
}

inline void
SnapTimer(timer *Timer, string_c *Identification)
{
    i64 NewSnap = GetWallClock();
    r32 CurrentSnap = GetSecondsElapsed(GlobalGameState.Time.PerfCountFrequency, Timer->LastSnap, NewSnap);
    r32 Total = GetSecondsElapsed(GlobalGameState.Time.PerfCountFrequency, Timer->Start, NewSnap);
    u8 *Addon = Identification ? Identification->S : (u8 *)"";
    DebugLog(255, "%s Timer snap %i: %.8f, total: %.8f\n", Addon, ++Timer->Count, CurrentSnap, Total);
    Timer->LastSnap = NewSnap;
}
#endif



inline entity
CreateEntity(u32 Health, r32 Mass, r32 Power)
{
    entity Result = {};
    
    Result.ID     = GlobalGameState.EntityCount++; // TODO:: Put entities in a list and stuff.
    Result.Health = Health;
    Result.Mass   = Mass;
    Result.Power  = Power;
    
    return Result;
}

internal void 
CalculateShipMovement(entity *Ship, r32 dTime, v3 ForceDir, r32 RotForceDir)
{
    r32 RForce = RotForceDir; // Clamp(RotForceDir, -1.0f, 1.0f);
    v3 Force   = ForceDir;
    if(Force.y != 0 && Force.x != 0) Normalize(&Force);
    
    // Ship Rotation --------------------
    RForce *= 25;
    r32 RAcceleration = (RForce/Ship->Mass) - 8*Ship->RotVelocity;
    Ship->RotVelocity = RAcceleration*dTime + Ship->RotVelocity;
    
    r32 R = Ship->RotVelocity*dTime + Ship->RenderObject->T.Rotation.y;
    
    SetRotation(Ship->RenderObject, 0, R, 0);
    
    // Ship Translation ------------------
    Force *= Ship->Power;
    Force  = RotateAroundY(-Ship->RenderObject->T.Rotation.y)*Force;
    
    v3 Acceleration = (Force/Ship->Mass) - 6*Ship->Velocity;
    Ship->Velocity = Acceleration*dTime + Ship->Velocity;
    
    v3 P = Ship->Velocity*dTime + Ship->RenderObject->T.Translation;
    SetTranslation(Ship->RenderObject, P);
}

internal void
SpawnBullet(entity *Bullet, entity *Ship)
{
    Bullet->RenderObject->Render = true;
    SetTranslation(Bullet->RenderObject, Ship->RenderObject->T.Translation);
    SetRotation(Bullet->RenderObject, Ship->RenderObject->T.Rotation);
    
    v3 P = RotateAroundZ(-Ship->RenderObject->T.Rotation.z)*(V3(0,1,0)*Bullet->Power);
    
    Bullet->Velocity = Ship->Velocity + P;
}

internal b32
CreateBaseRoom(base_room *Room, string_c ModelPath, string_c ImagePath)
{
    if(!LoadOBJFile(&Room->Model, ModelPath.S, &GlobalGameState.FixArena, &GlobalGameState.ScratchArena)) 
    {
        DebugLog(50, "ERROR:: We failed loading OBJ file.\n");
        return false;
    }
    
    Room->Bitmap = LoadImage_STB(ImagePath.S);
    //loaded_bitmap RoomBitmap = LoadBMPImage(&GameState->FixArena, (u8 *)"../Data/Models/576.bmp");
    if(!Room->Bitmap.WasLoaded) 
    {
        DebugLog(50, "ERROR:: We failed loading BMP file.\n");
        return false;
    }
    
    Room->VerticeData = PackVertexData(&GlobalGameState.FixArena, Room->Model.Vertice, 
                                       Room->Model.Normals, Room->Model.TexCoords, 0, Room->Model.Count);
    //Room->IndiceData  = SwizzleIndices({Room->Model.SurfaceGroups[0].Indices, Room->Model.SurfaceGroups[0].IndiceCount});
    Room->IndiceData  = {Room->Model.SurfaceGroups[0].Indices, Room->Model.SurfaceGroups[0].IndiceCount};
    Room->TexInfo     = {Room->Bitmap.Width, Room->Bitmap.Height, Room->Bitmap.Pixels};
    
    return true;
}
























