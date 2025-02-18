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
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		inline DXInputLayout(
			const std::array<D3D11_INPUT_ELEMENT_DESC, Elems>& descs,
			const ComPtr<ID3DBlob>& pVShaderBytecodeRef,
			ComPtr<ID3D11Device1>& pDeviceRef,
			ComPtr<ID3D11DeviceContext1>& pContextRef
		)
			: m_Descs(descs), mP_VShaderBytecodeRef(pVShaderBytecodeRef),
		      mP_DeviceRef(pDeviceRef), mP_ContextRef(pContextRef) {}

		inline DXInputLayout(
			const std::initializer_list<D3D11_INPUT_ELEMENT_DESC>& descList,
			const ComPtr<ID3DBlob>& pVShaderBytecodeRef,
			ComPtr<ID3D11Device1>& pDeviceRef,
			ComPtr<ID3D11DeviceContext1>& pContextRef
		)
			: mP_VShaderBytecodeRef(pVShaderBytecodeRef),
			  mP_DeviceRef(pDeviceRef), mP_ContextRef(pContextRef)
		{
			RUNTIME_ASSERT(
				Elems == descList.size(),
				"Buffer size and list size don't match. | " << "Buffer size : " << Elems << " | " << "List size : " << descList.size() << '\n'
			);
			std::copy(descList.begin(), descList.end(), m_Descs.begin());
		}


		~DXInputLayout() = default;
	public:
		inline HRESULT Create() noexcept
		{
			RUNTIME_ASSERT(!m_IsCreated, "An input layout shouldn't be re-created.\n");

			m_IsCreated = true;

			return mP_DeviceRef->CreateInputLayout(m_Descs.data(), (UINT)m_Descs.size(), mP_VShaderBytecodeRef->GetBufferPointer(), mP_VShaderBytecodeRef->GetBufferSize(), mP_Layout.GetAddressOf());
		}

		inline void Bind() noexcept
		{
			mP_ContextRef->IASetInputLayout(mP_Layout.Get());
		}
	public:
		inline [[nodiscard]] bool IsCreated() const noexcept { return m_IsCreated; }
	private:
		std::array<D3D11_INPUT_ELEMENT_DESC, Elems> m_Descs;
		const ComPtr<ID3DBlob>& mP_VShaderBytecodeRef;
		ComPtr<ID3D11InputLayout> mP_Layout;
		ComPtr<ID3D11Device1>& mP_DeviceRef;
		ComPtr<ID3D11DeviceContext1>& mP_ContextRef;
		bool m_IsCreated = false;
	};
}