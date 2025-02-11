#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/CoreUtility.hpp"
#include "Graphics/Graphics.hpp"

namespace CTMRenderer::Window::Graphics
{
	Graphics::Graphics(const Geometry::WindowArea& windowAreaRef, const Control::Mouse& mouseRef, const unsigned int targetFPS) noexcept
		: m_WindowHandle(nullptr), m_WindowAreaRef(windowAreaRef), m_InfoQueue(), mP_Device(nullptr),
		  mP_SwapChain(nullptr), mP_DeviceContext(nullptr), mP_RTV(nullptr), mP_DSBuffer(nullptr), mP_DSView(nullptr),
		  m_CBTransform(mP_Device, mP_DeviceContext), mP_CBRotation(nullptr), m_ClearColor(0, 0, 0, 0), m_TargetFPS(targetFPS), m_MouseRef(mouseRef)
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

		// Initialize the depth-stencil buffer. (Depth buffer)
		//D3D11_TEXTURE2D_DESC dsbDesc = {};
		//dsbDesc.Width = swapDesc.BufferDesc.Width;
		//dsbDesc.Height = swapDesc.BufferDesc.Height;
		//dsbDesc.MipLevels = 1; // Base mip level. (full resolution)
		//dsbDesc.ArraySize = 1; // Only 1 depth buffer.
		//dsbDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24-bit depth, 8-bit stencil.
		//dsbDesc.SampleDesc.Count = 1;
		//dsbDesc.SampleDesc.Quality = 0;
		//dsbDesc.Usage = D3D11_USAGE_DEFAULT;
		//dsbDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		//dsbDesc.CPUAccessFlags = 0;
		//dsbDesc.MiscFlags = 0;

		//// Create the depth stencil buffer.
		//hResult = mP_Device->CreateTexture2D(&dsbDesc, nullptr, &mP_DSBuffer);
		//RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		//// Initialize the depth-stencil view.
		//D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		//dsvDesc.Format = dsbDesc.Format;
		//dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		//dsvDesc.Texture2D.MipSlice = 0;

		//// Create the depth stencil view.
		//hResult = mP_Device->CreateDepthStencilView(mP_DSBuffer.Get(), &dsvDesc, &mP_DSView);
		//RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		InitTestScene();
	}

	#define NUM_INDICES 36

	void Graphics::InitTestScene() noexcept
	{
		struct Vertex {
			struct {
				float x, y, z;
			} pos;

			struct {
				unsigned char r, g, b, a;
			} color;
		};

		CTMDirectX::VertexBuffer<Vertex, 8> vBuffer(
			{
				// Front face
				{ -1.0f,  1.0f,  1.0f, 255, 255, 255, 255 }, // 0 - Top Left
				{  1.0f,  1.0f,  1.0f, 255, 255, 255, 255 }, // 1 - Top Right
				{  1.0f, -1.0f,  1.0f, 255, 255, 255, 255 }, // 2 - Bottom Right
				{ -1.0f, -1.0f,  1.0f, 255, 255, 255, 255 }, // 3 - Bottom Left

				// Back face
				{ -1.0f,  1.0f, -1.0f, 255, 255, 255, 255 }, // 4 - Top Left
				{  1.0f,  1.0f, -1.0f, 255, 255, 255, 255 }, // 5 - Top Right
				{  1.0f, -1.0f, -1.0f, 255, 255, 255, 255 }, // 6 - Bottom Right
				{ -1.0f, -1.0f, -1.0f, 255, 255, 255, 255 } // 7 - Bottom Left
			},
			mP_Device, mP_DeviceContext
		);

		RUNTIME_ASSERT(vBuffer.Create() == S_OK, "Failed to create buffer.\n");
		vBuffer.Bind();

		CTMDirectX::IndexBuffer<short, 36, DXGI_FORMAT_R16_UINT> iBuffer(
			{
				// Front face
				0, 1, 2,
				0, 2, 3,

				// Back face
				4, 6, 5,
				4, 7, 6,

				// Left face
				4, 0, 3,
				4, 3, 7,

				// Right face
				1, 5, 6,
				1, 6, 2,

				// Top face
				4, 5, 1,
				4, 1, 0,

				// Bottom face
				3, 2, 6,
				3, 6, 7
			},
			mP_Device, mP_DeviceContext
		);

		RUNTIME_ASSERT(iBuffer.Create() == S_OK, "Failed to create buffer.\n");
		iBuffer.Bind();

		m_CBTransform = { { /* Initialize with empty transform */} };
		RUNTIME_ASSERT(m_CBTransform.Create() == S_OK, "Failed to create buffer.\n");
		m_CBTransform.Bind();

		// Target Path (for now) : C:\dev\projects\cpp\Direct3D\RendererTest\bin\out\Debug-windows-x86_64\RendererCore\ 
		const std::filesystem::path shaderPath = Utility::GetBinDirectory().string() + Utility::GetOutDirectory().string();
		const std::string shaderPathStr = shaderPath.string();

		const std::filesystem::path pixelShaderPath = shaderPathStr + "DefaultCubePS.cso";
		const std::filesystem::path vertexShaderPath = shaderPathStr + "DefaultCubeVS.cso";
		
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		CTMDirectX::PixelShader pixelShader(mP_Device, mP_DeviceContext);
		pixelShader.Create(pixelShaderPath, pBlob);
		pixelShader.Bind();

		CTMDirectX::VertexShader vertexShader(mP_Device, mP_DeviceContext);
		vertexShader.Create(vertexShaderPath, pBlob);
		vertexShader.Bind();

		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
		D3D11_INPUT_ELEMENT_DESC inputDescs[] = {
			{ "Position", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
			{ "Color", 0u, DXGI_FORMAT_R8G8B8A8_UNORM, 0u, sizeof(Vertex::pos), D3D11_INPUT_PER_VERTEX_DATA, 0u}
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

		DEBUG_PRINT("Aspect ratio : " << m_WindowAreaRef.aspectRatio << '\n');
		DEBUG_PRINT("Aspect ratio reciprocal : " << m_WindowAreaRef.aspectRatioReciprocal << '\n');
	}

	void Graphics::StartFrame(double elapsedMillis) noexcept
	{
		// Update transformation before rendering.
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		mP_DeviceContext->Map(m_CBTransform.ComBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		RUNTIME_ASSERT(mappedResource.pData != nullptr, "Data is nullptr.\n");

		Transform* pData = reinterpret_cast<Transform*>(mappedResource.pData);
		pData->transform = DirectX::XMMatrixTranspose(
			DirectX::XMMatrixRotationX((float)elapsedMillis) *
			DirectX::XMMatrixRotationZ((float)elapsedMillis) *
			DirectX::XMMatrixTranslation(
				Utility::ToClipSpaceX((float)m_MouseRef.PosX(), 0, (float)m_WindowAreaRef.width),
				Utility::ToClipSpaceY((float)m_MouseRef.PosY(), 0, (float)m_WindowAreaRef.height),
				4.0f
			) *
			DirectX::XMMatrixPerspectiveLH(1.0f, m_WindowAreaRef.aspectRatioReciprocal, 0.5f, 10.0f)
		);

		mP_DeviceContext->Unmap(m_CBTransform.ComBuffer().Get(), 0);

		// Rebind the RenderTargetView.
		BindRTV();

		mP_DeviceContext->ClearRenderTargetView(mP_RTV.Get(), m_ClearColor.rgba);
	}

	void Graphics::Draw() noexcept
	{
		mP_DeviceContext->DrawIndexed(NUM_INDICES, 0, 0);
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
		//mP_DeviceContext->OMSetRenderTargets(1, mP_RTV.GetAddressOf(), mP_DSView.Get());
		mP_DeviceContext->OMSetRenderTargets(1, mP_RTV.GetAddressOf(), nullptr);
	}
}