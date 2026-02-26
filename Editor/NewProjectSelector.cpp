#include "stdafx.h"
#include "NewProjectSelector.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include "../Player/Helpers.h"
#include "../Engine/IO.h"
#include "../Engine/GUI/ImGUIWidgets.h"

#include "../boost/serialization/serialization.hpp"
#include "../boost/serialization/binary_object.hpp"
#include "../boost/archive/binary_oarchive.hpp"
#include "../boost/archive/binary_iarchive.hpp"

#include "../Engine/Engine.h"
#include "SolutionWorker.h"

#include "MainWindow.h"
#include "EditorClasses.h"

bool ProjectSelector::Run()
{
	SDL_DisplayMode dm;

	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	}

	int screen_w, screen_h;
	screen_w = dm.w;
	screen_h = dm.h;

	// Setup window
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
	window = SDL_CreateWindow("Falco Engine - Select Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 465, window_flags);
	gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	int window_w = 0;
	int window_h = 0;

	SDL_GetWindowSize(window, &window_w, &window_h);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	ImGui::StyleColorsDark();
	
	ImGui::GetStyle().WindowBorderSize = 0;
	ImGui::GetStyle().WindowRounding = 0;
	ImGui::GetStyle().WindowPadding = ImVec2(0, 0);
	ImGui::GetStyle().FramePadding = ImVec2(0, 0);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.82f, 0.82f, 0.82f, 0.54f);
	colors[ImGuiCol_Header] = ImVec4(0.62f, 0.62f, 0.62f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.67f, 0.67f, 0.67f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.62f, 0.62f, 0.62f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL2_Init();

	//io.Fonts->AddFontDefault();
	ImFont * stdFont = io.Fonts->AddFontFromFileTTF(string(Helper::ExePath() + "Editor/Fonts/Roboto-Medium.ttf").c_str(), 15.0f, 0, io.Fonts->GetGlyphRangesCyrillic());
	ImFont * stdBigFont = io.Fonts->AddFontFromFileTTF(string(Helper::ExePath() + "Editor/Fonts/Roboto-Medium.ttf").c_str(), 25.0f, 0, io.Fonts->GetGlyphRangesCyrillic());

	// Our state
	ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 btn1_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	ImVec4 btn2_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

	// Main loop
	bool done = false;
	bool closedByUser = false;

	LoadProjects();

	std::vector<pair<std::string, std::string>>(strList);
	int i = 0;

	for (auto it = projectsInfo.projects.begin(); it != projectsInfo.projects.end(); ++it, ++i)
	{
		strList.push_back(make_pair(CP_UNI(it->projectName), CP_UNI(it->projectPath)));
	}

	int selItem = 0;
	
	enum ActivePanel { AP_PROJECTS, AP_NEW_PROJECT };
	ActivePanel activePanel = AP_PROJECTS;

	string newProjectName = "";
	string newProjectPath = "";

	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
			{
				done = true;
				closedByUser = true;
			}
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		//ImGui::ShowStyleEditor();

		/* GUI */
		bool open = true;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(window_w, window_h));
		ImGui::SetNextWindowBgAlpha(0.0);
		ImGui::Begin("", &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

		if (activePanel == AP_PROJECTS)
		{
			ImGui::PushFont(stdBigFont);
			ImGui::SetCursorPos(ImVec2(10.0f, 23.0f));
			ImGui::TextColored(ImVec4(0.1f, 0.1f, 0.1f, 1.0f), "PROJECTS");
			ImGui::PopFont();

			ImGui::PushFont(stdFont);

			ImGui::SetCursorPos(ImVec2(window_w - 120, 28.0f));
			ImGui::TextColored(btn1_color, "NEW");
			if (ImGui::IsMouseHoveringRect(ImVec2(window_w - 120, 28.0f), ImVec2(window_w - 90, 43.0f)))
			{
				btn1_color = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					activePanel = AP_NEW_PROJECT;
					continue;
				}
			}
			else
				btn1_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

			ImGui::SetCursorPos(ImVec2(window_w - 60, 28.0f));
			ImGui::TextColored(btn2_color, "OPEN");
			if (ImGui::IsMouseHoveringRect(ImVec2(window_w - 60, 28.0f), ImVec2(window_w - 25, 43.0f)))
			{
				btn2_color = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					if (OpenNewProject() == 0)
					{
						done = true;
						closedByUser = false;
					}
				}
			}
			else
				btn2_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

			ImGui::SetCursorPos(ImVec2(0.0f, 70.0f));
			ImGui::PushItemWidth(window_w);
			if (ImGui::ListBox2("##listbox", &selItem, strList, 19))
			{
				if (OpenProject(projectsInfo.projects.at(selItem)) == 0)
				{
					done = true;
					closedByUser = false;
				}
			}
			ImGui::PopItemWidth();

			ImGui::SetCursorPos(ImVec2(window_w - 80, window_h - 26));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(25, 2));
			if (ImGui::Button("Exit"))
			{
				done = true;
				closedByUser = true;
			}
			ImGui::PopStyleVar();

			ImGui::PopFont();
		}

		if (activePanel == AP_NEW_PROJECT)
		{
			ImGui::PushFont(stdBigFont);
			ImGui::SetCursorPos(ImVec2(window_w / 2 - 115, 70.0f));
			ImGui::TextColored(ImVec4(0.1f, 0.1f, 0.1f, 1.0f), "CREATE NEW PROJECT");
			ImGui::PopFont();

			ImGui::PushFont(stdFont);
			
			ImGui::SetCursorPos(ImVec2(window_w / 2 - 100, 120.0f));
			ImGui::Text("Project name");
			ImGui::SetCursorPos(ImVec2(window_w / 2 - 100, 140.0f));
			ImGui::SetNextItemWidth(200);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
			ImGui::InputText("##project_name", &newProjectName);
			ImGui::PopStyleVar();

			ImGui::SetCursorPos(ImVec2(window_w / 2 - 100, 170.0f));
			ImGui::Text("Project location");
			ImGui::SetCursorPos(ImVec2(window_w / 2 - 100, 190.0f));
			ImGui::SetNextItemWidth(170);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
			ImGui::InputText("##project_path", &newProjectPath);
			ImGui::PopStyleVar();
			ImGui::SetCursorPos(ImVec2(window_w / 2 + 70, 190.0f));
			if (ImGui::Button("...", ImVec2(30, 21)))
			{
				CFolderPickerDialog folderPickerDialog(_T(""), OFN_FILEMUSTEXIST | OFN_ENABLESIZING, nullptr, sizeof(OPENFILENAME));

				CString folderPath;

				if (folderPickerDialog.DoModal() == IDOK)
				{
					folderPath = folderPickerDialog.GetFolderPath();
					folderPath.Replace(_T("\\"), _T("/"));

					if (folderPath.GetAt(folderPath.GetLength() - 1) != CString("/"))
						folderPath = folderPath + _T("/");

					newProjectPath = CP_UNI(CSTR2STRW(folderPath));
				}
			}

			ImGui::SetCursorPos(ImVec2(window_w / 2 - 100, 225.0f));
			if (ImGui::Button("Back", ImVec2(80, 20)))
			{
				activePanel = AP_PROJECTS;
				continue;
			}

			ImGui::SetCursorPos(ImVec2(window_w / 2 + 20, 225.0f));
			if (ImGui::Button("Create", ImVec2(80, 20)))
			{
				if (CreateNewProject(CP_SYS(newProjectName), CP_SYS(newProjectPath)) == 0)
				{
					done = true;
					closedByUser = false;
				}
			}

			ImGui::PopFont();
		}

		ImGui::End();
		/* GUI */

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	strList.clear();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);

	return closedByUser;
}

int ProjectSelector::OpenProject(SProject& project)
{
	if (IO::DirExists(project.projectPath))
	{
		if (IO::DirExists(project.projectPath + "Assets/"))
		{
			string tmp = (project.projectPath + "Temp/");
			CString temp_dir(tmp.c_str());

			string set = (project.projectPath + "Settings/");
			CString set_dir(set.c_str());

			// Create temp directory
			CreateDirectory(temp_dir, NULL);

			// Create settings directory
			CreateDirectory(set_dir, NULL);

			// Create solution directory
			CreateDirectory(CString((project.projectPath + "Project/").c_str()), NULL);

			// Create library directory
			CreateDirectory(CString((project.projectPath + "Library/").c_str()), NULL);

			SolutionWorker worker;
			worker.CreateSolution(project.projectPath + "Project/", project.projectName);

			MainWindow::SetProjectName(project.projectName);
			MainWindow::SetProjectDir(project.projectPath);

			//Place opened project to the first position
			auto it = std::find(projectsInfo.projects.begin(), projectsInfo.projects.end(), project);
			auto index = std::distance(projectsInfo.projects.begin(), it);

			SProject p = projectsInfo.projects[index];
			projectsInfo.projects.erase(projectsInfo.projects.begin() + index);
			projectsInfo.projects.insert(projectsInfo.projects.begin(), p);

			SaveProjects();

			return 0;
		}
		else
		{
			//MessageBox(0, _T("Error opening project. Assets folder not found!"), _T("Error"), MB_ICONERROR | MB_OK);
			cout << "Error opening project.Assets folder not found!" << endl;
		}
	}
	else
	{
		//MessageBox(0, _T("Error opening project. Project does not exist!"), _T("Error"), MB_ICONERROR | MB_OK);
		cout << "Error opening project. Project does not exist!" << endl;
	}

	return 1;
}

int ProjectSelector::OpenNewProject()
{
	CFolderPickerDialog folderPickerDialog((LPCTSTR)0, OFN_FILEMUSTEXIST | OFN_ENABLESIZING, nullptr, sizeof(OPENFILENAME));

	CString folderPath;

	if (folderPickerDialog.DoModal() == IDOK)
	{
		folderPath = folderPickerDialog.GetFolderPath();
		folderPath.Replace(_T("\\"), _T("/"));

		if (folderPath.GetAt(folderPath.GetLength() - 1) != CString("/"))
			folderPath = folderPath + _T("/");

		if (IO::DirExists(CSTR2STRW(folderPath + _T("Assets/"))))
		{
			SProject proj;
			proj.engineVersion = ENGINE_VERSION;
			proj.projectName = CSTR2STRW(folderPickerDialog.GetFileName());
			proj.projectPath = CSTR2STRW(folderPath);

			string tmp = (proj.projectPath + "Temp/");
			CString temp_dir(tmp.c_str());

			string set = (proj.projectPath + "Settings/");
			CString set_dir(set.c_str());

			// Create temp directory
			CreateDirectory(temp_dir, NULL);

			// Create settings directory
			CreateDirectory(set_dir, NULL);

			// Create solution directory
			CreateDirectory(CString((proj.projectPath + "Project/").c_str()), NULL);

			// Create library directory
			CreateDirectory(CString((proj.projectPath + "Library/").c_str()), NULL);

			SolutionWorker worker;
			worker.CreateSolution(proj.projectPath + "Project/", proj.projectName);

			MainWindow::SetProjectName(proj.projectName);
			MainWindow::SetProjectDir(CSTR2STRW(folderPath));

			if (!IsProjectExists(CSTR2STRW(folderPath)))
			{
				//Add new project to list
				projectsInfo.projects.insert(projectsInfo.projects.begin(), proj);
				SaveProjects();
			}
			else
			{
				//Place opened project to the first position
				auto it = std::find(projectsInfo.projects.begin(), projectsInfo.projects.end(), proj);
				auto index = std::distance(projectsInfo.projects.begin(), it);

				SProject p = projectsInfo.projects[index];
				projectsInfo.projects.erase(projectsInfo.projects.begin() + index);
				projectsInfo.projects.insert(projectsInfo.projects.begin(), p);

				SaveProjects();
			}

			return 0;
		}
		else
		{
			MessageBoxA(0, "Error opening project. Assets folder not found!", "Error", MB_ICONERROR | MB_OK);
		}
	}

	return -1;
}

int ProjectSelector::CreateNewProject(std::string projName, std::string projPath)
{
	SProject proj;
	proj.engineVersion = ENGINE_VERSION;
	proj.projectName = projName;
	proj.projectPath = projPath + projName + "/";

	boost::replace_all(proj.projectPath, "\\", "/");

	while (proj.projectPath.find("//") != string::npos)
	{
		boost::replace_all(proj.projectPath, "//", "/");
	}

	string as = (proj.projectPath + "Assets/");
	string tmp = (proj.projectPath + "Temp/");
	string set = (proj.projectPath + "Settings/");

	CString dir = CString(proj.projectPath.c_str());
	CString assets_dir = CString(as.c_str());
	CString temp_dir = CString(tmp.c_str());
	CString set_dir = CString(set.c_str());

	bool good = true;

	if (proj.projectName.empty() || proj.projectPath.empty())
		good = false;

	if (good)
	{
		//Create project directory
		if (CreateDirectory(dir, NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
			good = false;

		// Create assets directory
		if (CreateDirectory(assets_dir, NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
			good = false;

		// Create temp directory
		if (CreateDirectory(temp_dir, NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
			good = false;

		// Create settings directory
		if (CreateDirectory(set_dir, NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
			good = false;

		// Create solution directory
		if (CreateDirectory(CString((proj.projectPath + "Project/").c_str()), NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
			good = false;

		// Create library directory
		if (CreateDirectory(CString((proj.projectPath + "Library/").c_str()), NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
			good = false;
	}

	if (good)
	{
		SolutionWorker worker;
		worker.CreateSolution(proj.projectPath + "Project/", proj.projectName);

		if (IO::DirExists(proj.projectPath) && IO::DirExists(proj.projectPath + "Assets/") && IO::DirExists(proj.projectPath + "Temp/"))
		{
			if (!IsProjectExists(proj.projectPath))
			{
				//Add new project to list and save it
				projectsInfo.projects.insert(projectsInfo.projects.begin(), proj);
				SaveProjects();
			}

			MainWindow::SetProjectName(proj.projectName);
			MainWindow::SetProjectDir(proj.projectPath);

			//Close window
			return 0;
		}
		else
		{
			MessageBox(0, _T("Error creating project. Create directories failed!"), _T("Error"), MB_ICONERROR | MB_OK);
		}
	}
	else
	{
		MessageBox(0, _T("Error creating project. Project name or path is not valid!"), _T("Error"), MB_ICONERROR | MB_OK);
	}

	return -1;
}

bool ProjectSelector::IsProjectExists(string path)
{
	bool exists = false;
	std::vector<SProject>::iterator it;
	for (it = projectsInfo.projects.begin(); it < projectsInfo.projects.end(); ++it)
	{
		if ((*it).projectPath._Equal(path))
		{
			exists = true;
			break;
		}
	}

	return exists;
}

void ProjectSelector::LoadProjects()
{
	ifstream conffile;

	string conf_path = IO::ReplaceBackSlashes(string(getenv("APPDATA"))) + "/FalcoEngine/Projects.bin";

	conffile.open(conf_path);
	if (conffile)
	{
		conffile.close();

		std::ifstream ofs(conf_path, std::ios::binary);
		boost::archive::binary_iarchive stream(ofs);

		stream >> projectsInfo;

		std::vector<SProject>::iterator it;
		std::vector<SProject> to_remove;

		for (it = projectsInfo.projects.begin(); it < projectsInfo.projects.end(); ++it)
		{
			while (it->projectPath.find("//") != string::npos)
			{
				boost::replace_all(it->projectPath, "//", "/");
			}
		}

		//Remove projects which are not exists anymore
		for (it = projectsInfo.projects.begin(); it < projectsInfo.projects.end(); ++it)
		{
			if (!IO::DirExists((*it).projectPath))
			{
				to_remove.push_back(*it);
			}
		}

		auto pred = [&to_remove](const SProject& key) ->bool
		{
			return std::find(to_remove.begin(), to_remove.end(), key) != to_remove.end();
		};

		projectsInfo.projects.erase(std::remove_if(projectsInfo.projects.begin(), projectsInfo.projects.end(), pred), projectsInfo.projects.end());

		if (to_remove.size() > 0)
			SaveProjects();
	}
}

void ProjectSelector::SaveProjects()
{
	CreateDirectory(CString(IO::ReplaceBackSlashes(getenv("APPDATA")).c_str()) + CString("/FalcoEngine/"), NULL);
	string conf_path = IO::ReplaceBackSlashes(string(getenv("APPDATA"))) + "/FalcoEngine/Projects.bin";

	std::ofstream ofs(conf_path, std::ios::binary);
	boost::archive::binary_oarchive stream(ofs);

	stream << projectsInfo;
}
