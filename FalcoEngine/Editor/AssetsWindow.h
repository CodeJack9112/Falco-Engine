#pragma once

#include <string>
#include <vector>
#include <OgrePrerequisites.h>
#include <OgreTexture.h>

#include "../Engine/AssetsTools.h"

class TreeView;
class TreeNode;

#define MAX_DIRS 65536
#define MAX_FILES 65536
#define MAX_BUFFER 4096

class FileSystemWatcher : public CWinThread
{
	DECLARE_DYNCREATE(FileSystemWatcher)

private:
	typedef struct _DIRECTORY_INFO {
		HANDLE hDir;
		TCHAR lpszDirName[MAX_PATH];
		CHAR lpBuffer[MAX_BUFFER];
		DWORD dwBufLength;
		OVERLAPPED Overlapped;
	} DIRECTORY_INFO, * PDIRECTORY_INFO, * LPDIRECTORY_INFO;

private:
	void WatchDirectory(LPCWSTR path);

	DIRECTORY_INFO DirInfo[MAX_DIRS];   // Buffer for all of the directories
	TCHAR FileList[MAX_FILES * MAX_PATH]; // Buffer for all of the files
	DWORD numDirs;

	bool work = true;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance()
	{
		work = false;
		return 0;
	}
};

class AssetsWindow
{
public:
	AssetsWindow();
	~AssetsWindow();

	void init();
	void update();
	void show(bool show) { opened = show; }
	bool getVisible() { return opened; }

	TreeView* getTreeView() { return treeView; }
	void reloadFiles();
	void sortTree();
	
	std::vector<std::string> addNewFiles(std::vector<std::string> files = std::vector<std::string>());

	static Ogre::TexturePtr getIconByExtension(std::string ext);
	static void reassignTexture(std::string oldFullPath, std::string newFullPath);
	void focusOnFile(std::string filepath);

private:
	enum FileOp { FOP_RENAME, FOP_MOVE };

	CWinThread* pWinThread = nullptr;

	static AssetsWindow* self;

	bool opened = true;
	bool isWindowDragging = false;
	TreeView* treeView = nullptr;
	bool movingFiles = false;
	std::vector<std::string> expandedItems;

	bool updateFiles = false;
	bool deleteAssetNode = false;

	Ogre::TexturePtr addIcon;
	Ogre::TexturePtr addFolderIcon;
	Ogre::TexturePtr closeIcon;

	TreeNode* addedAssetNode = nullptr;
	std::vector<TreeNode*> deleteNodes;

	void updateAssetsMenu();
	void updateAssetsPopup(bool itemsOnly = false, TreeNode * selNode = nullptr);

	static void onNodesSelected(std::vector<std::string> names, void * userData);

	void onTreeViewEndUpdate();
	void onDropNode(TreeNode * node, TreeNode * from);

	void addScripts(std::vector<std::string> files = std::vector<std::string>());
	static std::vector<std::string> getImagesFileFormats();
	void deleteResource(std::string path, bool checkFolder = false);
	void renameScriptResource(std::string path, std::string resourceType);
	void rememberExpandedItems(TreeNode * root);
	void rememberAllItems(TreeNode* root, std::vector<std::string>& list);

	bool moveAsset(std::string oldPath, std::string newPath, FileOp op);
	bool moveFolder(TreeNode * item, std::string newPath);
	bool renameFolder(TreeNode * item, std::string oldName);
	std::vector<bool> onNodesMoved(std::vector<std::string> items, TreeNode* moveto);
	bool onNodeRenamed(TreeNode* item, std::string oldName);
	void onNodePopupMenu(TreeNode * node, int val);
	void onNodeDoubleClick(TreeNode * node);

	void createFolder(TreeNode* selNode = nullptr);
	void createMaterial(TreeNode* selNode = nullptr, AssetsTools::MaterialType type = AssetsTools::MaterialType::MT_DEFERRED);

	void sortTree(TreeNode* root);
};

