#include "MenuLgcS3.h"
#include "engine3dGL.h"
using namespace std;
#define arraySizeof(ary) sizeof(ary)/sizeof(ary[0])


MenuLgcS3::MenuLgcS3()
{
    scale = 2;
}

void MenuLgcS3::MakeMenu()
{
    //== Gui Container作成
    // ルートメニュー
    menu.drawArea = GlRect(pt2(0.2, 0.9), 0.03, 0.03, GlRect::STD::HORIZONAL);
    menu.padding = GlRect(0.01, GlRect::STD::HORIZONAL);
    menu.bkColor = pt3(0.8, 0.8, 1.0);
    menu.alpha = 0.6;
    menu.displayed = false;
    menu.selectedIdx = (int)MENU_S3::MAIN;

    // メインコンテナ初期化
    menu.childs.resize((int)(MENU_S3::len));
    for (int i = 0; i < menu.childs.size(); i++)
        menu.childs[i] = nullptr;

    // ルートメニュー
    GuiStringEx mainItems[]
    {
        GuiStringEx("Object", (MENU_ACT)MENU_ACT_S3::MOVE_TO_OBJECT_MENU),
        GuiStringEx("View", (MENU_ACT)MENU_ACT_S3::MOVE_TO_VIEW_MENU),
        GuiStringEx("World type", (MENU_ACT)MENU_ACT_S3::MOVE_TO_WORLD_TYPE_MENU),
        GuiStringEx("Quit", (MENU_ACT)MENU_ACT_S3::MOVE_TO_QUIT_MENU)
    };
    makeSubMenu((MENU)MENU_S3::MAIN, &menu, mainItems, arraySizeof(mainItems));

    // 退出メニュー
    GuiStringEx quitItems[]
    {
        GuiStringEx("OK", (MENU_ACT)MENU_ACT_S3::QUIT_PROGRAM),
        GuiStringEx("Cancel", (MENU_ACT)MENU_ACT_S3::MOVE_TO_MAIN_MENU)
    };
    makeSubMenu((MENU)MENU_S3::QUIT, &menu, quitItems, arraySizeof(quitItems));

    // オブジェクトメニュー
    GuiStringEx objItems[]
    {
        GuiStringEx("Change basic-object", (MENU_ACT)MENU_ACT_S3::MOVE_TO_BASIC_OBCECT_MENU),
        GuiStringEx("Change throw-object", (MENU_ACT)MENU_ACT_S3::MOVE_TO_THROW_OBCECT_MENU),
        GuiStringEx("Relocate throw-object", (MENU_ACT)MENU_ACT_S3::MOVE_TO_RELOCATE_THROW_OBCECT_MENU),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_MAIN_MENU)
    };
    makeSubMenu((MENU)MENU_S3::OBJECT, &menu, objItems, arraySizeof(objItems));

    // スローオブジェクト変更メニュー
    GuiStringEx throwObjItems[]
    {
        GuiStringEx("Cube", (MENU_ACT)MENU_ACT_S3::CHANGE_THROW_OBJECT_TO_CUBE),
        GuiStringEx("Earth", (MENU_ACT)MENU_ACT_S3::CHANGE_THROW_OBJECT_TO_EARTH),
        GuiStringEx("Horse", (MENU_ACT)MENU_ACT_S3::CHANGE_THROW_OBJECT_TO_HORSE),
        GuiStringEx("Asteroid", (MENU_ACT)MENU_ACT_S3::CHANGE_THROW_OBJECT_TO_ASTEROID),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_OBJECT_MENU)
    };
    makeSubMenu((MENU)MENU_S3::CHANGE_THROW_OBJECT, &menu, throwObjItems, arraySizeof(throwObjItems));

    // 基底オブジェクト変更メニュー
    GuiStringEx basicObjItems[]
    {
        GuiStringEx("None", (MENU_ACT)MENU_ACT_S3::MAKE_BASIC_OBJECT_INVISILE),
        GuiStringEx("6 lines (point)", (MENU_ACT)MENU_ACT_S3::CHANGE_BASIC_OBJECT_TO_LINES_PT),
        GuiStringEx("6 lines (line)", (MENU_ACT)MENU_ACT_S3::CHANGE_BASIC_OBJECT_TO_LINES_LN),
        GuiStringEx("Polar (point)", (MENU_ACT)MENU_ACT_S3::CHANGE_BASIC_OBJECT_TO_POLAR_PT),
        GuiStringEx("Polar (line)", (MENU_ACT)MENU_ACT_S3::CHANGE_BASIC_OBJECT_TO_POLAR_LN),
        GuiStringEx("120-cells", (MENU_ACT)MENU_ACT_S3::CHANGE_BASIC_OBJECT_TO_CELLS120),
        GuiStringEx("Torus", (MENU_ACT)MENU_ACT_S3::CHANGE_BASIC_OBJECT_TO_TORUS),
        GuiStringEx("Earth", (MENU_ACT)MENU_ACT_S3::CHANGE_BASIC_OBJECT_TO_EARTH),
        GuiStringEx("Green Field", (MENU_ACT)MENU_ACT_S3::CHANGE_BASIC_OBJECT_TO_GREEN_FIELD),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_OBJECT_MENU)
    };
    makeSubMenu((MENU)MENU_S3::CHANGE_BASIC_OBJECT, &menu, basicObjItems, arraySizeof(basicObjItems));

    // スローオブジェクト再配置メニュー
    GuiStringEx relocateThrowItems[]
    {
        GuiStringEx("Random(uniform)", (MENU_ACT)MENU_ACT_S3::RELOCATE_THROW_OBJECT_RANDOMLY2),
        GuiStringEx("Random(cluster)", (MENU_ACT)MENU_ACT_S3::RELOCATE_THROW_OBJECT_RANDOMLY),
        GuiStringEx("Clear", (MENU_ACT)MENU_ACT_S3::CLEAR_THROW_OBJECT),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_OBJECT_MENU)
    };
    makeSubMenu((MENU)MENU_S3::RELOCATE_THROW_OBJECT, &menu, relocateThrowItems, arraySizeof(relocateThrowItems));

    // ビューメニュー
    GuiStringEx viewItems[]
    {
        GuiStringEx("Back color", (MENU_ACT)MENU_ACT_S3::MOVE_TO_BACK_COLOR_MENU),
        GuiStringEx("Space radius", (MENU_ACT)MENU_ACT_S3::MOVE_TO_WORLD_RADIUS_MENU),
        GuiStringEx("Field of view", (MENU_ACT)MENU_ACT_S3::MOVE_TO_FOV_MENU),
        GuiStringEx("Depth effect", (MENU_ACT)MENU_ACT_S3::MOVE_TO_DEPTH_EFFECT_MENU),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_MAIN_MENU)
    };
    makeSubMenu((MENU)MENU_S3::VIEW, &menu, viewItems, arraySizeof(viewItems));

    // 深度効果メニュー
    GuiStringEx depthEffectItems[]
    {
        GuiStringEx("None", (MENU_ACT)MENU_ACT_S3::DEPTH_EFFECT_NONE),
        GuiStringEx("Linear decay", (MENU_ACT)MENU_ACT_S3::DEPTH_EFFECT_LINEAR),
        GuiStringEx("Shadow 1 (light = POV)", (MENU_ACT)MENU_ACT_S3::DEPTH_EFFECT_SHADOW1),
        GuiStringEx("Shadow 2 (light = POV)", (MENU_ACT)MENU_ACT_S3::DEPTH_EFFECT_SHADOW2),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_VIEW_MENU)
    };
    makeSubMenu((MENU)MENU_S3::DEPTH_EFFECT, &menu, depthEffectItems, arraySizeof(depthEffectItems));

    // 背景色メニュー
    GuiStringEx backColorItems[]
    {
        GuiStringEx("White", (MENU_ACT)MENU_ACT_S3::BACK_COLOR_WHITE),
        GuiStringEx("Black", (MENU_ACT)MENU_ACT_S3::BACK_COLOR_BLACK),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_VIEW_MENU)
    };
    makeSubMenu((MENU)MENU_S3::BACK_COLOR, &menu, backColorItems, arraySizeof(backColorItems));

    // 世界半径メニュー
    GuiStringEx radiusItems[]
    {
        GuiStringEx("Tiny", (MENU_ACT)MENU_ACT_S3::MAKE_RADIUS_TINY),
        GuiStringEx("Small", (MENU_ACT)MENU_ACT_S3::MAKE_RADIUS_SMALL),
        GuiStringEx("Middle", (MENU_ACT)MENU_ACT_S3::MAKE_RADIUS_MIDDLE),
        GuiStringEx("Large", (MENU_ACT)MENU_ACT_S3::MAKE_RADIUS_LARGE),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_VIEW_MENU)
    };
    makeSubMenu((MENU)MENU_S3::WORLD_RADIUS, &menu, radiusItems, arraySizeof(radiusItems));

    // FOVメニュー
    GuiStringEx fovItems[]
    {
        //GuiStringEx("Narrow", (MENU_ACT)MENU_ACT_S3::MAKE_FOV_NARROW),
        //GuiStringEx("Normal", (MENU_ACT)MENU_ACT_S3::MAKE_FOV_NORMAL),
        //GuiStringEx("Wide", (MENU_ACT)MENU_ACT_S3::MAKE_FOV_WIDE),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_VIEW_MENU)
    };
    makeSubMenu((MENU)MENU_S3::FIELD_OF_VIEW, &menu, fovItems, arraySizeof(fovItems));

    // 世界メニュー
    GuiStringEx worldGeoItems[]
    {
        GuiStringEx("Hyperbolic", (MENU_ACT)MENU_ACT_S3::CHANGE_WORLD_TYPE_TO_HYPERBOLIC),
        GuiStringEx("Spherical", (MENU_ACT)MENU_ACT_S3::CHANGE_WORLD_TYPE_TO_SPHERICAL),
        GuiStringEx("<< Back", (MENU_ACT)MENU_ACT_S3::MOVE_TO_MAIN_MENU)
    };
    makeSubMenu((MENU)MENU_S3::WORLD_TYPE, &menu, worldGeoItems, arraySizeof(worldGeoItems));

}



//== メニューメソッド

// メニュープロシージャ
bool MenuLgcS3::MenuMsgProc(MENU_ACT act)
{
    if (!owner)
        return false;

    switch (act)
    {
        // メニュー変更
    case MOVE_TO_MAIN_MENU:
        MoveToOtherMenu((MENU)MENU_S3::MAIN);
        break;
    case MOVE_TO_QUIT_MENU:
        MoveToOtherMenu((MENU)MENU_S3::QUIT);
        break;
    case MOVE_TO_OBJECT_MENU:
        MoveToOtherMenu((MENU)MENU_S3::OBJECT);
        break;
    case MOVE_TO_THROW_OBCECT_MENU:
        MoveToOtherMenu((MENU)MENU_S3::CHANGE_THROW_OBJECT);
        break;
    case MOVE_TO_BASIC_OBCECT_MENU:
        MoveToOtherMenu((MENU)MENU_S3::CHANGE_BASIC_OBJECT);
        break;
    case MOVE_TO_RELOCATE_THROW_OBCECT_MENU:
        MoveToOtherMenu((MENU)MENU_S3::RELOCATE_THROW_OBJECT);
        break;
    case MOVE_TO_VIEW_MENU:
        MoveToOtherMenu((MENU)MENU_S3::VIEW);
        break;
    case MOVE_TO_DEPTH_EFFECT_MENU:
        MoveToOtherMenu((MENU)MENU_S3::DEPTH_EFFECT);
        break;
    case MOVE_TO_BACK_COLOR_MENU:
        MoveToOtherMenu((MENU)MENU_S3::BACK_COLOR);
        break;
    case MOVE_TO_WORLD_RADIUS_MENU:
        MoveToOtherMenu((MENU)MENU_S3::WORLD_RADIUS);
        break;
    case MOVE_TO_FOV_MENU:
        MoveToOtherMenu((MENU)MENU_S3::FIELD_OF_VIEW);
        break;
    case MOVE_TO_WORLD_TYPE_MENU:
        MoveToOtherMenu((MENU)MENU_S3::WORLD_TYPE);
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

    case CHANGE_WORLD_TYPE_TO_HYPERBOLIC:
        ChangeWorldType(WORLD_TYPE::HYPERBOLIC);
        return false;
    case CHANGE_WORLD_TYPE_TO_SPHERICAL:
        ChangeWorldType(WORLD_TYPE::SPHERICAL);
        return false;

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




// 世界半径
void MenuLgcS3::ChangeWorldRadius(RADIUS rad)
{
    owner->radius = (int)rad;
}


// 背景色
void MenuLgcS3::ChangeBkColor(BK_COLOR color)
{
    owner->bgCol = (int)color;
}


// 深度効果
void MenuLgcS3::ChangeDepthEffect(DEPTH_EFF type)
{
    owner->decMode = (int)type;
}

// スローオブジェクト変更
void MenuLgcS3::ChangeThrowObject(THROW_OBJ obj)
{
    owner->ChangeThrowObject((int)obj);
}

// 基底オブジェクト変更
void MenuLgcS3::ChangeBasicObject(BASIC_OBJ obj, DRAW_TYPE type, bool sun)
{
    owner->ChangeBasicObject(obj, type, sun);
}

void MenuLgcS3::MakeBasicObjectInvisible()
{
    owner->ChangeBasicObject(-1, NULL);
}

void MenuLgcS3::RelocateThrowObjectRandomly()
{
    owner->player.ep = 0;
    owner->RandLoc(engine3d::RandMode::Cluster);
}

void MenuLgcS3::RelocateThrowObjectRandomly2()
{
    owner->player.ep = 0;
    owner->RandLoc(engine3d::RandMode::Uniform);
}

void MenuLgcS3::ClearThrowObject()
{
    owner->ClearFloatObjs();
}

void MenuLgcS3::ChangeWorldType(WORLD_TYPE type)
{
    if(type == WORLD_TYPE::HYPERBOLIC)
        owner->CreateNewEngine(" sim:H3;lang:EN;view:VR");
    else
        owner->CreateNewEngine(" sim:S3;lang:EN;view:VR");
}

//// 視野
//void MenuLgc::ChangeFov(FOV fov)
//{
//    CR_RANGE_X = (double)fov;
//    CR_RANGE_Y = clcRangeY((double)fov);
//}

void MenuLgcS3::setOwner(void* owner)
{
    this->owner = (engine3dGL*)owner;
}
