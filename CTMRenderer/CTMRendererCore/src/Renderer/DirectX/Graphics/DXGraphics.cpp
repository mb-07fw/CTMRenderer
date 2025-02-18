#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/CoreUtility.hpp"
#include "CTMRenderer/DirectX/Graphics/DXGraphics.hpp"
#include "CTMRenderer/DirectX/Graphics/DXGraphicsUtility.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXShape.hpp"
#include "CTMRenderer/DirectX/Graphics/Bindable/DXBuffer.hpp"
#include "CTMRenderer/DirectX/Graphics/Bindable/DXShader.hpp"
#include "CTMRenderer/DirectX/Graphics/Bindable/DXInputLayout.hpp"
#include "CTMRenderer/DirectX/Graphics/Bindable/DXViewport.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
	DXGraphics::DXGraphics(const DXRendererSettings& settingsRef, const Window::Geometry::WindowArea& windowAreaRef, const Control::Mouse& mouseRef) noexcept
		: m_SettingsRef(settingsRef), m_WindowAreaRef(windowAreaRef), m_MouseRef(mouseRef),
		  m_2DRender(), m_TextRender(), m_ClearColor()
	{
	}

	void DXGraphics::Init(const HWND windowHandle) noexcept
	{
		DEBUG_PRINT("(Graphics.Init) Target FPS : " + std::to_string(m_SettingsRef.TargetFPS) + '\n');

		m_WindowHandle = windowHandle;

		DXGI_SWAP_CHAIN_DESC swapDesc = {};
		swapDesc.BufferDesc.Width = m_WindowAreaRef.width;
		swapDesc.BufferDesc.Height = m_WindowAreaRef.height;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapDesc.BufferDesc.RefreshRate.Numerator = m_SettingsRef.TargetFPS;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0; // Quality must be zero for a sample count of 1.
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		swapDesc.BufferCount = 2; // For buffer flipping.
		swapDesc.OutputWindow = m_WindowHandle;
		swapDesc.Windowed = TRUE;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapDesc.Flags = 0;

		Microsoft::WRL::ComPtr<ID3D11Device> pBaseDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pBaseContext;

		HRESULT hResult = S_OK;
		hResult = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, // Bitwise or D3D11_CREATE_DEVICE_BGRA_SUPPORT for D2D interop.
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapDesc,
			&mP_SwapChain,
			&pBaseDevice,
			nullptr,
			&pBaseContext
		);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		// Upgrade device to ID3D11Device1. (D3D11.1)
		hResult = pBaseDevice.As(&mP_Device);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		// Upgrade device context to ID3D11DeviceContext1. (D3D11.1)
		hResult = pBaseContext.As(&mP_DeviceContext);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		m_InfoQueue.Init(mP_Device);

		// Get the back buffer.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
		hResult = mP_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		// Create the RTV.
		hResult = mP_Device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, mP_RTV.GetAddressOf());
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		Init2D();
		InitText();
		InitTestScene();
	}

	void DXGraphics::Init2D() noexcept
	{
		// Initialize the factory option's debug level.
		D2D1_FACTORY_OPTIONS options = {};
		options.debugLevel = D2D1_DEBUG_LEVEL_ERROR;

		// Create the D2D factory.
		HRESULT hResult = D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory1),
			&options,
			reinterpret_cast<void**>(m_2DRender.pFactory.GetAddressOf())
		);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		// Get IDXGIDevice from ID3D11Device1 for interop.
		Microsoft::WRL::ComPtr<IDXGIDevice> pDXGIDevice;
		hResult = mP_Device.As(&pDXGIDevice);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		// Create a D2D device using the D3D device as IDXGIDevice.
		hResult = m_2DRender.pFactory->CreateDevice(pDXGIDevice.Get(), m_2DRender.pDevice.GetAddressOf());
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		// Create the D2D device context.
		hResult = m_2DRender.pDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_2DRender.pDeviceContext.GetAddressOf());
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		// Get back buffer as IDXGISurface for D2D interop.
		Microsoft::WRL::ComPtr<IDXGISurface> pDXGIBackBuffer;
		hResult = mP_SwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(pDXGIBackBuffer.GetAddressOf()));
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));
		
		// Initialize RTV properties.
		D2D1_RENDER_TARGET_PROPERTIES properties = {};
		properties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		properties.pixelFormat = D2D1_PIXEL_FORMAT(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

		// Create RTV from the surface. (Back buffer)
		m_2DRender.pFactory->CreateDxgiSurfaceRenderTarget(
			pDXGIBackBuffer.Get(),
			&properties,
			m_2DRender.pRTV.GetAddressOf()
		);
	}

	void DXGraphics::InitText() noexcept
	{
		m_TextRender.text = L"Hello World!!!";
		
		HRESULT hResult = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_ISOLATED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(m_TextRender.pDWriteFactory.GetAddressOf())
		);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		hResult = m_TextRender.pDWriteFactory->CreateTextFormat(
			L"Gabriola", // Font family name.
			NULL,		 // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			72.0f,
			L"en-us",
			m_TextRender.pTextFormat.GetAddressOf()
		);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		hResult = m_TextRender.pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		hResult = m_TextRender.pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		hResult = m_2DRender.pRTV->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::BlueViolet),
			m_TextRender.pSCBrush.GetAddressOf()
		);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		m_TextRender.layoutRect = D2D1::RectF(
			0.0f,
			0.0f,
			(FLOAT)m_WindowAreaRef.width,
			(FLOAT)m_WindowAreaRef.height
		);
	}

	void DXGraphics::InitTestScene() noexcept
	{
		DEBUG_PRINT("Screen Left : " << NDCToScreenX(-1.0f, (float)m_WindowAreaRef.width) << '\n');
		DEBUG_PRINT("Screen Top : " << NDCToScreenY(-1.0f, (float)m_WindowAreaRef.height) << '\n');
		DEBUG_PRINT("Screen Right : " << NDCToScreenX(1.0f, (float)m_WindowAreaRef.width) << '\n');
		DEBUG_PRINT("Screen Bottom : " << NDCToScreenY(1.0f, (float)m_WindowAreaRef.height) << '\n');

		DEBUG_PRINT("Screen CenterX : " << NDCToScreenX(0, (float)m_WindowAreaRef.width) << '\n');
		DEBUG_PRINT("Screen CenterY : " << NDCToScreenY(0, (float)m_WindowAreaRef.height) << '\n');

		struct ScreenSpace {
			DirectX::XMFLOAT2 WidthHeight = {};
			DirectX::XMFLOAT2 Padding = {};
		};

		Bindable::DXStrictConstantBuffer<ScreenSpace, 1> cScreenBuffer(
			{
				{ { (float)m_WindowAreaRef.width, (float)m_WindowAreaRef.height } }
			},
			mP_Device, mP_DeviceContext
		);
		RUNTIME_ASSERT(cScreenBuffer.Create() == S_OK, "Failed to create constant buffer.\n");
		cScreenBuffer.Bind();

		struct Vertex {
			DirectX::XMFLOAT2 pos;
		};

		Geometry::DXRect sourceRect(
			0, 0, (float)m_WindowAreaRef.width, (float)m_WindowAreaRef.height,
			DXColor(DXColorType::BLUE)
		);

		DEBUG_PRINT("Rect Left : " << sourceRect.AABB.left << '\n');
		DEBUG_PRINT("Rect Top : " << sourceRect.AABB.top << '\n');
		DEBUG_PRINT("Rect Right : " << sourceRect.AABB.right << '\n');
		DEBUG_PRINT("Rect Bottom : " << sourceRect.AABB.bottom << '\n');

		// Define basic quad geometry from base quad / rect.
		Bindable::DXStrictVertexBuffer<Vertex, 4> vGeometryBuffer(
			{
				{ { sourceRect.AABB.left,  sourceRect.AABB.top }   },
				{ { sourceRect.AABB.right, sourceRect.AABB.top }   },
				{ { sourceRect.AABB.right, sourceRect.AABB.bottom } },
				{ { sourceRect.AABB.left,  sourceRect.AABB.bottom } }
			},
			mP_Device, mP_DeviceContext
		);
		RUNTIME_ASSERT(vGeometryBuffer.Create() == S_OK, "Failed to create buffer.\n");
		vGeometryBuffer.Bind(); // Pass in base quad as per vertex data.

		Geometry::DXRect targetRectOne(
			100, 100, 300, 300,
			DXColor(DXColorType::RED)
		);

		Geometry::DXRect targetRectTwo(
			450, 250, 700, 500,
			DXColor(DXColorType::GREEN)
		);

		float scalarXOne = targetRectOne.AABB.width / sourceRect.AABB.width;
		float scalarYOne = targetRectOne.AABB.height / sourceRect.AABB.height;

		float scalarXTwo = targetRectTwo.AABB.width / sourceRect.AABB.width;
		float scalarYTwo = targetRectTwo.AABB.height / sourceRect.AABB.height;

		float offsetXOne = targetRectOne.AABB.left - sourceRect.AABB.left;
		float offsetYOne = targetRectOne.AABB.top - sourceRect.AABB.top;

		float offsetXTwo = targetRectTwo.AABB.left - sourceRect.AABB.left;
		float offsetYTwo = targetRectTwo.AABB.top - sourceRect.AABB.top;

		struct InstanceData {
			DirectX::XMFLOAT2 scalarXY = {}; // X and Y scale factors.
			DirectX::XMFLOAT2 offsetXY = {}; // X and Y scale factors.
			DXColor color = {};
		};

		

		Bindable::DXStrictVertexBuffer<InstanceData, 2> vInstanceBuffer(
			{
				{ { scalarXOne, scalarYOne }, { offsetXOne, offsetYOne }, targetRectOne.color },
				{ { scalarXTwo, scalarYTwo}, { offsetXTwo, offsetYTwo }, targetRectTwo.color }
			},
			mP_Device, mP_DeviceContext
		);
		RUNTIME_ASSERT(vInstanceBuffer.Create() == S_OK, "Failed to create instance buffer.\n");
		vInstanceBuffer.Bind(1);

		Bindable::DXStrictIndexBuffer<short, 6, DXGI_FORMAT_R16_UINT> iBuffer(
			{
				0, 1, 2, // First Triangle
				2, 3, 0  // Second Triangle
			},
			mP_Device, mP_DeviceContext
		);
		RUNTIME_ASSERT(iBuffer.Create() == S_OK, "Failed to create index buffer.\n");
		iBuffer.Bind();

		// Target Path (for now) : C:\dev\projects\cpp\Direct3D\RendererTest\bin\out\Debug-windows-x86_64\RendererCore\ 
		const std::filesystem::path shaderPath = Utility::GetBinDirectory().string() + Utility::GetOutDirectory().string();
		const std::string shaderPathStr = shaderPath.string();

		const std::filesystem::path pixelShaderPath = shaderPathStr + "DefaultRectPS.cso";
		const std::filesystem::path vertexShaderPath = shaderPathStr + "DefaultRectVS.cso";
		
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		DXPixelShader pixelShader(mP_Device, mP_DeviceContext);
		RUNTIME_ASSERT(pixelShader.Create(pixelShaderPath, pBlob)  == S_OK, "Failed to create pixel shader.\n");
		pixelShader.Bind();

		DXVertexShader vertexShader(mP_Device, mP_DeviceContext);
		RUNTIME_ASSERT(vertexShader.Create(vertexShaderPath, pBlob) == S_OK, "Failed to create vertex shader.\n");
		vertexShader.Bind();

		DXInputLayout<4> inputLayout(
			{
				{ "POSITION", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },

				{ "INSTANCE_SCALAR", 0u, DXGI_FORMAT_R32G32_FLOAT, 1u, 0u, D3D11_INPUT_PER_INSTANCE_DATA, 1u },
				{ "INSTANCE_OFFSET", 0u, DXGI_FORMAT_R32G32_FLOAT, 1u, sizeof(DirectX::XMFLOAT2), D3D11_INPUT_PER_INSTANCE_DATA, 1u },
				{ "INSTANCE_COLOR", 0u, DXGI_FORMAT_R8G8B8A8_UNORM, 1u, sizeof(DirectX::XMFLOAT2) * 2, D3D11_INPUT_PER_INSTANCE_DATA, 1u }
			},
			pBlob, mP_Device, mP_DeviceContext
		);
		RUNTIME_ASSERT(inputLayout.Create() == S_OK, "Failed to create input layout.\n");
		inputLayout.Bind();

		Bindable::DXViewport viewport(
			0.0f, // Top-left x.
			0.0f, // Top-left y.
			(float)m_WindowAreaRef.width, // Viewport width.
			(float)m_WindowAreaRef.height, // Viewport height.
			0.0f, // Min depth.
			1.0f, // Max depth.
			mP_DeviceContext
		);
		viewport.Bind();

		mP_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DEBUG_PRINT("Aspect ratio : " << m_WindowAreaRef.aspectRatio << '\n');
		DEBUG_PRINT("Aspect ratio reciprocal : " << m_WindowAreaRef.aspectRatioReciprocal << '\n');
	}

	void DXGraphics::StartFrame(double elapsedMillis) noexcept
	{
		// Rebind the RenderTargetView.
		BindRTV();

		mP_DeviceContext->ClearRenderTargetView(mP_RTV.Get(), m_ClearColor.rgba);
	}

	void DXGraphics::Draw() noexcept
	{
		mP_DeviceContext->DrawIndexedInstanced(6, 2, 0, 0, 0);
		RUNTIME_ASSERT(m_InfoQueue.IsQueueEmpty() == true, m_InfoQueue.GetMessages());

		m_2DRender.pRTV->BeginDraw();
		// Note to self : Clearing the D2D RTV when it references the same texture as the D3D RTV 
		//			      also clears any rendering done with the D3D RTV. Save yourself some tears.
		//m_2DRender.pRTV->Clear(); 

		m_2DRender.pRTV->SetTransform(D2D1::IdentityMatrix());

		m_2DRender.pRTV->DrawText(
			m_TextRender.text.data(),
			(UINT32)m_TextRender.text.size(),
			m_TextRender.pTextFormat.Get(),
			m_TextRender.layoutRect,
			m_TextRender.pSCBrush.Get()
		);

		HRESULT hResult = m_2DRender.pRTV->EndDraw();
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));
	}

	void DXGraphics::EndFrame() noexcept
	{
		HRESULT hResult = mP_SwapChain->Present(SYNC_INTERVAL, 0u);
		RUNTIME_ASSERT(m_InfoQueue.IsQueueEmpty() == true, m_InfoQueue.GetMessages());
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));
	}

	void DXGraphics::BindRTV() const noexcept
	{
		mP_DeviceContext->OMSetRenderTargets(1, mP_RTV.GetAddressOf(), nullptr);
	}
}