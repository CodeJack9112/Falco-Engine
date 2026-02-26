#pragma once

#include <string>
#include <vector>

class TreeView;
class TreeNode;
class DialogProgress;

class DialogAssetExporter
{
public:
	enum class Mode { Import, Export };

	DialogAssetExporter();
	~DialogAssetExporter();

	void show(Mode m);
	void update();

	Mode getMode() { return mode; }
	
private:
	bool visible = false;
	bool allSelected = true;
	Mode mode = Mode::Export;
	void setMode(Mode m) { mode = m; }

	std::string openedPackage = "";
	TreeView* treeView = nullptr;
	DialogProgress* progressDialog = nullptr;

	void listFiles(std::vector<std::string>& outList, TreeNode * htStart);

	void copyTreeView(TreeNode * root, TreeNode * dstRoot);
	void exportAssets();
	void importAssets();

	void checkNodes(TreeNode * root, bool check);
};

