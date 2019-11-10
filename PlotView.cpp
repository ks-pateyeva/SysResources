#include "PlotView.h"
#include <string>

int PlotView::diff_x = 0;

void PlotView::DrawPlot(HDC hdc)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0));
	graphics.DrawRectangle(&pen, 120, 10, 550, 550);

	// AXIS   
	PlotLine(graphics, pen, 0, 0.0, 0, 100.0, false);
	PlotLine(graphics, pen, 0.0, 0.0, 100.0, 0.0, false);

	for (float i = 10.0; i <= 100.0; i += 10.0)
	{
		PlotLine(graphics, pen, 0.0, i, -1, i, false);
		PlotString(graphics, (i == 100.0 ? -6.0f : -5.0f), i,
			(PCWSTR)std::to_wstring(static_cast<int>(i)).c_str(), 3);
	}
}

void PlotView::PlotLine(Graphics& g, Pen& p, float x1, float y1, float x2, float y2, bool fDiff)
{
	if (fDiff)
	{
		x1 = x1 - diff_x;
		x2 = x2 - diff_x;
	}
	x1 = CENTER_X + x1 * SCALE_X;
	x2 = CENTER_X + x2 * SCALE_X;
	y1 = CENTER_Y - y1 * SCALE_Y;
	y2 = CENTER_Y - y2 * SCALE_Y;

	if ((x1 >= 0 && x2 >= 0) || (!fDiff))
	{
		g.DrawLine(&p, x1, y1, x2, y2);
	}
}

void PlotView::PlotString(Graphics& g, float x1, float y1, PCWSTR text, int len)
{
	x1 = CENTER_X + x1 * SCALE_X;
	y1 = CENTER_Y - y1 * SCALE_Y;

	PointF origin(x1, y1);
	SolidBrush blackBrush(Color(255, 0, 0, 0));
	Font myFont(L"Arial", 10);

	wchar_t* wa = new wchar_t[len];

	g.DrawString(
		text,
		len,
		&myFont,
		origin,
		&blackBrush);
}
