#include "stdafx.h"

#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "ProjectExporter.h"
#include "DialogProgress.h"

#include "../LibZip/zip.h"
#include "../Engine/StringConverter.h"
#include "../Engine/IO.h"
#include "../Player/Helpers.h"
#include "../Engine/Engine.h"

DialogProgress* ProjectExporter::progressDialog = nullptr;

ProjectExporter::ProjectExporter()
{
	
}

ProjectExporter::~ProjectExporter()
{
	delete progressDialog;
}

void ProjectExporter::ExportWinX64(string projectName, string projectPath)
{
	if (progressDialog == nullptr)
	{
		progressDialog = new DialogProgress();
		progressDialog->setTitle("Building project");
	}

	progressDialog->show();

	//Stage 1. Compiling scripts
	progressDialog->setStatusText("Compiling scripts...");
	progressDialog->setProgress(0.25f);

	GetEngine->GetMonoRuntime()->CompileLibrary(MonoRuntime::CompileConfiguration::Release);

	//Stage 2. Copy player files
	progressDialog->setStatusText("Copying player...");
	progressDialog->setProgress(0.45f);

	std::vector<std::pair<string, string>> copyFiles = {
		{ "mono-2.0-sgen.dll", "mono-2.0-sgen.dll" },
		{ "freetype.dll", "freetype.dll" },
		{ "cg.dll", "cg.dll" },
		{ "Player.exe", projectName + ".exe" },
		{ "SDL2.dll", "SDL2.dll" },
		{ "OpenAL32.dll", "OpenAL32.dll" },
		{ "wrap_oal.dll", "wrap_oal.dll" }
	};
	std::vector<std::pair<string, string>>::iterator f_it;

	for (f_it = copyFiles.begin(); f_it < copyFiles.end(); ++f_it)
	{
		IO::FileCopy(Helper::ExePath() + (*f_it).first, projectPath + (*f_it).second);
	}

	//Stage 3. Copy mono files
	progressDialog->setStatusText("Copying mono...");
	progressDialog->setProgress(0.55f);

	CreateDirectory(CString((projectPath + "Mono/").c_str()), NULL);
	CreateDirectory(CString((projectPath + "Mono/lib/").c_str()), NULL);
	CreateDirectory(CString((projectPath + "Mono/lib/mono/").c_str()), NULL);
		
	IO::DirCopy(MainWindow::GetProjectMonoDir() + "etc/", projectPath + "Mono/etc/", true);
	IO::DirCopy(MainWindow::GetProjectMonoDir() + "lib/mono/4.5/", projectPath + "Mono/lib/mono/4.5/", true);
	progressDialog->setProgress(0.65f);
	IO::DirCopy(MainWindow::GetProjectMonoDir() + "lib/mono/4.5-api/", projectPath + "Mono/lib/mono/4.5-api/", true);
	IO::DirCopy(MainWindow::GetProjectMonoDir() + "lib/mono/gac/", projectPath + "Mono/lib/mono/gac/", true);

	//Stage 4. Copy assets
	//CreateDirectory(CString((projectPath + "BuiltinResources/").c_str()), NULL);
	/*progressDialog->SetStatusText("Copying builtin resources");
	progressDialog->SetProgress(70);

	IO::DirCopy(theApp.ExePath() + "BuiltinResources/", projectPath + "BuiltinResources/", true);
	IO::DirCopy(theApp.ProjectLibraryDir, projectPath + "Library/", true);

	progressDialog->SetStatusText("Copying assets");
	progressDialog->SetProgress(80);

	IO::DirCopy(theApp.ProjectAssetsDir, projectPath + "Assets/", true);*/

	progressDialog->setStatusText("Packing builtin resources...");
	progressDialog->setProgress(0.70f);

	//IO::DirCopy(theApp.ExePath() + "BuiltinResources/", projectPath + "BuiltinResources/", true);
	//IO::DirCopy(theApp.ProjectLibraryDir, projectPath + "Library/", true);

	PackFiles(MainWindow::GetBuiltinResourcesDir(), projectPath + "BuiltinResources.resources", MainWindow::GetBuiltinResourcesDir());
	IO::DirCopy(MainWindow::GetProjectLibraryDir(), projectPath + "Library/", true);

	progressDialog->setStatusText("Packing assets...");
	progressDialog->setProgress(0.80f);

	//IO::DirCopy(theApp.ProjectAssetsDir, projectPath + "Assets/", true);
	PackFiles(MainWindow::GetProjectAssetsDir(), projectPath + "Assets.resources", MainWindow::GetProjectAssetsDir());

	progressDialog->setProgress(0.90f);
	IO::DirCopy(MainWindow::GetProjectAssetsDir() + "Cache/", projectPath + "Cache/", true);

	//Stage 5. Copy assemblies
	progressDialog->setStatusText("Copying C# assemblies...");
	progressDialog->setProgress(0.95f);

	IO::FileCopy(MainWindow::GetProjectDir() + "Project/bin/Release/MainAssembly.dll", projectPath + "MainAssembly.dll");
	IO::FileCopy(MainWindow::GetProjectDir() + "Project/FalcoEngine.dll", projectPath + "FalcoEngine.dll");

	//Stage 6. Copy settings
	progressDialog->setStatusText("Copying settings...");
	progressDialog->setProgress(1.0f);

	IO::DirCopy(MainWindow::GetProjectSettingsDir(), projectPath + "Settings/", true);

	progressDialog->hide();

	//MessageBoxA(0, "Building done!", "Done", MB_ICONINFORMATION | MB_OK);
	MainWindow::GetConsoleWindow()->log("Building done!", LogMessageType::LMT_SUCCESS);
}

void ProjectExporter::ExportWebGL(string projectName, string projectPath)
{
}

void ProjectExporter::PackFiles(std::string srcDir, std::string dstName, std::string root)
{
	if (IO::FileExists(dstName))
		IO::FileDelete(dstName);

	int* zErr = nullptr;
	zip_t* _zip = zip_open(CP_UNI(dstName).c_str(), ZIP_CREATE, zErr);

	std::vector<std::string> files;

	struct LambdaFunc
	{
		std::vector<std::string>* filesNames;
		std::string root;

		void operator() (std::string dir)
		{
		}

		void operator() (std::string dir, std::string filename)
		{
			std::string str = IO::RemovePart(dir + filename, root);

			if (str.find("Cache/") == string::npos)
				filesNames->push_back(str);
		}
	};

	LambdaFunc func;
	func.filesNames = &files;
	func.root = root;

	IO::listFiles(srcDir, true, nullptr, func);

	for (auto it = files.begin(); it != files.end(); ++it)
	{
		//Skip scripts
		if (IO::GetFileExtension(*it) == "cs")
			continue;

		zip_source* source = zip_source_file(_zip, CP_UNI(root + *it).c_str(), 0, 0);
		zip_file_add(_zip, CP_UNI(*it).c_str(), source, ZIP_FL_ENC_UTF_8);
	}

	zip_close(_zip);

	files.clear();
}

void ProjectExporter::UpdateProgressbar()
{
	if (progressDialog != nullptr)
		progressDialog->update();
}