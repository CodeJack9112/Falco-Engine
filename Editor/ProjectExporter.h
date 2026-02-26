#pragma once

#include <vector>
#include <string>
#include <thread>

using namespace std;

class DialogProgress;

class ProjectExporter
{
private:
	static void PackFiles(std::string srcDir, std::string dstName, std::string root);

	static DialogProgress* progressDialog;

public:
	ProjectExporter();
	~ProjectExporter();
	
	static void UpdateProgressbar();

	static void ExportWinX64(string projectName, string projectPath);
	static void ExportWebGL(string projectName, string projectPath);
};

