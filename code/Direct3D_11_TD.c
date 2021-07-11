#include "Direct3D_11_TD.h"

internal ID3D11Buffer *
CreateVertexData(direct_3d_info *D3D11Info, vertex_array VertexData)
{
    ID3D11Buffer *Result = 0;
    
    D3D11_BUFFER_DESC VertexBufferDesc = {};
    VertexBufferDesc.ByteWidth = sizeof(r32)*VertexData.Count;
    VertexBufferDesc.Usage     = D3D11_USAGE_IMMUTABLE;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    
    D3D11_SUBRESOURCE_DATA D3D11VertexData = { VertexData.Data };
    
    D3D11Info->Device->CreateBuffer(&VertexBufferDesc, &D3D11VertexData, &Result);
    return Result;
}

internal ID3D11Buffer *
CreateIndexData(direct_3d_info *D3D11Info, index_array IndexData)
{
    ID3D11Buffer *Result = 0;
    
    D3D11_BUFFER_DESC IndexBufferDesc = {};
    IndexBufferDesc.ByteWidth = sizeof(u32)*IndexData.Count;
    IndexBufferDesc.Usage     = D3D11_USAGE_IMMUTABLE;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    
    D3D11_SUBRESOURCE_DATA D3D11IndexData = { IndexData.Data };
    
    D3D11Info->Device->CreateBuffer(&IndexBufferDesc, &D3D11IndexData, &Result);
    
    return Result;
}

internal d3d11_texture
CreateTexture(direct_3d_info *D3D11Info, texture_info TextureInfo)
{
    d3d11_texture Result = {};
    
    D3D11_TEXTURE2D_DESC TextureDesc = {};
    TextureDesc.Width              = TextureInfo.Width;  // width in texels
    TextureDesc.Height             = TextureInfo.Height; // height in texels
    TextureDesc.MipLevels          = 1;
    TextureDesc.ArraySize          = 1;
    TextureDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    TextureDesc.SampleDesc.Count   = 1;
    TextureDesc.Usage              = D3D11_USAGE_IMMUTABLE;
    TextureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    
    D3D11_SUBRESOURCE_DATA D3D11TextureData = {};
    D3D11TextureData.pSysMem            = TextureInfo.Data;
    D3D11TextureData.SysMemPitch        = TextureInfo.Width * sizeof(u32); // 4 bytes per pixel
    
    ID3D11Texture2D* Texture;
    
    D3D11Info->Device->CreateTexture2D(&TextureDesc, &D3D11TextureData, &Texture);
    
    D3D11Info->Device->CreateShaderResourceView(Texture, 0, &Result.View);
    
    D3D11_SAMPLER_DESC SamplerDesc = {};
    SamplerDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT;
    SamplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    
    D3D11Info->Device->CreateSamplerState(&SamplerDesc, &Result.SamplerState);
    
    return Result;
}

internal d3d11_render_object *
CreateRenderObject(direct_3d_info *D3D11Info, vertex_array VertexData, index_array IndexData, texture_info TextureInfo)
{
    Assert(D3D11Info->ObjectList.Count < MAX_RENDER_OBJECTS);
    d3d11_render_object *Result = D3D11Info->ObjectList.Objects+D3D11Info->ObjectList.Count++;
    
    Result->ID = D3D11Info->ObjectList.Count-1;
    
    Result->VertexBuffer = CreateVertexData(D3D11Info, VertexData);
    Result->IndexBuffer  = CreateIndexData(D3D11Info, IndexData);
    Result->Texture      = CreateTexture(D3D11Info, TextureInfo);
    
    Result->IndexCount    = IndexData.Count;
    Result->T.Rotation    = { 0.0f, 0.0f, 0.0f };
    Result->T.Scale       = { 1.0f, 1.0f, 1.0f };
    Result->T.Translation = { 0.0f, 0.0f, 25.0f };
    Result->T.Changed     = true;
    Result->Render        = true;
    
    return Result;
}

internal direct_3d_info
InitializeDirect3D(arena_allocator FixArena, arena_allocator ScratchArena, HWND Window)
{
    direct_3d_info Result = {};
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    
    ID3D11Device*        BaseDevice;
    ID3D11DeviceContext* BaseDeviceContext;
    
    D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT, FeatureLevels,
                      ARRAYSIZE(FeatureLevels), D3D11_SDK_VERSION, &BaseDevice, 0, &BaseDeviceContext);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    BaseDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&Result.Device));
    
    BaseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&Result.DeviceContext));
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    IDXGIDevice1* DxgiDevice;
    
    Result.Device->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&DxgiDevice));
    
    IDXGIAdapter* DxgiAdapter;
    
    DxgiDevice->GetAdapter(&DxgiAdapter);
    
    IDXGIFactory2* DxgiFactory;
    
    DxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&DxgiFactory));
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
    SwapChainDesc.Width              = 0; // use window width
    SwapChainDesc.Height             = 0; // use window height
    SwapChainDesc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    SwapChainDesc.Stereo             = FALSE;
    SwapChainDesc.SampleDesc.Count   = 1;
    SwapChainDesc.SampleDesc.Quality = 0;
    SwapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount        = 2;
    SwapChainDesc.Scaling            = DXGI_SCALING_STRETCH;
    SwapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;
    SwapChainDesc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
    SwapChainDesc.Flags              = 0;
    
    DxgiFactory->CreateSwapChainForHwnd(Result.Device, Window, &SwapChainDesc, 0, 0, &Result.SwapChain);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    ID3D11Texture2D* FrameBuffer;
    
    Result.SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&FrameBuffer));
    
    Result.Device->CreateRenderTargetView(FrameBuffer, 0, &Result.FrameBufferView);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    D3D11_TEXTURE2D_DESC DepthBufferDesc;
    
    FrameBuffer->GetDesc(&DepthBufferDesc); // base on framebuffer properties
    DepthBufferDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    
    ID3D11Texture2D* DepthBuffer = 0;
    Result.Device->CreateTexture2D(&DepthBufferDesc, 0, &DepthBuffer);
    
    D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
    //DepthStencilViewDesc.Format             = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    DepthStencilViewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DepthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    DepthStencilViewDesc.Texture2D.MipSlice = 0;
    
    // Create the depth stencil view
    Result.Device->CreateDepthStencilView(DepthBuffer, // Depth stencil texture
                                          &DepthStencilViewDesc, // Depth stencil desc
                                          &Result.DepthBufferView);  // [out] Depth stencil view
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    ID3DBlob* VsBlob;
    D3DCompileFromFile(L"..\\code\\Shaders\\BaseShaders.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &VsBlob, nullptr);
    
    Result.Device->CreateVertexShader(VsBlob->GetBufferPointer(), VsBlob->GetBufferSize(), 0, &Result.VertexShader);
    
    D3D11_INPUT_ELEMENT_DESC InputElementDesc[] = // float3 position, float3 normal, float2 texcoord, float3 color
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
    Result.Device->CreateInputLayout(InputElementDesc, ARRAYSIZE(InputElementDesc), VsBlob->GetBufferPointer(), VsBlob->GetBufferSize(), &Result.InputLayout);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    ID3DBlob* PsBlob;
    D3DCompileFromFile(L"..\\code\\Shaders\\BaseShaders.hlsl", 0, 0, "ps_main", "ps_5_0", 0, 0, &PsBlob, 0);
    
    Result.Device->CreatePixelShader(PsBlob->GetBufferPointer(), PsBlob->GetBufferSize(), 0, &Result.PixelShader);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    D3D11_RASTERIZER_DESC1 RasterizerDesc = {};
    RasterizerDesc.FillMode = D3D11_FILL_SOLID;
    RasterizerDesc.CullMode = D3D11_CULL_BACK;
    //RasterizerDesc.CullMode = D3D11_CULL_NONE;
    
    Result.Device->CreateRasterizerState1(&RasterizerDesc, &Result.RasterizerState);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
    DepthStencilDesc.DepthEnable    = TRUE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS;
    
    // Stencil test parameters
    DepthStencilDesc.StencilEnable    = false;
    DepthStencilDesc.StencilReadMask  = 0xFF;
    DepthStencilDesc.StencilWriteMask = 0xFF;
    
    // Stencil operations if pixel is front-facing
    DepthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
    DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    DepthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
    DepthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
    
    // Stencil operations if pixel is back-facing
    DepthStencilDesc.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
    DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    DepthStencilDesc.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
    DepthStencilDesc.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
    
    Result.Device->CreateDepthStencilState(&DepthStencilDesc, &Result.DepthStencilState);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    D3D11_BUFFER_DESC ConstantBufferDesc = {};
    ConstantBufferDesc.ByteWidth      = sizeof(d3d11_constants) + 0xf & 0xfffffff0;
    ConstantBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
    ConstantBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    Result.Device->CreateBuffer(&ConstantBufferDesc, 0, &Result.ConstantBuffer);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    Result.DepthBufferWidth  = static_cast<r32>(DepthBufferDesc.Width);  // width
    Result.DepthBufferHeight = static_cast<r32>(DepthBufferDesc.Height); // height
    Result.NearClipping      = 1000.0f;                                    // near
    Result.FarClipping       = 1000000.0f;                                 // far
    
    return Result;
}

internal void
UpdateTransform(d3d11_render_object *Object)
{
    if(Object->T.Changed)
    {
        Object->T.Changed = false;
        m4 RotateX = 
        { 
            1, 0, 0, 0, 
            0, (r32)(cos(Object->T.Rotation.x)), -(r32)(sin(Object->T.Rotation.x)), 0, 
            0, (r32)(sin(Object->T.Rotation.x)), (r32)(cos(Object->T.Rotation.x)), 0, 
            0, 0, 0, 1 
        };
        m4 RotateY = 
        { 
            (r32)(cos(Object->T.Rotation.y)), 0, (r32)(sin(Object->T.Rotation.y)), 0, 
            0, 1, 0, 0, 
            -(r32)(sin(Object->T.Rotation.y)), 0, (r32)(cos(Object->T.Rotation.y)), 0, 
            0, 0, 0, 1 
        };
        m4 RotateZ = 
        { 
            (r32)(cos(Object->T.Rotation.z)), -(r32)(sin(Object->T.Rotation.z)), 0, 0, 
            (r32)(sin(Object->T.Rotation.z)), (r32)(cos(Object->T.Rotation.z)), 0, 0, 
            0, 0, 1, 0, 
            0, 0, 0, 1 
        };
        m4 Scale     = 
        { 
            Object->T.Scale.x, 0, 0, 0, 
            0, Object->T.Scale.y, 0, 0, 
            0, 0, Object->T.Scale.z, 0, 
            0, 0, 0, 1 
        };
        m4 Translate = 
        { 
            1, 0, 0, 0, 
            0, 1, 0, 0, 
            0, 0, 1, 0, 
            Object->T.Translation.x, Object->T.Translation.y, Object->T.Translation.z, 1 
        };
        
        Object->T._Transform = RotateX * RotateY * RotateZ * Scale * Translate;
    }
}

internal void
Direct3DDraw(direct_3d_info *D3D11Info, d3d11_render_object *Camera)
{
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    
    FLOAT BackgroundColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f};
    
    UINT Stride = 11 * 4; // vertex size (11 floats: float3 position, float3 normal, float2 texcoord, float3 color)
    UINT Offset = 0;
    
    D3D11_VIEWPORT Viewport = { 0.0f, 0.0f, D3D11Info->DepthBufferWidth, D3D11Info->DepthBufferHeight, 0.0f, 1.0f };
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    D3D11Info->DeviceContext->ClearRenderTargetView(D3D11Info->FrameBufferView, BackgroundColor);
    D3D11Info->DeviceContext->ClearDepthStencilView(D3D11Info->DepthBufferView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 
                                                    1.0f, 0);
    
    D3D11Info->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    D3D11Info->DeviceContext->IASetInputLayout(D3D11Info->InputLayout);
    
    D3D11Info->DeviceContext->OMSetRenderTargets(1, &D3D11Info->FrameBufferView, D3D11Info->DepthBufferView);
    D3D11Info->DeviceContext->OMSetDepthStencilState(D3D11Info->DepthStencilState, 1);
    D3D11Info->DeviceContext->OMSetBlendState(0, 0, 0xffffffff); // use default blend mode (i.e. disable)
    
    D3D11Info->DeviceContext->VSSetShader(D3D11Info->VertexShader, 0, 0);
    D3D11Info->DeviceContext->VSSetConstantBuffers(0, 1, &D3D11Info->ConstantBuffer);
    
    D3D11Info->DeviceContext->RSSetViewports(1, &Viewport);
    D3D11Info->DeviceContext->RSSetState(D3D11Info->RasterizerState);
    
    D3D11Info->DeviceContext->PSSetShader(D3D11Info->PixelShader, 0, 0);
    
    m4 Projection = 
    {
        2 * D3D11Info->NearClipping / D3D11Info->DepthBufferWidth, 0, 0, 0, 
        0, 2 * D3D11Info->NearClipping / D3D11Info->DepthBufferHeight, 0, 0, 
        0, 0, D3D11Info->FarClipping / (D3D11Info->FarClipping - D3D11Info->NearClipping), 1, 
        0, 0, D3D11Info->NearClipping * D3D11Info->FarClipping / (D3D11Info->NearClipping - D3D11Info->FarClipping), 0 
    };
    m4 CamMatrix = Invert(&Camera->T._Transform);
    For(D3D11Info->ObjectList.Count)
    {
        d3d11_render_object *Object = D3D11Info->ObjectList.Objects+It;
        if(!Object->Render) continue;
        
        D3D11_MAPPED_SUBRESOURCE MappedSubresource;
        D3D11Info->DeviceContext->Map(D3D11Info->ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource);
        
        d3d11_constants *Constants = reinterpret_cast<d3d11_constants *>(MappedSubresource.pData);
        Constants->Transform   = Object->T._Transform*CamMatrix;
        Constants->Projection  = Projection;
        Constants->LightVector = { 1.0f, -1.0f, 1.0f };
        
        D3D11Info->DeviceContext->Unmap(D3D11Info->ConstantBuffer, 0);
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        
        D3D11Info->DeviceContext->IASetVertexBuffers(0, 1, &Object->VertexBuffer, &Stride, &Offset);
        D3D11Info->DeviceContext->IASetIndexBuffer(Object->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        
        D3D11Info->DeviceContext->PSSetShaderResources(0, 1, &Object->Texture.View);
        D3D11Info->DeviceContext->PSSetSamplers(0, 1, &Object->Texture.SamplerState);
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        
        D3D11Info->DeviceContext->DrawIndexed(Object->IndexCount, 0, 0);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    
    D3D11Info->SwapChain->Present(1, 0);
}


// Transform functions **************************************************************************************

inline void
SetTransform(d3d11_render_object *Object, v3 Translation, v3 Rotation, v3 Scale)
{
    Object->T.Translation = Translation;
    Object->T.Rotation    = Rotation;
    Object->T.Scale       = Scale;
    
    Object->T.Changed     = true;
}

inline void
Translate(d3d11_render_object *Object, v3 T)
{
    Object->T.Translation += T;
    Object->T.Changed = true;
}

inline void
Translate(d3d11_render_object *Object, r32 X, r32 Y, r32 Z)
{
    Object->T.Translation += V3(X,Y,Z);
    Object->T.Changed = true;
}

inline void
SetTranslation(d3d11_render_object *Object, v3 T)
{
    Object->T.Translation = T;
    Object->T.Changed = true;
}

inline void
SetTranslation(d3d11_render_object *Object, r32 X, r32 Y, r32 Z)
{
    Object->T.Translation = V3(X,Y,Z);
    Object->T.Changed = true;
}

inline void
Rotate(d3d11_render_object *Object, v3 R)
{
    Object->T.Rotation += R;
    Object->T.Changed = true;
}

inline void
Rotate(d3d11_render_object *Object, r32 X, r32 Y, r32 Z)
{
    Object->T.Rotation += V3(X,Y,Z);
    Object->T.Changed = true;
}

inline void
SetRotation(d3d11_render_object *Object, v3 R)
{
    Object->T.Rotation = R;
    Object->T.Changed = true;
}

inline void
SetRotation(d3d11_render_object *Object, r32 X, r32 Y, r32 Z)
{
    Object->T.Rotation = V3(X,Y,Z);
    Object->T.Changed = true;
}

inline void
Scale(d3d11_render_object *Object, v3 S)
{
    Object->T.Scale += S;
    Object->T.Changed = true;
}

inline void
Scale(d3d11_render_object *Object, r32 X, r32 Y, r32 Z)
{
    Object->T.Scale += V3(X,Y,Z);
    Object->T.Changed = true;
}

inline void
SetScale(d3d11_render_object *Object, v3 S)
{
    Object->T.Scale = S;
    Object->T.Changed = true;
}

inline void
SetScale(d3d11_render_object *Object, r32 X, r32 Y, r32 Z)
{
    Object->T.Scale = V3(X,Y,Z);
    Object->T.Changed = true;
}

internal vertex_array
PackVertexData(arena_allocator *Arena, v3 *Vertice, v3 *Normals, v2 *TexCoords, v3 *Colors, u32 Count)
{
    vertex_data *Data = AllocateArray(Arena, Count, vertex_data);
    Assert(Vertice);
    Assert(Normals);
    
    For(Count)
    {
        Data[It].Vertex = Vertice[It];
        Data[It].Normal = Normals[It];
        if(TexCoords) Data[It].TexCoords = TexCoords[It];
        else          Data[It].TexCoords = V2(0);
        if(Colors)    Data[It].Color = Colors[It];
        else          Data[It].Color = V3(1);
    }
    
    vertex_array Result = {(r32 *)Data, Count*11};
    return Result;
}

internal index_array
SwizzleIndices(index_array Indices)
{
    For(Indices.Count/3)
    {
        i32 I = It*3;
        u32 Tmp = Indices.Data[I+1];
        Indices.Data[I+1] = Indices.Data[I+2];
        Indices.Data[I+2] = Tmp;
    }
    
    return Indices;
}
























