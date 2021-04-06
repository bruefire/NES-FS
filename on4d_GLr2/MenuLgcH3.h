#pragma once
#include <vector>
#include "MenuLgc.h"

class engine3dGL;
class MenuLgcH3 : public MenuLgc
{
public:
    // コンストラクタ
    MenuLgcH3();

    engine3dGL* owner;


    // メニュー種類
    // ※項目を増減させた場合は.cpp側でその項目について初期化も必須
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
    // メニューアクション
    enum MENU_ACT_H3
    {
        // メニュー遷移
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

        // 退出
        QUIT_PROGRAM,

        // 視野
        MAKE_FOV_NARROW,
        MAKE_FOV_NORMAL,
        MAKE_FOV_WIDE,

        // 半径
        MAKE_RADIUS_TINY,
        MAKE_RADIUS_SMALL,
        MAKE_RADIUS_MIDDLE,
        MAKE_RADIUS_LARGE,

        // 背景色
        BACK_COLOR_WHITE,
        BACK_COLOR_BLACK,

        // 深度効果
        DEPTH_EFFECT_NONE,
        DEPTH_EFFECT_LINEAR,
        DEPTH_EFFECT_SHADOW1,
        DEPTH_EFFECT_SHADOW2,

        // スローオブジェクト変更
        CHANGE_THROW_OBJECT_TO_CUBE,
        CHANGE_THROW_OBJECT_TO_EARTH,
        CHANGE_THROW_OBJECT_TO_HORSE,
        CHANGE_THROW_OBJECT_TO_ASTEROID,

        // 基底オブジェクト変更
        MAKE_BASIC_OBJECT_INVISILE,
        CHANGE_BASIC_OBJECT_TO_LINES_PT,
        CHANGE_BASIC_OBJECT_TO_LINES_LN,
        CHANGE_BASIC_OBJECT_TO_POLAR_PT,
        CHANGE_BASIC_OBJECT_TO_POLAR_LN,
        CHANGE_BASIC_OBJECT_TO_CELLS120,
        CHANGE_BASIC_OBJECT_TO_TORUS,
        CHANGE_BASIC_OBJECT_TO_EARTH,

        // スローオブェクト再配置
        RELOCATE_THROW_OBJECT_RANDOMLY,
        RELOCATE_THROW_OBJECT_RANDOMLY2,
        CLEAR_THROW_OBJECT,

        CHANGE_WORLD_TYPE_TO_HYPERBOLIC,
        CHANGE_WORLD_TYPE_TO_SPHERICAL

    };
    // スローオブジェクト
    enum THROW_OBJ
    {
        CUBE = 4,
        EARTH = 8,
        HORSE = 5,
        ASTEROID = 14
    };
    // 基底オブジェクト
    enum BASIC_OBJ
    {
        LINES = 0,
        POLAR = 3,
        TORUS = 13,
        CELLS120 = 11,
        EARTH2 = 2,
        GREEN_FIELD = 7
    };
    // 深度効果
    enum DEPTH_EFF
    {
        NONE = 0,
        LINEAR = 1,
        SHADOW1 = 2,
        SHADOW2 = 4
    };
    // 背景色
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
    // 視野
    enum FOV
    {
        NARROW = 70,
        NORMAL = 90,
        WIDE = 110
    };
    // 描画タイプ
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

    // メニューメソッド
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



