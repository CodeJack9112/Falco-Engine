#include "stdafx.h"
#include "Undo.h"

Undo::CommandList Undo::commandBuffer;
int Undo::stackPos = 0;

Undo::Undo()
{
}

Undo::~Undo()
{
}

void Undo::Command::Execute()
{
	if (actionCallback != nullptr)
		actionCallback(this);
}

void Undo::Push(Command commandUndo, Command commandRedo)
{
	if (commandBuffer.size() > stackPos)
	{
		for (auto it = commandBuffer.begin() + stackPos; it != commandBuffer.end(); ++it)
		{
			it->first.Dispose();
			it->second.Dispose();
		}
		commandBuffer.erase(commandBuffer.begin() + stackPos, commandBuffer.end());
	}

	commandBuffer.push_back(std::make_pair(commandUndo, commandRedo));
	stackPos += 1;
}

void Undo::SetCurrentActionUndo(Command commandUndo)
{
	if (stackPos > 0)
	{
		if (commandBuffer.size() > stackPos - 1)
		{
			commandBuffer[stackPos - 1].first = commandUndo;
		}
	}
}

void Undo::SetCurrentActionRedo(Command commandRedo)
{
	if (stackPos > 0)
	{
		if (commandBuffer.size() > stackPos - 1)
		{
			commandBuffer[stackPos - 1].second = commandRedo;
		}
	}
}

void Undo::DoUndo()
{
	if (commandBuffer.size() > 0 && stackPos > 0)
	{
		stackPos -= 1;
		Command& cb = commandBuffer[stackPos].first;
		cb.Execute();
	}
}

void Undo::DoRedo()
{
	if (commandBuffer.size() > 0 && stackPos < commandBuffer.size())
	{
		Command& cb = commandBuffer[stackPos].second;
		cb.Execute();
		stackPos += 1;
	}
}

bool Undo::IsUndoAvailable()
{
	return commandBuffer.size() > 0 && stackPos > 0;
}

bool Undo::IsRedoAvailable()
{
	return commandBuffer.size() > 0 && stackPos < commandBuffer.size();
}
