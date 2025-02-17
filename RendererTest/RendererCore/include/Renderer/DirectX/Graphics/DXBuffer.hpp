#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <winnt.h> // HRESULT
#include <minwindef.h> // UINT

#include <array>
#include <initializer_list>

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
	/*  Explanation:
	 *
	 *    Instead of restricting the size of the buffer at compile time (like DXStrictBuffer),
	 *    the size is instead set at runtime, allowing storage capacity to be determined at runtime.
	 * 
	 *  NOTE:
	 * 
	 *    !! The immutability factor of DXBuffer's does NOT change with this implementation. !!
	 * 
	 *    For both DXRuntimeBuffer, and DXStrictBuffer, the size of the buffers are fixed after instantiation.
	 *    However, DXStrictBuffer takes it's size as a template parameter, which allows stack allocation of elements via std::array.
	 *    On the contrary, since DXRuntimeBuffer's buffer size is determined at runtime, heap allocation is inevitable.
	 */
	class DXRuntimeBuffer
	{

	};

	template <typename T, UINT Elems, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags = (D3D11_CPU_ACCESS_FLAG)(0)>
	class DXStrictBuffer
	{
		template<typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline DXStrictBuffer(ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: m_Stride(sizeof(T)), m_Bytes(Elems* m_Stride), mP_DeviceRef(pDeviceRef),
			  mP_DeviceContextRef(pDeviceContextRef) {}

		inline DXStrictBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: m_Data(data), m_Stride(sizeof(T)), m_Bytes(Elems * m_Stride), mP_DeviceRef(pDeviceRef),
			  mP_DeviceContextRef(pDeviceContextRef) {}

		inline DXStrictBuffer(const std::initializer_list<T>& dataList, ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: m_Stride(sizeof(T)), m_Bytes(Elems * m_Stride), mP_DeviceRef(pDeviceRef),
			  mP_DeviceContextRef(pDeviceContextRef)
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

			m_Stride = sizeof(T);
			m_Bytes = Elems * m_Stride;

			return *this;
		}

		~DXStrictBuffer() = default;
	public:
		inline [[nodiscard]] HRESULT Create() noexcept
		{
			RUNTIME_ASSERT(!m_IsCreated, "A buffer should not be re-created.\n");

			D3D11_BUFFER_DESC vBufferDesc = {};
			vBufferDesc.ByteWidth = (unsigned int)m_Bytes;
			vBufferDesc.Usage = Usage;
			vBufferDesc.BindFlags = BindFlags;
			vBufferDesc.CPUAccessFlags = CPUFlags;
			vBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vBufferSubData = {};
			vBufferSubData.pSysMem = Data();

			m_IsCreated = true;
			return mP_DeviceRef->CreateBuffer(&vBufferDesc, &vBufferSubData, mP_Buffer.GetAddressOf());
		}
	public:
		inline constexpr [[nodiscard]] const T* Data() const noexcept { return m_Data.data(); }
		inline constexpr [[nodiscard]] UINT Elements() const noexcept { return Elems; }
		inline constexpr [[nodiscard]] UINT Stride() const noexcept { return m_Stride; }
		inline constexpr [[nodiscard]] UINT Bytes() { return m_Bytes; }
		inline [[nodiscard]] const ComPtr<ID3D11Buffer>& ComBuffer() const noexcept { return mP_Buffer; }
		inline [[nodiscard]] bool IsCreated() const noexcept { return m_IsCreated; }
	protected:
		std::array<T, Elems> m_Data;
		UINT m_Stride;
		UINT m_Bytes;
		UINT m_Offset = 0;
		ComPtr<ID3D11Buffer> mP_Buffer;
		ComPtr<ID3D11Device1>& mP_DeviceRef;
		ComPtr<ID3D11DeviceContext1>& mP_DeviceContextRef;
		bool m_IsCreated = false;
	};

	template <typename T, UINT Elems, D3D11_USAGE = D3D11_USAGE_IMMUTABLE>
	class DXStrictVertexBuffer : public DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>
	{
		template <typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline DXStrictVertexBuffer(ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>(pDeviceRef, pDeviceContextRef) {}

		inline DXStrictVertexBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>(data, pDeviceRef, pDeviceContextRef) {}

		inline DXStrictVertexBuffer(const std::initializer_list<T>& dataList, ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>(dataList, pDeviceRef, pDeviceContextRef) {}

		inline DXStrictVertexBuffer& operator=(const std::initializer_list<T>& dataList) noexcept
		{
			// Call the base class operator=.
			static_cast<DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>&>(*this).operator=(dataList);
			return *this;
		}

		~DXStrictVertexBuffer() = default;
	public:
		inline void Bind(UINT startSlot = 0u) noexcept
		{
			this->mP_DeviceContextRef->IASetVertexBuffers(startSlot, 1, this->mP_Buffer.GetAddressOf(), &this->m_Stride, &this->m_Offset);
		}
	};

	template <typename T, UINT Elems, DXGI_FORMAT DXGIFormat>
	class DXStrictIndexBuffer : public DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>
	{
		template <typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline DXStrictIndexBuffer(ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>(pDeviceRef, pDeviceContextRef) {}

		inline DXStrictIndexBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>(data, pDeviceRef, pDeviceContextRef) {}

		inline DXStrictIndexBuffer(const std::initializer_list<T>& dataList, ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>(dataList, pDeviceRef, pDeviceContextRef) {}

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
			this->mP_DeviceContextRef->IASetIndexBuffer(this->mP_Buffer.Get(), DXGIFormat, 0);
		}
	};

	template <typename T, UINT Elems>
	class DXStrictConstantBuffer : public DXStrictBuffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>
	{
		template <typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline DXStrictConstantBuffer(ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>(pDeviceRef, pDeviceContextRef) {}

		inline DXStrictConstantBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>(data, pDeviceRef, pDeviceContextRef) {}

		inline DXStrictConstantBuffer(const std::initializer_list<T>& dataList, ComPtr<ID3D11Device1>& pDeviceRef, ComPtr<ID3D11DeviceContext1>& pDeviceContextRef) noexcept
			: DXStrictBuffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE>(dataList, pDeviceRef, pDeviceContextRef) {}

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
			this->mP_DeviceContextRef->VSSetConstantBuffers(startSlot, 1, this->mP_Buffer.GetAddressOf());
		}
	};
}