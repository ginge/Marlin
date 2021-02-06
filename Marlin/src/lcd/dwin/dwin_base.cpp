
#include "../../inc/MarlinConfigPre.h"

#if ENABLED(MachineEnder6)

#include "../../inc/MarlinConfig.h"

#include "dwin_base.h"

#include <WString.h>
#include <stdio.h>
#include <string.h>

#include "../fontutils.h"
#include "../ultralcd.h"

#include "../../sd/cardreader.h"

#include "../../MarlinCore.h"
#include "../../core/serial.h"
#include "../../core/macros.h"
#include "../../gcode/queue.h"

#include "../../feature/powerloss.h"
#include "../../feature/babystep.h"
#include "../../feature/runout.h"

#include "../../module/settings.h"
#include "../../module/temperature.h"
#include "../../module/printcounter.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../libs/BL24CXX.h"

#if ENABLED(HOST_ACTION_COMMANDS)
  #include "../../feature/host_actions.h"
#endif

#if HAS_BED_PROBE
  #include "../../module/probe.h"
#endif

/* CONFIG */
// which serial port are we on
#ifndef LCD_SERIAL
    #define LCD_SERIAL MSerial3
#endif

/* END CONFIG */

void HMI_Init() { dwin_init(); }
void DWIN_Update() { dwin_loop(); }
void DWIN_Handshake() {;}
void DWIN_UpdateLCD() {;}
void HMI_StartFrame(bool) {;}
void DWIN_CompletedHoming() { dwin_homing_complete(); }
void DWIN_Frame_SetDir(uint8_t dir) {;}
void Popup_Window_Temperature(bool istoohot) { dwin_temperature_event(istoohot); }
void DWIN_CompletedLeveling() { dwin_auto_level_complete(); };
void DWIN_SetHomeFlag(bool flag) { /*dwin_set_homing(flag);*/ }
void DWIN_SetHeatFlag(bool flag, millis_t heat_time) { ; } // TODO
void DWIN_FilamentRunout() { dwin_filament_was_removed(); }
void MarlinUI::refresh() {}

int HMI_flag;
millis_t dwin_heat_time = 0;
static bool _homing = false;
static char *_page_name_to_return_to;
CRec CardRecbuf;
int temphot = 0;
int tempbed = 0;
float pause_z = 0;
float pause_e = 0;
char commandbuf[30];
const float manual_feedrate_mm_m[] = DEFAULT_MANUAL_FEEDRATE;

// TODO CHECK THIS
#define DWIN_LANGUAGE_EEPROM_ADDRESS  0x01   // Between 0x01 and 0x63 (EEPROM_OFFSET-1)
                                            // BL24CXX::check() uses 0x00
#define DWIN_BED_LEVEL_EEPROM_ADDRESS 0x02
#define DWIN_VOLUME_EEPROM_ADDRESS    0x04
static uint8_t _lang_id = 0;
static uint8_t _auto_bed_level = 0;

#define DEFAULT_VOLUME 0x80
static uint8_t _dwin_volume = DEFAULT_VOLUME;

void dwin_init() {
  LCD_SERIAL.begin(115200);
  dwin_init_language();
  dwin_lcd_model_init();
  dwin_init_volume();
  dwin_init_auto_level_status();
}

void dwin_send(const dwin_ui_element *element, int value) {
    rtscheck.RTS_SndData(value, element->element_id);
}

void dwin_send(const dwin_ui_element *element, float value) {
    rtscheck.RTS_SndData(value, element->element_id);
}

void dwin_send(const dwin_ui_element *element, const char *value) {
    rtscheck.RTS_SndData(value, element->element_id);
}

void dwin_plan_current_position(AxisEnum axis, float e_feedrate /*= 0*/) {
  if (!planner.is_full()) {
    int f = e_feedrate;
    if (e_feedrate == 0)
      f = MMM_TO_MMS(manual_feedrate_mm_m[(int8_t)axis]);

    planner.buffer_line(current_position, f, active_extruder);
  }
}

void _ui_loop() {
    // draw the window
    dwin_window *window = ui_window_get_current_window();
    if (window->draw)
        window->draw(window, NULL);
    return;
}

// looping at the loop function
void dwin_loop() {
  // Check the status of card
  rtscheck.RTS_SDCardUpate();

  _ui_loop();

  if (ui_input_is_locked()) 
    return;

  if(rtscheck.RTS_RecData() > 0) {
    rtscheck.RTS_HandleData();
  }
}

uint8_t dwin_init_language() {
    uint8_t lang;
    BL24CXX::read(DWIN_LANGUAGE_EEPROM_ADDRESS, (uint8_t*)&lang, sizeof(lang));
    _lang_id = lang - 0x30; // it arrives as ascii

    if (_lang_id > 1)
      _lang_id = 0;

    return lang;
}

uint8_t dwin_get_language() {
    return _lang_id;
}

void dwin_set_language(uint8_t lang_id) {
  lang_id += 0x30;
  BL24CXX::write(DWIN_LANGUAGE_EEPROM_ADDRESS, &lang_id, sizeof(lang_id));
  _lang_id = lang_id - 0x30;
}

uint8_t dwin_init_auto_level_status() {
    _auto_bed_level = BL24CXX::readOneByte(DWIN_BED_LEVEL_EEPROM_ADDRESS);
    
    dwin_set_auto_level_status(_auto_bed_level == 1);
    /* dump eeprom
    SERIAL_ECHOPAIR("\n *** Bed1 =", bedlev);

    uint8_t eeprom[256];
    BL24CXX::read(0x01, eeprom, sizeof(eeprom));
    char b[20] = {0};
    for (int i = 0; i < 255; i++) 
    {
      snprintf(b, 20, "0x%2x", eeprom[i]);
      SERIAL_ECHOPAIR(" ", b);
    }*/
    return _auto_bed_level;
}

uint8_t dwin_get_auto_level_status() {
    return _auto_bed_level;
}

void dwin_set_auto_level_status(bool is_enabled) {
  BL24CXX::writeOneByte(DWIN_BED_LEVEL_EEPROM_ADDRESS, is_enabled ? 1 : 0);
  _auto_bed_level = is_enabled ? 1 : 0;

  if (is_enabled)
    queue.inject_P(PSTR("M420 S1"));
  else
    queue.inject_P(PSTR("M420 S0"));
}

void dwin_auto_level_start(const char *page_name_to_return_to) {
  _page_name_to_return_to = (char *)page_name_to_return_to;
  //RTS_SndData(1, AutolevelIcon); 
  queue.inject_P(PSTR("G29 P1"));
}

void dwin_auto_level_complete() {
  static uint8_t step = 0;

  if (step == 0) {
    queue.enqueue_now_P(PSTR("G29 P3")); // fill the rest of the coordinates with extrapo
    queue.enqueue_now_P(PSTR("G29 S1")); // save
  }
  else if (step == 2) {
    // saved
    settings.save();
    ui_window_change_window(_page_name_to_return_to);
    _page_name_to_return_to = NULL;
    step = 0;
    return;
  }
  step++;
}

void dwin_set_probe_offset(float z) {
    #if HAS_BED_PROBE
      if (WITHIN(probe.offset.z + z, Z_PROBE_OFFSET_RANGE_MIN, Z_PROBE_OFFSET_RANGE_MAX)) {
        probe.offset.z += z;
        #if ENABLED(BABYSTEP_ZPROBE_OFFSET)
          babystep.add_mm(Z_AXIS, (z));
        #endif
      }
    #elif ENABLED(BABYSTEPPING)
      babystep.add_mm(Z_AXIS, (zprobe_zoffset - probe.offset.z));
    #else
      UNUSED(zprobe_zoffset - probe.offset.z);
    #endif

    NOLESS(probe.offset.z, (Z_PROBE_OFFSET_RANGE_MIN));
    NOMORE(probe.offset.z, (Z_PROBE_OFFSET_RANGE_MAX));
}
uint8_t _eeprom_read_volume() {
  uint8_t vol;
  vol = BL24CXX::readOneByte(DWIN_VOLUME_EEPROM_ADDRESS);
  return vol;
}

void dwin_init_volume() {
  _dwin_volume = _eeprom_read_volume();
  rtscheck.RTS_SndData(StartSoundSet, SoundAddr);

  rtscheck.RTS_SndData((unsigned long)_dwin_volume << 8, SoundAddr + 1);
}

void dwin_set_volume(uint8_t volume) {
  _dwin_volume = volume;
  BL24CXX::writeOneByte(DWIN_VOLUME_EEPROM_ADDRESS, volume);
  rtscheck.RTS_SndData((unsigned long)_dwin_volume << 8, SoundAddr + 1);
}

uint8_t dwin_get_volume() {
  return _dwin_volume;
}

void dwin_homing_complete() {
  if (!_homing)
    return;
    
  _homing = false;
  if (_page_name_to_return_to)
  {
    ui_window_change_window(_page_name_to_return_to);
    _page_name_to_return_to = NULL;
  }
}

void dwin_set_homing(bool is_homing) {
  _homing = is_homing;
}

bool dwin_get_homing() {
  return _homing;
}

void dwin_home(const char *page_name_to_display, const char *page_name_to_return_to, bool absolute) {
  dwin_set_homing(true);
  if (page_name_to_display)
    ui_window_change_window(page_name_to_display);
  queue.inject_P(PSTR("G28"));
  if (absolute)
    queue.enqueue_now_P(PSTR("G90"));
  //RTS_SndData(10,FilenameIcon);
  _page_name_to_return_to = (char *)page_name_to_return_to;
}

void dwin_home(const char *page_name_to_display) {
  dwin_home(page_name_to_display, NULL, false);
}


void dwin_settings_load() {
  settings.load();
}

bool dwin_get_fan() {
  return thermalManager.fan_speed[0] > 0;
}

void dwin_set_fan(bool enabled) {
  #if FAN_COUNT > 0
    for (uint8_t i = 0; i < FAN_COUNT; i++) 
      thermalManager.set_fan_speed(0, enabled ? FanOn : FanOff);
  #endif
}

bool dwin_is_paused() {
  return card.isPaused();
}

/*
int dwin_get_status_icon()
{
  int offset = (dwin_get_language() == 0 ? 12 : 0);
  if (card.isPrinting())
    offset += 2;
  else if (last_target_temperature_bed > thermalManager.target_temperature_bed || (last_target_temperature[0] > thermalManager.target_temperature[0])))
    offset += 8;
  else if ()
  return offset;
}
*/

void dwin_print_file_selected(int index) {
  if(!card.isMounted())
    return;

  CardRecbuf.recordcount = index - 1;
  
  int filelen = strlen(CardRecbuf.Cardshowfilename[CardRecbuf.recordcount]);
  filelen = (TEXTBYTELEN - filelen)/2;
  if(filelen > 0)
  {
    char buf[20];
    memset(buf,0,sizeof(buf));
    strncpy(buf,"         ",filelen);
    strcpy(&buf[filelen],CardRecbuf.Cardshowfilename[CardRecbuf.recordcount]);
    rtscheck.RTS_SndData(buf, Choosefilename);
  }
  else
  {
    rtscheck.RTS_SndData(CardRecbuf.Cardshowfilename[CardRecbuf.recordcount], Choosefilename);
  }

  for(int j = 0;j < 8;j++)
  {
    rtscheck.RTS_SndData(0,FilenameCount+j);
  }
  char buf[20];
  memset(buf,0,sizeof(buf));
  sprintf(buf,"%d/%d",index, CardRecbuf.Filesum);
  rtscheck.RTS_SndData(buf, FilenameCount);
  delay(2);
  for(int j = 1;j <= CardRecbuf.Filesum;j++)
  {
    rtscheck.RTS_SndData((unsigned long)0xFFFF,FilenameNature + j*16);
    rtscheck.RTS_SndData(10,FilenameIcon1+j);
  }
  rtscheck.RTS_SndData((unsigned long)0x87F0,FilenameNature + index*16);
  rtscheck.RTS_SndData(6,FilenameIcon1 + index);
}

void dwin_send_filename() {
  int filelen = strlen(CardRecbuf.Cardshowfilename[CardRecbuf.recordcount]);
  filelen = (TEXTBYTELEN - filelen)/2;
  SERIAL_ECHOPAIR("\n ***= f =", CardRecbuf.Cardshowfilename[CardRecbuf.recordcount]);
  if(filelen > 0)
  {
    char buf[20];
    memset(buf,0,sizeof(buf));
    strncpy(buf,"         ",filelen);
    strcpy(&buf[filelen], CardRecbuf.Cardshowfilename[CardRecbuf.recordcount]);
    rtscheck.RTS_SndData(buf, Printfilename);
  }
  else
  {
    rtscheck.RTS_SndData(CardRecbuf.Cardshowfilename[CardRecbuf.recordcount], Printfilename);
  }
}

void dwin_start_print(const char *window_to_nav_to) {
  char cmd[30];
  char* c;
  if (!card.isMounted()) {
    // TODO update no card icon
    return;
  }
  if (CardRecbuf.recordcount < 0) {
    // TODO icon
    return;
  }

  #if CHECKFILEMENT
    /**************checking filement status during printing beginning ************/
    if (runout.filament_ran_out)
      return;
  #endif

  sprintf_P(cmd, PSTR("M23 %s"), CardRecbuf.Cardfilename[CardRecbuf.recordcount]);
  for (c = &cmd[4]; *c; c++) *c = tolower(*c);

  //int filecount = CardRecbuf.recordcount;
  char cmdbuf[20] = {0};
  memset(cmdbuf,0,sizeof(cmdbuf));
  strcpy(cmdbuf,cmd);

  queue.enqueue_one_now(cmd);
  queue.enqueue_now_P(PSTR("M24"));
  
  dwin_send_filename();

  delay(2);

  #if FAN_COUNT > 0
    for (uint8_t i = 0; i < FAN_COUNT; i++) thermalManager.set_fan_speed(i, FanOn);
  #endif

  // TODO set heating icon
  // TODO print mode icon
  if (window_to_nav_to)
    ui_window_change_window(window_to_nav_to);
}


void dwin_pause_print() {
  pause_z = current_position[Z_AXIS];
  pause_e = current_position[E_AXIS] - 10;
  card.pauseSDPrint();
  print_job_timer.pause();
  if(!temphot)
    temphot = thermalManager.degTargetHotend(0);
  if(!tempbed)
    tempbed = thermalManager.degTargetBed();
  // thermalManager.setTargetHotend(0, 0);
  thermalManager.setTargetBed(0);
  queue.enqueue_one_P(PSTR("G28 X0 Y0"));
}

void dwin_resume_print() {
  // TODO check filament
  char pause_str_Z[16];
  char pause_str_E[16];
  memset(pause_str_Z, 0, sizeof(pause_str_Z));
  dtostrf(pause_z, 3, 2, pause_str_Z);
  memset(pause_str_E, 0, sizeof(pause_str_E));
  dtostrf(pause_e, 3, 2, pause_str_E);

  memset(commandbuf,0,sizeof(commandbuf));
  sprintf_P(commandbuf, PSTR("M190 S%i"), tempbed);
  queue.enqueue_one_now(commandbuf);
  memset(commandbuf,0,sizeof(commandbuf));
  sprintf_P(commandbuf, PSTR("M109 S%i"), temphot);
  queue.enqueue_one_now(commandbuf);
  memset(commandbuf,0,sizeof(commandbuf));
  sprintf_P(commandbuf, PSTR("G0 Z%s"), pause_str_Z);
  queue.enqueue_one_now(commandbuf);
  sprintf_P(commandbuf, PSTR("G92 E%s"), pause_str_E);
  queue.enqueue_one_now(commandbuf);
  tempbed = 0;
  temphot = 0;

  card.startFileprint();
  print_job_timer.start();

  // TODO print status icon?
}

void dwin_stop_print() {
  tempbed = 0;
  temphot = 0;
  rtscheck.RTS_SDcard_Stop();
}

uint8_t dwin_percent_done() {
  return card.percentDone();
}

// material selection
typedef struct material {
  float hotend_temp;
  float bed_temp;
  material_type type;
} material;


const material _materials[] ={
  { PREHEAT_1_TEMP_HOTEND, PREHEAT_1_TEMP_BED, Pla },
  { PREHEAT_2_TEMP_HOTEND, PREHEAT_2_TEMP_BED, Abs },
  { 220, 60, Petg }
};

static material *_current_material = (material *)&_materials[0];

material_type dwin_get_material_type(material *mat) {
  return mat->type;
}

float dwin_get_material_current_hotend_temp() {
  return _current_material->hotend_temp;
}

float dwin_get_material_hotend_temp(material *mat) {
  return mat->hotend_temp;
}

void  dwin_set_material_hotend_temp(material *mat, float temp) {
  mat->hotend_temp = temp;
}

float dwin_get_material_bed_temp(material *mat) {
  return mat->bed_temp;
}

void  dwin_set_material_bed_temp(material *mat, float temp) {
  mat->bed_temp = temp;
}

material *dwin_get_current_material() {
  return _current_material;
}

void dwin_set_current_material(material *mat) {
  _current_material = mat;
}

material *dwin_get_material(material_type mat_type) {
  for (unsigned int i = 0; i < (sizeof(_materials) / sizeof(material)); i++) {
    material *m = (material *)&_materials[i];
    if (m->type == mat_type)
      return m;
  }
  return NULL;
}


/** UI **/

int _ui_window_count;
static const dwin_window *_windows;
static dwin_window *_current_window;

void ui_register_windows(const dwin_window *windows, int window_count)
{
    _windows = windows;
    _ui_window_count = window_count;
    _current_window = (dwin_window *)windows;
}

dwin_window *ui_window_get_by_name(const char *name)
{
    for (int i = 0; i < _ui_window_count; i++)
    {
        dwin_window *w = (dwin_window *)&_windows[i];
        if (strncmp(w->name, name, 10) == 0)
        {
            return w;
        }
    }
    return NULL;
}

dwin_window *ui_window_get_current_window()
{
    return _current_window;
}

void ui_window_change_window(const char *name)
{
    ui_window_change_window(name, false);
}

void ui_window_change_window(const char *name, bool dont_navigate)
{
    dwin_window *window = ui_window_get_by_name(name);
    if (!window)
        return;
    int pageid = window->window_ids[dwin_get_language()];
    SERIAL_ECHOPAIR("\n *** Windowa =",window->name);
    // dtor the current window
    //_current_window->deinit();
    if (window->init)
        window->init(window, NULL);
    _current_window = window;

    if (!dont_navigate)
        rtscheck.RTS_SndData(ExchangePageBase + pageid, ExchangepageAddr);
}

static bool _ui_locked = false;

void ui_input_lock()
{
    _ui_locked = true;
}

void ui_input_unlock()
{
    _ui_locked = false;
}

bool ui_input_is_locked()
{
    return _ui_locked;
}



extern CardReader card;
bool lcd_sd_status;
RTSSHOW rtscheck;

RTSSHOW::RTSSHOW()
{
  recdat.head[0] = snddat.head[0] = FHONE;
  recdat.head[1] = snddat.head[1] = FHTWO;
  //memset(databuf,0, sizeof(databuf));
}

void RTSSHOW::RTS_SDCardInit(void)
{
  if(card.isMounted())
  {
    SERIAL_ECHOLN("***Initing card is OK***");
    uint16_t fileCnt = card.countFilesInWorkDir();
    card.getWorkDirName();
    if (card.filename[0] == '/') 
    {
      card.cdroot();
    }
    else 
    {
      card.cdup();
    }

    int addrnum =0;
    int num = 0;
    for (uint16_t i = 0; i < fileCnt && i < MaxFileNumber + addrnum; i++) {
      card.selectFileByIndex(fileCnt-1-i);
      char *pointFilename = card.longFilename;
      int filenamelen = strlen(card.longFilename);
      int j = 1;
      while((strncmp(&pointFilename[j],".gcode",6) && strncmp(&pointFilename[j],".GCODE",6)) && (j++) < filenamelen);
      if(j >= filenamelen)
      {
        addrnum++;
        continue;
      }

      if(j >= TEXTBYTELEN)	
      {
        strncpy(&card.longFilename[TEXTBYTELEN -3],"~~",2);
        card.longFilename[TEXTBYTELEN-1] = '\0';
        j = TEXTBYTELEN-1;
      }

      delay(3);
      strncpy(CardRecbuf.Cardshowfilename[num], card.longFilename,j);

      strcpy(CardRecbuf.Cardfilename[num], card.filename);
      CardRecbuf.addr[num] = SDFILE_ADDR +num*10;
      RTS_SndData(CardRecbuf.Cardshowfilename[num],CardRecbuf.addr[num]);
      CardRecbuf.Filesum = (++num);
      RTS_SndData(1, FilenameIcon + CardRecbuf.Filesum);
    }
    if (dwin_get_language() == 1) {
      // 0 for Ready
      RTS_SndData(0,IconPrintstatus);
	  }
    else
    {
      RTS_SndData(0 + CEIconGrap,IconPrintstatus);
    }
    lcd_sd_status = card.isMounted();
  }
  else
  {
    SERIAL_ECHOLN("***Initing card fails***");
	  if (dwin_get_language() == 1) {
      // 6 for Card Removed
      RTS_SndData(6,IconPrintstatus);
  	}
    else {
      RTS_SndData(6+CEIconGrap,IconPrintstatus);
    }
  }
}

void RTSSHOW::RTS_SDCardUpate(void)
{	
  
  const bool sd_status = card.isMounted();
  if (sd_status != lcd_sd_status)
  {
    if (sd_status)
    {
      card.mount();
      RTS_SDCardInit();
    }
    else
    {
      card.release();
	    // heating or printing
      if(thermalManager.isHeatingHotend(0) || thermalManager.isHeatingBed()) {
        RTS_SDcard_Stop();
      }

      if(dwin_get_language() != 0) {
        // 6 for Card Removed
        RTS_SndData(6,IconPrintstatus);
      }
      else {
        RTS_SndData(6+CEIconGrap,IconPrintstatus);
      }

      for(int i = 0; i < CardRecbuf.Filesum; i++) {
        for(int j = 0;j < 10;j++)
        RTS_SndData(0,CardRecbuf.addr[i]+j);
        
        // white
        RTS_SndData((unsigned long)0xFFFF, FilenameNature + (i+1)*16);
      }

      for(int j = 0;j < 10;j++)	{
        // clean screen.
        //RTS_SndData(0,Printfilename+j);
        // clean filename
        RTS_SndData(0,Choosefilename+j);
      }

      for(int j = 0;j < 8;j++) {
        RTS_SndData(0,FilenameCount+j);
      }

      // clean filename Icon
      for(int j = 1;j <= 20;j++) {
        RTS_SndData(10,FilenameIcon+j);
        RTS_SndData(10,FilenameIcon1+j);
      }
      memset(&CardRecbuf,0,sizeof(CardRecbuf));
    }
    lcd_sd_status = sd_status;

    RTS_SDCardInit();
  }
}

int RTSSHOW::_handle_packet() {
  if(recdat.len == 0x03 && (recdat.command == VarAddr_W || recdat.command == RegAddr_W) 
      && (recdat.cdata.data[0] == 'O') && (recdat.cdata.data[1] == 'K')) {    //response for writing byte 
    return -1;
  }
  else if(recdat.command == VarAddr_R) {
    // response for reading the data from the variate
    recdat.addr = recdat.cdata.data[0];
    recdat.addr = (recdat.addr << 8 ) | recdat.cdata.data[1];
    recdat.bytelen = recdat.cdata.data[2];

    for(unsigned long i = 0; i < recdat.bytelen; i += 2) {
      recdat.cdata.data[i / 2] = recdat.cdata.data[3 + i];
      recdat.cdata.data[i / 2] = (recdat.cdata.data[i / 2] << 8 ) | recdat.cdata.data[4 + i];
    }
  }
  else if(recdat.command == RegAddr_R) {
    // response for reading the page from the register
    recdat.addr = recdat.cdata.data[0];
    recdat.bytelen = recdat.cdata.data[1];

    for(unsigned long i = 0; i < recdat.bytelen; i++) {
        recdat.cdata.data[i] = recdat.cdata.data[2 + i];
    }
  }

  return 1;
}

int RTSSHOW::RTS_RecData() {
  static int recv_cnt = 0;
  static uint8_t _recv_state = 0;
  /*
  static millis_t _next_rts_update_ms = 0;
  const millis_t ms = millis();

  if (PENDING(ms, _next_rts_update_ms)) 
    return - 2;
  SERIAL_ECHOPAIR(" *** Tick ", _next_rts_update_ms);
  _next_rts_update_ms = ms + 20;
*/
// I have no idea here, but if we don't do this delay and instead rely on
// any kind of pending timer logic, we don't get any bytes in available()
// it offends me greatly
// i wonder if something is globally disabling interrupts and we only get the data in the delay
// becuase we are halting all execution. HMMMMMM
  delay(20);
  while (LCD_SERIAL.available())
  {
      uint8_t d = LCD_SERIAL.read();
      SERIAL_ECHOPAIR(" *** D ", d);
      SERIAL_ECHOPAIR(" *** S ", _recv_state);
      if (_recv_state == 0) {
        // match first header byte
        if (d != FHONE) {
          recv_cnt = 0;
          return -1;
        }

        _recv_state = 1;
      }
      else if (_recv_state == 1) {
        // match second header byte or 0 for an OK message
        if (d != FHTWO && d != 0) {
          recv_cnt = 0;
          _recv_state = 0;
          return -1;
        }

        _recv_state = 2;
      }
      // Len. We got the header. next up if length
      else if (_recv_state == 2) {
        recdat.len = d;
        _recv_state = 3;
      }
      // command byte
      else if (_recv_state == 3) {
        recdat.command = d;
        _recv_state = 4;
      }
      // data payload next
      else if (_recv_state == 4) {
        recdat.cdata.data[recv_cnt] = d;
        recv_cnt++;

        if (recv_cnt >= recdat.len - 1) {
          // done
          _handle_packet();
          _recv_state = 0;
          recv_cnt = 0;
          return 1;
        }
      }
  }

  return 0;
}

void RTSSHOW::RTS_SndData(void)
{
  if((snddat.head[0] == FHONE) && (snddat.head[1] == FHTWO) && snddat.len >= 3)
  {
    databuf[0] = snddat.head[0];
    databuf[1] = snddat.head[1];
    databuf[2] = snddat.len;
    databuf[3] = snddat.command;
	// to write data to the register
    if(snddat.command ==0x80)
    {
      databuf[4] = snddat.addr;
      for(int i =0;i <(snddat.len - 2);i++)
      {
        databuf[5+i] = snddat.cdata.data[i];
      }
    }
    else if(snddat.len == 3 && (snddat.command ==0x81))
    {
      // to read data from the register
      databuf[4] = snddat.addr;
      databuf[5] = snddat.bytelen;
    }
    else if(snddat.command ==0x82)
    {
      // to write data to the variate
      databuf[4] = snddat.addr >> 8;
      databuf[5] = snddat.addr & 0xFF;
      for(int i =0;i <(snddat.len - 3);i += 2)
      {
        databuf[6 + i] = snddat.cdata.data[i/2] >> 8;
        databuf[7 + i] = snddat.cdata.data[i/2] & 0xFF;
      }
    }
    else if(snddat.len == 4 && (snddat.command ==0x83))
    {
      // to read data from the variate
      databuf[4] = snddat.addr >> 8;
      databuf[5] = snddat.addr & 0xFF;
      databuf[6] = snddat.bytelen;
    }
    for(int i = 0;i < (snddat.len + 3);i++)
    {
      LCD_SERIAL.write(databuf[i]);
      delayMicroseconds(1);
    }

    memset(&snddat,0,sizeof(snddat));
    memset(databuf,0, sizeof(databuf));
    snddat.head[0] = FHONE;
    snddat.head[1] = FHTWO;
  }
}

void RTSSHOW::RTS_SndData(const String &s, unsigned long addr, unsigned char cmd /*= VarAddr_W*/)
{
  if(s.length() < 1)
  {
    return;
  }
  RTS_SndData(s.c_str(), addr, cmd);
}

void RTSSHOW::RTS_SndData(const char *str, unsigned long addr, unsigned char cmd/*= VarAddr_W*/)
{
  int len = strlen(str);
  if( len > 0)
  {
    databuf[0] = FHONE;
    databuf[1] = FHTWO;
    databuf[2] = 3+len;
    databuf[3] = cmd;
    databuf[4] = addr >> 8;
    databuf[5] = addr & 0x00FF;
    for(int i =0;i <len ;i++)
    {
      databuf[6 + i] = str[i];
    }

    for(int i = 0;i < (len + 6);i++)
    {
      LCD_SERIAL.write(databuf[i]);
      delayMicroseconds(1);
    }
    memset(databuf,0, sizeof(databuf));
  }
}

void RTSSHOW::RTS_SndData(char c, unsigned long addr, unsigned char cmd/*= VarAddr_W*/)
{
  snddat.command = cmd;
  snddat.addr = addr;
  snddat.cdata.data[0] = (unsigned long)c;
  snddat.cdata.data[0] = snddat.cdata.data[0] << 8;
  snddat.len = 5;
  RTS_SndData();
}

void RTSSHOW::RTS_SndData(unsigned char* str, unsigned long addr, unsigned char cmd){RTS_SndData((char *)str, addr, cmd);}

void RTSSHOW::RTS_SndData(int n, unsigned long addr, unsigned char cmd/*= VarAddr_W*/)
{
  if(cmd == VarAddr_W )
  {
    if(n > 0xFFFF)
    {
      snddat.cdata.data[0] = n >> 16;
      snddat.cdata.data[1] = n & 0xFFFF;
      snddat.len = 7;
    }
    else
    {
      snddat.cdata.data[0] = n;
      snddat.len = 5;
    }
  }
  else if(cmd == RegAddr_W)
  {
    snddat.cdata.data[0] = n;
    snddat.len = 3;
  }
  else if(cmd == VarAddr_R)
  {
    snddat.bytelen = n;
    snddat.len = 4;
  }
  snddat.command = cmd;
  snddat.addr = addr;
  RTS_SndData();
}

void RTSSHOW::RTS_SndData(unsigned int n, unsigned long addr, unsigned char cmd){ RTS_SndData((int)n, addr, cmd); }

void RTSSHOW::RTS_SndData(float n, unsigned long addr, unsigned char cmd){ RTS_SndData((int)n, addr, cmd); }

void RTSSHOW::RTS_SndData(long n, unsigned long addr, unsigned char cmd){ RTS_SndData((unsigned long)n, addr, cmd); }

void RTSSHOW::RTS_SndData(unsigned long n, unsigned long addr, unsigned char cmd/*= VarAddr_W*/)
{
  if(cmd == VarAddr_W )
  {
    if(n > 0xFFFF)
    {
      snddat.cdata.data[0] = n >> 16;
      snddat.cdata.data[1] = n & 0xFFFF;
      snddat.len = 7;
    }
    else
    {
      snddat.cdata.data[0] = n;
      snddat.len = 5;
    }
  }
  else if(cmd == VarAddr_R)
  {
    snddat.bytelen = n;
    snddat.len = 4;
  }
  snddat.command = cmd;
  snddat.addr = addr;
  RTS_SndData();
}


void RTSSHOW::RTS_SDcard_Stop() {
  card.endFilePrint();
  card.flag.abort_sd_printing = true;
  
  quickstop_stepper();
  print_job_timer.stop();
  thermalManager.disable_all_heaters();
  print_job_timer.reset();
  #if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
    card.openPowerOffFile(power_off_info.power_off_filename, O_CREAT | O_WRITE | O_TRUNC | O_SYNC);
    power_off_info.valid_head = 0;

    power_off_info.valid_foot = 0;
    if (card.savePowerOffInfo(&power_off_info, sizeof(power_off_info)) == -1)
    {
      SERIAL_PROTOCOLLN("Stop to Write power off file failed.");
    }
    card.closePowerOffFile();
    power_off_commands_count = 0;
  #endif

  #if FAN_COUNT > 0
    for (uint8_t i = 0; i < FAN_COUNT; i++) 
        thermalManager.fan_speed[i] = 0;
  #endif
  
  // shut down the stepper motor.
  // enqueue_and_echo_commands_P(PSTR("M84"));
  RTS_SndData(11, FilenameIcon); 
  
  // for system

  // delay(1000);
  // RTS_SndData(0,Timehour);
  // delay(2);
  // RTS_SndData(0,Timemin);
  // delay(2);
  RTS_SndData(0,PrintscheduleIcon);
  RTS_SndData(0,PrintscheduleIcon+1);
  RTS_SndData(0,Percentage);
  delay(2);
  for(int j = 0;j < 10;j++)	
  {
    // clean screen.
    RTS_SndData(0,Printfilename+j);
	// clean filename
    RTS_SndData(0,Choosefilename+j);
  }
  for(int j = 0;j < 8;j++)
  {
    RTS_SndData(0,FilenameCount+j);
  }
  
}

void RTSSHOW::RTS_HandleData() {
    dwin_window *w = _current_window;
    for(int i = 0; i < w->element_count; i++) {
        const dwin_ui_element *el = &w->elements[i];
        if (!el)
            continue;
        
        if (el->type != ElementButton)
            continue;

        if (el->element_id == recdat.addr)
        {
            if (el->key_code == 0xFF || recdat.cdata.data[0] == (uint8_t)el->key_code)
            {
                if (el->callback)
                    el->callback((dwin_ui_element *)el, (void *)&recdat.cdata.data[0]);

                memset(&recdat,0 , sizeof(recdat));
                recdat.head[0] = FHONE;
                recdat.head[1] = FHTWO;
                return;
            }
        }
    }
    SERIAL_ECHOPAIR("\n *** DWIN RECV UNHANDLED =", recdat.addr);
    SERIAL_ECHOPAIR("\n *** DWIN RECV UNHANDLED KC =", recdat.cdata.data[0]);
    memset(&recdat,0 , sizeof(recdat));
    recdat.head[0] = FHONE;
    recdat.head[1] = FHTWO;
}

#endif