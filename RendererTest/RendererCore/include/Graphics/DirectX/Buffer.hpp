#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <winnt.h> // HRESULT
#include <minwindef.h> // UINT

#include <array>
#include <initializer_list>

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Window::Graphics::CTMDirectX
{
	template <typename T, UINT Elems, D3D11_USAGE Usage, D3D11_BIND_FLAG BindFlags, D3D11_CPU_ACCESS_FLAG CPUFlags = (D3D11_CPU_ACCESS_FLAG)(0)>
	class Buffer
	{
		template<typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline Buffer(ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: m_Data(), m_Stride(sizeof(T)), m_Bytes(Elems * m_Stride), mP_DeviceRef(pDeviceRef),
			  mP_DeviceContextRef(pDeviceContextRef), mP_Buffer() {
		}

		inline Buffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: m_Data(data), m_Stride(sizeof(T)), m_Bytes(Elems * m_Stride), mP_DeviceRef(pDeviceRef),
			  mP_DeviceContextRef(pDeviceContextRef), mP_Buffer() {}

		inline Buffer(const std::initializer_list<T>& list, ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: m_Stride(sizeof(T)), m_Bytes(Elems * m_Stride), mP_DeviceRef(pDeviceRef),
			  mP_DeviceContextRef(pDeviceContextRef), mP_Buffer()
		{
			RUNTIME_ASSERT(Elems == list.size(), "Buffer size and list size don't match.\n");
			std::copy(list.begin(), list.end(), m_Data.data());
		}

		inline Buffer<T, Elems, Usage, BindFlags, CPUFlags>& operator=(const std::initializer_list<T>& list) noexcept
		{
			RUNTIME_ASSERT(Elems == list.size(), "Buffer size and list size don't match.\n");
			std::copy(list.begin(), list.end(), m_Data.data());

			m_Stride = sizeof(T);
			m_Bytes = Elems * m_Stride;

			return *this;
		}

		virtual ~Buffer() = default;
	public:
		inline [[nodiscard]] HRESULT Create() noexcept
		{
			RUNTIME_ASSERT(!m_Created, "A buffer should not be re-created.\n");

			D3D11_BUFFER_DESC vBufferDesc = {};
			vBufferDesc.ByteWidth = (unsigned int)m_Bytes;
			vBufferDesc.Usage = Usage;
			vBufferDesc.BindFlags = BindFlags;
			vBufferDesc.CPUAccessFlags = CPUFlags;
			vBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vBufferSubData = {};
			vBufferSubData.pSysMem = Data();

			m_Created = true;
			return mP_DeviceRef->CreateBuffer(&vBufferDesc, &vBufferSubData, mP_Buffer.GetAddressOf());
		}

		virtual void Bind(UINT startSlot = 0u) noexcept = 0;
	public:
		inline constexpr [[nodiscard]] const T* Data() const noexcept { return m_Data.data(); }
		inline constexpr [[nodiscard]] UINT Elements() const noexcept { return Elems; }
		inline constexpr [[nodiscard]] UINT Stride() const noexcept { return m_Stride; }
		inline constexpr [[nodiscard]] UINT Bytes() { return m_Bytes; }
		inline [[nodiscard]] const ComPtr<ID3D11Buffer>& ComBuffer() const noexcept { return mP_Buffer; }
	protected:
		std::array<T, Elems> m_Data;
		UINT m_Stride;
		UINT m_Bytes;
		UINT m_Offset = 0;
		ComPtr<ID3D11Buffer> mP_Buffer;
		ComPtr<ID3D11Device>& mP_DeviceRef;
		ComPtr<ID3D11DeviceContext>& mP_DeviceContextRef;
		bool m_Created = false;
	};

	template <typename T, UINT Elems>
	class VertexBuffer : public Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>
	{
		template <typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline VertexBuffer(ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>(pDeviceRef, pDeviceContextRef) {}

		inline VertexBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>(data, pDeviceRef, pDeviceContextRef) {}

		inline VertexBuffer(const std::initializer_list<T>& list, ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>(list, pDeviceRef, pDeviceContextRef) {}

		inline VertexBuffer& operator=(const std::initializer_list<T>& list) noexcept
		{
			// Call the base class operator=.
			static_cast<Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER>&>(*this).operator=(list);
			return *this;
		}

		~VertexBuffer() = default;
	public:
		virtual void Bind(UINT startSlot = 0u) noexcept override
		{
			this->mP_DeviceContextRef->IASetVertexBuffers(startSlot, 1, this->mP_Buffer.GetAddressOf(), &this->m_Stride, &this->m_Offset);
		}
	};

	template <typename T, UINT Elems, DXGI_FORMAT DXGIFormat>
	class IndexBuffer : public Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>
	{
		template <typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline IndexBuffer(ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>(pDeviceRef, pDeviceContextRef) {}

		inline IndexBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>(data, pDeviceRef, pDeviceContextRef) {}

		inline IndexBuffer(const std::initializer_list<T>& list, ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>(list, pDeviceRef, pDeviceContextRef) {}

		inline IndexBuffer& operator=(const std::initializer_list<T>& list) noexcept
		{
			// Call the base class operator=.
			static_cast<Buffer<T, Elems, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER>&>(*this).operator=(list);
			return *this;
		}

		~IndexBuffer() = default;
	public:
		virtual void Bind(UINT startSlot = 0u) noexcept override
		{
			this->mP_DeviceContextRef->IASetIndexBuffer(this->mP_Buffer.Get(), DXGIFormat, 0);
		}
	};

	template <typename T, UINT Elems, D3D11_CPU_ACCESS_FLAG CPUFlags>
	class ConstantBuffer : public Buffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, CPUFlags>
	{
		template <typename Ty>
		using ComPtr = Microsoft::WRL::ComPtr<Ty>;
	public:
		inline ConstantBuffer(ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: Buffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, CPUFlags>(pDeviceRef, pDeviceContextRef) {}

		inline ConstantBuffer(const std::array<T, Elems>& data, ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: Buffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, CPUFlags>(data, pDeviceRef, pDeviceContextRef) {}

		inline ConstantBuffer(const std::initializer_list<T>& list, ComPtr<ID3D11Device>& pDeviceRef, ComPtr<ID3D11DeviceContext>& pDeviceContextRef) noexcept
			: Buffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, CPUFlags>(list, pDeviceRef, pDeviceContextRef) {}

		inline ConstantBuffer& operator=(const std::initializer_list<T>& list) noexcept
		{
			// Call the base class operator=.
			static_cast<Buffer<T, Elems, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, CPUFlags>&>(*this).operator=(list);
			return *this;
		}

		~ConstantBuffer() = default;
	public:
		virtual void Bind(UINT startSlot = 0u) noexcept override
		{
			this->mP_DeviceContextRef->VSSetConstantBuffers(startSlot, 1, this->mP_Buffer.GetAddressOf());
		}
	};
}