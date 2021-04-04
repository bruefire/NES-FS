#pragma once
#include <vector>
#include "guiItem.h"

class MenuLgc
{
public:
    // コンストラクタ
    MenuLgc();

    GuiContainer menu;  // content
    double scale;

    enum class INPUT
    {
        NONE,
        UP,
        DOWN,
        OK,
        CANCEL,
        RETURN
    };


    // メニュー種類
    // ※項目を増減させた場合は.cpp側でその項目について初期化も必須
    enum MENU
    {
    };
    // メニューアクション
    enum MENU_ACT
    {
    };

    //typedef void (engine3dOSX::*menuFunc)();

    class GuiStringEx : public GuiString
    {
    public:
        MENU_ACT act;

        GuiStringEx();
        GuiStringEx(std::string, MENU_ACT);
    };

    virtual void MakeMenu() = 0;
    uint64_t makeSubMenu(MENU, GuiContainer*, GuiStringEx*, int);
    void disposeMenu(GuiContainer*);


    // メニューメソッド
    void Init();
    void Dispose();
    virtual bool MenuMsgProc(MENU_ACT) = 0;
    bool InputProc(INPUT);

    void MoveToOtherMenu(MENU);
    virtual void setOwner(void*) = 0;

    //---

};



