#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "constants.h"
#include "geometry.h"


// ���j���[�֌W
class GlRect : public Rect
{
public:
	enum STD
	{
		NORMAL,
		HORIZONAL,
		VERTICAL
	};
	STD mode;

	GlRect();
	GlRect(pt2, double, double, STD);
	GlRect(double, double, double, double, STD);
	GlRect(double, STD);
};


class GuiItem
{
public:
	enum GuiType
	{
		BASE,
		CONTAINER,
		STRING
	};
	GuiType type;

	GuiItem* owner;

	GlRect drawArea;
	GlRect padding;
	GlRect margin;

	pt3 bkColor;
	double alpha;

	bool displayed;
	bool checked;

	GuiItem();

};

class GuiContainer : public GuiItem
{
public:
	std::vector<GuiItem*> childs;
	int selectedIdx;

	GuiContainer();
	GuiItem* SelectedChild();
};


class GuiString : public GuiItem
{
public:
	std::string content;
	double fontSz;
	double fontSpan;

	// �R���X�g���N�^
	GuiString();
	GuiString(std::string content, GlRect rect);

};
// end ���j���[�֌W 


