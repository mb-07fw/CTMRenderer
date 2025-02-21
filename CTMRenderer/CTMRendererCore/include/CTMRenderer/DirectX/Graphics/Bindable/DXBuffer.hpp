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

	template<typename Ty>
	using ComPtr = Microsoft::WRL::ComPtr<Ty>;

	// Note : Not all valid combinations are specified, as I'm not insane enough to write them all.
	template <D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags>
	struct IsValidBufferConfig : std::false_type {};

	//
	// Default Usage
	//
	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, CPU_NONE>
		: std::true_type {};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, CPU_NONE>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, CPU_NONE>
		: std::true_type {
	};


	//
	// Immutable Usage
	//
	template<>
	struct IsValidBufferConfig<D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, CPU_NONE>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, CPU_NONE>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, CPU_NONE>
		: std::true_type {
	};


	//
	// Dynamic Usage
	//
	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DYNAMIC, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE>
		: std::true_type {
	};



	//
	// Staging Usage
	//
	template<>
	struct IsValidBufferConfig<D3D11_USAGE_STAGING, BIND_NONE, D3D11_CPU_ACCESS_WRITE>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_STAGING, BIND_NONE, D3D11_CPU_ACCESS_READ>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_STAGING, BIND_NONE, CPU_READ_WRITE>
		: std::true_type {
	};

	template <typename DataTy, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE>
		requires IsValidBufferConfig<Usage, BindFlag, CPUFlags>::value
	class DXBuffer
	{
		static_assert(
			Usage == D3D11_USAGE_DEFAULT ||
			Usage == D3D11_USAGE_DYNAMIC ||
			Usage == D3D11_USAGE_IMMUTABLE ||
			Usage == D3D11_USAGE_STAGING,
			"Provided D3D11_USAGE is invalid."
		);
		static_assert(
			BindFlags == D3D11_BIND_VERTEX_BUFFER ||
			BindFlags == D3D11_BIND_INDEX_BUFFER ||
			BindFlags == D3D11_BIND_CONSTANT_BUFFER ||
			BindFlags == D3D11_BIND_SHADER_RESOURCE,
			"Provided D3D11_BIND_FLAG flag is invalid."
		);
	public:
		inline DXBuffer(ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: mP_Device(std::move(pDevice)), mP_DeviceContext(std::move(pDeviceContext)) {}

		~DXBuffer() = default;
	public:
		[[nodiscard]] HRESULT CreateBuffer(UINT byteWidth, DataTy* data) noexcept;
		[[nodiscard]] HRESULT Recreate(UINT byteWidth, DataTy* data) noexcept;

		constexpr [[nodiscard]] UINT Stride() const noexcept { return S_STRIDE; }
		[[nodiscard]] ComPtr<ID3D11Buffer> ComBuffer() const noexcept { return mP_Buffer; }
		[[nodiscard]] bool IsCreated() const noexcept { return m_IsCreated; }
	protected:
		constexpr static UINT S_STRIDE = sizeof(DataTy);
		ComPtr<ID3D11Buffer> mP_Buffer;
		ComPtr<ID3D11Device1> mP_Device;
		ComPtr<ID3D11DeviceContext1> mP_DeviceContext;
		bool m_IsCreated = false;
	};



	template <typename T, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE>
	class DXVertexBuffer : public DXBuffer<T, Usage, D3D11_BIND_VERTEX_BUFFER, CPUFlags>
	{
	public:
		inline DXVertexBuffer(ComPtr< ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXBuffer<T, Usage, D3D11_BIND_VERTEX_BUFFER, CPUFlags>(std::move(pDevice), std::move(pDeviceContext)) {}

		~DXVertexBuffer() = default;
	public:
		void Bind(UINT startSlot = 0) noexcept;
	private:
		UINT m_Offset = 0;
	};



	template <typename T, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE, DXGI_FORMAT DXGIFormat>
	class DXIndexBuffer : public DXBuffer<T, Usage, D3D11_BIND_INDEX_BUFFER, CPUFlags>
	{
	public:
		inline DXIndexBuffer(ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXBuffer<T, Usage, D3D11_BIND_VERTEX_BUFFER, CPUFlags>(std::move(pDevice), std::move(pDeviceContext)) {}

		~DXIndexBuffer() = default;
	public:
		void Bind(UINT startSlot = 0) noexcept;
	private:
		UINT m_Offset = 0;
	};



	template <typename T, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE>
	class DXConstantBuffer : public DXBuffer<T, Usage, D3D11_BIND_CONSTANT_BUFFER, CPUFlags>
	{
	public:
		DXConstantBuffer(ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXBuffer<T, Usage, D3D11_BIND_VERTEX_BUFFER, CPUFlags>(std::move(pDevice), std::move(pDeviceContext)) {}

		~DXConstantBuffer() = default;
	public:
		void Bind(UINT startSlot = 0) noexcept;
	};

	

	template <typename DataTy, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags = D3D11_CPU_ACCESS_WRITE>
	class DXRuntimeBuffer
	{
	public:
		DXRuntimeBuffer(size_t initialSize, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept;
		~DXRuntimeBuffer() = default;
	public:
		[[nodiscard]] HRESULT Create() noexcept;
		[[nodiscard]] HRESULT UpdateData() noexcept;

		void Reserve(size_t size) noexcept;

		template <typename... Args>
		void EmplaceNext(Args&&... args) noexcept;
	private:
		DXBuffer<T, D3D11_USAGE_DYNAMIC, BindFlag, CPUFlags> m_Buffer;
		std::vector<T> m_Data;
		size_t m_CurrentByteWidth;
	};



	template <typename DataTy, D3D11_BIND_FLAG BindFlag> 
	class DXRuntimeVertexBuffer : public DXRuntimeBuffer<DataTy, BindFlag>
	{
	public:
		inline DXRuntimeVertexBuffer(size_t initialSize, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXRuntimeBuffer<DataTy, BindFlag>(initialSize, std::move(pDevice), std::move(pDeviceContext)) {}
	};



	template <typename DataTy, UINT Elems, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE>
	class DXStrictBuffer
	{
	public:
		inline DXStrictBuffer(ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: m_Buffer(std::move(pDevice), std::move(pDeviceContext)) {}

		inline DXStrictBuffer(const std::array<DataTy, Elems>& data, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: m_Buffer(std::move(pDevice), std::move(pDeviceContext)), m_Data(data) {}

		DXStrictBuffer(const std::initializer_list<DataTy>& dataList, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept;

		~DXStrictBuffer() = default;
	public:
		DXStrictBuffer<DataTy, Elems, Usage, BindFlag, CPUFlags>& operator=(const std::initializer_list<DataTy>& dataList) noexcept;

		[[nodiscard]] HRESULT Create() noexcept;

		constexpr [[nodiscard]] UINT Bytes() { return S_BYTES; }
		[[nodiscard]] const DataTy* Data() const noexcept { return m_Data.data(); }
	private:
		void CopyList(const std::initializer_list<DataTy>& dataList) noexcept;
	protected:
		constexpr static UINT S_BYTES = DXBuffer<DataTy, Usage, BindFlag, CPUFlags>::S_STRIDE * Elems;
		DXBuffer<DataTy, Usage, BindFlag, CPUFlags> m_Buffer;
		std::array<DataTy, Elems> m_Data; 
	};



	template <typename DataTy, UINT Elems, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags = CPU_NONE>
	class DXStrictVertexBuffer : public DXStrictBuffer<DataTy, Elems, Usage, D3D11_BIND_VERTEX_BUFFER, CPUFlags>
	{
	public:
		using Parent = DXStrictBuffer<DataTy, Elems, Usage, D3D11_BIND_VERTEX_BUFFER, CPUFlags>;
		using Parent::Parent;
		using Parent::operator=;

		void Bind(UINT startSlot = 0u) noexcept;

		~DXStrictVertexBuffer() = default;
	};

#pragma region Implementations

#pragma region DXBuffer's
	template <typename DataTy, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags>
		requires IsValidBufferConfig<Usage, BindFlag, CPUFlags>::value
	inline [[nodiscard]] HRESULT DXBuffer<DataTy, Usage, BindFlag, CPUFlags>::CreateBuffer(UINT byteWidth, DataTy* data) noexcept
	{
		D3D11_BUFFER_DESC vBufferDesc = {};
		vBufferDesc.ByteWidth = byteWidth;
		vBufferDesc.Usage = Usage;
		vBufferDesc.BindFlags = BindFlags;
		vBufferDesc.CPUAccessFlags = CPUFlags;
		vBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vBufferSubData = {};
		vBufferSubData.pSysMem = data;

		return this->mP_Device->CreateBuffer(&vBufferDesc, &vBufferSubData, mP_Buffer.GetAddressOf());
	}

	template <typename DataTy, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags>
		requires IsValidBufferConfig<Usage, BindFlag, CPUFlags>::value
	inline [[nodiscard]] HRESULT DXBuffer<DataTy, Usage, BindFlag, CPUFlags>::Recreate(UINT byteWidth, DataTy* data) noexcept
	{
		mP_Buffer.Reset();

		return CreateBuffer(byteWidth, data);
	}

	template <typename T, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline void DXVertexBuffer<T, Usage, CPUFlags>::Bind(UINT startSlot) noexcept
	{
		UINT stride = this->S_STRIDE;

		this->mP_DeviceContext->IASetVertexBuffers(startSlot, 1, this->mP_Buffer.GetAddressOf(), &stride, &m_Offset);
	}

	template <typename T, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags, DXGI_FORMAT DXGIFormat>
	inline void DXIndexBuffer<T, Usage, CPUFlags, DXGIFormat>::Bind(UINT startSlot) noexcept
	{
		this->mP_DeviceContext->IASetIndexBuffer(this->mP_Buffer.GetAddressOf(), DXGIFormat, &m_Offset);
	}

	template <typename T, D3D11_USAGE Usage, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline void DXConstantBuffer<T, Usage, CPUFlags>::Bind(UINT startSlot) noexcept
	{
		this->mP_DeviceContext->VSSetConstantBuffers(startSlot, 1, this->mP_Buffer.GetAddressOf());
	}
#pragma endregion

#pragma region DXRuntimeBuffer's
	template <typename DataTy, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline DXRuntimeBuffer<DataTy, BindFlags, CPUFlags>::DXRuntimeBuffer(size_t initialSize, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
		: m_Buffer(std::move(pDevice), std::move(pDeviceContext))
	{
		RUNTIME_ASSERT(initialSize >= 0, "Size cannot be negative : " << initialSize << '\n');

		m_Data.reserve(initialSize);

		m_CurrentByteWidth = m_Buffer.Stride() * initialSize;
	}

	template <typename T, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline [[nodiscard]] HRESULT DXRuntimeBuffer<T, BindFlags, CPUFlags>::Create() noexcept
	{
		return this->CreateBuffer(m_CurrentByteWidth, m_Data.data());
	}

	template <typename T, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline [[nodiscard]] HRESULT DXRuntimeBuffer<T, BindFlags, CPUFlags>::UpdateData() noexcept
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT hResult = this->mP_DeviceContext->Map(this->mP_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (hResult != S_OK)
			return hResult;

		std::copy(m_Data.begin(), m_Data.end(), static_cast<T*>(mappedResource.pData));
		this->mP_DeviceContext->Unmap(this->mP_Buffer.Get(), 0);

		return S_OK;
	}

	template <typename T, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline void DXRuntimeBuffer<T, BindFlags, CPUFlags>::Reserve(size_t size) noexcept
	{
		RUNTIME_ASSERT(m_Data.size() < size, "Cannot resize to smaller size.\n");

		m_Data.reserve(size);
		m_CurrentByteWidth = (UINT)m_Data.size * this->S_STRIDE;
	}

	template <typename T, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags>
	template <typename... Args>
	void DXRuntimeBuffer<T, BindFlags, CPUFlags>::EmplaceNext(Args&&... args) noexcept
	{
		m_CurrentByteWidth += this->S_STRIDE;

		m_Data.emplace_back(std::forward<Args>(args)...);
	}
#pragma endregion

	template <typename DataTy, UINT Elems, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline DXStrictBuffer<DataTy, Elems, Usage, BindFlag, CPUFlags>::DXStrictBuffer(const std::initializer_list<DataTy>& dataList, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
		: DXBuffer<T, Usage, BindFlag, CPUFlags>(pDevice, pDeviceContext)
	{
		CopyList(dataList);
	}

	template <typename DataTy, UINT Elems, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline DXStrictBuffer<DataTy, Elems, Usage, BindFlag, CPUFlags>& DXStrictBuffer<DataTy, Elems, Usage, BindFlag, CPUFlags>::operator=(const std::initializer_list<DataTy>& dataList) noexcept
	{
		CopyList(dataList);
		return *this;
	}

	template <typename T, UINT Elems, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline [[nodiscard]] HRESULT DXStrictBuffer<T, Elems, Usage, BindFlag, CPUFlags>::Create() noexcept
	{
		RUNTIME_ASSERT(!m_Buffer.IsCreated(), "A DXStrictBuffer should not be recreated.\n");
		return m_Buffer.CreateBuffer(S_BYTES, m_Data.data());
	}

	template <typename T, UINT Elems, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlag, D3D11_CPU_ACCESS_FLAG CPUFlags>
	inline void DXStrictBuffer<T, Elems, Usage, BindFlag, CPUFlags>::CopyList(const std::initializer_list<T>& dataList) noexcept
	{
		RUNTIME_ASSERT(
			Elems == dataList.size(),
			"Buffer size and list size don't match. | " << "Buffer size : " << Elems << " | " << "List size : " << dataList.size() << '\n'
		);

		std::copy(dataList.begin(), dataList.end(), m_Data.data());
	}
#pragma endregion
}