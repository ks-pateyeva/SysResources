#include "PlotView.h"
#include <string>

float PlotView::prevTime = 0;
float PlotView::prevValue = 0;

void PlotView::DrawPlot(HDC hdc)
{
	Pen pen(Color(255, 0, 0, 0));
	Graphics graphics(hdc);
	graphics.DrawRectangle(&pen, 120, 10, 550, 550);

	// AXIS   
	PlotLine(graphics, 0, 0.0, 0, 100.0, false);
	PlotLine(graphics, 0.0, 0.0, 100.0, 0.0, false);

	for (float i = 10.0; i <= 100.0; i += 10.0)
	{
		PlotLine(graphics, 0.0, i, -1, i, false);
		PlotString(graphics, (i == 100.0 ? -6.0f : -5.0f), i,
		           (PCWSTR)std::to_wstring(static_cast<int>(i)).c_str(), 3);
	}
}

void PlotView::PlotLine(Graphics& g, float x1, float y1, float x2, float y2, bool fDiff)
{
	Pen pen(Color(255, 0, 0, 0));


	if ((!fDiff))
	{
		x1 = CENTER_X + x1 * SCALE_X;
		x2 = CENTER_X + x2 * SCALE_X;
		y1 = CENTER_Y - y1 * SCALE_Y;
		y2 = CENTER_Y - y2 * SCALE_Y;
		g.DrawLine(&pen, x1, y1, x2, y2);
	}
	if (fDiff)
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
