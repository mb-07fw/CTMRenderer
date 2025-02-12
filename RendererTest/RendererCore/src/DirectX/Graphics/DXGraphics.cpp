#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/CoreUtility.hpp"
#include "DirectX/Graphics/DXGraphics.hpp"

namespace CTMRenderer::CTMDirectX::Window::Graphics
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

	static constexpr UINT NUM_INDICES = 6;

	void Graphics::InitTestScene() noexcept
	{
		struct Vertex {
			struct {
				float x, y;
			} pos;

			struct {
				unsigned char r, g, b, a;
			} color;
		};

		VertexBuffer<Vertex, 4> vBuffer(
			{
				{ -0.5f,  0.5, 255, 255, 255, 255 },
				{  0.5f,  0.5, 255, 255, 255, 255 },
				{  0.5f, -0.5, 255, 255, 255, 255 },
				{ -0.5f, -0.5, 255, 255, 255, 255 }
			},
			mP_Device, mP_DeviceContext
		);

		RUNTIME_ASSERT(vBuffer.Create() == S_OK, "Failed to create buffer.\n");
		vBuffer.Bind();

		IndexBuffer<short, 6, DXGI_FORMAT_R16_UINT> iBuffer(
			{
				0, 2, 3,
				0, 1, 2
			},
			mP_Device, mP_DeviceContext
		);

		RUNTIME_ASSERT(iBuffer.Create() == S_OK, "Failed to create buffer.\n");
		iBuffer.Bind();

		// Target Path (for now) : C:\dev\projects\cpp\Direct3D\RendererTest\bin\out\Debug-windows-x86_64\RendererCore\ 
		const std::filesystem::path shaderPath = Utility::GetBinDirectory().string() + Utility::GetOutDirectory().string();
		const std::string shaderPathStr = shaderPath.string();

		const std::filesystem::path pixelShaderPath = shaderPathStr + "DefaultRectPS.cso";
		const std::filesystem::path vertexShaderPath = shaderPathStr + "DefaultRectVS.cso";
		
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		PixelShader pixelShader(mP_Device, mP_DeviceContext);
		RUNTIME_ASSERT(pixelShader.Create(pixelShaderPath, pBlob)  == S_OK, "Failed to create pixel shader.\n");
		pixelShader.Bind();

		VertexShader vertexShader(mP_Device, mP_DeviceContext);
		RUNTIME_ASSERT(vertexShader.Create(vertexShaderPath, pBlob) == S_OK, "Failed to create vertex shader.\n");
		vertexShader.Bind();

		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
		D3D11_INPUT_ELEMENT_DESC inputDescs[] = {
			{ "Position", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
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