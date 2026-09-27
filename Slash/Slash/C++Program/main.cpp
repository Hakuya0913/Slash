/*

Applicationクラスの初期化・起動

*/

#include<Windows.h>

#include"App.h"

int WINAPI WinMain(
	_In_ HINSTANCE,
	_In_opt_ HINSTANCE,
	_In_ LPSTR,
	_In_ int nShowCmd
)
{

	App app;

	app.Init();

	app.Update();

	return 0;

}