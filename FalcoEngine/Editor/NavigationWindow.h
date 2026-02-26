#pragma once

class NavigationWindow
{
public:
	NavigationWindow();
	~NavigationWindow();

	void update();
	void show(bool show) { opened = show; }
	bool getVisible() { return opened; }

	void setWalkableSlopeAngle(float value) { walkableSlopeAngle = value; }
	void setWalkableHeight(float value) { walkableHeight = value; }
	void setWalkableClimb(float value) { walkableClimb = value; }
	void setWalkableRadius(float value) { walkableRadius = value; }
	void setMaxEdgeLength(float value) { maxEdgeLength = value; }
	void setMaxSimplificationError(float value) { maxSimplificationError = value; }
	void setMinRegionArea(float value) { minRegionArea = value; }
	void setMergeRegionArea(float value) { mergeRegionArea = value; }
	void setCellSize(float value) { cellSize = value; }
	void setCellHeight(float value) { cellHeight = value; }

private:
	bool opened = true;

	float walkableSlopeAngle = 0;
	float walkableHeight = 0;
	float walkableClimb = 0;
	float walkableRadius = 0;
	float maxEdgeLength = 0;
	float maxSimplificationError = 0;
	float minRegionArea = 0;
	float mergeRegionArea = 0;
	float cellSize = 0;
	float cellHeight = 0;
};

