#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <wrl/client.h> // ComPtr
#include <d3dcompiler.h> // D3DReadFileToBlob
#include <winnt.h> // HRESULT

#include <filesystem>

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
	class DXPixelShader
	{
	public:
		inline DXPixelShader(Microsoft::WRL::ComPtr<ID3D11Device1> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pDeviceContext)
			: mP_Device(std::move(pDevice)), mP_DeviceContext(std::move(pDeviceContext))
		{
		}

		~DXPixelShader() = default;
	public:
		inline [[nodiscard]] HRESULT Create(const std::filesystem::path& shaderPath, Microsoft::WRL::ComPtr<ID3DBlob> pReadBlob) noexcept
		{
			RUNTIME_ASSERT(!m_IsCreated, "A shader shouldn't be re-created.\n");
			RUNTIME_ASSERT(std::filesystem::exists(shaderPath), "Path doesn't exist : " << shaderPath << '\n');
			RUNTIME_ASSERT(shaderPath.extension() == ".cso", "Shader file must be in shader bytecode format of .cso : " << shaderPath.extension() << '\n');

			std::wstring wPath = shaderPath.wstring();
			D3DReadFileToBlob(wPath.c_str(), &pReadBlob);

			m_IsCreated = true;

			return mP_Device->CreatePixelShader(pReadBlob->GetBufferPointer(), pReadBlob->GetBufferSize(), nullptr, mP_PixelShader.GetAddressOf());
		}

		inline void Bind() noexcept
		{
			mP_DeviceContext->PSSetShader(mP_PixelShader.Get(), nullptr, 0u);
		}
	public:
		inline [[nodiscard]] bool IsCreated() const noexcept { return m_IsCreated; }
	private:
		bool m_IsCreated = false;
		Microsoft::WRL::ComPtr<ID3D11Device1> mP_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mP_PixelShader;
	};

	class DXVertexShader
	{
	public:
		inline DXVertexShader(Microsoft::WRL::ComPtr<ID3D11Device1> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pDeviceContext)
			: mP_Device(pDevice), mP_DeviceContext(pDeviceContext)
		{
		}

		~DXVertexShader() = default;
	public:
		inline [[nodiscard]] HRESULT Create(const std::filesystem::path& shaderPath, Microsoft::WRL::ComPtr<ID3DBlob>& pReadBlob) noexcept
		{
			RUNTIME_ASSERT(!m_Created, "A shader shouldn't be re-created.\n");
			RUNTIME_ASSERT(std::filesystem::exists(shaderPath), "Path doesn't exist : " << shaderPath << '\n');
			RUNTIME_ASSERT(shaderPath.extension() == ".cso", "Shader file must be in shader bytecode format of .cso : " << shaderPath.extension() << '\n');

			std::wstring wPath = shaderPath.wstring();
			D3DReadFileToBlob(wPath.c_str(), &pReadBlob);

			m_Created = true;
			return mP_Device->CreateVertexShader(pReadBlob->GetBufferPointer(), pReadBlob->GetBufferSize(), nullptr, mP_VertexShader.GetAddressOf());
		}

		inline void Bind() noexcept
		{
			mP_DeviceContext->VSSetShader(mP_VertexShader.Get(), nullptr, 0);
		}
	private:
		bool m_Created = false;
		Microsoft::WRL::ComPtr<ID3D11Device1> mP_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mP_VertexShader;
	};
}