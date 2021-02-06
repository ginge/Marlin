#include "dwin_base.h"
#include <stdio.h>
#include <string.h>
#include "../../MarlinCore.h"
#include "../../module/temperature.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../module/settings.h"
#include "../../module/printcounter.h"
#include "../../sd/cardreader.h"

void _boot_window_init(dwin_window *window, void *context);
void _boot_window_draw(dwin_window *window, void *context);

//main
void _main_window_draw(dwin_window *window, void *context);
void _main_print_pressed(dwin_ui_element *element, void *context);
void _main_temp_pressed(dwin_ui_element *element, void *context);
void _main_settings_pressed(dwin_ui_element *element, void *context);

// file window
void _file_window_draw(dwin_window *window, void *context);
void _file_start_print_pressed(dwin_ui_element *element, void *context);
void _file_print_file_pressed(dwin_ui_element *element, void *context);
void _file_back_pressed(dwin_ui_element *element, void *context);

//about
void _about_window_init(dwin_window *window, void *context);

//settings
void _settings_init(dwin_window *window, void *context);
void _settings_levelling_pressed(dwin_ui_element *element, void *context);
void _settings_refuel_ressed(dwin_ui_element *element, void *context);
void _settings_move_pressed(dwin_ui_element *element, void *context);
void _settings_language_pressed(dwin_ui_element *element, void *context);
void _settings_info_pressed(dwin_ui_element *element, void *context);
void _settings_disable_motor_pressed(dwin_ui_element *element, void *context);
void _settings_back_pressed(dwin_ui_element *element, void *context);
void _settings_volume_set_pressed(dwin_ui_element *element, void *context);

// levelling
void _level_init(dwin_window *window, void *context);
void _level_home_pressed(dwin_ui_element *element, void *context);
void _level_z_plus_pressed(dwin_ui_element *element, void *context);
void _level_z_minus_pressed(dwin_ui_element *element, void *context);
void _level_aux_pressed(dwin_ui_element *element, void *context);
void _level_measuring_pressed(dwin_ui_element *element, void *context);
void _level_auto_switch_pressed(dwin_ui_element *element, void *context);
void _level_back(dwin_ui_element *element, void *context);
//void _level_draw(dwin_window *window, void *context);

void _autolevel_init(dwin_window *window, void *context);

// refuel
void _nav_to_refuel();
void _refuel_draw(dwin_window *window, void *context);
void _refuel_init(dwin_window *window, void *context);
void _refuel_back(dwin_ui_element *element, void *context);
void _refuel_ret(dwin_ui_element *element, void *context);
void _refuel_feed(dwin_ui_element *element, void *context);
void _refuel_heatup(dwin_ui_element *element, void *context);
void _refuel_cancel(dwin_ui_element *element, void *context);
void _refuel_set_e_mm(dwin_ui_element *element, void *context);

// move
static void _move_n(AxisEnum axis, uint16_t value, float min, float max);
void _move_init(dwin_window *window, void *context);
void _move_back(dwin_ui_element *element, void *context);
void _move_x(dwin_ui_element *element, void *context);
void _move_y(dwin_ui_element *element, void *context);
void _move_z(dwin_ui_element *element, void *context);
void _move_x_set(dwin_ui_element *element, void *context);
void _move_y_set(dwin_ui_element *element, void *context);
void _move_z_set(dwin_ui_element *element, void *context);
void _move_one_mm(dwin_ui_element *element, void *context);
void _move_home(dwin_ui_element *element, void *context);

// aux move
void _aux_move_back(dwin_ui_element *element, void *context);
void _aux_move_1(dwin_ui_element *element, void *context);
void _aux_move_2(dwin_ui_element *element, void *context);
void _aux_move_3(dwin_ui_element *element, void *context);
void _aux_move_4(dwin_ui_element *element, void *context);
void _aux_move_5(dwin_ui_element *element, void *context);

// lang
void _lang_select(dwin_ui_element *element, void *context);
void _lang_back(dwin_ui_element *element, void *context);

// homing
void _homing_draw(dwin_window *window, void *context);

// pla abs
void _plaabs_pla(dwin_ui_element *element, void *context);
void _plaabs_abs(dwin_ui_element *element, void *context);
void _plaabs_cancel(dwin_ui_element *element, void *context);
void _plaabs_draw(dwin_window *window, void *context);

// temp menu
void _menu_temp_draw(dwin_window *window, void *context);
void _nav_to_temp_menu(bool dont_nav);
void _menu_temp_back(dwin_ui_element *element, void *context);
void _menu_temp_auto(dwin_ui_element *element, void *context);
void _menu_temp_manual(dwin_ui_element *element, void *context);
void _menu_temp_cool(dwin_ui_element *element, void *context);
void _menu_temp_fan(dwin_ui_element *element, void *context);
void _menu_temp_cool_no(dwin_ui_element *element, void *context);
void _menu_temp_cool_yes(dwin_ui_element *element, void *context);

// temp screen
void _ntemp_back(dwin_ui_element *element, void *context);
void _ntemp_cool1(dwin_ui_element *element, void *context);
void _ntemp_cool2(dwin_ui_element *element, void *context);
void _ntemp_set_end(dwin_ui_element *element, void *context);
void _ntemp_set_bed(dwin_ui_element *element, void *context);

// print
void _nav_to_print_menu();
void _print_init(dwin_window *window, void *context);
void _print_draw(dwin_window *window, void *context);
void _print_stop_no(dwin_ui_element *element, void *context);
void _print_stop_yes(dwin_ui_element *element, void *context);
void _print_pause_yes(dwin_ui_element *element, void *context);
void _print_pause_no(dwin_ui_element *element, void *context);
void _print_resume(dwin_ui_element *element, void *context);
void _print_adjust(dwin_ui_element *element, void *context);

// adjust
void _adjust_init(dwin_window *window, void *context);
void _adjust_noz_temp(dwin_ui_element *element, void *context);
void _adjust_bed_temp(dwin_ui_element *element, void *context);
void _adjust_speed(dwin_ui_element *element, void *context);
void _adjust_back(dwin_ui_element *element, void *context);
void _adjust_fan(dwin_ui_element *element, void *context);
void _adjust_z(dwin_ui_element *element, void *context);

void _nofil_yes(dwin_ui_element *element, void *context);
void _nofil_no(dwin_ui_element *element, void *context);

#define ARRSIZE(X) (sizeof( X ) / sizeof(dwin_ui_element))

static const dwin_ui_element ui_elements_ender_6_boot[] = {
    { StartIcon, 0, ElementInput, "progress1", NULL, NULL },
    { StartIcon1, 0, ElementInput, "progress2", NULL, NULL },
};

static const dwin_ui_element ui_elements_ender_6_main[] = {
    { NozzlePreheat, 0, ElementInput, "", NULL, NULL },
    { NozzleTemp, 0, ElementInput, "", NULL, NULL },
    { BedPreheat, 0, ElementInput, "", NULL, NULL },
    { Bedtemp, 0, ElementInput, "", NULL, NULL },
    // buttons
    { 0x1002, 1, ElementButton, "", _main_print_pressed, NULL },
    { 0x1002, 3, ElementButton, "", _main_temp_pressed, NULL },
    { 0x1002, 4, ElementButton, "", _main_settings_pressed, NULL },
};

static const dwin_ui_element ui_elements_ender_6_file[] = {
    { 0x20D4, 0, ElementInput, "", NULL, NULL }, // select 1
    // buttons
    { 0x20D2, 0, ElementButton, "", _file_back_pressed, NULL },
    { 0x20D2, 1, ElementButton, "", _file_start_print_pressed, NULL }, // 
    { 0x20D2, 2, ElementButton, "", NULL, NULL }, // next page
    { 0x20D2, 3, ElementButton, "", NULL, NULL }, // prev page
    { 0x20D3, 255, ElementButton, "", _file_print_file_pressed, NULL }, // select 1
};

static const dwin_ui_element ui_elements_ender_6_settings[] = {
    { 0x108C, 8, ElementIcon, "", NULL, NULL }, // 8 normal  - 9 muted
    { 0x108A, 0, ElementIcon, "", NULL, NULL }, // icon strip for volume 0 - 7
    // buttons
    { 0x1088, 0xFF, ElementButton, "", _settings_volume_set_pressed, NULL },
    { 0x103E, 0, ElementButton, "", _settings_back_pressed, NULL }, // back
    { 0x103E, 1, ElementButton, "", _settings_levelling_pressed, NULL }, // 
    { 0x103E, 2, ElementButton, "", _settings_refuel_ressed, NULL }, // 
    { 0x103E, 3, ElementButton, "", _settings_move_pressed, NULL }, // 
    { 0x103E, 4, ElementButton, "", _settings_language_pressed, NULL },
    { 0x103E, 5, ElementButton, "", _settings_info_pressed, NULL },
    { 0x103E, 6, ElementButton, "", _settings_disable_motor_pressed, NULL },
    // 10
    { 0x1200, 0, ElementIcon, "", NULL, NULL },
    { 0x1088, 0, ElementInput, "", NULL, NULL },
};


static const dwin_ui_element ui_elements_ender_6_about[] = {
    { 0x1060, 0, ElementInput, "", NULL, NULL },
    { 0x106A, 0, ElementInput, "", NULL, NULL },
    { 0x1074, 0, ElementInput, "", NULL, NULL },
    { 0x107E, 0, ElementInput, "", NULL, NULL },
};

static const dwin_ui_element ui_elements_ender_6_level[] = {
    { 0x1044, 1, ElementButton, "", _level_home_pressed, NULL },
    { 0x1044, 2, ElementButton, "", _level_z_plus_pressed, NULL },
    { 0x1044, 3, ElementButton, "", _level_z_minus_pressed, NULL },
    { 0x1044, 4, ElementButton, "", _level_aux_pressed, NULL },
    { 0x1044, 5, ElementButton, "", _level_measuring_pressed, NULL },
    { 0x1044, 11, ElementButton, "", _level_auto_switch_pressed, NULL },
    { 0x1045, 2, ElementIcon, "", NULL, NULL }, // toggle off
    { 0x1045, 3, ElementIcon, "", NULL, NULL }, // toggle on
    { 0x1040, 1, ElementButton, "", _level_back, NULL },
    { 0x1100, 0, ElementIcon, "", NULL, NULL }, // first element on the grid
};

static const dwin_ui_element ui_elements_ender_6_refuel[] = {
    { 0x1040, 1, ElementButton, "", _refuel_back, NULL },
    { 0x1056, 1, ElementButton, "", _refuel_ret, NULL },
    { 0x1056, 2, ElementButton, "", _refuel_feed, NULL },
    { 0x1056, 5, ElementButton, "", _refuel_heatup, NULL },
    { 0x1056, 6, ElementButton, "", _refuel_cancel, NULL },
    { 0x1056, 0xF1, ElementButton, "", _refuel_cancel, NULL },
    { 0x1054, 0, ElementInput, "", NULL, NULL }, // mm label
    { 0x1020, 0, ElementInput, "", NULL, NULL }, // min material temp
    { 0x1024, 0, ElementInput, "", NULL, NULL }, // progress bar
    { 0x1054, 0xFF, ElementButton, "", _refuel_set_e_mm, NULL }, // mm label
    { NozzlePreheat, 0, ElementInput, "", NULL, NULL },
    { NozzleTemp, 0, ElementInput, "", NULL, NULL },
};

static const dwin_ui_element ui_elements_ender_6_move[] = {
    { 0x1040, 1, ElementButton, "", _move_back, NULL },
    { 0x1048, 0xFF, ElementButton, "", _move_x, NULL },
    { 0x104A, 0xFF, ElementButton, "", _move_y, NULL },
    { 0x104C, 0xFF, ElementButton, "", _move_z, NULL },
    { 0x1046, 1, ElementButton, "", NULL, NULL },
    { 0x1046, 3, ElementButton, "", _move_home, NULL },
    { DisplayXaxis, 0, ElementInput, "", NULL, NULL }, // labels x y z
    { DisplayYaxis, 0, ElementInput, "", NULL, NULL },
    { DisplayZaxis, 0, ElementInput, "", NULL, NULL },  
};

static const dwin_ui_element ui_elements_ender_6_aux_move[] = {
    { 0x1044, 6, ElementButton, "", _aux_move_1, NULL },
    { 0x1044, 7, ElementButton, "", _aux_move_2, NULL },
    { 0x1044, 8, ElementButton, "", _aux_move_3, NULL },
    { 0x1044, 9, ElementButton, "", _aux_move_4, NULL },
    { 0x1044, 10, ElementButton, "", _aux_move_5, NULL },
    { 0x1040, 2, ElementButton, "", _aux_move_back, NULL },
};

static const dwin_ui_element ui_elements_ender_6_home[] = {
    { 0x1042, 0, ElementIcon, "", NULL, NULL }, // icon 0 -9 are for move animation
};

static const dwin_ui_element ui_elements_ender_6_lang[] = {
    { 0x1111, 1, ElementButton, "", _lang_back, NULL },
    { 0x1111, 1, ElementButton, "", _lang_select, NULL },  
};

// auto page switch is enabled on all of these buttons
static const dwin_ui_element ui_elements_ender_6_plaabs[] = {
    { 0x1014, 0, ElementIcon, "", NULL, NULL }, // file name
    { 0x1030, 5, ElementButton, "", _plaabs_pla, NULL },
    { 0x1030, 6, ElementButton, "", _plaabs_abs, NULL },  
    { 0x1030, 7, ElementButton, "", _plaabs_cancel, NULL }, 
    { NozzleTemp, 0, ElementInput, "", NULL, NULL },
    { Bedtemp, 0, ElementInput, "", NULL, NULL },
};

static const dwin_ui_element ui_elements_ender_6_temp_menu[] = {
    { 0x1014, 0, ElementIcon, "", NULL, NULL }, // icon 0 - 23
    { 0x1030, 0, ElementButton, "", _menu_temp_back, NULL },  
    { 0x1030, 1, ElementButton, "", _menu_temp_auto, NULL }, 
    { 0x1030, 2, ElementButton, "", _menu_temp_manual, NULL }, 
    { 0x1030, 3, ElementButton, "", _menu_temp_fan, NULL }, 
    { 0x1034, 0, ElementInput, "", NULL, NULL }, // Set nozzle temp
    { 0x1036, 0, ElementInput, "", NULL, NULL }, // current nozzle temp
    { 0x103A, 0, ElementInput, "", NULL, NULL }, // hot bed temp
    { 0x103C, 0, ElementInput, "", NULL, NULL }, // hot bed cur temp
    // cooling popup
    { 0x1030, 0xF0, ElementButton, "", _menu_temp_cool_no, NULL }, 
    { 0x1030, 0xF1, ElementButton, "", _menu_temp_cool_yes, NULL }, 
};

static const dwin_ui_element ui_elements_ender_6_ntemp[] = {
    { 0x1034, 0, ElementInput, "", NULL, NULL }, // Set nozzle temp
    { 0x1034, 0xFF, ElementButton, "", _ntemp_set_end, NULL }, // Set nozzle temp
    { 0x1036, 0, ElementInput, "", NULL, NULL }, // current nozzle temp
    { 0x103A, 0, ElementInput, "", NULL, NULL }, // hot bed temp
    { 0x103A, 0xFF, ElementButton, "", _ntemp_set_bed, NULL }, // button set hot bed temp
    { 0x103C, 0, ElementInput, "", NULL, NULL }, // hot bed cur temp
    { 0x1032, 0, ElementButton, "", _ntemp_back, NULL }, // back
    { 0x1032, 1, ElementButton, "", _ntemp_cool1, NULL }, // cooling 1
    { 0x1032, 2, ElementButton, "", _ntemp_cool2, NULL }, // cooling 2
};

static const dwin_ui_element ui_elements_ender_6_print[] = {
    { 0x1034, 0, ElementInput, "", NULL, NULL }, // Set nozzle temp
    { 0x1036, 0, ElementInput, "", NULL, NULL }, // current nozzle temp
    { 0x103A, 0, ElementInput, "", NULL, NULL }, // hot bed temp
    { 0x103C, 0, ElementInput, "", NULL, NULL }, // hot bed cur temp
    { 0x1008, 0xF0, ElementButton, "", _print_stop_no, NULL },
    { 0x1008, 0xF1, ElementButton, "", _print_stop_yes, NULL },
    { 0x100A, 0xF0, ElementButton, "", _print_pause_yes, NULL },
    { 0x100A, 0xF1, ElementButton, "", _print_pause_no, NULL },
    { 0x100C, 1, ElementButton, "", _print_resume, NULL },
    { 0x1004, 1, ElementButton, "", _print_adjust, NULL }, // auto switch
    { 0x1014, 1, ElementIcon, "", NULL, NULL }, // status strip 0 - 23
    // 11
    { 0x2000, 0, ElementInput, "", NULL, NULL }, // filename
    { 0x1016, 0, ElementInput, "", NULL, NULL }, // % done
    { 0x1010, 0, ElementInput, "", NULL, NULL }, // tm elapsed
    { 0x1012, 0, ElementInput, "", NULL, NULL }, // tm total
    { 0x100E, 0, ElementInput, "", NULL, NULL }, // pct bar 1
    { 0x100F, 0, ElementInput, "", NULL, NULL }, // pct bar 2
    { 0x1018, 7, ElementIcon, "", NULL, NULL }, // 7 - 10 energy saving 8 on, 9 off
};

static const dwin_ui_element ui_elements_ender_6_adjust[] = {
    { 0x1034, 0, ElementInput, "", NULL, NULL }, // Set nozzle temp
    { 0x1034, 0xFF, ElementButton, "", _adjust_noz_temp, NULL }, // Set nozzle temp
    { 0x103A, 0, ElementInput, "", NULL, NULL }, // hot bed temp
    { 0x103A, 0xFF, ElementButton, "", _adjust_bed_temp, NULL }, // button set hot bed temp
    { 0x1006, 0, ElementInput, "", NULL, NULL },  // speed
    { 0x1006, 0xFF, ElementButton, "", _adjust_speed, NULL }, 
    { 0x1004, 2, ElementButton, "", _adjust_back, NULL },  // back
    { 0x1004, 3, ElementButton, "", _adjust_fan, NULL },  // fan
    { 0x1026, 0xFF, ElementButton, "", _adjust_z, NULL },  // z
    { 0x101E, 2, ElementIcon, "", NULL, NULL },  // fan off
    { 0x101E, 3, ElementIcon, "", NULL, NULL },  // fan on
};

static const dwin_ui_element ui_elements_ender_6_nofilament[] = {
    { 0x105E, 1, ElementButton, "", _nofil_yes, NULL }, 
    { 0x105E, 2, ElementButton, "", _nofil_no, NULL }, 
};

#define LANGS(...) {__VA_ARGS__}

#define WINDOW(IDS, ELE, NAME, INIT, DRAW) { \
    .window_ids = IDS,\
    .elements = ELE, \
    .element_count = ARRSIZE(ELE), \
    .name = NAME, \
    .init = INIT, \
    .draw = DRAW }

static const dwin_window windows[] = {
    WINDOW(LANGS(0, 0),   ui_elements_ender_6_boot,      "Boot",        &_boot_window_init, &_boot_window_draw),
    WINDOW(LANGS(45, 1),  ui_elements_ender_6_main,      "Main",        NULL,               &_main_window_draw),
    WINDOW(LANGS(46, 2),  ui_elements_ender_6_file,      "File",        NULL,               &_file_window_draw),
    WINDOW(LANGS(75, 0),  ui_elements_ender_6_about,     "About",       NULL,               NULL),
    WINDOW(LANGS(63, 0),  ui_elements_ender_6_settings,  "Settings",    &_settings_init,    NULL),
    WINDOW(LANGS(64, 0),  ui_elements_ender_6_level,     "Level",       &_level_init,       NULL),
    WINDOW(LANGS(84, 0),  ui_elements_ender_6_aux_move,  "AuxLevel",    NULL,               NULL),
    WINDOW(LANGS(65, 0),  ui_elements_ender_6_refuel,    "Refuel1",     &_refuel_init,      &_refuel_draw),
    WINDOW(LANGS(66, 0),  ui_elements_ender_6_refuel,    "Refuel2",     &_refuel_init,      &_refuel_draw),
    WINDOW(LANGS(68, 0),  ui_elements_ender_6_refuel,    "Refuel3",     &_refuel_init,      &_refuel_draw),
    WINDOW(LANGS(71, 0),  ui_elements_ender_6_move,      "Move",        &_move_init,        NULL),
    WINDOW(LANGS(33, 34), ui_elements_ender_6_lang,      "Lang",        NULL,               NULL),
    WINDOW(LANGS(74, 0),  ui_elements_ender_6_home,      "AutoHome",    NULL,               &_homing_draw),
    WINDOW(LANGS(85, 0),  NULL,                          "AutoLevel",   &_autolevel_init,   NULL),
    WINDOW(LANGS(59, 0),  ui_elements_ender_6_plaabs,    "PLAABS1",     NULL,               &_plaabs_draw),
    WINDOW(LANGS(60, 0),  ui_elements_ender_6_plaabs,    "PLAABS2",     NULL,               &_plaabs_draw),
    WINDOW(LANGS(57, 0),  ui_elements_ender_6_temp_menu, "TempMenu1",   NULL,               &_menu_temp_draw),
    WINDOW(LANGS(58, 0),  ui_elements_ender_6_temp_menu, "TempMenu2",   NULL,               &_menu_temp_draw),
    WINDOW(LANGS(61, 0),  ui_elements_ender_6_ntemp,     "NozTemp",     NULL,               &_menu_temp_draw), // TODO
    WINDOW(LANGS(87, 0),  NULL,                          "Processing",  NULL,               NULL), 
    WINDOW(LANGS(78, 0),  ui_elements_ender_6_nofilament,"NoFilament",  NULL,               NULL), 
    WINDOW(LANGS(52, 0),  ui_elements_ender_6_print,     "PrintEnd",    &_print_init,       &_print_draw), 
    WINDOW(LANGS(52, 0),  ui_elements_ender_6_print,     "PrintHeat",   &_print_init,       &_print_draw), 
    WINDOW(LANGS(53, 0),  ui_elements_ender_6_print,     "PrintRun",    &_print_init,       &_print_draw), 
    WINDOW(LANGS(54, 0),  ui_elements_ender_6_print,     "PrintPause",  &_print_init,       &_print_draw),
    WINDOW(LANGS(56, 0),  ui_elements_ender_6_adjust,    "Adjust",      &_adjust_init,      NULL), 
    WINDOW(LANGS(88, 0),  NULL,                          "TooHot",      NULL,               NULL), 
    WINDOW(LANGS(89, 0),  NULL,                          "TooCold",     NULL,               NULL), 
    WINDOW(LANGS(90, 0),  NULL,                          "ThermFail",   NULL,               NULL), 
    
};




void dwin_lcd_model_init() {
    ui_register_windows(windows, sizeof(windows) / sizeof(dwin_window));
}

// generic 
// filament was removed while printing
void dwin_filament_was_removed() {
    dwin_window *windowfil = ui_window_get_by_name("NoFilament");
    dwin_window *window1 = ui_window_get_by_name("Refuel1");
    dwin_window *window2 = ui_window_get_by_name("Refuel2");
    dwin_window *window3 = ui_window_get_by_name("Refuel3");
    dwin_window *curwin = ui_window_get_current_window();

    if (curwin != window1 &&
        curwin != window2 &&
        curwin != window3 &&
        curwin != windowfil)
        ui_window_change_window("NoFilament");
}

void dwin_temperature_event(bool istoohot) {
    if (istoohot)
        ui_window_change_window("TooHot");
    else
        ui_window_change_window("TooCold");
}

/* Boot window */
static uint8_t _progress;

void _boot_window_init(dwin_window *window, void *context) {
    _progress = 0;
}

bool _delay_until(int interval_ms) {
    static millis_t _next_update = 0;
    static int _interval_ms = 0;
    millis_t ms = millis();
    
    // first run
    if (_next_update == 0)
        _next_update = ms + interval_ms;

    // interval changed
    if (interval_ms != _interval_ms)
    {
        _next_update = ms + interval_ms;
        _interval_ms = interval_ms;
        return true;
    }

    if(ms >= _next_update)
    {
        _next_update = 0;
        return true;
    }

    return false;
}

void _boot_window_draw(dwin_window *window, void *context) {
    uint8_t progress = _progress;
    // there is only a splash boot screen on here. 
    // It has two progress bars on it
    if (!_delay_until(10)) // 10ms = 2s
        return;

    if (progress > 200)
        return; // we are done, should be into another screen by now!

    // progress goes from 0 -> 200
    dwin_ui_element *uiprogress = NULL;

    if (progress <= 100)
        uiprogress = (dwin_ui_element *)&ui_elements_ender_6_boot[0];
    else if (progress <= 200) 
    {
        uiprogress = (dwin_ui_element *)&ui_elements_ender_6_boot[1];
        progress -= 100;
    }

    // we have the ui element, lets update it
    if (uiprogress)
        dwin_send(uiprogress, progress);
    
    _progress++;

    if (_progress >= 200)
    {
        delay(30);
        SERIAL_ECHOPAIR("\n *** CWindow =",window->name);
        ui_window_change_window("Main");
    }
}

/* main menu */
void _main_print_pressed(dwin_ui_element *element, void *context) {
    ui_window_change_window("File");
}

void _main_temp_pressed(dwin_ui_element *element, void *context) {
    _nav_to_temp_menu(false);
}

void _main_settings_pressed(dwin_ui_element *element, void *context) {
    ui_window_change_window("Settings", true);
}

void _main_window_draw(dwin_window *window, void *context) {
    if (!_delay_until(500)) // 500ms refresh interval 
        return;

    dwin_send(&ui_elements_ender_6_main[0], thermalManager.temp_hotend[0].target);
    dwin_send(&ui_elements_ender_6_main[1], thermalManager.temp_hotend[0].celsius);
    dwin_send(&ui_elements_ender_6_main[2], thermalManager.temp_bed.target);
    dwin_send(&ui_elements_ender_6_main[3], thermalManager.temp_bed.celsius);
}

/* file window */

void _file_start_print_pressed(dwin_ui_element *element, void *context) {
    dwin_start_print(NULL);
    _nav_to_print_menu();
}

void _file_print_file_pressed(dwin_ui_element *element, void *context) {
    // a file was selected
    uint8_t index = *(uint8_t *)context;
    dwin_print_file_selected(index);
}


void _file_window_draw(dwin_window *window, void *context) {
    // render the file names
}

void _file_back_pressed(dwin_ui_element *element, void *context) {
    ui_window_change_window("Main", true);
}

/* No FIlament */
void _nofil_yes(dwin_ui_element *element, void *context) {
    dwin_resume_print(); // ??
}

void _nofil_no(dwin_ui_element *element, void *context) {
    dwin_stop_print();
    ui_window_change_window("Main");
}

/* Settings Window */
void _settings_init(dwin_window *window, void *context) {
    uint16_t volume = dwin_get_volume();
    _settings_volume_set_pressed(NULL, (void *)&volume);
    // set the value of the incrementor
    dwin_send(&ui_elements_ender_6_settings[11], volume);
}

void _settings_levelling_pressed(dwin_ui_element *element, void *context) {
    // auto nav to page
    ui_window_change_window("Level", true);
}

void _settings_refuel_ressed(dwin_ui_element *element, void *context) {
    _nav_to_refuel();
}

void _settings_move_pressed(dwin_ui_element *element, void *context) {
    ui_window_change_window("Move");
}

void _settings_language_pressed(dwin_ui_element *element, void *context) {
    // auto switch
    ui_window_change_window("Lang", true);
}

void _settings_info_pressed(dwin_ui_element *element, void *context) {
    // actually we don't change window. The default dwin isn't set
    // to notify us of a back press (No data upload enabled)
    //ui_window_change_window("About");
    // send the gui version info
    dwin_send(&ui_elements_ender_6_about[0], "Ender 6 Marlin 2");
    dwin_send(&ui_elements_ender_6_about[1], SHORT_BUILD_VERSION);
    dwin_send(&ui_elements_ender_6_about[2], "260x260x400");
    dwin_send(&ui_elements_ender_6_about[3], WEBSITE_URL);
}

void _settings_disable_motor_pressed(dwin_ui_element *element, void *context) {
    queue.enqueue_one_P(PSTR("M84"));
    dwin_send(&ui_elements_ender_6_settings[10], 11);
}

void _settings_back_pressed(dwin_ui_element *element, void *context) {
    ui_window_change_window("Main", true);
}

void _settings_volume_set_pressed(dwin_ui_element *element, void *context) {
    short volume = *(short *)context;
    NOMORE(volume, (short)(0xFF));
    NOLESS(volume, (short)(0x00));
    if(volume == 0) {
        // mute icon
        dwin_send(&ui_elements_ender_6_settings[0], ui_elements_ender_6_settings[0].key_code + 1);
        // volume slider thing
        dwin_send(&ui_elements_ender_6_settings[1], ui_elements_ender_6_settings[1].key_code);
    }
    else {
        // unmute icon
        dwin_send(&ui_elements_ender_6_settings[0], ui_elements_ender_6_settings[0].key_code);
        // volume slider icon
        dwin_send(&ui_elements_ender_6_settings[1], ui_elements_ender_6_settings[1].key_code + (volume + 1)/32 - 1);
    }

    dwin_set_volume((uint8_t)volume);
}

/* levelling menu */
void _level_home_pressed(dwin_ui_element *element, void *context) {
    dwin_home("AutoHome", "Level", false);
}

void _level_z_minus_pressed(dwin_ui_element *element, void *context) {
    dwin_set_probe_offset(0.1);
}

void _level_z_plus_pressed(dwin_ui_element *element, void *context) {
    dwin_set_probe_offset(-0.1);
}

void _level_aux_pressed(dwin_ui_element *element, void *context) {
    dwin_home("Processing", "AuxLevel", true);
}

void _level_measuring_pressed(dwin_ui_element *element, void *context) {
    ui_window_change_window("AutoLevel");
}

void _level_auto_switch_pressed(dwin_ui_element *element, void *context) {
    // Autolevel switch
    if (dwin_get_auto_level_status()) {
        // turn on the Autolevel
        dwin_send(&ui_elements_ender_6_level[6], 0x3);
        dwin_set_auto_level_status(false);        
    }
    else {
        // turn off the Autolevel
        dwin_send(&ui_elements_ender_6_level[6], 0x2);
        dwin_set_auto_level_status(true);
    }

    //RTS_SndData(zprobe_zoffset*100, 0x1026); 
}

void _level_back(dwin_ui_element *element, void *context) {
    settings.save();
    ui_window_change_window("Settings");
}

void _level_init(dwin_window *window, void *context) {
    // turn on the Autolevel icon    
    dwin_send(&ui_elements_ender_6_level[6], dwin_get_auto_level_status() ? 0x02 : 0x03);

    // send the levels
    bool zig = true;
    int8_t inStart, inStop, inInc, showcount;
    showcount = 0;
    
    dwin_settings_load();
    for(int y = 0;y < GRID_MAX_POINTS_Y;y++) {
      // away from origin
      if (zig) {
        inStart = 0;
        inStop = GRID_MAX_POINTS_X - 1;
        inInc = 1;
      }
      else {
        // towards origin
        inStart = GRID_MAX_POINTS_X - 2;
        inStop = -1;
        inInc = -1;
      }
      zig ^= true;
      for(int x = inStart;x != inStop; x += inInc) {
        rtscheck.RTS_SndData(ubl.z_values[x][y] *1000, AutolevelVal + showcount*2);
        showcount++;
      }
    }
}

// auto level
void _autolevel_init(dwin_window *window, void *context) {
    dwin_auto_level_start("Level");
}

// refuel
static float _refuel_e_mm = 100;

void _nav_to_refuel() {
    // check temperature and whatnot
    if (thermalManager.hotEnoughToExtrude(0) 
            && thermalManager.temp_hotend[0].celsius >= thermalManager.degTargetHotend(0) 
            && !thermalManager.isHeatingHotend(0)) {
        ui_window_change_window("Refuel1");
    }
    else if (thermalManager.isHeatingHotend(0)) {
        ui_window_change_window("Refuel3");
    }
    else {
        ui_window_change_window("Refuel2");
    }
}

void _refuel_init(dwin_window *window, void *context) {
    material *mat = dwin_get_current_material();
    dwin_send(&ui_elements_ender_6_refuel[6], _refuel_e_mm); // mm
    dwin_send(&ui_elements_ender_6_refuel[7], dwin_get_material_hotend_temp(mat)); // target temp
    _refuel_draw(window, context);
}

void _refuel_draw(dwin_window *window, void *context) {
    if (!_delay_until(800)) // 800ms refresh interval 
        return;
    
    dwin_send(&ui_elements_ender_6_main[0], thermalManager.temp_hotend[0].target);
    dwin_send(&ui_elements_ender_6_main[1], thermalManager.temp_hotend[0].celsius);

    // progress bar
    dwin_window *w = ui_window_get_by_name("Refuel3");
    
    if (w == window) {
        uint32_t icon;
        if(thermalManager.degTargetHotend(0))
            icon = thermalManager.temp_hotend[0].celsius * 100/thermalManager.temp_hotend[0].target;
        else 
            icon = 100;

        if(icon >= 100)
            icon = 100;
        
        dwin_send(&ui_elements_ender_6_refuel[8], icon);

        // done
        if (icon >= 99)
            ui_window_change_window("Refuel1");
    }
}

void _refuel_back(dwin_ui_element *element, void *context)
{
    // if we are heating, confirm? nah

    ui_window_change_window("Settings");
}

void _refuel_ret(dwin_ui_element *element, void *context) {
    current_position[E_AXIS] -= _refuel_e_mm;
    dwin_plan_current_position(E_AXIS, 150);
}

void _refuel_feed(dwin_ui_element *element, void *context) {
    current_position[E_AXIS] += _refuel_e_mm;
    dwin_plan_current_position(E_AXIS, 150);
}

void _refuel_set_e_mm(dwin_ui_element *element, void *context) {
    _refuel_e_mm = ((float)(*(uint16_t *)context)/10);
}

void _refuel_heatup(dwin_ui_element *element, void *context) {
    material *mat = dwin_get_current_material();
    thermalManager.setTargetHotend(dwin_get_material_hotend_temp(mat), 0);
    _nav_to_refuel();
}

void _refuel_cancel(dwin_ui_element *element, void *context) {
    thermalManager.setTargetHotend(0, 0);
    ui_window_change_window("Settings");
}

// move
void _move_init(dwin_window *window, void *context) {
    dwin_send(&ui_elements_ender_6_move[6], 10 * current_position[X_AXIS]);
    dwin_send(&ui_elements_ender_6_move[7], 10 * current_position[Y_AXIS]);
    dwin_send(&ui_elements_ender_6_move[8], 10 * current_position[Z_AXIS]);
}

void _move_back(dwin_ui_element *element, void *context) {
    ui_window_change_window("Settings");
}

static void _move_n(AxisEnum axis, uint16_t value, float min, float max) {
    current_position[axis] = ((float)value)/10;
    if (current_position[axis] < min)
        current_position[axis] = min;
    else if (current_position[axis] > max)
        current_position[axis] = max;

    dwin_plan_current_position(axis);
}

void _move_x(dwin_ui_element *element, void *context) {
    uint16_t val = *(uint16_t *)context;
    _move_n(X_AXIS, val, X_MIN_POS, X_MAX_POS);
}

void _move_y(dwin_ui_element *element, void *context) {
    uint16_t val = *(uint16_t *)context;
    _move_n(Y_AXIS, val, Y_MIN_POS, Y_MAX_POS);
}

void _move_z(dwin_ui_element *element, void *context) {
    uint16_t val = *(uint16_t *)context;
    _move_n(Z_AXIS, val, Z_MIN_POS, Z_MAX_POS);
}

void _move_home(dwin_ui_element *element, void *context) {
    dwin_home("AutoHome", "Move", false);
}

// aux level
void _aux_move_back(dwin_ui_element *element, void *context) {
    ui_window_change_window("Level");
}

void _aux_move(dwin_ui_element *element, int x, int y, int z) {
    ui_input_lock();
    char buf[50];
    snprintf(buf, 50, "G1 X%d Y%d F%d", x, y, z);
    queue.inject_P(PSTR("G1 F3600 Z3")); 
    queue.inject(buf);
    queue.inject_P(PSTR("G1 F3600 Z0"));
    ui_input_unlock();
}

void _aux_move_1(dwin_ui_element *element, void *context) { _aux_move(element, 130, 130, 3600); }
void _aux_move_2(dwin_ui_element *element, void *context) { _aux_move(element, 50, 50, 3600); }
void _aux_move_3(dwin_ui_element *element, void *context) { _aux_move(element, 210, 50, 3600); }
void _aux_move_4(dwin_ui_element *element, void *context) { _aux_move(element, 210, 210, 3600); }
void _aux_move_5(dwin_ui_element *element, void *context) { _aux_move(element, 50, 210, 3600); }

// auto home
static uint8_t _homing_icon;
void _homing_draw(dwin_window *window, void *context) {
    if (!_delay_until(500)) // 500ms refresh interval 
        return;

    dwin_send(&ui_elements_ender_6_home[0], _homing_icon++);
    if(_homing_icon > 9)
        _homing_icon = 0;
}


// lang
void _lang_select(dwin_ui_element *element, void *context)
{

}
void _lang_back(dwin_ui_element *element, void *context)
{

}

// pla abs
void _plaabs_pla(dwin_ui_element *element, void *context) {    
    material *mat = dwin_get_material(Pla);
    thermalManager.setTargetHotend(dwin_get_material_hotend_temp(mat), 0);
    thermalManager.setTargetBed(dwin_get_material_bed_temp(mat));
    dwin_set_current_material(mat);

    _nav_to_temp_menu(true);
}

void _plaabs_abs(dwin_ui_element *element, void *context) {
    material *mat = dwin_get_material(Abs);
    thermalManager.setTargetHotend(dwin_get_material_hotend_temp(mat), 0);
    thermalManager.setTargetBed(dwin_get_material_bed_temp(mat));    
    
    dwin_set_current_material(mat);

    _nav_to_temp_menu(true);
}

void _plaabs_cancel(dwin_ui_element *element, void *context) {
    _nav_to_temp_menu(true);
}

void _plaabs_draw(dwin_window *window, void *context) {
    if (!_delay_until(500))
        return;

    dwin_send(&ui_elements_ender_6_plaabs[4], thermalManager.temp_hotend[0].celsius);
    dwin_send(&ui_elements_ender_6_plaabs[5], thermalManager.temp_bed.target);
}

// temp menu
void _nav_to_temp_menu(bool dont_nav) {
    if (dwin_get_fan())
        ui_window_change_window("TempMenu1", dont_nav);
    else
        ui_window_change_window("TempMenu2", dont_nav);
}

void _nav_to_plaabs() {
    if (dwin_get_fan())
        ui_window_change_window("PLAABS1", true);
    else
        ui_window_change_window("PLAABS2", true);
}

void _menu_temp_back(dwin_ui_element *element, void *context) {
    ui_window_change_window("Main", true);
}

void _menu_temp_auto(dwin_ui_element *element, void *context) {
    _nav_to_plaabs();
}

void _menu_temp_manual(dwin_ui_element *element, void *context) {
    ui_window_change_window("NozTemp", true);
}

void _menu_temp_cool(dwin_ui_element *element, void *context) {
    ui_window_change_window("Cool", true);
}

void _menu_temp_fan(dwin_ui_element *element, void *context) {
    dwin_set_fan(!dwin_get_fan());
    _nav_to_temp_menu(false);
}

void _menu_temp_draw(dwin_window *window, void *context) {
    if (!_delay_until(500))
        return;

    dwin_send(&ui_elements_ender_6_temp_menu[5], thermalManager.temp_hotend[0].target);
    dwin_send(&ui_elements_ender_6_temp_menu[6], thermalManager.temp_hotend[0].celsius);
    dwin_send(&ui_elements_ender_6_temp_menu[7], thermalManager.temp_bed.target);
    dwin_send(&ui_elements_ender_6_temp_menu[8], thermalManager.temp_bed.celsius);
}

void _menu_temp_cool_no(dwin_ui_element *element, void *context) {

}

void _menu_temp_cool_yes(dwin_ui_element *element, void *context) {
    dwin_set_fan(true);
    thermalManager.disable_all_heaters();
    _nav_to_temp_menu(false);
}

// temp screen
void _ntemp_back(dwin_ui_element *element, void *context) {
    _nav_to_temp_menu(false);
}

void _ntemp_cool1(dwin_ui_element *element, void *context) {
    thermalManager.setTargetHotend(0, 0);
}

void _ntemp_cool2(dwin_ui_element *element, void *context) {
    thermalManager.setTargetBed(0);
}

void _ntemp_set_bed(dwin_ui_element *element, void *context) {
    thermalManager.setTargetBed(*(int16_t *)context);
}

void _ntemp_set_end(dwin_ui_element *element, void *context) {
    thermalManager.setTargetHotend(*(int16_t *)context, 0);
}

// print
void _print_init(dwin_window *window, void *context) {
    dwin_send_filename();

    //dwin_send(&ui_elements_ender_6_print[17], energysave);
    _print_draw(window, context);
}

void _print_draw(dwin_window *window, void *context) {
    if (!_delay_until(500))
        return;

    duration_t elapsed = print_job_timer.duration();

    dwin_send(&ui_elements_ender_6_print[0], thermalManager.temp_hotend[0].target);
    dwin_send(&ui_elements_ender_6_print[1], thermalManager.temp_hotend[0].celsius);
    dwin_send(&ui_elements_ender_6_print[2], thermalManager.temp_bed.target);
    dwin_send(&ui_elements_ender_6_print[3], thermalManager.temp_bed.celsius);

    dwin_send(&ui_elements_ender_6_print[12], dwin_percent_done());
    dwin_send(&ui_elements_ender_6_print[13], (elapsed.value/3600));
    dwin_send(&ui_elements_ender_6_print[14], (elapsed.value%3600)/60);
    
    if(dwin_percent_done() <= 50) {
        dwin_send(&ui_elements_ender_6_print[15], (unsigned int)dwin_percent_done() * 2);
        dwin_send(&ui_elements_ender_6_print[16], 0);
    }
    else {
        dwin_send(&ui_elements_ender_6_print[15], 100);
        dwin_send(&ui_elements_ender_6_print[16], (unsigned int)dwin_percent_done() * 2 - 100);
        rtscheck.RTS_SndData(100 ,PrintscheduleIcon);
    }
}

void _nav_to_print_menu() {
    if (dwin_is_paused())
        ui_window_change_window("PrintPause");
    else if(thermalManager.isHeatingBed() || thermalManager.isHeatingHotend(0))
        ui_window_change_window("PrintHeat");
    else if (card.isPrinting())
        ui_window_change_window("PrintRun");
    else if (card.isFileOpen())
        ui_window_change_window("PrintHeat");
    else
        ui_window_change_window("PrintEnd");
}

void _print_stop_no(dwin_ui_element *element, void *context) {

}

void _print_stop_yes(dwin_ui_element *element, void *context) {
    dwin_stop_print();
    ui_window_change_window("Main");
}

void _print_pause_yes(dwin_ui_element *element, void *context) {
    dwin_pause_print();
    _nav_to_print_menu();
}

void _print_pause_no(dwin_ui_element *element, void *context) {

}

void _print_resume(dwin_ui_element *element, void *context) {
    dwin_resume_print();
    _nav_to_print_menu();
}

void _print_adjust(dwin_ui_element *element, void *context) {
    ui_window_change_window("Adjust", true);
}

// adjust window
void _adjust_init(dwin_window *window, void *context) {
    if (dwin_get_fan())
        dwin_send(&ui_elements_ender_6_adjust[10], ui_elements_ender_6_adjust[10].key_code);
    else
        dwin_send(&ui_elements_ender_6_adjust[9], ui_elements_ender_6_adjust[9].key_code);

    dwin_send(&ui_elements_ender_6_adjust[0], thermalManager.temp_hotend[0].target);
    dwin_send(&ui_elements_ender_6_adjust[2], thermalManager.temp_bed.target);
    dwin_send(&ui_elements_ender_6_adjust[4], feedrate_percentage);
    dwin_send(&ui_elements_ender_6_adjust[8], probe.offset.z * 100);
}

void _adjust_noz_temp(dwin_ui_element *element, void *context) {
    uint16_t t = *(uint16_t *)context;
    thermalManager.setTargetHotend(t, 0);
    dwin_send(&ui_elements_ender_6_adjust[0], t);
}

void _adjust_bed_temp(dwin_ui_element *element, void *context) {
    uint16_t t = *(uint16_t *)context;
    thermalManager.setTargetBed(t);
    dwin_send(&ui_elements_ender_6_adjust[2], t);
}

void _adjust_speed(dwin_ui_element *element, void *context) {
    feedrate_percentage = *(uint16_t *)context;
    dwin_send(&ui_elements_ender_6_adjust[4], feedrate_percentage);
}

void _adjust_back(dwin_ui_element *element, void *context) {
    settings.save();
    _nav_to_print_menu();
}

void _adjust_fan(dwin_ui_element *element, void *context) {
    dwin_set_fan(!dwin_get_fan());

    if (dwin_get_fan())
        dwin_send(&ui_elements_ender_6_adjust[9], ui_elements_ender_6_adjust[9].key_code);
    else
        dwin_send(&ui_elements_ender_6_adjust[10], ui_elements_ender_6_adjust[10].key_code);
}

void _adjust_z(dwin_ui_element *element, void *context) {
    uint16_t off = *(uint16_t *)context;
    float zoff = 0;

    if(off >= 32768) {
        zoff = ((float)off - 65536) / 100;
    }
    else {
        zoff = ((float)off) / 100;
    }
    
    dwin_set_probe_offset(zoff - probe.offset.z);
}
