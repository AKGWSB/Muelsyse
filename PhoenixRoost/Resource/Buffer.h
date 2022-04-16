#pragma once

#include "../DX12/d3dx12.h"
#include "../DX12/DirectXHelper.h"

class Buffer
{
public:
	ComPtr<ID3D12Resource> buffer;
	Buffer();
	~Buffer();
	virtual void UploadData();
};
