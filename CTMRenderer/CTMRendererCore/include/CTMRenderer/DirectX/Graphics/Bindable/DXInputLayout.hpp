#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <array>
#include <initializer_list>

namespace CTMRenderer::CTMDirectX::Graphics
{
	template <size_t Elems>
	class DXInputLayout
	{
	public:
		inline DXInputLayout(
			const std::array<D3D11_INPUT_ELEMENT_DESC, Elems>& descs,
			Microsoft::WRL::ComPtr<ID3DBlob> pVSBytecode,
			Microsoft::WRL::ComPtr<ID3D11Device1> pDevice,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pContext
		) noexcept;

		inline DXInputLayout(
			const std::initializer_list<D3D11_INPUT_ELEMENT_DESC>& descList,
			Microsoft::WRL::ComPtr<ID3DBlob> pVSBytecode,
			Microsoft::WRL::ComPtr<ID3D11Device1> pDevice,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pContext
		) noexcept;

		~DXInputLayout() = default;
	public:
		inline [[nodiscard]] HRESULT Create() noexcept;
		inline void Bind() noexcept;

		inline [[nodiscard]] bool IsCreated() const noexcept { return m_IsCreated; }
	private:
		std::array<D3D11_INPUT_ELEMENT_DESC, Elems> m_Descs;
		Microsoft::WRL::ComPtr<ID3DBlob> mP_VSBytecode;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mP_Layout;
		Microsoft::WRL::ComPtr<ID3D11Device1>& mP_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1>& mP_Context;
		bool m_IsCreated = false;
	};



	template <size_t Elems>
	inline DXInputLayout<Elems>::DXInputLayout(
		const std::array<D3D11_INPUT_ELEMENT_DESC, Elems>& descs,
		Microsoft::WRL::ComPtr<ID3DBlob> pVSBytecode,
		Microsoft::WRL::ComPtr<ID3D11Device1> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pContext
	) noexcept
		: m_Descs(descs), mP_VSBytecode(std::move(pVSBytecode)),
		  mP_Device(std::move(pDevice)), mP_Context(std::move(pContext))
	{
	}



	template <size_t Elems>
	inline DXInputLayout<Elems>::DXInputLayout(
		const std::initializer_list<D3D11_INPUT_ELEMENT_DESC>& descList,
		Microsoft::WRL::ComPtr<ID3DBlob> pVSBytecode,
		Microsoft::WRL::ComPtr<ID3D11Device1> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pContext
	) noexcept
		: mP_VSBytecode(std::move(pVSBytecode)), mP_Device(std::move(pDevice)),
		mP_Context(std::move(pContext))
	{
		RUNTIME_ASSERT(
			Elems == descList.size(),
			"Buffer size and list size don't match. | " << "Buffer size : " << Elems << " | " << "List size : " << descList.size() << '\n'
		);
		std::copy(descList.begin(), descList.end(), m_Descs.begin());
	}



	template <size_t Elems>
	inline [[nodiscard]] HRESULT DXInputLayout<Elems>::Create() noexcept
	{
		RUNTIME_ASSERT(!m_IsCreated, "An input layout shouldn't be re-created.\n");

		m_IsCreated = true;

		return mP_Device->CreateInputLayout(m_Descs.data(), (UINT)m_Descs.size(), mP_VSBytecode->GetBufferPointer(), mP_VSBytecode->GetBufferSize(), mP_Layout.GetAddressOf());
	}



	template <size_t Elems>
	inline void DXInputLayout<Elems>::Bind() noexcept
	{
		mP_Context->IASetInputLayout(mP_Layout.Get());
	}
}