#pragma once
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

const int CENTER_X = 150;
const int CENTER_Y = 530;
const int SCALE_X = 5;
const int SCALE_Y = 5;


class PlotView
{
public:
	static void DrawPlot(HDC hdc);
	static void PlotLine(Graphics& g, float x1, float y1, float x2, float y2, bool fDiff);
	static float prevTime, prevValue;
private:
	static void PlotString(Graphics& g, float x1, float y1, PCWSTR text, int len);
};
