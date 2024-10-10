#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <D3D11.h>

#define IMPLEMENT_RENDER \
ID3D11Device* RendererHelper::m_device = nullptr; \
ID3D11DeviceContext* RendererHelper::m_context = nullptr;\
IDXGIAdapter1* RendererHelper::m_adapter1 = nullptr;\
HWND* RendererHelper::m_hWnd = nullptr; \
HINSTANCE* RendererHelper::m_hInstance = nullptr;

struct RendererHelper
{
public:
	friend class Direct11;
	friend class Graphics;
	RendererHelper() = delete;
	RendererHelper(RendererHelper&) = delete;
	RendererHelper(RendererHelper&&) = delete;

	static ID3D11Device* GetDevice()
	{
		return m_device;
	}
	static ID3D11DeviceContext* GetDeviceContext()
	{
		return m_context;
	}

	static IDXGIAdapter1* GetAdapter()
	{
		return m_adapter1;
	}

	static HWND GetHWND()
	{
		return *m_hWnd;
	}

	static HINSTANCE getHINSTANCE() {
		return reinterpret_cast<HINSTANCE>(GetWindowLongPtr(*m_hWnd, GWLP_HINSTANCE));;
	}

private:
	static void SetDevice(ID3D11Device* device)
	{
		m_device = device;
	}

	static void SetDeviceContext(ID3D11DeviceContext* context)
	{
		m_context = context;
	}

	static void SetAdapter(IDXGIAdapter1* adapter)
	{
		m_adapter1 = adapter;
	}

	static void SetHWND(HWND* hWnd)
	{
		m_hWnd = hWnd;
	}

	static void SetHINSTANCE(HINSTANCE* hInstance)
	{
		m_hInstance = hInstance;
	}

	static ID3D11Device* m_device;
	static ID3D11DeviceContext* m_context;
	static IDXGIAdapter1* m_adapter1;
	static HWND* m_hWnd;
	static HINSTANCE* m_hInstance;

};

#endif // !RENDERER_H_
