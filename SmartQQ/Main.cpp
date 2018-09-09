#include <iostream>
#include <WinSock2.h>
#include <errno.h>
#include <fstream>
#include <queue>
#include <string>
#include <sstream>
#include <regex>
#include "windows.h"
#include "TCHAR.h"
#define _TIMESPEC_DEFINED
#include "pthread.h"
using namespace std;
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"pthreadVC2.lib")


char picbuff[1024] = { 0 };
int piclen = 0;



int ParseURL(const string &url, string &host, string &resource)
{
	if (strlen(url.c_str()) > 200)
	{
		printf("url too long");
		return 0;
	}
	
	const char *pos = strstr(url.c_str(), "http://");
	if (pos != NULL)
		pos += strlen("http://");
	else
	{
		if ((pos = strstr(url.c_str(), "https://")) == NULL)
			pos = url.c_str();
		else
			pos += strlen("https://");
	}

	if (strstr(pos, "/") == 0)
	{
		printf("url pause fail:%s\n", url);
		return 0;
	}

}

int CreatSocket(string host)
{
	struct hostent* hp = gethostbyname(host.c_str());
	if (hp == NULL)
	{
		printf("can not get host address\n");
		return 0;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
	{
		printf("socket error\n");
		return 0;
	}

	SOCKADDR_IN sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(80);
	memcpy(&sin.sin_addr, hp->h_addr, sizeof(hp->h_addr));
	//sin.sin_addr.S_un.S_addr = inet_addr(hp->h_addr);
	printf("IP:%d.%d.%d.%d\n", sin.sin_addr.S_un.S_un_b.s_b1,
		sin.sin_addr.S_un.S_un_b.s_b1,
		sin.sin_addr.S_un.S_un_b.s_b3,
		sin.sin_addr.S_un.S_un_b.s_b4);

	if (connect(sock, (sockaddr*)&sin, sizeof(sin)))
	{
		printf("connect fail\n");
		closesocket(sock);
		return 0;
	}
	else
		printf("connect success\n");
	return sock;
}

int GetData(SOCKET sock, char* buf, string host, string path, string get_conent)
{
	stringstream str;
	str << "GET " << path << "?" << get_conent;
	str << " HTTP/1.0\r\n";
	str << "HOST: " << host << "\r\n";
	str << "accept: */*;\r\n";
	str << "accept-language: zh-CN, zh;\r\n";
	//str << "User_Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	str << "Connection:close\r\n\r\n";

	send(sock, str.str().c_str(), str.str().size(), 0);

	Sleep(300);
	int offset = 0;
	int rc = 0;
	while (rc = recv(sock, buf + offset, 1024, 0))
	{
		offset+=rc;
	}

	printf("recieve OK: %d\n",offset);
	return offset;
}

int MsgProsess()
{
	regex state("HTTP/1.[0-1] [0-9]* [A-Z]*\r\n");
	regex date("Date:.*\r\n");
	regex allow("Allow:.*\r\n");
	regex content_encoding("Content-Encoding:.*\r\n");
	regex content_lenth("Content-Length:.*\r\n");
	regex content_type("Conent-Type:.*\r\n");
	regex connection("Connection:.*\r\n");
	regex cache_control("Cache-Control:.*\r\n");
	regex expires("Expries:.*\r\n");
	regex last_modified("Last-Modified:.*\r\n");
	regex location("Location:.*\r\n");
	regex refresh("Refresh:.*\r\n");
	regex server("Server:.*\r\n");
	regex pragma("Pragma:.*\r\n");
	regex p3p("P3P:.*\r\n");
	regex set_cookie("Set-Cookie:.*\r\n");
	regex transfer_encoding("Transfer-Encoding:.*\r\n");
	regex www_authenticate("WWW-Authenticate:.*\r\n");


	return 0;
}

int GetHead(char* buff)
{
	string str = buff;
	int index = str.find("\r\n\r\n");
	return index+4;
}

void draw(HWND hWnd);
LRESULT CALLBACK WndPrc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
);

int wnd_main()
{
	HINSTANCE hInstance;
	hInstance = GetModuleHandle(NULL);
	WNDCLASS Draw;
	Draw.cbClsExtra = 0;
	Draw.cbWndExtra = 0;
	Draw.hCursor = LoadCursor(hInstance, IDC_ARROW);
	Draw.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	Draw.lpszMenuName = NULL;
	Draw.style = CS_HREDRAW | CS_VREDRAW;
	Draw.hbrBackground = (HBRUSH)COLOR_WINDOW;
	Draw.lpfnWndProc = WndPrc;
	Draw.lpszClassName = _T("dDraw");
	Draw.hInstance = hInstance;

	RegisterClass(&Draw);

	HWND hwnd = CreateWindow(
		_T("dDraw"),
		L"ЖўЮЌТы",
		WS_OVERLAPPEDWINDOW,
		30,
		30,
		150,
		150,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass(_T("dDraw"),Draw.hInstance);
	return 0;
}


LRESULT CALLBACK WndPrc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		draw(hWnd);
		EndPaint(hWnd, &ps);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

#if 0
#include <ocidl.h>
#include <olectl.h>
void draw(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);

	HGLOBAL hImageMemory = GlobalAlloc(GMEM_MOVEABLE, piclen);
	void *pImageMemory = GlobalLock(hImageMemory);
	memcpy(pImageMemory, picbuff, piclen);
	GlobalUnlock(hImageMemory);

	IStream *pIStream = NULL;
	IPicture *pIPicture = NULL;

	HRESULT ss = CreateStreamOnHGlobal(hImageMemory, FALSE, &pIStream);
	HRESULT hr = OleLoadPicture(pIStream, piclen, FALSE, IID_IPicture, (LPVOID*)&pIPicture);
	if (SUCCEEDED(hr) && pIPicture)
	{
		OLE_XSIZE_HIMETRIC hmwidth;
		OLE_XSIZE_HIMETRIC hmheight;
		pIPicture->get_Width(&hmwidth);
		pIPicture->get_Height(&hmheight);

		int nWidth = MulDiv(hmwidth, GetDeviceCaps(hdc, LOGPIXELSX), 2540);
		int nHeight = MulDiv(hmheight, GetDeviceCaps(hdc, LOGPIXELSY), 2540);

		pIPicture->Render(hdc, 0, 0, nWidth, nHeight, 0, hmheight, hmwidth, -hmheight, NULL);
	}

}
#else
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
void draw(HWND hWnd)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput = NULL;
	ULONG_PTR gdiplusToken = NULL;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		HDC hdc = GetDC(hWnd);
		Gdiplus::Graphics graphics(hdc);


		IStream *pstmp = NULL;
		CreateStreamOnHGlobal(NULL, TRUE, &pstmp);
		if (pstmp == NULL)
		{
			printf("Stream Create Failed");
			return;
		}

		LARGE_INTEGER LiTemp = { 0 };
		ULARGE_INTEGER uLiZero = { 0 };
		ULONG ulRealSize = 0;
		pstmp->Seek(LiTemp, STREAM_SEEK_SET, NULL);
		pstmp->SetSize(uLiZero);

		pstmp->Write(picbuff, piclen, &ulRealSize);

		Gdiplus::Image * image = Gdiplus::Image::FromStream(pstmp);


		graphics.DrawImage(image, 0, 0, image->GetWidth(), image->GetHeight());


		pstmp->Release();
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
}


#endif

void * create_wnd(void *)
{
	wnd_main();
	return 0;
}


int main(int argc, char* argv[])
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData))
	{
		return 0;
	}
#if 1
	string host = "ssl.ptlogin2.qq.com";
	string path = "/ptqrshow";
	string get_conent = "appid=501004106&e=2&l=M&s=3&d=72&v=4&t=0.9462605109547753&daid=164&pt_3rd_aid=0";
#else
	string host = "www.baidu.com";
	string path = "/";
	string get_conent = "";
#endif



	char buf[20*1024] = { 0 };
	SOCKET sock = CreatSocket(host);
	int size;
	size=GetData(sock, buf, host, path, get_conent);
	
	int head_len = GetHead(buf);
	piclen = size - head_len;
	memcpy(picbuff, buf + head_len, piclen);

	//------------------------------------------------------------------------
	ofstream file_head;
	file_head.open("test_head", ios::out|ios::binary);
	if (!file_head)
	{
		printf("file_head open fail");
		return 1;
	}
	file_head.write(buf, head_len);
	file_head.close();

	//-------------------------------------------------------------------------
	ofstream file_body;
	file_body.open("test_body", ios::out | ios::binary);
	if (!file_body)
	{
		printf("file_body open fail");
		return 1;
	}
	file_body.write(buf + head_len, size - head_len);
	file_body.close();

	
	pthread_t wnd_id;
	void *rst;
	pthread_create(&wnd_id, NULL, create_wnd, NULL);
	pthread_join(wnd_id, &rst);

	printf("pthread over\n");
	return 0;
}

