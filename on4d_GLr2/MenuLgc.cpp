#include <string>
#include <exception>
#include <glew.h>
#include <GL/gl.h>
#include "MenuLgc.h"
#include "engine3dGL.h"
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


void MenuLgc::MakeMenu()
{
    //== Gui Container作成
    // ルートメニュー
    menu.drawArea = GlRect(pt2(0.2, 0.9), 0.03, 0.03, GlRect::STD::HORIZONAL);
    menu.padding = GlRect(0.01, GlRect::STD::HORIZONAL);
    menu.bkColor = pt3(0.8, 0.8, 1.0);
    menu.alpha = 0.6;
    menu.displayed = false;
    menu.selectedIdx = (int)MENU::MAIN;

    // メインコンテナ初期化
    menu.childs.resize((int)(MENU::len));
    for (int i = 0; i < menu.childs.size(); i++)
        menu.childs[i] = nullptr;

    // ルートメニュー
    GuiStringEx mainItems[]
    {
        GuiStringEx("Object", MENU_ACT::MOVE_TO_OBJECT_MENU),
        GuiStringEx("View", MENU_ACT::MOVE_TO_VIEW_MENU),
        GuiStringEx("Quit", MENU_ACT::MOVE_TO_QUIT_MENU)
    };
    makeSubMenu(MENU::MAIN, &menu, mainItems, arraySizeof(mainItems));

    // 退出メニュー
    GuiStringEx quitItems[]
    {
        GuiStringEx("OK", MENU_ACT::QUIT_PROGRAM),
        GuiStringEx("Cancel", MENU_ACT::MOVE_TO_MAIN_MENU)
    };
    makeSubMenu(MENU::QUIT, &menu, quitItems, arraySizeof(quitItems));

    // オブジェクトメニュー
    GuiStringEx objItems[]
    {
        GuiStringEx("Change basic-object", MENU_ACT::MOVE_TO_BASIC_OBCECT_MENU),
        GuiStringEx("Change throw-object", MENU_ACT::MOVE_TO_THROW_OBCECT_MENU),
        GuiStringEx("Relocate throw-object", MENU_ACT::MOVE_TO_RELOCATE_THROW_OBCECT_MENU),
        GuiStringEx("<< Back", MENU_ACT::MOVE_TO_MAIN_MENU)
    };
    makeSubMenu(MENU::OBJECT, &menu, objItems, arraySizeof(objItems));

    // スローオブジェクト変更メニュー
    GuiStringEx throwObjItems[]
    {
        GuiStringEx("Cube", MENU_ACT::CHANGE_THROW_OBJECT_TO_CUBE),
        GuiStringEx("Earth", MENU_ACT::CHANGE_THROW_OBJECT_TO_EARTH),
        GuiStringEx("Horse", MENU_ACT::CHANGE_THROW_OBJECT_TO_HORSE),
        GuiStringEx("Asteroid", MENU_ACT::CHANGE_THROW_OBJECT_TO_ASTEROID),
        GuiStringEx("<< Back", MENU_ACT::MOVE_TO_OBJECT_MENU)
    };
    makeSubMenu(MENU::CHANGE_THROW_OBJECT, &menu, throwObjItems, arraySizeof(throwObjItems));

    // 基底オブジェクト変更メニュー
    GuiStringEx basicObjItems[]
    {
        GuiStringEx("None", MENU_ACT::MAKE_BASIC_OBJECT_INVISILE),
        GuiStringEx("6 lines (point)", MENU_ACT::CHANGE_BASIC_OBJECT_TO_LINES_PT),
        GuiStringEx("6 lines (line)", MENU_ACT::CHANGE_BASIC_OBJECT_TO_LINES_LN),
        GuiStringEx("Polar (point)", MENU_ACT::CHANGE_BASIC_OBJECT_TO_POLAR_PT),
        GuiStringEx("Polar (line)", MENU_ACT::CHANGE_BASIC_OBJECT_TO_POLAR_LN),
        GuiStringEx("120-cells", MENU_ACT::CHANGE_BASIC_OBJECT_TO_CELLS120),
        GuiStringEx("Torus", MENU_ACT::CHANGE_BASIC_OBJECT_TO_TORUS),
        GuiStringEx("Earth", MENU_ACT::CHANGE_BASIC_OBJECT_TO_EARTH),
        GuiStringEx("Green Field", MENU_ACT::CHANGE_BASIC_OBJECT_TO_GREEN_FIELD),
        GuiStringEx("<< Back", MENU_ACT::MOVE_TO_OBJECT_MENU)
    };
    makeSubMenu(MENU::CHANGE_BASIC_OBJECT, &menu, basicObjItems, arraySizeof(basicObjItems));

    // スローオブジェクト再配置メニュー
    GuiStringEx relocateThrowItems[]
    {
        GuiStringEx("Random(uniform)", MENU_ACT::RELOCATE_THROW_OBJECT_RANDOMLY2),
        GuiStringEx("Random(cluster)", MENU_ACT::RELOCATE_THROW_OBJECT_RANDOMLY),
        GuiStringEx("Clear", MENU_ACT::CLEAR_THROW_OBJECT),
        GuiStringEx("<< Back", MENU_ACT::MOVE_TO_OBJECT_MENU)
    };
    makeSubMenu(MENU::RELOCATE_THROW_OBJECT, &menu, relocateThrowItems, arraySizeof(relocateThrowItems));

    // ビューメニュー
    GuiStringEx viewItems[]
    {
        GuiStringEx("Back color", MENU_ACT::MOVE_TO_BACK_COLOR_MENU),
        GuiStringEx("Space radius", MENU_ACT::MOVE_TO_WORLD_RADIUS_MENU),
        GuiStringEx("Field of view", MENU_ACT::MOVE_TO_FOV_MENU),
        GuiStringEx("Depth effect", MENU_ACT::MOVE_TO_DEPTH_EFFECT_MENU),
        GuiStringEx("<< Back", MENU_ACT::MOVE_TO_MAIN_MENU)
    };
    makeSubMenu(MENU::VIEW, &menu, viewItems, arraySizeof(viewItems));

    // 深度効果メニュー
    GuiStringEx depthEffectItems[]
    {
        GuiStringEx("None", MENU_ACT::DEPTH_EFFECT_NONE),
        GuiStringEx("Linear decay", MENU_ACT::DEPTH_EFFECT_LINEAR),
        GuiStringEx("Shadow 1 (light = POV)", MENU_ACT::DEPTH_EFFECT_SHADOW1),
        GuiStringEx("Shadow 2 (light = POV)", MENU_ACT::DEPTH_EFFECT_SHADOW2),
        GuiStringEx("<< Back", MENU_ACT::MOVE_TO_VIEW_MENU)
    };
    makeSubMenu(MENU::DEPTH_EFFECT, &menu, depthEffectItems, arraySizeof(depthEffectItems));

    // 背景色メニュー
    GuiStringEx backColorItems[]
    {
        GuiStringEx("White", MENU_ACT::BACK_COLOR_WHITE),
        GuiStringEx("Black", MENU_ACT::BACK_COLOR_BLACK),
        GuiStringEx("<< Back", MENU_ACT::MOVE_TO_VIEW_MENU)
    };
    makeSubMenu(MENU::BACK_COLOR, &menu, backColorItems, arraySizeof(backColorItems));

    // 世界半径メニュー
    GuiStringEx radiusItems[]
    {
        GuiStringEx("Tiny", MENU_ACT::MAKE_RADIUS_TINY),
        GuiStringEx("Small", MENU_ACT::MAKE_RADIUS_SMALL),
        GuiStringEx("Middle", MENU_ACT::MAKE_RADIUS_MIDDLE),
        GuiStringEx("Large", MENU_ACT::MAKE_RADIUS_LARGE),
        GuiStringEx("<< Back", MENU_ACT::MOVE_TO_VIEW_MENU)
    };
    makeSubMenu(MENU::WORLD_RADIUS, &menu, radiusItems, arraySizeof(radiusItems));

    // FOVメニュー
    GuiStringEx fovItems[]
    {
        GuiStringEx("Narrow", MENU_ACT::MAKE_FOV_NARROW),
        GuiStringEx("Normal", MENU_ACT::MAKE_FOV_NORMAL),
        GuiStringEx("Wide", MENU_ACT::MAKE_FOV_WIDE),
        GuiStringEx("<< Back", MENU_ACT::MOVE_TO_VIEW_MENU)
    };
    makeSubMenu(MENU::FIELD_OF_VIEW, &menu, fovItems, arraySizeof(fovItems));

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


//== メニューメソッド

// メニュープロシージャ
bool MenuLgc::MenuMsgProc(MENU_ACT act)
{
    if (!owner)
        return false;

    switch (act)
    {
        // メニュー変更
    case MOVE_TO_MAIN_MENU:
        MoveToOtherMenu(MENU::MAIN);
        break;
    case MOVE_TO_QUIT_MENU:
        MoveToOtherMenu(MENU::QUIT);
        break;
    case MOVE_TO_OBJECT_MENU:
        MoveToOtherMenu(MENU::OBJECT);
        break;
    case MOVE_TO_THROW_OBCECT_MENU:
        MoveToOtherMenu(MENU::CHANGE_THROW_OBJECT);
        break;
    case MOVE_TO_BASIC_OBCECT_MENU:
        MoveToOtherMenu(MENU::CHANGE_BASIC_OBJECT);
        break;
    case MOVE_TO_RELOCATE_THROW_OBCECT_MENU:
        MoveToOtherMenu(MENU::RELOCATE_THROW_OBJECT);
        break;
    case MOVE_TO_VIEW_MENU:
        MoveToOtherMenu(MENU::VIEW);
        break;
    case MOVE_TO_DEPTH_EFFECT_MENU:
        MoveToOtherMenu(MENU::DEPTH_EFFECT);
        break;
    case MOVE_TO_BACK_COLOR_MENU:
        MoveToOtherMenu(MENU::BACK_COLOR);
        break;
    case MOVE_TO_WORLD_RADIUS_MENU:
        MoveToOtherMenu(MENU::WORLD_RADIUS);
        break;
    case MOVE_TO_FOV_MENU:
        MoveToOtherMenu(MENU::FIELD_OF_VIEW);
        break;

        // 退出
    case QUIT_PROGRAM:
        return false;

    //    // 視野
    //case MAKE_FOV_NARROW:
    //    ChangeFov(FOV::NARROW);
    //    break;
    //case MAKE_FOV_NORMAL:
    //    ChangeFov(FOV::NORMAL);
    //    break;
    //case MAKE_FOV_WIDE:
    //    ChangeFov(FOV::WIDE);
    //    break;

        // 半径
    case MAKE_RADIUS_TINY:
        ChangeWorldRadius(RADIUS::TINY);
        break;
    case MAKE_RADIUS_SMALL:
        ChangeWorldRadius(RADIUS::SMALL);
        break;
    case MAKE_RADIUS_MIDDLE:
        ChangeWorldRadius(RADIUS::MIDDLE);
        break;
    case MAKE_RADIUS_LARGE:
        ChangeWorldRadius(RADIUS::LARGE);
        break;

        // 背景色
    case BACK_COLOR_WHITE:
        ChangeBkColor(BK_COLOR::WHITE);
        break;
    case BACK_COLOR_BLACK:
        ChangeBkColor(BK_COLOR::BLACK);
        break;

        // 深度効果変更
    case DEPTH_EFFECT_NONE:
        ChangeDepthEffect(DEPTH_EFF::NONE);
        break;
    case DEPTH_EFFECT_LINEAR:
        ChangeDepthEffect(DEPTH_EFF::LINEAR);
        break;
    case DEPTH_EFFECT_SHADOW1:
        ChangeDepthEffect(DEPTH_EFF::SHADOW1);
        break;
    case DEPTH_EFFECT_SHADOW2:
        ChangeDepthEffect(DEPTH_EFF::SHADOW2);
        break;

        // スローオブジェクト変更
    case CHANGE_THROW_OBJECT_TO_EARTH:
        ChangeThrowObject(THROW_OBJ::EARTH);
        break;
    case CHANGE_THROW_OBJECT_TO_CUBE:
        ChangeThrowObject(THROW_OBJ::CUBE);
        break;
    case CHANGE_THROW_OBJECT_TO_ASTEROID:
        ChangeThrowObject(THROW_OBJ::ASTEROID);
        break;
    case CHANGE_THROW_OBJECT_TO_HORSE:
        ChangeThrowObject(THROW_OBJ::HORSE);
        break;

        // 基底オブジェクト変更
    case MAKE_BASIC_OBJECT_INVISILE:
        MakeBasicObjectInvisible();
        break;
    case CHANGE_BASIC_OBJECT_TO_LINES_PT:
        ChangeBasicObject(BASIC_OBJ::LINES, DRAW_TYPE::POINT);
        break;
    case CHANGE_BASIC_OBJECT_TO_LINES_LN:
        ChangeBasicObject(BASIC_OBJ::LINES, DRAW_TYPE::LINE);
        break;
    case CHANGE_BASIC_OBJECT_TO_POLAR_PT:
        ChangeBasicObject(BASIC_OBJ::POLAR, DRAW_TYPE::POINT);
        break;
    case CHANGE_BASIC_OBJECT_TO_POLAR_LN:
        ChangeBasicObject(BASIC_OBJ::POLAR, DRAW_TYPE::LINE);
        break;
    case CHANGE_BASIC_OBJECT_TO_CELLS120:
        ChangeBasicObject(BASIC_OBJ::CELLS120, DRAW_TYPE::SURFACE);
        break;
    case CHANGE_BASIC_OBJECT_TO_TORUS:
        ChangeBasicObject(BASIC_OBJ::TORUS, DRAW_TYPE::SURFACE);
        break;
    case CHANGE_BASIC_OBJECT_TO_EARTH:
        ChangeBasicObject(BASIC_OBJ::EARTH2, DRAW_TYPE::SURFACE);
        break;
    case CHANGE_BASIC_OBJECT_TO_GREEN_FIELD:
        ChangeBasicObject(BASIC_OBJ::GREEN_FIELD, DRAW_TYPE::SURFACE, true);
        break;

        // スローオブジェクト再配置
    case RELOCATE_THROW_OBJECT_RANDOMLY:
        RelocateThrowObjectRandomly();
        break;
    case RELOCATE_THROW_OBJECT_RANDOMLY2:
        RelocateThrowObjectRandomly2();
        break;
    case CLEAR_THROW_OBJECT:
        ClearThrowObject();
        break;

    }

    return true;
}



// 別のメニューへ移動
void MenuLgc::MoveToOtherMenu(MENU menuKey)
{
    menu.selectedIdx = (int)menuKey;
}

//// 視野
//void MenuLgc::ChangeFov(FOV fov)
//{
//    CR_RANGE_X = (double)fov;
//    CR_RANGE_Y = clcRangeY((double)fov);
//}


// 世界半径
void MenuLgc::ChangeWorldRadius(RADIUS rad)
{
    owner->radius = (int)rad;
}


// 背景色
void MenuLgc::ChangeBkColor(BK_COLOR color)
{
    owner->bgCol = (int)color;
}


// 深度効果
void MenuLgc::ChangeDepthEffect(DEPTH_EFF type)
{
    owner->decMode = (int)type;
}

// スローオブジェクト変更
void MenuLgc::ChangeThrowObject(THROW_OBJ obj)
{
    owner->ChangeThrowObject((int)obj);
}

// 基底オブジェクト変更
void MenuLgc::ChangeBasicObject(BASIC_OBJ obj, DRAW_TYPE type, bool sun)
{
    owner->ChangeBasicObject(obj, type, sun);
}

void MenuLgc::MakeBasicObjectInvisible()
{
    owner->ChangeBasicObject(-1, NULL);
}

void MenuLgc::RelocateThrowObjectRandomly()
{
    owner->player.ep = 0;
    owner->RandLoc(engine3d::RandMode::Cluster);
}

void MenuLgc::RelocateThrowObjectRandomly2()
{
    owner->player.ep = 0;
    owner->RandLoc(engine3d::RandMode::Uniform);
}

void MenuLgc::ClearThrowObject()
{
    owner->ClearFloatObjs();
}

