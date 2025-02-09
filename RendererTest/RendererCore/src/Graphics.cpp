#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/CoreUtility.hpp"
#include "Graphics.hpp"

namespace CTMRenderer::Window::Graphics
{
	Graphics::Graphics(const Geometry::WindowArea& windowAreaRef, const Control::Mouse& mouseRef, const unsigned int targetFPS) noexcept
		: m_WindowHandle(nullptr), m_WindowAreaRef(windowAreaRef), m_InfoQueue(), m_InitializedScene(false),
		  mP_Device(nullptr), mP_SwapChain(nullptr), mP_DeviceContext(nullptr), mP_RTV(nullptr), mP_ConstantBuffer(),
		  m_ClearColor(0, 0, 0, 0), m_TargetFPS(targetFPS), m_MouseRef(mouseRef)
	{
	}

	void Graphics::Init(const HWND windowHandle) noexcept
	{
		DEBUG_PRINT("(Graphics.Init) Target FPS : " + std::to_string(m_TargetFPS) + '\n');

		m_WindowHandle = windowHandle;

		DXGI_SWAP_CHAIN_DESC swapDesc = {};
		swapDesc.BufferDesc.Width = m_WindowAreaRef.width;
		swapDesc.BufferDesc.Height = m_WindowAreaRef.height;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapDesc.BufferDesc.RefreshRate.Numerator = m_TargetFPS;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0; // Quality must be zero for a sample count of 1.
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.BufferCount = 2; // For buffer flipping.
		swapDesc.OutputWindow = m_WindowHandle;
		swapDesc.Windowed = TRUE;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapDesc.Flags = 0;

		HRESULT hResult = S_OK;
		hResult = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapDesc,
			&mP_SwapChain,
			&mP_Device,
			nullptr,
			&mP_DeviceContext
		);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		m_InfoQueue.Init(mP_Device);

		// Get the back buffer.
		Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
		hResult = mP_SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		// Create the RTV.
		hResult = mP_Device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, mP_RTV.GetAddressOf());
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		InitTestScene();
	}

	void Graphics::InitTestScene() noexcept
	{
		struct Vertex {
			struct {
				float x, y;
			} pos;

			struct {
				float r, g, b, a;
			} color;
		};

		const Vertex vertices[] = {
			{ -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
			{ -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
		};

		const short indices[] = {
			0, 1, 2,
			0, 2, 3
		};

		const unsigned int vBufferStride = sizeof(Vertex);
		const unsigned int vBufferOffset = 0;

		Microsoft::WRL::ComPtr<ID3D11Buffer> pVBuffer = nullptr;
		D3D11_BUFFER_DESC vBufferDesc = {};
		vBufferDesc.ByteWidth = sizeof(vertices);
		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.CPUAccessFlags = 0;
		vBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vBufferSubData = {};
		vBufferSubData.pSysMem = vertices;

		mP_Device->CreateBuffer(&vBufferDesc, &vBufferSubData, pVBuffer.GetAddressOf());
		mP_DeviceContext->IASetVertexBuffers(0, 1, pVBuffer.GetAddressOf(), &vBufferStride, &vBufferOffset);

		Microsoft::WRL::ComPtr<ID3D11Buffer> pIBuffer = nullptr;
		D3D11_BUFFER_DESC iBufferDesc = {};
		iBufferDesc.ByteWidth = sizeof(indices);
		iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iBufferDesc.CPUAccessFlags = 0;
		iBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iBufferSubData = {};
		iBufferSubData.pSysMem = indices;

		mP_Device->CreateBuffer(&iBufferDesc, &iBufferSubData, pIBuffer.GetAddressOf());
		mP_DeviceContext->IASetIndexBuffer(pIBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		Transform transformData = { { 0.0f, 0.0f }, 0, 0 };
		D3D11_BUFFER_DESC cBufferDesc = {};
		cBufferDesc.ByteWidth = sizeof(transformData);
		cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cBufferDesc.MiscFlags = 0;
		
		D3D11_SUBRESOURCE_DATA cBufferData = {};
		cBufferData.pSysMem = &transformData;

		mP_Device->CreateBuffer(&cBufferDesc, &cBufferData, mP_ConstantBuffer.GetAddressOf());
		mP_DeviceContext->VSSetConstantBuffers(0, 1, mP_ConstantBuffer.GetAddressOf());

		// Target Path (for now) : C:\dev\projects\cpp\Direct3D\RendererTest\bin\out\Debug-windows-x86_64\RendererCore\ 
		const std::filesystem::path shaderPath = Utility::GetBinDirectory().string() + Utility::GetOutDirectory().string();
		const std::string shaderPathStr = shaderPath.string();

		const std::filesystem::path pixelShaderPath = shaderPathStr + "DefaultPS.cso";
		const std::filesystem::path vertexShaderPath = shaderPathStr + "DefaultVS.cso";

		//const std::string pixelShaderPathStr = pixelShaderPath.string();
		//const std::string vertexShaderPathStr = vertexShaderPath.string();

		const std::wstring pixelShaderPathWStr = pixelShaderPath.wstring();
		const std::wstring vertexShaderPathWStr = vertexShaderPath.wstring();

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob = nullptr;
		Microsoft::WRL::ComPtr <ID3D11PixelShader> pPShader;
		D3DReadFileToBlob(pixelShaderPathWStr.c_str(), &pBlob);

		mP_Device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pPShader.GetAddressOf());
		mP_DeviceContext->PSSetShader(pPShader.Get(), nullptr, 0u);

		D3DReadFileToBlob(vertexShaderPathWStr.c_str(), &pBlob);

		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVShader = nullptr;

		mP_Device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pVShader.GetAddressOf());
		mP_DeviceContext->VSSetShader(pVShader.Get(), nullptr, 0);

		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
		D3D11_INPUT_ELEMENT_DESC inputDescs[] = {
			{ "Position", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
			{ "Color", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u, sizeof(float) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0u}
		};

		mP_Device->CreateInputLayout(inputDescs, std::size(inputDescs), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), pInputLayout.GetAddressOf());
		mP_DeviceContext->IASetInputLayout(pInputLayout.Get());

		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (FLOAT)m_WindowAreaRef.width;
		viewport.Height = (FLOAT)m_WindowAreaRef.height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;

		mP_DeviceContext->RSSetViewports(1, &viewport);

		mP_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Graphics::StartFrame() noexcept
	{
		// Update transformation before rendering.
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		mP_DeviceContext->Map(mP_ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		RUNTIME_ASSERT(mappedResource.pData != nullptr, "Data is nullptr.\n");

		Transform* pData = reinterpret_cast<Transform*>(mappedResource.pData);
		pData->translation = { 
			Utility::ToClipSpaceX((float)m_MouseRef.PosX(), 0, (float)m_WindowAreaRef.width), 
			Utility::ToClipSpaceY((float)m_MouseRef.PosY(), 0, (float)m_WindowAreaRef.height) 
		};

		mP_DeviceContext->Unmap(mP_ConstantBuffer.Get(), 0);
	
		// Rebind the RenderTargetView.
		BindRTV();

		mP_DeviceContext->ClearRenderTargetView(mP_RTV.Get(), m_ClearColor.rgba);
	}

	void Graphics::Draw() noexcept
	{
		mP_DeviceContext->DrawIndexed(6, 0, 0);
		RUNTIME_ASSERT(m_InfoQueue.IsQueueEmpty() == true, m_InfoQueue.GetMessagesAsStr());
	}

	void Graphics::EndFrame() noexcept
	{
		HRESULT hResult = mP_SwapChain->Present(SYNC_INTERVAL, 0u);
		RUNTIME_ASSERT(m_InfoQueue.IsQueueEmpty() == true, m_InfoQueue.GetMessagesAsStr());
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));
	}

	void Graphics::BindRTV() const noexcept
	{
		// Bind the RTV.
		mP_DeviceContext->OMSetRenderTargets(1, mP_RTV.GetAddressOf(), nullptr);
	}
}