#pragma once
#include <vector>
#include "guiItem.h"

class MenuLgc
{
public:
    // �R���X�g���N�^
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


    // ���j���[���
    // �����ڂ𑝌��������ꍇ��.cpp���ł��̍��ڂɂ��ď��������K�{
    enum MENU
    {
    };
    // ���j���[�A�N�V����
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


    // ���j���[���\�b�h
    void Init();
    void Dispose();
    virtual bool MenuMsgProc(MENU_ACT) = 0;
    bool InputProc(INPUT);

    void MoveToOtherMenu(MENU);
    virtual void setOwner(void*) = 0;

    //---

};



