#pragma once

void menuCheckDef(HMENU hMenu, MENUITEMINFO* miInfo);
void menuCheck(HMENU hMenu, MENUITEMINFO* miInfo, int ckId, int* unCks, int len);
void menuCheck2(HMENU hMenu, MENUITEMINFO* miInfo, int ckId);

void changeLang(HMENU hMenu, MENUITEMINFO* miInfo, int lang);
void changeLangItem(HMENU hMenu, MENUITEMINFO* miInfo, char* itemName, int menuID);