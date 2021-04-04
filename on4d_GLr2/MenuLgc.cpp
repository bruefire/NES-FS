#include <string>
#include <exception>
#include <glew.h>
#include <GL/gl.h>
#include "MenuLgc.h"
using namespace std;
#define arraySizeof(ary) sizeof(ary)/sizeof(ary[0])

MenuLgc::GuiStringEx::GuiStringEx()
{}
MenuLgc::GuiStringEx::GuiStringEx(string content, MENU_ACT act)
{
    this->content = content;
    this->act = act;
}


void MenuLgc::Init()
{
    MakeMenu();
}

void MenuLgc::Dispose()
{
    disposeMenu(&menu);
}


MenuLgc::MenuLgc()
    : scale(1)
{
}



// ルートメニュー初期化
uint64_t MenuLgc::makeSubMenu(MENU TYPE, GuiContainer* menu, GuiStringEx* gs, int itemLen)
{
    if (menu->childs[(int)TYPE] != nullptr)
        throw new std::exception("メニュー初期化エラー");

    // サブコンテナ (アイテムコンテナ)
    GuiContainer* sub = new GuiContainer();

    // 親設定のコピー
    sub->drawArea = menu->drawArea;
    sub->padding = menu->padding;
    sub->bkColor = menu->bkColor;
    sub->alpha = menu->alpha;

    sub->displayed = false;
    menu->childs[(int)TYPE] = sub;

    // 各メニューアイテムの設定
    for (int i = 0; i < itemLen; i++)
    {
        GuiStringEx* item = new GuiStringEx();
        item->content = gs[i].content;
        item->act = gs[i].act;
        double parL = menu->drawArea.l + menu->padding.l;
        double parT = menu->drawArea.t + menu->padding.t;
        item->fontSz = 0.04 * scale;
        item->fontSpan = 0.8;
        item->padding = GlRect(0.01, GlRect::STD::HORIZONAL);
        item->drawArea = GlRect
        (
            pt2
            (
                item->content.length() * item->fontSz * item->fontSpan
                + item->padding.l + item->padding.r,
                item->fontSz + item->padding.t + item->padding.b
                ),
            parL + 0.02,
            parT + 0.02 + 0.08 * i * scale,
            GlRect::STD::HORIZONAL
            );

        item->bkColor = pt3(1.0, 1.0, 1.0);
        item->alpha = 0.6;

        sub->childs.push_back(item);
    }
    sub->selectedIdx = 0;


    return 1;
}


void MenuLgc::disposeMenu(GuiContainer* ct)
{
    int len = ct->childs.size();

    // 各子要素の破棄
    for (int i = 0; i < len; i++)
    {
        switch (ct->childs[i]->type)
        {
            // コンテナであれば再帰
        case GuiItem::GuiType::CONTAINER:
            disposeMenu((GuiContainer*)ct->childs[i]);
            break;
            //            case GuiItem::GuiType::STRING:
            //                break;
        }

        delete ct->childs[i];
    }
}


bool MenuLgc::InputProc(INPUT input)
{
    // ターゲットメニュー
    GuiContainer* trg = (GuiContainer*)menu.childs[menu.selectedIdx];

    switch (input)
    {
    case INPUT::UP:
    {
        trg->selectedIdx
            = (trg->selectedIdx - 1 < 0)
            ? trg->childs.size() - 1
            : trg->selectedIdx - 1;
        break;
    }
    case INPUT::DOWN:
    {
        trg->selectedIdx
            = (
                trg->childs.size() - 1
                <
                trg->selectedIdx + 1
                )
            ? 0
            : trg->selectedIdx + 1;
        break;
    }
    case INPUT::OK:
    {
        if(!MenuMsgProc(((MenuLgc::GuiStringEx*)trg->SelectedChild())->act))
            return false;
        break;
    }
    case INPUT::CANCEL:
        break;
    case INPUT::RETURN:
        menu.displayed = !menu.displayed;
        break;
    }

    return true;
}



// 別のメニューへ移動
void MenuLgc::MoveToOtherMenu(MENU menuKey)
{
    menu.selectedIdx = (int)menuKey;
}

