#include "stdafx.h"
#include "SolutionWorker.h"
#include "../Engine/Engine.h"

void SolutionWorker::CreateSolution(std::string dir, std::string projectName, std::vector<std::string> files)
{
	//if (!IO::FileExists(dir + "FalcoEngine.dll"))
	IO::FileCopy(Helper::ExePath() + "FalcoEngine.dll", dir + "FalcoEngine.dll");

	unsigned char _guid[16];
	boost::uuids::random_generator gen;
	boost::uuids::uuid u = gen();
	memcpy(&u, _guid, 16);

	//if (theApp.project_guid.empty())
	//{
	string guid = boost::uuids::to_string(u);
	//theApp.project_guid = guid;
	string project_guid = guid;
	//}

	string _namespace = boost::replace_all_copy(projectName, " ", "");

	string solution = boost::replace_all_copy(solutionText, "%ProjectName%", projectName);
	solution = boost::replace_all_copy(solution, "%GUID%", project_guid);
	string project = boost::replace_all_copy(projectText, "%ProjectName%", projectName);
	project = boost::replace_all_copy(project, "%GUID%", project_guid);
	string script = boost::replace_all_copy(mainScript, "%ProjectName%", _namespace);

	string sources = "";

	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		sources += "<Compile Include=\"" + *it + "\" />\n";
	}

	project = boost::replace_all_copy(project, "%SourceFiles%", sources);
	project = CP_UNI(project.c_str());

	solution = CP_UNI(solution);
	script = CP_UNI(script);

	if (!IO::FileExists(dir + "Main.cs"))
		IO::WriteText(dir + "Main.cs", script);

	if (!IO::FileExists(dir + projectName + ".sln"))
		IO::WriteText(dir + projectName + ".sln", solution);

	if (files.size() > 0)
	{
		if (IO::FileExists(dir + projectName + ".csproj"))
		{
			size_t size = IO::ReadText(dir + projectName + ".csproj").size() - 1;
			if (size != project.size())
			{
				IO::WriteText(dir + projectName + ".csproj", project);
				if (GetEngine->GetMonoRuntime() != nullptr)
					GetEngine->GetMonoRuntime()->CompileLibrary();
			}
		}
		else
		{
			IO::WriteText(dir + projectName + ".csproj", project);
			if (GetEngine->GetMonoRuntime() != nullptr)
				GetEngine->GetMonoRuntime()->CompileLibrary();
		}
	}
	else
	{
		if (!IO::FileExists(dir + projectName + ".csproj"))
		{
			IO::WriteText(dir + projectName + ".csproj", project);
			if (GetEngine->GetMonoRuntime() != nullptr)
				GetEngine->GetMonoRuntime()->CompileLibrary();
		}
	}
}
