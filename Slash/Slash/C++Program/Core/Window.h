#pragma once

/*

ウィンドウの生成・管理を行うクラス

ウィンドウハンドル等の提供も行う

*/

#include<Windows.h>

class Window
{
public:

	void Init();

	HINSTANCE GetHInstance() const { return hInstance; }
	HWND GetHWND() const { return hwnd; }

private:

	HINSTANCE hInstance;
	HWND hwnd;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);

};