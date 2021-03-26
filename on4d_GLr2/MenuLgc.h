#pragma once
#include <vector>
#include "guiItem.h"

class engine3dGL;
class MenuLgc
{
public:
    // �R���X�g���N�^
    MenuLgc();

    engine3dGL* owner;
    GuiContainer menu;  // content

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
        MAIN = 0,
        OBJECT,
        CHANGE_THROW_OBJECT,
        CHANGE_BASIC_OBJECT,
        RELOCATE_THROW_OBJECT,
        VIEW,
        DEPTH_EFFECT,
        BACK_COLOR,
        WORLD_RADIUS,
        FIELD_OF_VIEW,
        QUIT,
        len
    };
    // ���j���[�A�N�V����
    enum MENU_ACT
    {
        // ���j���[�J��
        MOVE_TO_MAIN_MENU,
        MOVE_TO_OBJECT_MENU,
        MOVE_TO_VIEW_MENU,
        MOVE_TO_THROW_OBCECT_MENU,
        MOVE_TO_BASIC_OBCECT_MENU,
        MOVE_TO_RELOCATE_THROW_OBCECT_MENU,
        MOVE_TO_DEPTH_EFFECT_MENU,
        MOVE_TO_BACK_COLOR_MENU,
        MOVE_TO_WORLD_RADIUS_MENU,
        MOVE_TO_FOV_MENU,
        MOVE_TO_QUIT_MENU,

        // �ޏo
        QUIT_PROGRAM,

        // ����
        MAKE_FOV_NARROW,
        MAKE_FOV_NORMAL,
        MAKE_FOV_WIDE,

        // ���a
        MAKE_RADIUS_TINY,
        MAKE_RADIUS_SMALL,
        MAKE_RADIUS_MIDDLE,
        MAKE_RADIUS_LARGE,

        // �w�i�F
        BACK_COLOR_WHITE,
        BACK_COLOR_BLACK,

        // �[�x����
        DEPTH_EFFECT_NONE,
        DEPTH_EFFECT_LINEAR,
        DEPTH_EFFECT_SHADOW1,
        DEPTH_EFFECT_SHADOW2,

        // �X���[�I�u�W�F�N�g�ύX
        CHANGE_THROW_OBJECT_TO_CUBE,
        CHANGE_THROW_OBJECT_TO_EARTH,
        CHANGE_THROW_OBJECT_TO_HORSE,
        CHANGE_THROW_OBJECT_TO_ASTEROID,

        // ���I�u�W�F�N�g�ύX
        MAKE_BASIC_OBJECT_INVISILE,
        CHANGE_BASIC_OBJECT_TO_LINES_LN,
        CHANGE_BASIC_OBJECT_TO_POLAR_LN,
        CHANGE_BASIC_OBJECT_TO_CELLS120,
        CHANGE_BASIC_OBJECT_TO_TORUS,
        CHANGE_BASIC_OBJECT_TO_EARTH,
        CHANGE_BASIC_OBJECT_TO_GREEN_FIELD,

        // �X���[�I�u�F�N�g�Ĕz�u
        RELOCATE_THROW_OBJECT_RANDOMLY,
        RELOCATE_THROW_OBJECT_RANDOMLY2,
        CLEAR_THROW_OBJECT

    };
    // �X���[�I�u�W�F�N�g
    enum THROW_OBJ
    {
        CUBE = 4,
        EARTH = 8,
        HORSE = 5,
        ASTEROID = 14
    };
    // ���I�u�W�F�N�g
    enum BASIC_OBJ
    {
        LINES = 0,
        POLAR = 3,
        TORUS = 13,
        CELLS120 = 11,
        EARTH2 = 2,
        GREEN_FIELD = 7
    };
    // �[�x����
    enum DEPTH_EFF
    {
        NONE = 0,
        LINEAR = 1,
        SHADOW1 = 2,
        SHADOW2 = 4
    };
    // �w�i�F
    enum BK_COLOR
    {
        WHITE = 1,
        BLACK = 0
    };
    enum RADIUS
    {
        TINY = 20,
        SMALL = 30,
        MIDDLE = 60,
        LARGE = 100
    };
    // ����
    enum FOV
    {
        NARROW = 70,
        NORMAL = 90,
        WIDE = 110
    };
    // �`��^�C�v
    enum DRAW_TYPE
    {
        POINT = 0,
        LINE = 1,
        SURFACE = 2
    };

    //typedef void (engine3dOSX::*menuFunc)();

    class GuiStringEx : public GuiString
    {
    public:
        MENU_ACT act;

        GuiStringEx();
        GuiStringEx(std::string, MENU_ACT);
    };

    void MakeMenu();
    uint64_t makeSubMenu(MENU, GuiContainer*, GuiStringEx*, int);
    void disposeMenu(GuiContainer*);


    // ���j���[���\�b�h
    void Init();
    void Dispose();
    bool MenuMsgProc(MENU_ACT);
    bool InputProc(INPUT);

    void MoveToOtherMenu(MENU);
    void ChangeThrowObject(THROW_OBJ);
    void MakeBasicObjectInvisible();
    void ChangeBasicObject(BASIC_OBJ, DRAW_TYPE, bool sun = false);
    void RelocateThrowObjectRandomly();
    void RelocateThrowObjectRandomly2();
    void ClearThrowObject();
    void ChangeDepthEffect(DEPTH_EFF);
    void ChangeBkColor(BK_COLOR);
    void ChangeWorldRadius(RADIUS);
    void ChangeFov(FOV);

    //---

};



