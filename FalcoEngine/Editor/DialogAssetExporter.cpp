#include "stdafx.h"
#include "DialogAssetExporter.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include "../LibZip/zip.h"

#include "../boost/algorithm/string.hpp"
#include "../Engine/StringConverter.h"
#include "../Engine/ResourceMap.h"
#include "../Engine/IO.h"

#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "TreeView.h"
#include "TreeNode.h"
#include "EditorClasses.h"
#include "DialogProgress.h"
#include "AssetsWindow.h"
#include "DialogProgress.h"

#include <io.h>
#include <fcntl.h>
#include <fstream>

DialogAssetExporter::DialogAssetExporter()
{
	treeView = new TreeView();
	treeView->setCheckChildren(true);

	progressDialog = new DialogProgress();
	progressDialog->setTitle("Exporting package");
}

DialogAssetExporter::~DialogAssetExporter()
{
	delete progressDialog;
}

void DialogAssetExporter::show(Mode m)
{
	visible = true;
	allSelected = true;
	setMode(m);

	treeView->clear();

	if (mode == Mode::Import)
	{
		CFileDialog fileDialog(TRUE, _T(".package"), NULL, OFN_HIDEREADONLY, _T("Falco Engine Assets Package (*.package)|*.package"));
		int result = fileDialog.DoModal();
		if (result == IDOK)
		{
			CString fn = fileDialog.GetPathName();
			fn.Replace(_T("\\"), _T("/"));

			std::string _fn = CSTR2STRW(fn);

			if (IO::FileExists(_fn))
			{
				struct zip* za;
				int err;
				char buf[100];
				struct zip_stat sb;

				if ((za = zip_open(CP_UNI(_fn).c_str(), 0, &err)) == NULL) {
					zip_error_to_str(buf, sizeof(buf), err, errno);
					MessageBoxA(0, string("Filed to open package: " + string(buf)).c_str(), "Error", MB_OK);

					visible = false;
					return;
				}

				openedPackage = _fn;

				for (int i = 0; i < zip_get_num_entries(za, 0); ++i)
				{
					zip_stat_index(za, i, 0, &sb);
					std::string path = CP_SYS(sb.name);

					if (treeView->getNodeByPath(path, treeView->getRootNode()) == nullptr)
					{
						std::vector<std::string> results;
						boost::split(results, path, [](char c) { return c == '/' || c == '\\'; });

						TreeNode* item = treeView->getRootNode();

						for (std::vector<string>::iterator it2 = results.begin(); it2 != results.end(); ++it2)
						{
							if (*it2 == "AssetPackage_ResourceMap.bin")
								continue;

							TreeNode* newItem = treeView->getNodeByPath(*it2, item);
							if (newItem == nullptr)
							{
								TexturePtr icon = AssetsWindow::getIconByExtension(IO::GetFileExtension(*it2));

								TreeNode* node = new TreeNode(treeView);
								node->alias = *it2;
								node->name = ResourceMap::genGuid();
								node->icon = icon;
								node->enableDrag = false;
								node->checked = true;
								node->showCheckBox = true;
								node->setEditable(false);
								item->addChild(node);

								item = node;

								string ext = IO::GetFileExtension(*it2);
								std::string _str = item->getPath();

								if (IO::FileExists(MainWindow::GetProjectAssetsDir() + _str) || IO::DirExists(MainWindow::GetProjectAssetsDir() + _str))
								{
									item->checked = true;
									item->showCheckBox = false;
								}
							}
							else
							{
								item = newItem;
							}
						}

						results.clear();
					}
				}

				zip_close(za);
			}
			else
				visible = false;
		}
		else
		{
			visible = false;
		}
	}
	else
	{
		copyTreeView(MainWindow::GetAssetsWindow()->getTreeView()->getRootNode(), treeView->getRootNode());
	}
}

void DialogAssetExporter::update()
{
	if (!visible)
		return;

	string caption = "";
	string btnImportExport = "";

	if (mode == Mode::Export)
	{
		caption = "Export Assets";
		btnImportExport = "Export";
	}
	else
	{
		caption = "Import Assets";
		btnImportExport = "Import";
	}

	bool closed = false;
	bool ok = false;

	ImGui::SetNextWindowSize(ImVec2(300.0f, 500.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowPos(ImVec2(MainWindow::GetWindowSize().x / 2, MainWindow::GetWindowSize().y / 2), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
	if (ImGui::Begin(caption.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
	{
		if (ImGui::Checkbox("", &allSelected))
		{
			TreeNode* root = treeView->getRootNode();

			for (auto it = root->children.begin(); it != root->children.end(); ++it)
				checkNodes(*it, allSelected);
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		ImGuiWindow* window = GImGui->CurrentWindow;
		ImGuiID id = window->GetIDNoKeepAlive("##AssetsExporterVS");
		ImGui::BeginChild(id, ImVec2(0, ImGui::GetWindowSize().y - 95));
		treeView->update();
		ImGui::EndChild();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (ImGui::Button("Cancel"))
		{
			closed = true;
			ok = false;
		}
		
		ImGui::SameLine();

		if (ImGui::Button(btnImportExport.c_str()))
		{
			closed = true;
			ok = true;
		}
	}

	ImGui::End();

	if (closed && ok)
	{
		progressDialog->show();

		if (mode == Mode::Export)
		{
			MainWindow::AddOnEndUpdateCallback([=]() {
				exportAssets();
				visible = false;
				progressDialog->hide();
			});
		}
		else
		{
			MainWindow::AddOnEndUpdateCallback([=]() {
				importAssets();
				visible = false;
				progressDialog->hide();
			});
		}
	}

	if (closed && !ok)
	{
		visible = false;
	}

	progressDialog->update();
}

void DialogAssetExporter::listFiles(std::vector<std::string>& outList, TreeNode* htStart)
{
	for (auto it = htStart->children.begin(); it != htStart->children.end(); ++it)
	{
		string path = (*it)->getPath();

		if (!IO::GetFileExtension(path).empty() && (*it)->children.size() == 0)
		{
			if ((*it)->checked)
				outList.push_back(path);
		}
		else
			listFiles(outList, *it);
	}
}

void DialogAssetExporter::copyTreeView(TreeNode* root, TreeNode* dstRoot)
{
	for (auto it = root->children.begin(); it != root->children.end(); ++it)
	{
		TreeNode* copy = *it;

		TreeNode* node = new TreeNode(dstRoot->treeView);
		node->alias = copy->alias;
		node->name = ResourceMap::genGuid();
		node->icon = copy->icon;
		node->enableDrag = false;
		node->checked = true;
		node->showCheckBox = true;
		node->setEditable(false);
		dstRoot->addChild(node);

		copyTreeView(copy, node);
	}
}

void DialogAssetExporter::exportAssets()
{
	CFileDialog fileDialog(FALSE, _T(".package"), NULL, OFN_HIDEREADONLY, _T("Falco Engine Assets Package (*.package)|*.package"));
	int result = fileDialog.DoModal();
	if (result == IDOK)
	{
		CString fn = fileDialog.GetPathName();
		fn.Replace(_T("\\"), _T("/"));

		std::string _fn = CSTR2STRW(fn);
		string ext = IO::GetFileExtension(_fn);

		if (ext != "package")
		{
			MainWindow::GetConsoleWindow()->log("Error exporting package. Can not write to file of unsupported package format (." + ext + ")", LogMessageType::LMT_ERROR);
			return;
		}

		std::vector<std::string> files;
		listFiles(files, treeView->getRootNode());

		if (files.size() == 0)
		{
			MainWindow::GetConsoleWindow()->log("Error exporting package. No files specified", LogMessageType::LMT_ERROR);
			return;
		}

		progressDialog->setTitle("Exporting package");
		progressDialog->setStatusText("Packing assets...");

		ResourceMapTemp* tmpResMap = new ResourceMapTemp();

		for (auto it = files.begin(); it != files.end(); ++it)
		{
			ResourceID resId;
			resId.resourceName = *it;
			resId.resourceGuid = ResourceMap::getResourceGuidFromName(resId.resourceName);
			tmpResMap->resourceMap.push_back(resId);
		}

		std::string ap_resMap = MainWindow::GetProjectTempDir() + "AssetPackage_ResourceMap.bin";
		tmpResMap->save(ap_resMap);

		int* zErr = nullptr;
		zip_t* _zip = zip_open(CP_UNI(_fn + "_tmp").c_str(), ZIP_CREATE, zErr);

		zip_source* source = zip_source_file(_zip, CP_UNI(MainWindow::GetProjectTempDir() + "AssetPackage_ResourceMap.bin").c_str(), 0, 0);
		zip_file_add(_zip, CP_UNI("AssetPackage_ResourceMap.bin").c_str(), source, ZIP_FL_ENC_UTF_8);

		int totalFiles = files.size();
		int i = 0;

		for (auto it = files.begin(); it != files.end(); ++it, ++i)
		{
			float progress = (float)1.0f / (float)totalFiles * (float)i;
			float progressP = (float)100.0f / (float)totalFiles * (float)i;

			progressDialog->setStatusText("Compressing files... " + to_string((int)progressP) + "%% (" + to_string(i) + "/" + to_string(totalFiles) + ")");
			progressDialog->setProgress(progress);

			zip_source* source = zip_source_file(_zip, CP_UNI(MainWindow::GetProjectAssetsDir() + *it).c_str(), 0, 0);
			zip_file_add(_zip, CP_UNI(*it).c_str(), source, ZIP_FL_ENC_UTF_8);
		}

		zip_close(_zip);

		IO::FileDelete(ap_resMap);

		if (IO::FileExists(_fn))
			IO::FileDelete(_fn);

		IO::FileRename(_fn + "_tmp", _fn);

		MainWindow::GetConsoleWindow()->log("Assets successfully exported!", LogMessageType::LMT_SUCCESS);
	}
}

void DialogAssetExporter::importAssets()
{
	std::vector<std::string> selectedAssets;
	listFiles(selectedAssets, treeView->getRootNode());

	progressDialog->setTitle("Importing package");
	progressDialog->setStatusText("Extracting assets...");

	const char* archive;
	struct zip* za;
	struct zip_file* zf;
	struct zip_stat sb;
	char buf[100];
	int err;
	int i, len;
	int fd;
	long long sum;

	if ((za = zip_open(CP_UNI(openedPackage).c_str(), 0, &err)) == NULL)
	{
		zip_error_to_str(buf, sizeof(buf), err, errno);

		MainWindow::GetConsoleWindow()->log("Filed to open package: " + string(buf), LogMessageType::LMT_ERROR);
	}

	int totalFiles = 0;
	int j = 0;

	for (auto it = selectedAssets.begin(); it != selectedAssets.end(); ++it)
	{
		if (!IO::FileExists(MainWindow::GetProjectAssetsDir() + *it))
			totalFiles += 1;
	}

	for (auto it = selectedAssets.begin(); it != selectedAssets.end(); ++it)
	{
		float progress = (float)1.0f / (float)totalFiles * (float)j;
		float progressP = (float)100.0f / (float)totalFiles * (float)j;

		progressDialog->setStatusText("Decompressing files... " + to_string((int)progressP) + "%% (" + to_string(j) + "/" + to_string(totalFiles) + ")");
		progressDialog->setProgress(progress);

		if (!IO::FileExists(MainWindow::GetProjectAssetsDir() + *it))
		{
			++j;

			std::string _p = IO::GetFilePath(MainWindow::GetProjectAssetsDir() + *it);
			IO::CreateDir(_p, true);

			if (zip_stat(za, CP_UNI(*it).c_str(), ZIP_FL_ENC_UTF_8, &sb) == 0)
			{
				zf = zip_fopen(za, CP_UNI(*it).c_str(), ZIP_FL_ENC_UTF_8);

				fd = open((MainWindow::GetProjectAssetsDir() + CP_SYS(sb.name)).c_str(), O_RDWR | O_TRUNC | O_CREAT | O_BINARY, 0644);
				if (fd < 0)
				{
					string err = zip_strerror(za);
					MainWindow::GetConsoleWindow()->log("Error extracting package: " + err + " (" + (*it) + ")", LogMessageType::LMT_ERROR);
				}

				sum = 0;
				while (sum != sb.size)
				{
					len = zip_fread(zf, buf, 100);
					if (len < 0)
					{
						string err = zip_strerror(za);
						MainWindow::GetConsoleWindow()->log("Error extracting package: " + err + " (" + (*it) + ")", LogMessageType::LMT_ERROR);
					}
					write(fd, buf, len);
					sum += len;
				}

				close(fd);
				zip_fclose(zf);
			}
			else
			{
				string err = zip_strerror(za);
				MainWindow::GetConsoleWindow()->log("Error extracting package: " + err + " (" + (*it) + ")", LogMessageType::LMT_ERROR);
			}
		}
	}

	std::string outName = MainWindow::GetProjectTempDir() + IO::GetFileName(openedPackage) + "_ResourceMap.bin";
	zip_stat(za, CP_UNI("AssetPackage_ResourceMap.bin").c_str(), ZIP_FL_ENC_UTF_8, &sb);
	zf = zip_fopen(za, CP_UNI("AssetPackage_ResourceMap.bin").c_str(), ZIP_FL_ENC_UTF_8);

	fd = open(outName.c_str(), O_RDWR | O_TRUNC | O_CREAT | O_BINARY, 0644);

	sum = 0;
	while (sum != sb.size)
	{
		len = zip_fread(zf, buf, 100);
		write(fd, buf, len);
		sum += len;
	}

	close(fd);
	zip_fclose(zf);

	zip_close(za);

	ResourceMapTemp tmpResMap;
	tmpResMap.load(outName);

	for (auto it = tmpResMap.resourceMap.begin(); it != tmpResMap.resourceMap.end(); ++it)
	{
		ResourceMap::addResource(it->resourceName, it->resourceGuid);
	}

	ResourceMap::save();
	IO::FileDelete(outName);

	MainWindow::Get()->WindowRestored();

	MainWindow::GetConsoleWindow()->log("Assets successfully imported!", LogMessageType::LMT_SUCCESS);
}

void DialogAssetExporter::checkNodes(TreeNode* root, bool check)
{
	root->checked = check;

	for (auto it = root->children.begin(); it != root->children.end(); ++it)
		checkNodes(*it, allSelected);
}
