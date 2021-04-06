#pragma once
#include <vector>
#include "MenuLgc.h"

class engine3dGL;
class MenuLgcH3 : public MenuLgc
{
public:
    // �R���X�g���N�^
    MenuLgcH3();

    engine3dGL* owner;


    // ���j���[���
    // �����ڂ𑝌��������ꍇ��.cpp���ł��̍��ڂɂ��ď��������K�{
    enum class MENU_H3
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
        WORLD_TYPE,
        QUIT,
        len
    };
    // ���j���[�A�N�V����
    enum MENU_ACT_H3
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
        MOVE_TO_WORLD_TYPE_MENU,

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
        CHANGE_BASIC_OBJECT_TO_LINES_PT,
        CHANGE_BASIC_OBJECT_TO_LINES_LN,
        CHANGE_BASIC_OBJECT_TO_POLAR_PT,
        CHANGE_BASIC_OBJECT_TO_POLAR_LN,
        CHANGE_BASIC_OBJECT_TO_CELLS120,
        CHANGE_BASIC_OBJECT_TO_TORUS,
        CHANGE_BASIC_OBJECT_TO_EARTH,

        // �X���[�I�u�F�N�g�Ĕz�u
        RELOCATE_THROW_OBJECT_RANDOMLY,
        RELOCATE_THROW_OBJECT_RANDOMLY2,
        CLEAR_THROW_OBJECT,

        CHANGE_WORLD_TYPE_TO_HYPERBOLIC,
        CHANGE_WORLD_TYPE_TO_SPHERICAL

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
    // 
    enum WORLD_TYPE
    {
        HYPERBOLIC,
        SPHERICAL
    };


    virtual void MakeMenu() override;
    //void disposeMenu(GuiContainer*);

    // ���j���[���\�b�h
    virtual bool MenuMsgProc(MENU_ACT) override;

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
    void ChangeWorldType(WORLD_TYPE);

    //---
    virtual void setOwner(void*) override;

};



