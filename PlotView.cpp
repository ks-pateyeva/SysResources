#include "PlotView.h"
#include <string>

float PlotView::prevTime = 0;
float PlotView::prevValue = 0;
RECT PlotView::rect = {120, 10, 120 + 550, 10 + 550};

void PlotView::DrawPlot(Graphics& graphics/*, float captionStep, float captionMax*/)
{

	Pen pen(Color(255, 0, 0, 0));
	Color const whiteColor(255, 255, 255);
	Brush *whiteBrush = new SolidBrush (whiteColor);
	graphics.FillRectangle(whiteBrush, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	graphics.DrawRectangle(&pen, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	
	// AXIS   
	Line(graphics, 0, 0.0, 0, 100.0, false);
	Line(graphics, 0.0, 0.0, 100.0, 0.0, false);

	for (float i = 10.0; i <= 100.0; i += 10.0)
	{
		Line(graphics, 0.0, i, -1, i, false);
		PlotString(graphics, (i == 100.0 ? -6.0f : -5.0f), i,
		           (PCWSTR)std::to_wstring(static_cast<int>(i)).c_str(), 3);
	}
}

void PlotView::Line(Graphics& g, float x1, float y1, float x2, float y2, bool fDiff)
{
	Pen pen(Color(255, 0, 0, 0));

	if (x1 >= MAX_X)
	{
		x1 -= MAX_X;
		x2 -= MAX_X;

		DrawPlot(g);
	}

	if (!fDiff)
	{
		x1 = CENTER_X + x1 * SCALE_X;
		x2 = CENTER_X + x2 * SCALE_X;
		y1 = CENTER_Y - y1 * SCALE_Y;
		y2 = CENTER_Y - y2 * SCALE_Y;
		g.DrawLine(&pen, x1, y1, x2, y2);
	}
	else
	{
		prevTime = x2;
		prevValue = y2;
		x1 = CENTER_X + x1;
		x2 = CENTER_X + x2;
		y1 = CENTER_Y - y1 * SCALE_Y;
		y2 = CENTER_Y - y2 * SCALE_Y;
		if (prevTime != 0)
		{
			g.DrawLine(&pen, x1, y1, x2, y2);
		}
	}
}

void PlotView::PlotString(Graphics& g, float x1, float y1, PCWSTR text, int len)
{
	x1 = CENTER_X + x1 * SCALE_X;
	y1 = CENTER_Y - y1 * SCALE_Y;

	PointF origin(x1, y1);
	SolidBrush blackBrush(Color(255, 0, 0, 0));
	Font myFont(L"Arial", 10);

	g.DrawString(
		text,
		len,
		&myFont,
		origin,
		&blackBrush);
}
