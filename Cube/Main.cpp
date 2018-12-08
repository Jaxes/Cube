#include "CubeApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	CubeApp theApp(hInstance);
	mciSendString(TEXT("open Music/Hana.mp3 alias BGM"), NULL, 0, NULL);
	mciSendString(TEXT("play BGM repeat"), NULL, 0, NULL);
	if (!theApp.Init())
		return 0;
	srand((unsigned)time(NULL));
	return theApp.Run();
	mciSendString(TEXT("close BGM"), NULL, 0, NULL);
}