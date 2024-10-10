#include "Cube.h"
#include <D3DX11.h>
#include "../../CoreApp/Renderer.h"

IMPLEMENT_RENDER

Cube::Cube()
{
    wmp = std::make_unique<ConstantBuffer_WMP>();
    worldMatrix = XMMatrixIdentity();
}

Cube::Cube(float x, float y, float z)
{
    worldMatrix = XMMatrixIdentity();
    
    worldMatrix *= XMMatrixTranslation(x,y,z);
}

Cube::Cube(const char* TexturePath)
{
    wmp = std::make_unique<ConstantBuffer_WMP>();
    worldMatrix = XMMatrixIdentity();

    this->TexturePath = TexturePath;
}

Cube::Cube(float x, float y, float z, const char* TexturePath)
{
    wmp = std::make_unique<ConstantBuffer_WMP>();
    worldMatrix = XMMatrixIdentity();

    this->TexturePath = TexturePath;

    worldMatrix = XMMatrixTranslation(x,y,z);
}

Cube::Cube(float x, float y, float z, ID3D11ShaderResourceView* Texture)
{
    wmp = std::make_unique<ConstantBuffer_WMP>();
    worldMatrix = XMMatrixIdentity();

    this->Texture = Texture;

    worldMatrix = XMMatrixTranslation(x, y, z);
}

Cube::~Cube()
{
    if (lay)
        lay->Release();

    if (vs)
        vs->Release();

    if (ps)
        ps->Release();

    if (VertexBuffer)
        VertexBuffer->Release();

    if (vsBlob)
        vsBlob->Release();

    if (psBlob)
        psBlob->Release();

    if (wmp)
        wmp.release();

    if (Transparency)
        Transparency->Release();
}
bool Cube::Initialize()
{
    dev = RendererHelper::GetDevice();
    devcon = RendererHelper::GetDeviceContext();
    
    Vertex vertices[24] = {
        // Front Face
        Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
        Vertex(-1.0f,  1.0f, -1.0f,0.0f, 0.0f),
        Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
        Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

        // Back Face
        Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
        Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
        Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
        Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

        // Top Face
        Vertex(-1.0f, +1.0f, -1.0f, +0.0f, +1.0f),//8
        Vertex(-1.0f, +1.0f, +1.0f, +0.0f, +0.0f),
        Vertex(+1.0f, +1.0f, -1.0f, +1.0f, +1.0f),
        Vertex(+1.0f, +1.0f, +1.0f, +1.0f, +0.0f),
        
        // Bottom Face
        Vertex(-1.0f, -1.0f, -1.0f, +0.0f, +1.0f),//8
        Vertex(-1.0f, -1.0f, +1.0f, +0.0f, +0.0f),
        Vertex(+1.0f, -1.0f, -1.0f, +1.0f, +1.0f),
        Vertex(+1.0f, -1.0f, +1.0f, +1.0f, +0.0f),

        //Left Face
        Vertex(-1.0f, +1.0f, -1.0f, +0.0f, 1.0f),//16
        Vertex(-1.0f, -1.0f, -1.0f, +0.0f, 0.0f),
        Vertex(-1.0f, +1.0f, +1.0f, +1.0f, 1.0f),
        Vertex(-1.0f, -1.0f, +1.0f, +1.0f, 0.0f),

        //Right Face
        Vertex(+1.0f, +1.0f, -1.0f, +0.0f, 1.0f),//16
        Vertex(+1.0f, -1.0f, -1.0f, +0.0f, 0.0f),
        Vertex(+1.0f, +1.0f, +1.0f, +1.0f, 1.0f),
        Vertex(+1.0f, -1.0f, +1.0f, +1.0f, 0.0f),
    };

    int indices[IndexCount] = {
        // Front Face
        0,  1,  2,
        2,  0,  3,

        // Back Face
        4,  5,  6,
        4,  6,  7,

        //Top
        8, 9, 10,
        9, 10, 11,

        //Bottom
        12, 13, 14,
        13, 14, 15,

        //Left
        16, 17, 18,
        17, 18, 19,

        //Right
        20, 21, 22,
        21, 22, 23
    };

    CompileShader();
    
    //Initialize the resource and add the vertecies
    D3D11_SUBRESOURCE_DATA vertexData;
    ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
    vertexData.pSysMem = vertices;
    

    //Initialize the resource and add the indicies
    D3D11_SUBRESOURCE_DATA indexData;
    ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
    indexData.pSysMem = indices;
    

    //Create the input layout that is gonna defines how the buffer send
    HRESULT hr = dev->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &lay);

    ENDSTR(hr, "Failed to create input layout!");

    devcon->IASetInputLayout(lay);

    if (!CreateBuffers(vertexData, indexData))
        ENDSTR(E_FAIL,"Failed to create buffers! The object will not be drawen!!");

    if (!InitializeTexture())
        ENDSTR(E_FAIL,"Failed to initialize textures!");

    delete layoutDesc;

    return true;
}

void Cube::Render()
{
    bool noRender = 
        !devcon ||
        !IndexBuffer ||
        !VertexBuffer ||
        !vs || !ps ||
        wmp == nullptr;

    if (noRender)
        return;

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    wmp->WMP = XMMatrixTranspose(worldMatrix * (*viewMatrix) * (*projMatrix));

    if (Texture && samplerState)
    {
        devcon->PSSetShaderResources(0,1,&Texture);
        devcon->PSSetSamplers(0,1,&samplerState);
    }

    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    devcon->UpdateSubresource(WVPBuffer,0,nullptr,wmp.get(), 0, 0);
    devcon->VSSetConstantBuffers(0,1,&WVPBuffer);
    devcon->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
    devcon->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT,0);
    if (Rasterizer)
        devcon->RSSetState(Rasterizer);
    devcon->OMSetDepthStencilState(DepthState,0);
    //devcon->OMSetBlendState(Transparency,nullptr, 0xffffffff);
    devcon->VSSetShader(vs, NULL, 0);
    devcon->PSSetShader(ps, NULL, 0);
    devcon->DrawIndexed(IndexCount, 0,0);
}

void Cube::Update(float t)
{
}

void Cube::Shutdown()
{
    if(lay)
        lay->Release();

    if (vs)
        vs->Release();

    if (ps)
        ps->Release();

    if (VertexBuffer)
        VertexBuffer->Release();

    if (vsBlob)
        vsBlob->Release();

    if (psBlob)
        psBlob->Release();

    if(wmp)
        wmp.release();

    if (Transparency)
        Transparency->Release();

    if (DepthState)
        DepthState->Release();
}

bool Cube::InitializeTexture()
{
    HRESULT hr = S_OK;

    if(Texture == nullptr)
        hr = D3DX11CreateShaderResourceViewFromFileA(dev,TexturePath,NULL,NULL,&Texture,NULL);

    ENDSTR(hr, "Failed to create a shader resource!");

    D3D11_SAMPLER_DESC SamplerDesc;
    ZeroMemory(&SamplerDesc,sizeof(D3D11_SAMPLER_DESC));

    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = dev->CreateSamplerState(&SamplerDesc,&samplerState);

    ENDSTR(hr,"Failed to create a sampler state");

    SetBlending();

    return true;
}

void Cube::PassMatrices(Matrix* view, Matrix* proj)
{
    this->viewMatrix = view;
    this->projMatrix = proj;
}

void Cube::CompileShader()
{
    if (!CompileTextureShader())
    {
        DX_ERROR("Failed to create a texture shader! Color shader is using instead");
        CompileColorShader();
    }
}

bool Cube::CompileColorShader()
{
    layoutDesc = new D3D11_INPUT_ELEMENT_DESC[2]
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    //Before using vertex shader, compile file
    ID3D10Blob* errorBlob = nullptr;
    HRESULT hr = D3DX11CompileFromFile("ColorShader.hlsl", nullptr, nullptr, "VS", "vs_4_0", D3D10_SHADER_DEBUG, 0, nullptr, &vsBlob, &errorBlob, &hr);

    //Some error handling
    if (FAILED(hr))
        if (errorBlob)
        {
            char* compileErrors = static_cast<char*>(errorBlob->GetBufferPointer());
            std::cerr << "Shader compilation failed with error:\n" << compileErrors << std::endl;

            return false;
        }
        else
            ENDSTR(hr, "HELP");

    //Create the vertex shader
    hr = dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vs);
    ENDSTR(hr, "Failed to create vertex shader!");


    //Before using vertex shader, compile file
    D3DX11CompileFromFile("ColorShader.hlsl", 0, 0, "PS", "ps_4_0", 0, 0, 0, &psBlob, 0, &hr);
    ENDSTR(hr, "Failed to compile the pixel shader file!");

    //Create the pixel shader
    hr = dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &ps);
    ENDSTR(hr, "Failed to create pixel shader!");

    return true;
}

bool Cube::CompileTextureShader()
{
    layoutDesc = new D3D11_INPUT_ELEMENT_DESC[2]
    {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    //Before using vertex shader, compile file
    ID3D10Blob* errorBlob = nullptr;
    HRESULT hr = D3DX11CompileFromFile("TextureShader.hlsl", nullptr, nullptr, "VS", "vs_4_0", D3D10_SHADER_DEBUG, 0, nullptr, &vsBlob, &errorBlob, &hr);

    //Some error handling
    if (FAILED(hr))
        if (errorBlob)
        {
            char* compileErrors = static_cast<char*>(errorBlob->GetBufferPointer());
            std::cerr << "Shader compilation failed with error:\n" << compileErrors << std::endl;

            return false;
        }
        else
            ENDSTR(hr, "HELP");

    //Create the vertex shader
    hr = dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vs);
    ENDSTR(hr, "Failed to create vertex shader!");


    //Before using vertex shader, compile file
    D3DX11CompileFromFile("TextureShader.hlsl", 0, 0, "PS", "ps_4_0", 0, 0, 0, &psBlob, 0, &hr);
    ENDSTR(hr, "Failed to compile the pixel shader file!");

    //Create the pixel shader
    hr = dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &ps);
    ENDSTR(hr, "Failed to create pixel shader!");

    return true;
}

void Cube::SetBlending()
{
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc,sizeof(D3D11_BLEND_DESC));

    D3D11_RENDER_TARGET_BLEND_DESC renderDesc;
    ZeroMemory(&renderDesc,sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
    renderDesc.BlendEnable = true;
    //Equation for color
    renderDesc.SrcBlend = D3D11_BLEND_SRC_COLOR;
    renderDesc.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    renderDesc.BlendOp = D3D11_BLEND_OP_ADD;

    //Equation for alpha
    renderDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    renderDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
    renderDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    renderDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0] = renderDesc;

    HRESULT hr = dev->CreateBlendState(&blendDesc,&Transparency);

    if(FAILED(hr))
        DX_WARN("Failed to create blendstate!");


    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = true;
    rasterDesc.DepthClipEnable = true;

    
    dev->CreateRasterizerState(&rasterDesc,&Rasterizer);
    
    D3D11_DEPTH_STENCIL_DESC depthdesc;
    ZeroMemory(&depthdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthdesc.DepthEnable = true;
    depthdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    dev->CreateDepthStencilState(&depthdesc,&DepthState);
}


bool Cube::CreateBuffers(D3D11_SUBRESOURCE_DATA vertexData, D3D11_SUBRESOURCE_DATA indexData)
{
    HRESULT hr = S_OK;

    //Create the vertex buffer
    D3D11_BUFFER_DESC VertexDesc;
    ZeroMemory(&VertexDesc, sizeof(D3D11_BUFFER_DESC));

    VertexDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexDesc.ByteWidth = sizeof(Vertex) * IndexCount;
    VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    if(dev)
    hr = dev->CreateBuffer(
        &VertexDesc, 
        &vertexData, 
        &VertexBuffer);

    ENDSTR(hr, "Failed to create vertex buffer!")


    //Initialize the index buffer descriptipn
    D3D11_BUFFER_DESC IndexDesc;
    ZeroMemory(&IndexDesc, sizeof(D3D11_BUFFER_DESC));

    IndexDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexDesc.ByteWidth = sizeof(int) * IndexCount;
    IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexDesc.MiscFlags = 0;
    IndexDesc.CPUAccessFlags = 0;

    //Create Index buffer
    hr = dev->CreateBuffer(&IndexDesc, &indexData, &IndexBuffer);

    ENDSTR(hr, "Failed to create index buffer!")

    //Initialize the MVP buffer description
    D3D11_BUFFER_DESC cbDesc;
    ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(ConstantBuffer_WMP);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.MiscFlags = 0;
    cbDesc.CPUAccessFlags = 0;

    hr = dev->CreateBuffer(&cbDesc, nullptr, &WVPBuffer);

    ENDSTR(hr, "Failed to create Constant Buffer!");

    return true;
}

