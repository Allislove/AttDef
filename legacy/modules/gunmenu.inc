#if !defined _attdef_header_incl
	#error "Module's header file include is not allowed here, include it in the correct section of the 'legacy/modules.inc' file."
#endif

#if defined _attdef_mod_gunmenu
	#endinput
#endif
#define _attdef_mod_gunmenu

/*
	Guns selection script
*/

#include "modules/this_core.inc"

#define MAX_GUNMENU_GUNS						9

#define GUNMENU_OBJECTS_CIRCLE_RADIUS			1.9 // The gun objects are shown in a circular shape, this is the radius of the circle
#define GUNMENU_OBJECTS_CIRCLE_RADIUS_INVEHICLE 4.5 // Same GUNMENU_OBJECTS_CIRCLE_RADIUS but for while in vehicles
#define GUNMENU_OBJECT_Z_CHANGES				0.4 // The gun objects keep moving up and down repeatedly, this is the range
#define GUNMENU_OBJECT_Z_MOVE_SPEED				(GUNMENU_OBJECT_Z_CHANGES / 3.0)

#define GUNMENU_LIMIT_TEXTDRAW_SIZE				(MAX_GUNMENU_GUNS * 50) // Each gun needs 50 characters in the textdraw
#define GUNMENU_LIMIT_DIALOG_SIZE				(70 * MAX_GUNMENU_GUNS) // Each gun needs 70 characters

// Gunmenu styles
#define GUNMENU_STYLE_DIALOG					0
#define GUNMENU_STYLE_OBJECT					1

// Gunmenu objects movement routes
#define GUN_MOVING_ROUTE_UP						0
#define GUN_MOVING_ROUTE_DOWN					1
#define GUN_MOVING_ROUTE_TOPLAYER				2

// Gunmenu modification types
#define GUNMENU_MOD_TYPE_WEAPON					0
#define GUNMENU_MOD_TYPE_LIMIT					1
#define GUNMENU_MOD_TYPE_AMMO					2

enum GunmenuData_STRUCT {
	GunModel,						   // model id
	GunID,							   // weapon id
	GunPlayerObject[MAX_PLAYERS],	   // player object id
	GunFlamePlayerObject[MAX_PLAYERS], // player flame object
	TimesSelected[MAX_TEAMS],		   // how many times it is selected for each team
	bool: HasGun[MAX_PLAYERS],		   // whether a player has this gun or not
	GunMovingRoute[MAX_PLAYERS],	   // whether it's going down or up for each player
	GunLimit,						   // how many times it can be selected (for each team)
	GunAmmo,						   // ammo when given
}

new GunmenuData[MAX_GUNMENU_GUNS][GunmenuData_STRUCT];
new Iterator: GMFrozenCars<MAX_VEHICLES>; // Temporarily contains IDs of vehicles when in gunmenu
new GMFrozenCarTimers[MAX_VEHICLES];	  // Contains IDs of timers for GMFrozenCars

new Iterator: PlayerGunObjects[MAX_PLAYERS]<MAX_PLAYERS>;

new bool: GunmenuRestrictions = true;
new MeleeWeaponsArray_ID[6]	  = { 1, 8, 10, 16, 17, 46 };
new MeleeWeaponsArray_AMMO[6] = { 1, 1, 1, 3, 3, 1 };
