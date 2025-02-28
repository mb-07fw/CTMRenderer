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
	inline constexpr D3D11_RESOURCE_MISC_FLAG MISC_NONE = static_cast<D3D11_RESOURCE_MISC_FLAG>(0);

	template <D3D11_BIND_FLAG BindFlag>
	inline constexpr bool IsDefaultUsageBindFlag = (
		//BindFlag != D3D11_BIND_CONSTANT_BUFFER &&
		BindFlag != BIND_NONE
	);

	template <D3D11_BIND_FLAG BindFlag>
	inline constexpr bool IsBindFlag = (
		BindFlag == D3D11_BIND_VERTEX_BUFFER   || 
		BindFlag == D3D11_BIND_INDEX_BUFFER	   || 
		BindFlag == D3D11_BIND_CONSTANT_BUFFER ||
		BindFlag == D3D11_BIND_SHADER_RESOURCE
	);

	template <D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline constexpr bool IsStagingCPUFlag = (
		CPUFlags == D3D11_CPU_ACCESS_READ ||
		CPUFlags == D3D11_CPU_ACCESS_WRITE ||
		CPUFlags == CPU_READ_WRITE
	);

	// TODO : Implement all 20+ flags later. I'm tired.
	template <D3D11_RESOURCE_MISC_FLAG MiscFlags>
	inline constexpr bool IsMiscFlag = true;

	// Note : Not all valid combinations are specified. I'm not insane enough to write them all.
	template <D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
	inline constexpr bool IsValidBufferConfig = (
		(Usage == D3D11_USAGE_DEFAULT	&& IsDefaultUsageBindFlag<BindFlag>		  && CPUFlags == CPU_NONE				&& IsMiscFlag<MiscFlags>) ||
		(Usage == D3D11_USAGE_IMMUTABLE && BindFlag != D3D11_BIND_CONSTANT_BUFFER && CPUFlags == CPU_NONE				&& IsMiscFlag<MiscFlags>) ||
		(Usage == D3D11_USAGE_DYNAMIC	&& IsBindFlag<BindFlag>					  && CPUFlags == D3D11_CPU_ACCESS_WRITE && IsMiscFlag<MiscFlags>) ||
		(Usage == D3D11_USAGE_STAGING	&& BindFlag == BIND_NONE				  && IsStagingCPUFlag<CPUFlags>			&& IsMiscFlag<MiscFlags>)
	);

	template <DXGI_FORMAT Format>
	inline constexpr bool IsValidDXGIFormat = (
		(Format == DXGI_FORMAT_R8_UNORM) || 
		(Format == DXGI_FORMAT_R16_UINT) ||
		(Format == DXGI_FORMAT_R16_UNORM)
	);

	template <D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
	concept ValidBufferConfig = IsValidBufferConfig<Usage, BindFlag, CPUFlags, MiscFlags>;

	template <DXGI_FORMAT Format>
	concept ValidDXGIFormat = IsValidDXGIFormat<Format>;

#pragma region Declarations

#pragma region DXBuffers
	template <typename DataTy, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE, D3D11_RESOURCE_MISC_FLAG MiscFlags = MISC_NONE>
	requires ValidBufferConfig<Usage, BindFlag, CPUFlags, MiscFlags>
	class DXBuffer
	{
	public:
		DXBuffer(Microsoft::WRL::ComPtr<ID3D11Device1> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept;
		~DXBuffer() noexcept = default;
	public:
		void Reset() noexcept;

		[[nodiscard]] HRESULT CreateBuffer(UINT byteWidth, DataTy* pData) noexcept;
	protected:
		static constexpr UINT S_STRIDE = sizeof(DataTy);
		Microsoft::WRL::ComPtr<ID3D11Device1> mP_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mP_Buffer;
	};



	template <typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE, D3D11_RESOURCE_MISC_FLAG MiscFlags = MISC_NONE>
	class DXVertexBuffer : public DXBuffer<DataTy, Usage, D3D11_BIND_VERTEX_BUFFER, CPUFlags, MiscFlags>
	{
	public:
		using Parent = DXBuffer<DataTy, Usage, D3D11_BIND_VERTEX_BUFFER, CPUFlags, MiscFlags>;
		using Parent::Parent;
		using Parent::mP_DeviceContext;
		using Parent::mP_Buffer;

		~DXVertexBuffer() = default;

		void Bind(UINT startSlot = 0u, UINT offset = 0u) noexcept;
	};



	template <DXGI_FORMAT Format, typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE, D3D11_RESOURCE_MISC_FLAG MiscFlags = MISC_NONE>
	requires ValidDXGIFormat<Format>
	class DXIndexBuffer : public DXBuffer<DataTy, Usage, D3D11_BIND_INDEX_BUFFER, CPUFlags, MiscFlags>
	{
	public:
		using Parent = DXBuffer<DataTy, Usage, D3D11_BIND_INDEX_BUFFER, CPUFlags, MiscFlags>;
		using Parent::Parent;
		using Parent::mP_DeviceContext;
		using Parent::mP_Buffer;

		~DXIndexBuffer() = default;

		void Bind(UINT offset = 0u) noexcept;
	};



	template <typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE, D3D11_RESOURCE_MISC_FLAG MiscFlags = MISC_NONE>
	class DXConstantBuffer : public DXBuffer<DataTy, Usage, D3D11_BIND_CONSTANT_BUFFER, CPUFlags, MiscFlags>
	{
	public:
		using Parent = DXBuffer<DataTy, Usage, D3D11_BIND_CONSTANT_BUFFER, CPUFlags, MiscFlags>;
		using Parent::Parent;
		using Parent::mP_DeviceContext;
		using Parent::mP_Buffer;

		~DXConstantBuffer() = default;

		void Bind(UINT startSlot = 0u) noexcept;
	};
#pragma endregion

#pragma region Buffer Wrappers
	template <typename DXBufferTy, typename DataTy, size_t Elems>
	class DXStrictBuffer
	{
	public:
		inline DXStrictBuffer(const std::array<DataTy, Elems>& data, Microsoft::WRL::ComPtr<ID3D11Device1> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: m_Buffer(std::move(pDevice), std::move(pDeviceContext)), m_Data(data) {}

		DXStrictBuffer(const std::initializer_list<DataTy>& list, Microsoft::WRL::ComPtr<ID3D11Device1> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept;

		~DXStrictBuffer() = default;
	public:
		[[nodiscard]] HRESULT Create() noexcept;
	protected:
		static constexpr UINT S_BYTE_WIDTH = Elems * sizeof(DataTy);
		DXBufferTy m_Buffer;
		std::array<DataTy, Elems> m_Data;
	};



	template <size_t Elems, typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE, D3D11_RESOURCE_MISC_FLAG MiscFlags = MISC_NONE>
	class DXStrictVertexBuffer : public DXStrictBuffer<DXVertexBuffer<DataTy, Usage, CPUFlags, MiscFlags>, DataTy, Elems>
	{
	public:
		using Parent = DXStrictBuffer<DXVertexBuffer<DataTy, Usage, CPUFlags, MiscFlags>, DataTy, Elems>;
		using Parent::Parent;
		using Parent::m_Buffer;

		~DXStrictVertexBuffer() = default;

		void Bind(UINT startSlot = 0u, UINT offset = 0u) noexcept;
	};



	template <DXGI_FORMAT Format, size_t Elems, typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE, D3D11_RESOURCE_MISC_FLAG MiscFlags = MISC_NONE>
	class DXStrictIndexBuffer : public DXStrictBuffer<DXIndexBuffer<Format, DataTy, Usage, CPUFlags, MiscFlags>, DataTy, Elems>
	{
	public:
		using Parent = DXStrictBuffer<DXIndexBuffer<Format, DataTy, Usage, CPUFlags, MiscFlags>, DataTy, Elems>;
		using Parent::Parent;
		using Parent::m_Buffer;

		void Bind(UINT offset = 0u) noexcept;
	};

	template <size_t Elems, typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE, D3D11_RESOURCE_MISC_FLAG MiscFlags = MISC_NONE>
	class DXStrictConstantBuffer : public DXStrictBuffer<DXConstantBuffer<DataTy, Usage, CPUFlags, MiscFlags>, DataTy, Elems>
	{
	public:
	public:
		using Parent = DXStrictBuffer<DXConstantBuffer<DataTy, Usage, CPUFlags, MiscFlags>, DataTy, Elems>;
		using Parent::Parent;
		using Parent::m_Buffer;

		void Bind(UINT startSlot = 0u) noexcept;
	};
#pragma endregion

#pragma endregion

#pragma region Implementations

#pragma region Buffers
	template <typename DataTy, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
		requires ValidBufferConfig<Usage, BindFlag, CPUFlags, MiscFlags>
	inline DXBuffer<DataTy, Usage, BindFlag, CPUFlags, MiscFlags>::DXBuffer(
		Microsoft::WRL::ComPtr<ID3D11Device1> pDevice, 
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
	{
		mP_Device = std::move(pDevice);
		mP_DeviceContext = std::move(pDeviceContext);
	}

	template <typename DataTy, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
		requires ValidBufferConfig<Usage, BindFlag, CPUFlags, MiscFlags>
	inline void DXBuffer<DataTy, Usage, BindFlag, CPUFlags, MiscFlags>::Reset() noexcept
	{
		// Releases and nullifies the COM-interface.
		mP_Buffer.Reset();
	}

	template <typename DataTy, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
		requires ValidBufferConfig<Usage, BindFlag, CPUFlags, MiscFlags>
	inline [[nodiscard]] HRESULT DXBuffer<DataTy, Usage, BindFlag, CPUFlags, MiscFlags>::CreateBuffer(UINT byteWidth, DataTy* pData) noexcept
	{
		CD3D11_BUFFER_DESC cDesc(byteWidth, BindFlag, Usage, CPUFlags, MiscFlags, S_STRIDE);

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = pData;

		return mP_Device->CreateBuffer(&cDesc, &data, mP_Buffer.GetAddressOf());
	}

	template <typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
	inline void DXVertexBuffer<DataTy, Usage, CPUFlags, MiscFlags>::Bind(UINT startSlot, UINT offset) noexcept
	{
		UINT stride = Parent::S_STRIDE;

		mP_DeviceContext->IASetVertexBuffers(startSlot, 1, mP_Buffer.GetAddressOf(), &stride, &offset);
	}

	template <DXGI_FORMAT Format, typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
	requires ValidDXGIFormat<Format>
	inline void DXIndexBuffer<Format, DataTy, Usage, CPUFlags, MiscFlags>::Bind(UINT offset) noexcept
	{
		mP_DeviceContext->IASetIndexBuffer(mP_Buffer.Get(), Format, offset);
	}

	template <typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
	inline void DXConstantBuffer<DataTy, Usage, CPUFlags, MiscFlags>::Bind(UINT startSlot) noexcept
	{
		mP_DeviceContext->VSSetConstantBuffers(startSlot, 1, mP_Buffer.GetAddressOf());
	}
#pragma endregion



#pragma region Buffer Wrappers

	template <typename DXBufferTy, typename DataTy, size_t Elems>
	inline DXStrictBuffer<DXBufferTy, DataTy, Elems>::DXStrictBuffer(
		const std::initializer_list<DataTy>& list,
		Microsoft::WRL::ComPtr<ID3D11Device1> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pDeviceContext
	) noexcept
		: m_Buffer(std::move(pDevice), std::move(pDeviceContext))
	{
		RUNTIME_ASSERT(list.size() == Elems, "List size doesn't match templated buffer size.\n");
		std::copy(list.begin(), list.end(), m_Data.begin());
	}

	template <typename DXBufferTy, typename DataTy, size_t Elems>
	inline [[nodiscard]] HRESULT DXStrictBuffer<DXBufferTy, DataTy, Elems>::Create() noexcept
	{
		return m_Buffer.CreateBuffer(S_BYTE_WIDTH, m_Data.data());
	}

	template <size_t Elems, typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
	inline void DXStrictVertexBuffer<Elems, DataTy, Usage, CPUFlags, MiscFlags>::Bind(UINT startSlot, UINT offset) noexcept
	{
		m_Buffer.Bind(startSlot, offset);
	}

	template <DXGI_FORMAT Format, size_t Elems, typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
	inline void DXStrictIndexBuffer<Format, Elems, DataTy, Usage, CPUFlags, MiscFlags>::Bind(UINT offset) noexcept
	{
		m_Buffer.Bind(offset);
	}

	template <size_t Elems, typename DataTy, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags, D3D11_RESOURCE_MISC_FLAG MiscFlags>
	inline void DXStrictConstantBuffer<Elems, DataTy, Usage, CPUFlags, MiscFlags>::Bind(UINT startSlot) noexcept
	{
		m_Buffer.Bind(startSlot);
	}
#pragma endregion

#pragma endregion
}