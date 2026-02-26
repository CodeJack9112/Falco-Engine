#pragma once

#include <vector>
#include <functional>

class Undo
{
public:
	class Command
	{
	public:
		Command() = default;
		Command(std::function<void(Undo::Command* command)> action, void* data)
		{
			actionCallback = action;
			actionData = data;
		}

		~Command() = default;

		void Execute();
		void* GetData() { return actionData; }
		void Dispose() { if (actionData != nullptr) delete actionData; }

	private:
		std::function<void(Command* command)> actionCallback = nullptr;
		void* actionData = nullptr;
	};

	typedef std::vector<std::pair<Undo::Command, Undo::Command>> CommandList;

	Undo();
	~Undo();

	static void Push(Undo::Command commandUndo, Undo::Command commandRedo);
	static void SetCurrentActionUndo(Undo::Command commandUndo);
	static void SetCurrentActionRedo(Undo::Command commandRedo);
	static void DoUndo();
	static void DoRedo();
	static bool IsUndoAvailable();
	static bool IsRedoAvailable();

private:
	static CommandList commandBuffer;
	static int stackPos;
};

