#pragma once

#include <d3d11.h>
#include <wrl/client.h> // ComPtr
#include <d3dcompiler.h> // D3DReadFileToBlob
#include <winnt.h> // HRESULT

#include <filesystem>

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Window::Graphics::CTMDirectX
{
	class PixelShader
	{
	public:
		inline PixelShader(Microsoft::WRL::ComPtr<ID3D11Device>& pDeviceRef, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pDeviceContextRef)
			: mP_DeviceRef(pDeviceRef), mP_DeviceContext(pDeviceContextRef), mP_PixelShader() {}

		~PixelShader() = default;
	public:
		inline [[nodiscard]] HRESULT Create(const std::filesystem::path& shaderPath, Microsoft::WRL::ComPtr<ID3DBlob>& pReadBlob) noexcept
		{
			RUNTIME_ASSERT(!m_Created, "A shader shouldn't be re-created.\n");
			RUNTIME_ASSERT(std::filesystem::exists(shaderPath), "Path doesn't exist : " << shaderPath << '\n');
			RUNTIME_ASSERT(shaderPath.extension() == ".cso", "Shader file must be in shader bytecode format of .cso : " << shaderPath.extension() << '\n');

			std::wstring wPath = shaderPath.wstring();
			D3DReadFileToBlob(wPath.c_str(), &pReadBlob);

			m_Created = true;
			return mP_DeviceRef->CreatePixelShader(pReadBlob->GetBufferPointer(), pReadBlob->GetBufferSize(), nullptr, mP_PixelShader.GetAddressOf());
		}

		inline void Bind() noexcept
		{
			mP_DeviceContext->PSSetShader(mP_PixelShader.Get(), nullptr, 0u);
		}
	private:
		bool m_Created = false;
		Microsoft::WRL::ComPtr<ID3D11Device>& mP_DeviceRef;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mP_PixelShader;
	};

	class VertexShader
	{
	public:
		inline VertexShader(Microsoft::WRL::ComPtr<ID3D11Device>& pDeviceRef, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pDeviceContextRef)
			: mP_DeviceRef(pDeviceRef), mP_DeviceContext(pDeviceContextRef), mP_VertexShader() {
		}

		~VertexShader() = default;
	public:
		inline [[nodiscard]] HRESULT Create(const std::filesystem::path& shaderPath, Microsoft::WRL::ComPtr<ID3DBlob>& pReadBlob) noexcept
		{
			RUNTIME_ASSERT(!m_Created, "A shader shouldn't be re-created.\n");
			RUNTIME_ASSERT(std::filesystem::exists(shaderPath), "Path doesn't exist : " << shaderPath << '\n');
			RUNTIME_ASSERT(shaderPath.extension() == ".cso", "Shader file must be in shader bytecode format of .cso : " << shaderPath.extension() << '\n');

			std::wstring wPath = shaderPath.wstring();
			D3DReadFileToBlob(wPath.c_str(), &pReadBlob);

			m_Created = true;
			return mP_DeviceRef->CreateVertexShader(pReadBlob->GetBufferPointer(), pReadBlob->GetBufferSize(), nullptr, mP_VertexShader.GetAddressOf());
		}

		inline void Bind() noexcept
		{
			mP_DeviceContext->VSSetShader(mP_VertexShader.Get(), nullptr, 0);
		}
	private:
		bool m_Created = false;
		Microsoft::WRL::ComPtr<ID3D11Device>& mP_DeviceRef;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mP_VertexShader;
	};
}