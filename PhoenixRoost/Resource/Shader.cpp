#include "Shader.h"

Shader::Shader(std::wstring fullPath)
{
#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif
    
    ComPtr<ID3DBlob> vsErrorMsgs;
    D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShaderBlob, &vsErrorMsgs);
    if (vsErrorMsgs != nullptr)
    {
        //OutputDebugStringA((char*)vsErrorMsgs->GetBufferPointer());
    }
    

    ComPtr<ID3DBlob> psErrorMsgs;
    D3DCompileFromFile(fullPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShaderBlob, &psErrorMsgs);
    if (psErrorMsgs != nullptr)
    {
        //OutputDebugStringA((char*)psErrorMsgs->GetBufferPointer());
    }
    
}

Shader::~Shader()
{

}

ID3DBlob* Shader::GetVertexShader()
{
    return vertexShaderBlob.Get();
}

ID3DBlob* Shader::GetPixelShader()
{
    return pixelShaderBlob.Get();
}
