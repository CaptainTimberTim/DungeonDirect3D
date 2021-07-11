/* date = November 28th 2020 10:43 am */

#ifndef _DIRECT3_D_11__T_D_H
#define _DIRECT3_D_11__T_D_H

struct d3d11_constants
{
    m4 Transform;
    m4 Projection;
    v3 LightVector;
};

struct v3_transform
{
    b32 Changed;
    v3 Rotation;
    v3 Scale;
    v3 Translation;
    
    m4 _Transform;
};

struct d3d11_texture 
{
    ID3D11ShaderResourceView *View;
    ID3D11SamplerState       *SamplerState;
};

struct d3d11_render_object
{
    i32 ID;
    b32 Render;
    ID3D11Buffer *VertexBuffer;
    ID3D11Buffer *IndexBuffer;
    u32 IndexCount;
    
    d3d11_texture Texture;
    v3_transform T;
};

#define MAX_RENDER_OBJECTS 200
struct object_list
{
    d3d11_render_object Objects[MAX_RENDER_OBJECTS];
    u32 Count;
};

struct direct_3d_info
{
    ID3D11Device1            *Device;
    ID3D11DeviceContext1     *DeviceContext;
    ID3D11Buffer             *ConstantBuffer;
    ID3D11RenderTargetView   *FrameBufferView;
    ID3D11DepthStencilView   *DepthBufferView;
    ID3D11InputLayout        *InputLayout;
    ID3D11VertexShader       *VertexShader;
    ID3D11RasterizerState1   *RasterizerState;
    ID3D11PixelShader        *PixelShader;
    
    ID3D11DepthStencilState  *DepthStencilState;
    IDXGISwapChain1          *SwapChain;
    
    r32 DepthBufferWidth;
    r32 DepthBufferHeight;
    r32 NearClipping;
    r32 FarClipping;
    
    object_list ObjectList;
};

struct vertex_array
{
    // Data = float3 position, float3 normal, float2 texcoord, float3 color
    r32 *Data;
    u32 Count;
};

struct vertex_data
{
    v3 Vertex;
    v3 Normal;
    v2 TexCoords;
    v3 Color;
};

struct index_array
{
    u32 *Data;
    u32 Count;
};

struct texture_info
{
    u32 Width;
    u32 Height;
    u32 *Data;
};

internal ID3D11Buffer *CreateVertexData(direct_3d_info *D3D11Info, vertex_array VertexData);
internal ID3D11Buffer *CreateIndexData(direct_3d_info *D3D11Info, index_array IndexData);
internal d3d11_texture CreateTexture(direct_3d_info *D3D11Info, texture_info TextureInfo);
internal d3d11_render_object *CreateRenderObject(direct_3d_info *D3D11Info, vertex_array VertexData, index_array IndexData, texture_info TextureInfo);
internal direct_3d_info InitializeDirect3D(arena_allocator FixArena, arena_allocator ScratchArena, HWND Window);
internal void UpdateTransform(d3d11_render_object *Object);
internal void Direct3DDraw(direct_3d_info *D3D11Info);


inline void SetTransform(d3d11_render_object *Object, v3 Translation, v3 Rotation, v3 Scale);

inline void Translate(d3d11_render_object *Object, v3 T);
inline void Translate(d3d11_render_object *Object, r32 X, r32 Y, r32 Z);
inline void SetTranslation(d3d11_render_object *Object, v3 T);
inline void SetTranslation(d3d11_render_object *Object, r32 X, r32 Y, r32 Z);

inline void Rotate(d3d11_render_object *Object, v3 R);
inline void Rotate(d3d11_render_object *Object, r32 X, r32 Y, r32 Z);
inline void SetRotation(d3d11_render_object *Object, v3 R);
inline void SetRotation(d3d11_render_object *Object, r32 X, r32 Y, r32 Z);

inline void Scale(d3d11_render_object *Object, v3 S);
inline void Scale(d3d11_render_object *Object, r32 X, r32 Y, r32 Z);
inline void SetScale(d3d11_render_object *Object, v3 S);
inline void SetScale(d3d11_render_object *Object, r32 X, r32 Y, r32 Z);

internal vertex_array PackVertexData(arena_allocator *Arena, v3 *Vertice, v3 *Normals, v2 *TexCoords, v3 *Colors, u32 Count);
internal index_array SwizzleIndices(index_array Indices);

// **********************************************************
// Test Object data *****************************************
// **********************************************************
r32 RectVertexData[] = 
{
    /*pos*/ -1.0f, -1.0f, -1.0f, /*norm*/ 0.0f, 0.0f, -1.0f, /*texC*/ 0.0f, 0.0f, /*color*/ 0.5f, 0.6f, 0.5f,
    /*pos*/ -1.0f,  1.0f, -1.0f, /*norm*/ 0.0f, 0.0f, -1.0f, /*texC*/ 0.0f, 1.0f, /*color*/ 0.5f, 0.6f, 0.5f,
    /*pos*/  1.0f,  1.0f, -1.0f, /*norm*/ 0.0f, 0.0f, -1.0f, /*texC*/ 1.0f, 1.0f, /*color*/ 0.5f, 0.6f, 0.5f,
    /*pos*/  1.0f, -1.0f, -1.0f, /*norm*/ 0.0f, 0.0f, -1.0f, /*texC*/ 1.0f, 0.0f, /*color*/ 0.5f, 0.6f, 0.5f,
};

u32 RectIndexData[] = 
{
    0, 1, 3, 
    1, 2, 3,
};

u32 ThingTextureData[] =
{
    0xffffffff, 0xff7f7f7f, 0xffffffff, 0xff7f7f7f,
    0xff7f7f7f, 0xffffffff, 0xff7f7f7f, 0xffffffff,
    0xffffffff, 0xff7f7f7f, 0xffffffff, 0xff7f7f7f,
    0xff7f7f7f, 0xffffffff, 0xff7f7f7f, 0xffffffff,
};

r32 ThingVertexData[] = // float3 position, float3 normal, float2 texcoord, float3 color
{
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    -0.6f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  2.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    0.6f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  8.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 10.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    -0.6f,  0.6f, -1.0f,  0.0f,  0.0f, -1.0f,  2.0f,  2.0f,  0.973f,  0.480f,  0.002f,
    0.6f,  0.6f, -1.0f,  0.0f,  0.0f, -1.0f,  8.0f,  2.0f,  0.973f,  0.480f,  0.002f,
    -0.6f, -0.6f, -1.0f,  0.0f,  0.0f, -1.0f,  2.0f,  8.0f,  0.973f,  0.480f,  0.002f,
    0.6f, -0.6f, -1.0f,  0.0f,  0.0f, -1.0f,  8.0f,  8.0f,  0.973f,  0.480f,  0.002f,
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 10.0f,  0.973f,  0.480f,  0.002f,
    -0.6f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  2.0f, 10.0f,  0.973f,  0.480f,  0.002f,
    0.6f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  8.0f, 10.0f,  0.973f,  0.480f,  0.002f,
    1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 10.0f, 10.0f,  0.973f,  0.480f,  0.002f,
    1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f,  1.0f, -0.6f,  1.0f,  0.0f,  0.0f,  2.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f,  1.0f,  0.6f,  1.0f,  0.0f,  0.0f,  8.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 10.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f,  0.6f, -0.6f,  1.0f,  0.0f,  0.0f,  2.0f,  2.0f,  0.897f,  0.163f,  0.011f,
    1.0f,  0.6f,  0.6f,  1.0f,  0.0f,  0.0f,  8.0f,  2.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -0.6f, -0.6f,  1.0f,  0.0f,  0.0f,  2.0f,  8.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -0.6f,  0.6f,  1.0f,  0.0f,  0.0f,  8.0f,  8.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 10.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -1.0f, -0.6f,  1.0f,  0.0f,  0.0f,  2.0f, 10.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -1.0f,  0.6f,  1.0f,  0.0f,  0.0f,  8.0f, 10.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 10.0f, 10.0f,  0.897f,  0.163f,  0.011f,
    1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    0.6f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  2.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -0.6f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  8.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 10.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    0.6f,  0.6f,  1.0f,  0.0f,  0.0f,  1.0f,  2.0f,  2.0f,  0.612f,  0.000f,  0.069f,
    -0.6f,  0.6f,  1.0f,  0.0f,  0.0f,  1.0f,  8.0f,  2.0f,  0.612f,  0.000f,  0.069f,
    0.6f, -0.6f,  1.0f,  0.0f,  0.0f,  1.0f,  2.0f,  8.0f,  0.612f,  0.000f,  0.069f,
    -0.6f, -0.6f,  1.0f,  0.0f,  0.0f,  1.0f,  8.0f,  8.0f,  0.612f,  0.000f,  0.069f,
    1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 10.0f,  0.612f,  0.000f,  0.069f,
    0.6f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  2.0f, 10.0f,  0.612f,  0.000f,  0.069f,
    -0.6f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  8.0f, 10.0f,  0.612f,  0.000f,  0.069f,
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 10.0f, 10.0f,  0.612f,  0.000f,  0.069f,
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f,  1.0f,  0.6f, -1.0f,  0.0f,  0.0f,  2.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f,  1.0f, -0.6f, -1.0f,  0.0f,  0.0f,  8.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 10.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f,  0.6f,  0.6f, -1.0f,  0.0f,  0.0f,  2.0f,  2.0f,  0.127f,  0.116f,  0.408f,
    -1.0f,  0.6f, -0.6f, -1.0f,  0.0f,  0.0f,  8.0f,  2.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -0.6f,  0.6f, -1.0f,  0.0f,  0.0f,  2.0f,  8.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -0.6f, -0.6f, -1.0f,  0.0f,  0.0f,  8.0f,  8.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 10.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -1.0f,  0.6f, -1.0f,  0.0f,  0.0f,  2.0f, 10.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -1.0f, -0.6f, -1.0f,  0.0f,  0.0f,  8.0f, 10.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 10.0f, 10.0f,  0.127f,  0.116f,  0.408f,
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  2.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  8.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 10.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  1.0f,  0.6f,  0.0f,  1.0f,  0.0f,  2.0f,  2.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  1.0f,  0.6f,  0.0f,  1.0f,  0.0f,  8.0f,  2.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  1.0f, -0.6f,  0.0f,  1.0f,  0.0f,  2.0f,  8.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  1.0f, -0.6f,  0.0f,  1.0f,  0.0f,  8.0f,  8.0f,  0.000f,  0.254f,  0.637f,
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 10.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  2.0f, 10.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  8.0f, 10.0f,  0.000f,  0.254f,  0.637f,
    1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 10.0f, 10.0f,  0.000f,  0.254f,  0.637f,
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  2.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  8.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 10.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -1.0f, -0.6f,  0.0f, -1.0f,  0.0f,  2.0f,  2.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -1.0f, -0.6f,  0.0f, -1.0f,  0.0f,  8.0f,  2.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -1.0f,  0.6f,  0.0f, -1.0f,  0.0f,  2.0f,  8.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -1.0f,  0.6f,  0.0f, -1.0f,  0.0f,  8.0f,  8.0f,  0.001f,  0.447f,  0.067f,
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 10.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  2.0f, 10.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  8.0f, 10.0f,  0.001f,  0.447f,  0.067f,
    1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 10.0f, 10.0f,  0.001f,  0.447f,  0.067f,
    -0.6f,  0.6f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    -0.6f,  0.6f, -0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    -0.6f, -0.6f, -0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    -0.6f, -0.6f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    0.6f,  0.6f, -0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    0.6f,  0.6f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    0.6f, -0.6f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    0.6f, -0.6f, -0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    -0.6f, -0.6f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    -0.6f, -0.6f, -0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    0.6f, -0.6f, -0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    0.6f, -0.6f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    -0.6f,  0.6f, -0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    -0.6f,  0.6f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    0.6f,  0.6f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    0.6f,  0.6f, -0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.973f,  0.480f,  0.002f,
    1.0f,  0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    0.6f,  0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    0.6f, -0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    0.6f,  0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f,  0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    0.6f, -0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f,  0.6f,  0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    0.6f,  0.6f,  0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    0.6f,  0.6f, -0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f,  0.6f, -0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    0.6f, -0.6f,  0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -0.6f,  0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    1.0f, -0.6f, -0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    0.6f, -0.6f, -0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.897f,  0.163f,  0.011f,
    0.6f,  0.6f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    0.6f,  0.6f,  0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    0.6f, -0.6f,  0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    0.6f, -0.6f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -0.6f,  0.6f,  0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -0.6f,  0.6f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -0.6f, -0.6f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -0.6f, -0.6f,  0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    0.6f, -0.6f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    0.6f, -0.6f,  0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -0.6f, -0.6f,  0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -0.6f, -0.6f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    0.6f,  0.6f,  0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    0.6f,  0.6f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -0.6f,  0.6f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -0.6f,  0.6f,  0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.612f,  0.000f,  0.069f,
    -1.0f,  0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -0.6f,  0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -0.6f, -0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -0.6f,  0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f,  0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -0.6f, -0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -0.6f,  0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -0.6f, -0.6f,  0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -0.6f, -0.6f, -0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f, -0.6f, -0.6f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -0.6f,  0.6f,  0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f,  0.6f,  0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -1.0f,  0.6f, -0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -0.6f,  0.6f, -0.6f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.127f,  0.116f,  0.408f,
    -0.6f,  1.0f,  0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  0.6f,  0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  0.6f, -0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  1.0f, -0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  0.6f,  0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  1.0f,  0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  1.0f, -0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  0.6f, -0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  1.0f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  1.0f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f,  1.0f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  1.0f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    0.6f,  0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.000f,  0.254f,  0.637f,
    -0.6f, -0.6f,  0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -1.0f,  0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -1.0f, -0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -0.6f, -0.6f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -1.0f,  0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -0.6f,  0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -0.6f, -0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -1.0f, -0.6f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -1.0f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -1.0f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -0.6f, -0.6f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -1.0f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    -0.6f, -0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -0.6f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
    0.6f, -1.0f,  0.6f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.001f,  0.447f,  0.067f,
};

u32 ThingIndexData[] =
{
    0,   1,   9,   9,   8,   0,   1,   2,   5,   5,   4,   1,   6,   7,  10,  10,   9,   6,   2,   3,  11,  11,  10,   2,
    12,  13,  21,  21,  20,  12,  13,  14,  17,  17,  16,  13,  18,  19,  22,  22,  21,  18,  14,  15,  23,  23,  22,  14,
    24,  25,  33,  33,  32,  24,  25,  26,  29,  29,  28,  25,  30,  31,  34,  34,  33,  30,  26,  27,  35,  35,  34,  26,
    36,  37,  45,  45,  44,  36,  37,  38,  41,  41,  40,  37,  42,  43,  46,  46,  45,  42,  38,  39,  47,  47,  46,  38,
    48,  49,  57,  57,  56,  48,  49,  50,  53,  53,  52,  49,  54,  55,  58,  58,  57,  54,  50,  51,  59,  59,  58,  50,
    60,  61,  69,  69,  68,  60,  61,  62,  65,  65,  64,  61,  66,  67,  70,  70,  69,  66,  62,  63,  71,  71,  70,  62,
    72,  73,  74,  74,  75,  72,  76,  77,  78,  78,  79,  76,  80,  81,  82,  82,  83,  80,  84,  85,  86,  86,  87,  84,
    88,  89,  90,  90,  91,  88,  92,  93,  94,  94,  95,  92,  96,  97,  98,  98,  99,  96, 100, 101, 102, 102, 103, 100,
    104, 105, 106, 106, 107, 104, 108, 109, 110, 110, 111, 108, 112, 113, 114, 114, 115, 112, 116, 117, 118, 118, 119, 116,
    120, 121, 122, 122, 123, 120, 124, 125, 126, 126, 127, 124, 128, 129, 130, 130, 131, 128, 132, 133, 134, 134, 135, 132,
    136, 137, 138, 138, 139, 136, 140, 141, 142, 142, 143, 140, 144, 145, 146, 146, 147, 144, 148, 149, 150, 150, 151, 148,
    152, 153, 154, 154, 155, 152, 156, 157, 158, 158, 159, 156, 160, 161, 162, 162, 163, 160, 164, 165, 166, 166, 167, 164,
};

#endif //_DIRECT3_D_11__T_D_H
