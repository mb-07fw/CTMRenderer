#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <winnt.h> // HRESULT
#include <minwindef.h> // UINT

#include <array>
#include <initializer_list>

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Bindable
{
	inline constexpr D3D11_CPU_ACCESS_FLAG CPU_NONE = static_cast<D3D11_CPU_ACCESS_FLAG>(0);
	inline constexpr D3D11_CPU_ACCESS_FLAG CPU_READ_WRITE = static_cast<D3D11_CPU_ACCESS_FLAG>(D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE);
	inline constexpr D3D11_BIND_FLAG BIND_NONE = static_cast<D3D11_BIND_FLAG>(0);
	inline constexpr UINT MISC_NONE = 0;


	namespace MS_WRL = Microsoft::WRL;

	template <typename DerivedTy, typename DataTy>
	class DXBuffer
	{
	public:
		DXBuffer(MS_WRL::ComPtr<ID3D11Device1> pDevice, MS_WRL::ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept;
		~DXBuffer() noexcept = default;
	public:
		[[nodiscard]] DerivedTy* AsDerived() noexcept;

		void Reset() noexcept;

		template <D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlags, UINT Stride, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE, UINT MiscFlags = MISC_NONE>
		void CreateBuffer(UINT byteWidth, DataTy* pData) noexcept;
	private:
		MS_WRL::ComPtr<ID3D11Device1> mP_Device;
		MS_WRL::ComPtr<ID3D11DeviceContext1> mP_DeviceContext;
		MS_WRL::ComPtr<ID3D11Buffer> mP_Buffer;
	};

	template <typename DerivedTy, typename DataTy>
	inline [[nodiscard]] DerivedTy* DXBuffer<DerivedTy, DataTy>::AsDerived() noexcept
	{
		return static_cast<DerivedTy*>(this);
	}

	template <typename DerivedTy, typename DataTy>
	inline DXBuffer<DerivedTy, DataTy>::DXBuffer(MS_WRL::ComPtr<ID3D11Device1> pDevice, MS_WRL::ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
	{
		mP_Device = std::move(pDevice);
		mP_DeviceContext = std::move(pDeviceContext);
	}

	template <typename DerivedTy, typename DataTy>
	inline void DXBuffer<DerivedTy, DataTy>::Reset() noexcept
	{
		// Release and nullify the COM-interface.
		mP_Buffer.Reset();
	}

	template <typename DerivedTy, typename DataTy>
	template <D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlags, UINT Stride, D3D11_CPU_ACCESS_FLAG CPUFlags, UINT MiscFlags>
	inline void DXBuffer<DerivedTy, DataTy>::CreateBuffer(UINT byteWidth, DataTy* pData) noexcept
	{
		CD3D11_BUFFER_DESC cDesc(byteWidth, BindFlags, Usage, CPUFlags, MiscFlags, Stride);

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = pData;

		mP_Device->CreateBuffer(&desc, &data, mP_Buffer);
	}


}