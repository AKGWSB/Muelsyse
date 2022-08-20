#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>

using Microsoft::WRL::ComPtr;

enum EBlendMode
{
    Opaque = 0,
    AlphaTest = 1,
    Transparent = 2
};

struct PsoDescriptor
{
    // custom info
    std::string shaderRef;
    EBlendMode blendMode = EBlendMode::Opaque;

    // name align to PSO
    UINT NumRenderTargets = 1;
    DXGI_FORMAT RTVFormats[8] = { DXGI_FORMAT_R8G8B8A8_UNORM };
    DXGI_FORMAT DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};

// custom eq func
struct eqfunc
{
    bool operator()(const PsoDescriptor& pd1, const PsoDescriptor& pd2) const
    {
        return pd1.shaderRef == pd2.shaderRef
            && pd1.blendMode == pd2.blendMode
            && pd1.NumRenderTargets == pd2.NumRenderTargets
            && pd1.RTVFormats[0] == pd2.RTVFormats[0]
            && pd1.RTVFormats[1] == pd2.RTVFormats[1]
            && pd1.RTVFormats[2] == pd2.RTVFormats[2]
            && pd1.RTVFormats[3] == pd2.RTVFormats[3]
            && pd1.RTVFormats[4] == pd2.RTVFormats[4]
            && pd1.RTVFormats[5] == pd2.RTVFormats[5]
            && pd1.RTVFormats[6] == pd2.RTVFormats[6]
            && pd1.RTVFormats[7] == pd2.RTVFormats[7]
            && pd1.DSVFormat == pd2.DSVFormat;
    }
};

// custom hash func
struct hashfunc
{
    size_t operator()(const PsoDescriptor& pd) const
    {
        std::hash<std::string> sh;
        size_t hashValue = sh(pd.shaderRef);

        hashValue += pd.blendMode;
        hashValue += pd.NumRenderTargets;
        for (int i = 0; i < 8; i++)
        {
            hashValue += pd.RTVFormats[i];
        }
        hashValue += pd.DSVFormat;

        return hashValue;
    }
};

class PsoCache
{
private:
    std::unordered_map<PsoDescriptor,ComPtr<ID3D12PipelineState>, hashfunc, eqfunc> m_psoPool;

    PsoCache();
    PsoCache(const PsoCache&);
    PsoCache& operator=(const PsoCache&);

public:
	~PsoCache();

    // single ton
    static PsoCache* GetInstance();

    ID3D12PipelineState* Find(const PsoDescriptor& pd);
};


