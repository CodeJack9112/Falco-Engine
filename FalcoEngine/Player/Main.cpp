#include "../Engine/Engine.h"
#include "../Engine/IO.h"
#include "../Engine/ProjectSettings.h"
#include "../Engine/SceneSerializer.h"
#include "../Engine/SoundManager.h"
#include "../Engine/PhysicsManager.h"
#include "../Engine/InputManager.h"
#include "../Engine/NavigationManager.h"
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreLogManager.h>

#include <atlstr.h>
#include "Helpers.h"
#include <OgreInput.h>

#include "../Ogre/source/build/Dependencies/include/SDL2/SDL.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_video.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_syswm.h"

#include "../boost/algorithm/string.hpp"
#include "../Engine/DeferredShading/DeferredShading.h"
#include "../Engine/GUI/ImguiManager.h"
#include "../Engine/StringConverter.h"

#include "../Engine/ResourceMap.h"
#include "../Engine/PlayerPrefs.h"

#include "Resource.h"
#include <commctrl.h>

#include "../LibZip/zip.h"
#include "../Engine/ZipHelper.h"

#include "imgui_impl_sdl.h"

using namespace Ogre;
using namespace std;

#pragma warning(disable : 4996)

bool running = true;
float fixedTimeStep = 0.01f;
float fixedTimeSimulated = 0.0f;
float currentTime = 0.0f;

ProjectSettings projectSettings;

HBITMAP hConfBitmap;
RECT hConfRect;
HWND hSplash;
std::vector<std::pair<int, int>> screenSizeList;

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

SDL_Surface * SDL_LoadBMPFromResource(HINSTANCE hInst, LPCTSTR lpResName)
{
	HBITMAP hBitmap;
	BITMAP bm;
	Uint8 *bits = NULL;
	Uint8 *temp = NULL;
	SDL_Surface *surf = NULL;

	//Load Bitmap From the Resource into HBITMAP
	hBitmap = (HBITMAP)LoadImage(hInst, lpResName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	//Now Get a BITMAP structure for the HBITMAP
	GetObject(hBitmap, sizeof(bm), &bm);

	//create a new surface
	surf = SDL_CreateRGBSurface(SDL_SWSURFACE, bm.bmWidth, bm.bmHeight, bm.bmBitsPixel,
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

	bits = new Uint8[bm.bmWidthBytes*bm.bmHeight];
	temp = new Uint8[bm.bmWidthBytes*bm.bmHeight];
	memcpy(temp, bm.bmBits, bm.bmWidthBytes*bm.bmHeight);

	//first flip image over
	//this is probably not the fastest/best way to do this
	Uint8 *ptemp;
	Uint8 *pbits = bits;
	for (int j = bm.bmHeight - 1; j >= 0; j--)
	{
		ptemp = temp + j * bm.bmWidthBytes;
		for (int x = 0; x < bm.bmWidthBytes; x++)
		{
			*pbits = *ptemp;
			pbits++;
			ptemp++;
		}
	}

	//Now reverse BGR data to be RGB
	for (int i = 0; i < bm.bmWidthBytes*bm.bmHeight; i += 3)
	{
		Uint8 temp;
		temp = bits[i];
		bits[i] = bits[i + 2];
		bits[i + 2] = temp;
	}

	//Now just copy bits onto surface
	if (SDL_MUSTLOCK(surf)) SDL_LockSurface(surf);
	memcpy(surf->pixels, bits, bm.bmWidthBytes*bm.bmHeight);
	if (SDL_MUSTLOCK(surf)) SDL_UnlockSurface(surf);

	delete[] bits;
	delete[] temp;

	//Finally, convert surface to display format so it displays correctly
//	SDL_Surface *surf2 = SDL_DisplayFormat(surf);
	//SDL_FreeSurface(surf);

	return surf;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_INITDIALOG)
	{
		BITMAP bm;
		hConfBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_CONFIG), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		hSplash = GetDlgItem(hWnd, IDC_SPLASH);
		GetClientRect(hSplash, &hConfRect);

		//Screen resolutions
		HWND resComboBox = GetDlgItem(hWnd, IDC_COMBO_SCREEN_SIZE);
		HWND resFullscreen = GetDlgItem(hWnd, IDC_FULLSCREEN);

		SetWindowLong(resComboBox, GWL_STYLE, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | ES_READONLY);

		DEVMODE dm = { 0 };
		dm.dmSize = sizeof(dm);
		
		for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++)
		{
			//cout << "Mode #" << iModeNum << " = " <<  << "x" << dm.dmPelsHeight << endl;
			//std::string _mode = to_string(dm.dmPelsWidth) + "x" + to_string(dm.dmPelsHeight);
			screenSizeList.push_back(make_pair(dm.dmPelsWidth, dm.dmPelsHeight));
		}

		auto end = std::unique(screenSizeList.begin(), screenSizeList.end());
		screenSizeList.erase(end, screenSizeList.end());
		sort(screenSizeList.begin(), screenSizeList.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) -> bool { return a.first < b.first; });

		for (std::vector<std::pair<int, int>>::iterator it = screenSizeList.begin(); it != screenSizeList.end(); ++it)
		{
			std::string _mode = to_string(it->first) + "x" + to_string(it->second);
			
			SendMessage(resComboBox, CB_ADDSTRING, 0, (LPARAM)StringConvert::s2ws(_mode, GetACP()).c_str());
		}

		std::string _res = "";

		if (projectSettings.screenWidth != 0 && projectSettings.screenHeight != 0)
		{
			_res = to_string(projectSettings.screenWidth) + "x" + to_string(projectSettings.screenHeight);
		}
		else
		{
			RECT desktop;
			const HWND hDesktop = GetDesktopWindow();
			GetWindowRect(hDesktop, &desktop);
			int horizontal = desktop.right;
			int vertical = desktop.bottom;
			_res = to_string(horizontal) + "x" + to_string(vertical);
		}
		
		auto _selRes = std::find_if(screenSizeList.begin(), screenSizeList.end(), [_res](const pair<int, int> & a) -> bool { return (to_string(a.first) + "x" + to_string(a.second)).c_str() == _res; });
		int index = std::distance(screenSizeList.begin(), _selRes);

		SetWindowText(resComboBox, CString(_res.c_str()));
		SendMessage(resComboBox, CB_SETCURSEL, index, 0);

		int _checked = projectSettings.fullscreen ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(hWnd, IDC_FULLSCREEN, _checked);

		COMBOBOXINFO cmbxInfo;
		cmbxInfo.cbSize = sizeof(COMBOBOXINFO);
		if (GetComboBoxInfo(resComboBox, &cmbxInfo))
		{
			RECT rcListBox;
			::GetWindowRect(cmbxInfo.hwndList, &rcListBox);
			::SetWindowPos(cmbxInfo.hwndList, 0, 0, 0, rcListBox.right, 100, SWP_NOMOVE | SWP_NOZORDER);
		}
	}

	if (msg == WM_PAINT)
	{
		PAINTSTRUCT     ps;
		HDC             hdc;
		BITMAP          bitmap;
		HDC             hdcMem;
		HGDIOBJ         oldBitmap;

		hdc = BeginPaint(hSplash, &ps);

		hdcMem = CreateCompatibleDC(hdc);
		oldBitmap = SelectObject(hdcMem, hConfBitmap);

		GetObject(hConfBitmap, sizeof(bitmap), &bitmap);
		BitBlt(hdc, 0, 0, hConfRect.right, hConfRect.bottom, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);

		EndPaint(hSplash, &ps);
	}

	switch (msg)
	{
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			HWND resComboBox = GetDlgItem(hWnd, IDC_COMBO_SCREEN_SIZE);
			CHAR rStr[64];
			GetWindowTextA(resComboBox, rStr, 256);

			auto _res = std::find_if(screenSizeList.begin(), screenSizeList.end(), [rStr](const pair<int, int> & a) -> bool { return (to_string(a.first) + "x" + to_string(a.second)).c_str() == std::string(rStr); });
			projectSettings.screenWidth = _res->first;
			projectSettings.screenHeight = _res->second;

			bool checked = IsDlgButtonChecked(hWnd, IDC_FULLSCREEN);
			projectSettings.fullscreen = checked;

			EndDialog(hWnd, 0);
			DestroyWindow(hWnd);
		}

		if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDCLOSE)
		{
			EndDialog(hWnd, 0);
			DestroyWindow(hWnd);
			running = false;
		}

		//return 0;

		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		DestroyWindow(hWnd);
		running = false;
		break;
	case WM_DESTROY:
		DeleteObject(hConfBitmap);
		PostQuitMessage(WM_QUIT);
		break;
	/*default:
		return DefWindowProc(hWnd, msg, wParam, lParam);*/
	}

	return 0;
}

int main(int argc, char* argv[])
{
	InitCommonControls();

	bool nologo = false;
	bool useCmd = false;
	bool showConfigDialog = true;

	string _argv0 = StringConvert::cp_convert(argv[0], 65001, GetACP());
	string _argv1 = "";
	string _argv2 = "";
	string _argv3 = "";
	string _argv4 = "";
	string _argv5 = "";

	string _argv6 = ""; // Screen width
	string _argv7 = ""; // Screen height
	string _argv8 = ""; // Fullscreen

	if (argc > 1)
		_argv1 = StringConvert::cp_convert(argv[1], 65001, GetACP());
	if (argc > 2)
		_argv2 = StringConvert::cp_convert(argv[2], 65001, GetACP());
	if (argc > 3)
		_argv3 = StringConvert::cp_convert(argv[3], 65001, GetACP());
	if (argc > 4)
		_argv4 = StringConvert::cp_convert(argv[4], 65001, GetACP());
	if (argc > 5)
		_argv5 = StringConvert::cp_convert(argv[5], 65001, GetACP());
	if (argc > 2)
	{
		nologo = true;
		useCmd = true;
	}
	if (argc > 6)
	{
		showConfigDialog = false;

		_argv6 = argv[6];
		_argv7 = argv[7];
		_argv8 = argv[8];
	}

	bool useUnpackedResources = _argv5 == "useUnpackedResources";

	//Exe name
	string exeName = "Falco Engine game";
	
	if (argc > 1)
		exeName = IO::GetFileName(_argv1);
	else
		exeName = IO::GetFileName(_argv0);

	//Exe directory
	string projectDir = Helper::WorkingDir();

	if (argc > 2)
		projectDir = IO::ReplaceBackSlashes(_argv2);
	
	if (!useCmd)
	{
		if (IO::FileExists(Helper::WorkingDir() + "exepath.txt"))
		{
			std::string _txt = IO::ReadText(Helper::WorkingDir() + "exepath.txt");
			_txt = StringConvert::cp_convert(_txt, 65001, GetACP());

			::CString d = ::CString(_txt.c_str());
			d.Replace(_T("\n"), _T(""));
			projectDir = IO::ReplaceBackSlashes(Helper::CSTR2STR(d));
		}
	}

	std::string builtinResourcesPath = Helper::WorkingDir() + "BuiltinResources/";

	if (IO::FileExists(Helper::WorkingDir() + "builtinresources.txt"))
	{
		std::string _txt = IO::ReadText(Helper::WorkingDir() + "builtinresources.txt");
		_txt = StringConvert::cp_convert(_txt, 65001, GetACP());

		::CString d = ::CString(_txt.c_str());
		d.Replace(_T("\n"), _T(""));
		builtinResourcesPath = IO::ReplaceBackSlashes(Helper::CSTR2STR(d));
	}

	if (IO::FileExists(Helper::WorkingDir() + "useunpackedresources.txt"))
	{
		std::string _txt = IO::ReadText(Helper::WorkingDir() + "useunpackedresources.txt");
		::CString d = ::CString(_txt.c_str());
		d.Replace(_T("\n"), _T(""));

		if (d == "yes")
			useUnpackedResources = true;
	}

	//Mono path
	::CString mono_p = "";
	if (IO::FileExists(Helper::WorkingDir() + "Mono.conf"))
	{
		std::string _mono_p = IO::ReadText(Helper::WorkingDir() + "Mono.conf");
		_mono_p = StringConvert::cp_convert(_mono_p, 65001, GetACP());

		mono_p = ::CString(_mono_p.c_str());
		mono_p.Replace(_T("\n"), _T(""));
	}

	if (mono_p.IsEmpty())
		mono_p = "Mono/";

	string monoPath = Helper::WorkingDir() + Helper::CSTR2STR(mono_p);

	//Assembly path
	string asmPath = Helper::WorkingDir();

	if (!useCmd)
	{
		if (IO::FileExists(Helper::WorkingDir() + "assembly.txt"))
		{
			std::string _txt = IO::ReadText(Helper::WorkingDir() + "assembly.txt");
			_txt = StringConvert::cp_convert(_txt, 65001, GetACP());

			::CString d = ::CString(_txt.c_str());
			d.Replace(_T("\n"), _T(""));
			asmPath = Helper::CSTR2STR(d) + "/";
		}
	}

	if (argc > 3)
		asmPath = _argv3;

	//Set project assets path
	GetEngine->SetSystemCP(GetACP());
	GetEngine->SetRootPath(projectDir);
	GetEngine->SetAssetsPath(projectDir + "Assets/");
	GetEngine->SetLibraryPath(projectDir + "Library/");
	GetEngine->SetMonoPath(monoPath);
	GetEngine->SetBuiltinResourcesPath(builtinResourcesPath);
	GetEngine->SetAssemblyPath(asmPath + "MainAssembly.dll");
	GetEngine->SetCachePath(projectDir + "Assets/Cache/");
	GetEngine->SetUseUnpackedResources(useUnpackedResources);

	if (!useUnpackedResources)
		GetEngine->SetCachePath(projectDir + "Cache/");

	if (!useCmd)
	{
		if (IO::FileExists(Helper::WorkingDir() + "cachepath.txt"))
		{
			std::string _txt = IO::ReadText(Helper::WorkingDir() + "cachepath.txt");
			_txt = StringConvert::cp_convert(_txt, 65001, GetACP());

			::CString d = ::CString(_txt.c_str());
			d.Replace(_T("\n"), _T(""));
			_txt = Helper::CSTR2STR(d) + "/";

			GetEngine->SetCachePath(_txt);
		}
	}

	if (IO::FileExists(projectDir + "Settings/Main.settings"))
	{
		projectSettings.Load(projectDir + "Settings/Main.settings");
	}

	PlayerPrefs::Singleton.load(projectDir + "Library/PlayerPrefs.bin");

	/* Config dialog */
	if (showConfigDialog)
	{
		MSG msg;

		HWND hwndDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CONFIG), NULL, (DLGPROC)WndProc);
		ShowWindow(hwndDlg, SW_SHOW);

		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_FALCO));
		SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		UpdateWindow(hwndDlg);

		RECT rc;

		GetWindowRect(hwndDlg, &rc);

		int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
		int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;

		SetWindowPos(hwndDlg, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		while (GetMessage(&msg, NULL, 0, 0) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (running == false)
			return 0;

		projectSettings.Save(projectDir + "Settings/Main.settings");
	}
	else
	{
		projectSettings.screenWidth = atoi(_argv6.c_str());
		projectSettings.screenHeight = atoi(_argv7.c_str());
		projectSettings.fullscreen = _argv8 == "true";
	}

	//--------------SDL-------------//

	SDL_DisplayMode displayMode;
	SDL_Init(SDL_INIT_VIDEO);

	int request = SDL_GetDesktopDisplayMode(0, &displayMode);
	SDL_Window * win = SDL_CreateWindow(StringConvert::cp_convert(exeName, GetACP(), 65001).c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, projectSettings.screenWidth, projectSettings.screenHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	if (projectSettings.fullscreen)
		SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	if (!nologo)
	{
		SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, 0);
		SDL_RenderSetLogicalSize(renderer, projectSettings.screenWidth, projectSettings.screenHeight);

		SDL_Surface* image = SDL_LoadBMPFromResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_SPLASH));
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		Sleep(4000);

		SDL_DestroyRenderer(renderer);
		SDL_DestroyTexture(texture);
	}
	else
	{
		SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, 0);
		SDL_RenderSetLogicalSize(renderer, projectSettings.screenWidth, projectSettings.screenHeight);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);

		SDL_DestroyRenderer(renderer);
	}

	if (projectSettings.fullscreen)
		SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);

	//------------------------------//

	Ogre::RenderWindow * wnd;

	GetEngine->SetOnAfterRootCreatedCallback([=, &wnd] 
	{
		Ogre::NameValuePairList params;
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(win, &info);
		params["externalWindowHandle"] = Ogre::StringConverter::toString(size_t(info.info.win.window));

		wnd = Root::getSingleton().createRenderWindow(exeName, projectSettings.screenWidth, projectSettings.screenHeight, projectSettings.fullscreen, &params);
		wnd->setVisible(true);

		GetEngine->SetRenderWindow(wnd);
	});

	GetEngine->Init(Engine::Environment::Player, false);

	//* READ ZIP ARCHIVE *//
	string resourcePackage = GetEngine->GetAssetsPath().substr(0, GetEngine->GetAssetsPath().length() - 1) + ".resources";
	string builtinPackage = GetEngine->GetBuiltinResourcesPath().substr(0, GetEngine->GetBuiltinResourcesPath().length() - 1) + ".resources";

	const char* archive;
	struct zip* za1 = nullptr;
	struct zip* za2 = nullptr;
	char buf[100];
	int err;

	if (!useUnpackedResources)
	{
		if ((za1 = zip_open(CP_UNI(resourcePackage).c_str(), 0, &err)) == NULL)
		{
			zip_error_to_str(buf, sizeof(buf), err, errno);
			MessageBoxA(0, string("Filed to open package: " + string(buf)).c_str(), "Error", MB_OK);

			return 0;
		}

		if ((za2 = zip_open(CP_UNI(builtinPackage).c_str(), 0, &err)) == NULL)
		{
			zip_error_to_str(buf, sizeof(buf), err, errno);
			MessageBoxA(0, string("Filed to open package: " + string(buf)).c_str(), "Error", MB_OK);

			return 0;
		}

		GetEngine->SetOpenedZipAssets(za1);
		GetEngine->SetOpenedZipBuiltin(za2);
	}
	//* END READ ZIP ARCHIVE *//

	GetEngine->LoadResources(useUnpackedResources);
	GetEngine->GetMonoRuntime()->OpenLibrary();

	SceneManager * mSceneMgr = GetEngine->GetSceneManager();

	string startupScene = "";

	if (argc > 4)
	{
		startupScene = _argv4;
	}
	else
	{
		if (IO::FileExists(projectDir + "Settings/Main.settings"))
		{
			projectSettings.Load(projectDir + "Settings/Main.settings");
			
			string scenePath = ResourceMap::getResourceNameFromGuid(projectSettings.StartupSceneGuid);
			if (scenePath.empty())
				scenePath = projectSettings.StartupScene;

			if (GetEngine->GetUseUnpackedResources())
			{
				if (IO::FileExists(GetEngine->GetAssetsPath() + scenePath))
				{
					if (!IO::isDir(GetEngine->GetAssetsPath() + scenePath))
					{
						startupScene = GetEngine->GetAssetsPath() + scenePath;
					}
				}
			}
			else
			{
				startupScene = GetEngine->GetAssetsPath() + scenePath;
			}
		}
	}

	//Load startup scene
	if (!startupScene.empty())
	{
		SceneSerializer sceneSerializer;
		sceneSerializer.Deserialize(mSceneMgr, startupScene);
	}

	SDL_Event evt;

	ImGui_ImplSDL2_InitForOpenGL(win, nullptr);

	Root::getSingleton().renderOneFrame();

	/*thread t1([=]() {
		while (running && !GetEngine->shouldExit)
		{
			GetEngine->GetSoundManager()->Update();
			Sleep(10);
		}
	});
	t1.detach();*/

	while (running && !GetEngine->shouldExit)
	{
		// Handle events
		while (SDL_PollEvent(&evt))
		{
			// Handle window close event
			if (evt.type == SDL_QUIT)
				running = false;

			if (evt.type == SDL_WINDOWEVENT)
			{
				if (evt.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					//Resize the screen
					GetEngine->GetRenderWindow()->windowMovedOrResized();
				}
			}

			//Handle input events
			if (evt.type == SDL_MOUSEMOTION)
			{
				GetEngine->GetInputManager()->MouseMoveEvent(evt.motion.x, evt.motion.y);
				InputManager::SetMouseDirection(evt.motion.xrel, evt.motion.yrel);
			}

			if (evt.type == SDL_MOUSEBUTTONDOWN)
			{
				if (evt.button.button == SDL_BUTTON_LEFT)
				{
					GetEngine->GetInputManager()->MouseDownEvent(InputManager::MouseButton::MBE_LEFT, evt.button.x, evt.button.y);
					InputManager::SetMouseButton(0, true);
					InputManager::SetMouseButtonDown(0, true);

					int button = 0;
					void * args[1] = { &button };
					GetEngine->GetMonoRuntime()->Execute("MouseDown", args, "int");
				}

				if (evt.button.button == SDL_BUTTON_RIGHT)
				{
					GetEngine->GetInputManager()->MouseDownEvent(InputManager::MouseButton::MBE_RIGHT, evt.button.x, evt.button.y);
					InputManager::SetMouseButton(1, true);
					InputManager::SetMouseButtonDown(1, true);

					int button = 1;
					void * args[1] = { &button };
					GetEngine->GetMonoRuntime()->Execute("MouseDown", args, "int");
				}

				if (evt.button.button == SDL_BUTTON_MIDDLE)
				{
					GetEngine->GetInputManager()->MouseDownEvent(InputManager::MouseButton::MBE_MIDDLE, evt.button.x, evt.button.y);
					InputManager::SetMouseButton(2, true);
					InputManager::SetMouseButtonDown(2, true);

					int button = 2;
					void * args[1] = { &button };
					GetEngine->GetMonoRuntime()->Execute("MouseDown", args, "int");
				}
			}

			if (evt.type == SDL_MOUSEBUTTONUP)
			{
				if (evt.button.button == SDL_BUTTON_LEFT)
				{
					GetEngine->GetInputManager()->MouseUpEvent(InputManager::MouseButton::MBE_LEFT, evt.button.x, evt.button.y);
					InputManager::SetMouseButton(0, false);
					InputManager::SetMouseButtonUp(0, true);

					int button = 0;
					void * args[1] = { &button };
					GetEngine->GetMonoRuntime()->Execute("MouseUp", args, "int");
				}

				if (evt.button.button == SDL_BUTTON_RIGHT)
				{
					GetEngine->GetInputManager()->MouseUpEvent(InputManager::MouseButton::MBE_RIGHT, evt.button.x, evt.button.y);
					InputManager::SetMouseButton(1, false);
					InputManager::SetMouseButtonUp(1, true);

					int button = 1;
					void * args[1] = { &button };
					GetEngine->GetMonoRuntime()->Execute("MouseUp", args, "int");
				}

				if (evt.button.button == SDL_BUTTON_MIDDLE)
				{
					GetEngine->GetInputManager()->MouseUpEvent(InputManager::MouseButton::MBE_MIDDLE, evt.button.x, evt.button.y);
					InputManager::SetMouseButton(2, false);
					InputManager::SetMouseButtonUp(2, true);

					int button = 2;
					void * args[1] = { &button };
					GetEngine->GetMonoRuntime()->Execute("MouseUp", args, "int");
				}
			}

			if (evt.type == SDL_MOUSEWHEEL)
			{
				GetEngine->GetInputManager()->MouseWheelEvent(evt.wheel.x, evt.wheel.y);
				InputManager::SetMouseWheel(evt.wheel.x, evt.wheel.y);
			}

			if (evt.type == SDL_KEYDOWN)
			{
				InputManager::SetKey(evt.key.keysym.scancode, true);

				if (evt.key.repeat == 0)
				{
					InputManager::SetKeyDown(evt.key.keysym.scancode, true);

					int key = (int)evt.key.keysym.scancode;

					void * args[1] = { &key };
					
					GetEngine->GetMonoRuntime()->Execute("KeyDown", args, "ScanCode");
				}
			}

			if (evt.type == SDL_KEYUP)
			{
				InputManager::SetKey(evt.key.keysym.scancode, false);

				if (evt.key.repeat == 0)
				{
					InputManager::SetKeyUp(evt.key.keysym.scancode, true);

					int key = (int)evt.key.keysym.scancode;

					void * args[1] = { &key };

					GetEngine->GetMonoRuntime()->Execute("KeyUp", args, "ScanCode");
				}
			}
		}

		int _x = 0;
		int _y = 0;

		SDL_GetGlobalMouseState(&_x, &_y);
		InputManager::SetMousePosition(_x, _y);

		SDL_GetMouseState(&_x, &_y);
		InputManager::SetMouseRelativePosition(_x, _y);

		ImGui_ImplSDL2_NewFrame(win);

		float dt = GetEngine->GetDeltaTime();

		currentTime += dt * GetEngine->getTimeScale();

		while (fixedTimeSimulated < currentTime)
		{
			GetEngine->GetMonoRuntime()->Execute("FixedUpdate");
			fixedTimeSimulated += fixedTimeStep;
		}

		void* args[1] = { &dt };

		GetEngine->GetMonoRuntime()->Execute("Update", args, "single");
		GetEngine->GetMonoRuntime()->Execute("Update");
		GetEngine->GetSoundManager()->Update();
		GetEngine->GetNavigationManager()->Update();
		GetEngine->GetPhysicsManager()->Update();

		GetEngine->GetInputManager()->SetMouseDirection(0, 0);
		GetEngine->GetInputManager()->SetMouseWheel(0, 0);
		InputManager::ResetKeys();

		// Draw one frame
		Root::getSingleton().renderOneFrame();
		SDL_GL_SwapWindow(win);

		GetEngine->GetMonoRuntime()->CheckSceneToLoad();
	}

	PlayerPrefs::Singleton.save(projectDir + "Library/PlayerPrefs.bin");

	//SDL_GL_DeleteContext(glcontext);
	SDL_Quit();

	//Cleanup resources
	Root* mOgreRoot = Root::getSingletonPtr();
	if (mOgreRoot->hasSceneManager(mSceneMgr->getName()))
		mOgreRoot->destroySceneManager(mSceneMgr);

	if (useUnpackedResources)
	{
		zip_close(za1);
		zip_close(za2);
	}

	return 0;
}