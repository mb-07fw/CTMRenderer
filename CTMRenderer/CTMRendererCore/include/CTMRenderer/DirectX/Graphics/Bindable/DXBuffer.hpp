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
	// Note : Not all valid combinations are specified, as I'm not insane enough to write them all.
	template <D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags>
	struct IsValidBufferConfig : std::false_type {};

	//
	// Default Usage
	//
	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)(0)>
		: std::true_type {};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)(0)>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, (D3D11_CPU_ACCESS_FLAG)(0)>
		: std::true_type {
	};


	//
	// Immutable Usage
	//
	template<>
	struct IsValidBufferConfig<D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)(0)>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)(0)>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, (D3D11_CPU_ACCESS_FLAG)(0)>
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
	struct IsValidBufferConfig<D3D11_USAGE_STAGING, (D3D11_BIND_FLAG)(0), D3D11_CPU_ACCESS_WRITE>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_STAGING, (D3D11_BIND_FLAG)(0), D3D11_CPU_ACCESS_READ>
		: std::true_type {
	};

	template<>
	struct IsValidBufferConfig<D3D11_USAGE_STAGING, (D3D11_BIND_FLAG)(0), (D3D11_CPU_ACCESS_FLAG)(D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ)>
		: std::true_type {
	};





	template <typename T, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags = (D3D11_CPU_ACCESS_FLAG)(0)>
		requires (IsValidBufferConfig<Usage, BindFlags, CPUFlags>::value)
	class DXBuffer
	{
		template<typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;

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
		inline DXBuffer(ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext)
			: mP_Device(pDevice), mP_DeviceContext(pDeviceContext)
		{
		}

		~DXBuffer() = default;
	public:
		inline constexpr [[nodiscard]] UINT Stride() const noexcept { return S_STRIDE; }
		inline [[nodiscard]] const ComPtr<ID3D11Buffer>& ComBuffer() const noexcept { return mP_Buffer; }
		inline [[nodiscard]] bool IsCreated() const noexcept { return m_IsCreated; }
	protected:
		constexpr static UINT S_STRIDE = sizeof(T);
		ComPtr<ID3D11Buffer> mP_Buffer;
		ComPtr<ID3D11Device1> mP_Device;
		ComPtr<ID3D11DeviceContext1> mP_DeviceContext;
		bool m_IsCreated = false;
	};

	/*  Explanation:
	 *
	 *    Instead of restricting the capacity of the buffer at compile time (like DXStrictBuffer),
	 *    the capacity is instead set at runtime, which is very useful when the amount of data varies.
	 * 
	 *    DXRuntimeBuffer utilizes D3D11_USAGE_DYNAMIC, as it is meant to be modified through data,
	 *    or buffer size.
	 * 
	 *    DXRuntimeBuffer has the CPUFlags of D3D11_CPU_ACCESS_WRITE and D3D11_CPU_ACCESS_READ by default, unless specified otherwise.
	 * 
	 *  NOTE:
	 * 
	 *    For DXStrictBuffer, the capacity of the buffer is fixed after instantiation.
	 * 
	 *    This differs from DXRuntimeBuffer, which allows recreation with a differing buffer capacity.
	 * 
	 *    However, since DXStrictBuffer takes it's capacity as a template parameter, stack allocation of elements via std::array is allowed, improving performance.
	 * 
	 *    On the contrary, since DXRuntimeBuffer's buffer capacity is determined at runtime, heap allocation is inevitable, 
	 *    which may hurt performance in edge cases, such as frequent buffer resizing.
	 * 
	 *	Usage:
	 * 
	 *	  DXRuntimeBuffer should be used where the amount of data varies.
	 * 
	 *    DXStrictBuffer should be used where the amount of the data is known at compile time, for better data optimization.
	 */
	template <typename T, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags = (D3D11_CPU_ACCESS_FLAG)(D3D11_CPU_ACCESS_WRITE)>
	class DXRuntimeBuffer : public DXBuffer<T, D3D11_USAGE_DYNAMIC, BindFlags, CPUFlags>
	{
		template<typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline DXRuntimeBuffer(size_t initialSize, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext)
			: DXBuffer<T, D3D11_USAGE_DYNAMIC, BindFlags, CPUFlags>(pDevice, pDeviceContext),
			  m_CurrentByteWidth(initialSize * this->S_STRIDE)
		{
			m_Data.reserve(initialSize);
		}

		~DXRuntimeBuffer() = default;
	public:
		// Creates the buffer with the current size.
		// If the buffer needs to expand, Call Expand instead.
		inline [[nodiscard]] HRESULT Create() noexcept
		{
			D3D11_BUFFER_DESC vBufferDesc = {};
			vBufferDesc.ByteWidth = m_CurrentByteWidth;
			vBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vBufferDesc.BindFlags = BindFlags;
			vBufferDesc.CPUAccessFlags = CPUFlags;
			vBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vBufferSubData = {};
			vBufferSubData.pSysMem = m_Data.data();

			return this->mP_Device->CreateBuffer(&vBufferDesc, &vBufferSubData, this->mP_Buffer.GetAddressOf());
		}
		
		template <typename... Args>
		inline [[nodiscard]] void EmplaceNext(Args&&... args) noexcept
		{
			m_CurrentByteWidth += this->S_STRIDE;

			m_Data.emplace_back(std::forward<Args>(args)...);
		}

		inline [[nodiscard]] void Expand(UINT size) noexcept
		{
			RUNTIME_ASSERT((UINT)m_Data.size() < size, "Cannot resize to smaller size.\n");

			m_Data.reserve(size);

			m_CurrentByteWidth = m_Data.size * this->S_STRIDE;
		}

		inline [[nodiscard]] HRESULT UpdateData()
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource = {};

			HRESULT hResult = this->mP_DeviceContext->Map(this->mP_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			if (hResult != S_OK)
				return hResult;

			std::copy(m_Data.begin(), m_Data.end(), static_cast<T*>(mappedResource.pData));
			this->mP_DeviceContext->Unmap(this->mP_Buffer.Get(), 0);

			return S_OK;
		}
	private:
		inline [[nodiscard]] HRESULT Recreate()
		{
			this->mP_Buffer.Reset();

			return Create();
		}
	private:
		std::vector<T> m_Data;
		UINT m_CurrentByteWidth;
	};

	template <typename T>
	class DXRuntimeVertexBuffer : public DXRuntimeBuffer<T, D3D11_BIND_VERTEX_BUFFER>
	{
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		inline DXRuntimeVertexBuffer(size_t initialSize, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext)
			: DXRuntimeBuffer<T, D3D11_BIND_VERTEX_BUFFER>(initialSize, pDevice, pDeviceContext)
		{
		}

		~DXRuntimeVertexBuffer() = default;
	public:
		inline void Bind(UINT startSlot = 0u) noexcept
		{
			// Store stride in a temporary variable since S_STRIDE is a static constexpr, meaning it's address cannot be referenced.
			UINT stride = this->S_STRIDE;

			this->mP_DeviceContext->IASetVertexBuffers(startSlot, 1, this->mP_Buffer.GetAddressOf(), &stride, &m_Offset);
		}
	private:
		UINT m_Offset = 0;
	};

	template <typename T, UINT Elems, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags = (D3D11_CPU_ACCESS_FLAG)(0)>
	class DXStrictBuffer : public DXBuffer<T, Usage, BindFlags, CPUFlags>
	{
		template<typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline DXStrictBuffer(ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXBuffer<T, Usage, BindFlags, CPUFlags>(pDevice, pDeviceContext) {}

		inline DXStrictBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1>& pDeviceContext) noexcept
			: DXBuffer<T, Usage, BindFlags, CPUFlags>(pDevice, pDeviceContext), m_Data(data) {}

		inline DXStrictBuffer(const std::initializer_list<T>& dataList, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXBuffer<T, Usage, BindFlags, CPUFlags>(pDevice, pDeviceContext)
		{
			RUNTIME_ASSERT(
				Elems == dataList.size(),
				"Buffer size and list size don't match. | " << "Buffer size : " << Elems <<  " | " << "List size : " << dataList.size() << '\n'
			);
			std::copy(dataList.begin(), dataList.end(), m_Data.data());
		}

		inline DXStrictBuffer<T, Elems, Usage, BindFlags, CPUFlags>& operator=(const std::initializer_list<T>& dataList) noexcept
		{
			RUNTIME_ASSERT(Elems == dataList.size(), "Buffer size and list size don't match.\n");
			std::copy(dataList.begin(), dataList.end(), m_Data.data());

			return *this;
		}

		~DXStrictBuffer() = default;
	public:
		inline [[nodiscard]] HRESULT Create() noexcept
		{
			RUNTIME_ASSERT(!this->m_IsCreated, "A DXStrictBuffer should not be recreated.\n");

			D3D11_BUFFER_DESC vBufferDesc = {};
			vBufferDesc.ByteWidth = S_BYTES;
			vBufferDesc.Usage = Usage;
			vBufferDesc.BindFlags = BindFlags;
			vBufferDesc.CPUAccessFlags = CPUFlags;
			vBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vBufferSubData = {};
			vBufferSubData.pSysMem = Data();

			this->m_IsCreated = true;
			return this->mP_Device->CreateBuffer(&vBufferDesc, &vBufferSubData, this->mP_Buffer.GetAddressOf());
		}
	public:
		inline constexpr [[nodiscard]] UINT Bytes() { return S_BYTES; }
		inline [[nodiscard]] const T* Data() const noexcept { return m_Data.data(); }
	protected:
		constexpr static UINT S_BYTES = DXBuffer<T, Usage, BindFlags, CPUFlags>::S_STRIDE * Elems;
		std::array<T, Elems> m_Data;
	};

	template <typename T, UINT Elems, D3D11_USAGE Usage = D3D11_USAGE_IMMUTABLE>
	class DXStrictVertexBuffer : public DXStrictBuffer<T, Elems, Usage, D3D11_BIND_VERTEX_BUFFER>
	{
		template <typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline DXStrictVertexBuffer(ComPtr<ID3D11Device1> pDeviceRef, ComPtr<ID3D11DeviceContext1> pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, Usage, D3D11_BIND_VERTEX_BUFFER>(pDeviceRef, pDeviceContextRef) {}

		inline DXStrictVertexBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXStrictBuffer<T, Elems, Usage, D3D11_BIND_VERTEX_BUFFER>(data, pDevice, pDeviceContext) {}

		inline DXStrictVertexBuffer(const std::initializer_list<T>& dataList, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXStrictBuffer<T, Elems, Usage, D3D11_BIND_VERTEX_BUFFER>(dataList, pDevice, pDeviceContext) {}

		inline DXStrictVertexBuffer& operator=(const std::initializer_list<T>& dataList) noexcept
		{
			// Call the base class operator=.
			static_cast<DXStrictBuffer<T, Elems, Usage, D3D11_BIND_VERTEX_BUFFER>&>(*this).operator=(dataList);
			return *this;
		}

		~DXStrictVertexBuffer() = default;
	public:
		inline void Bind(UINT startSlot = 0u) noexcept
		{
			UINT stride = this->S_STRIDE;

			this->mP_DeviceContext->IASetVertexBuffers(startSlot, 1, this->mP_Buffer.GetAddressOf(), &stride, &m_Offset);
		}
	private:
		UINT m_Offset = 0;
	};

	template <typename T, UINT Elems, DXGI_FORMAT DXGIFormat>
	class DXStrictIndexBuffer : public DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>
	{
		template <typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline DXStrictIndexBuffer(ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>(pDevice, pDeviceContext) {}

		inline DXStrictIndexBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>(data, pDevice, pDeviceContext) {}

		inline DXStrictIndexBuffer(const std::initializer_list<T>& dataList, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>(dataList, pDevice, pDeviceContext) {}

		inline DXStrictIndexBuffer& operator=(const std::initializer_list<T>& dataList) noexcept
		{
			// Call the base class operator=.
			static_cast<DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>&>(*this).operator=(dataList);
			return *this;
		}

		~DXStrictIndexBuffer() = default;
	public:
		inline void Bind(UINT startSlot = 0u) noexcept
		{
			this->mP_DeviceContext->IASetIndexBuffer(this->mP_Buffer.Get(), DXGIFormat, 0);
		}
	};

	template <typename T, UINT Elems>
	class DXStrictConstantBuffer : public DXStrictBuffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>
	{
		template <typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline DXStrictConstantBuffer(ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>(pDevice, pDeviceContext) {}

		inline DXStrictConstantBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>(data, pDevice, pDeviceContext) {}

		inline DXStrictConstantBuffer(const std::initializer_list<T>& dataList, ComPtr<ID3D11Device1> pDevice, ComPtr<ID3D11DeviceContext1> pDeviceContext) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>(dataList, pDevice, pDeviceContext) {}

		inline DXStrictConstantBuffer& operator=(const std::initializer_list<T>& dataList) noexcept
		{
			// Call the base class operator=.
			static_cast<DXStrictBuffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>&>(*this).operator=(dataList);
			return *this;
		}

		~DXStrictConstantBuffer() = default;
	public:
		inline void Bind(UINT startSlot = 0u) noexcept
		{
			this->mP_DeviceContext->VSSetConstantBuffers(startSlot, 1, this->mP_Buffer.GetAddressOf());
		}
	};
}