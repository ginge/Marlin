#pragma once
#include <Arduino.h>
#include "../../module/motion.h"
#include "../../feature/babystep.h"
#if HAS_BED_PROBE
  #include "../../module/probe.h"
#endif
#if HAS_LEVELING
  #include "../../feature/bedlevel/bedlevel.h"
#endif

#define LCD_WIDTH 480
#define LCD_HEIGHT 272
#define CHECKFILEMENT true
#define MAX_WINDOWS 10
#define MAX_LANG 2 // 0 English, 1 Chinese


enum dwin_ui_pages {
    Boot,
    Home,
};

struct dwin_window;
struct dwin_ui_element;

typedef void (*ui_element_cb)(dwin_ui_element *element, void *context);

enum dwin_ui_element_type {
    ElementInput,
    ElementButton,
    ElementIcon
};

typedef struct dwin_ui_element {
    unsigned long element_id;
    uint8_t key_code;
    dwin_ui_element_type type;
    const char * default_value;
    ui_element_cb callback;
    void * context;
} dwin_ui_element;

typedef void (*ui_window_cb)(dwin_window *window, void *context);

typedef struct dwin_window {
    int window_ids[MAX_LANG];
    const dwin_ui_element *elements;
    int element_count;
    const char *name;
    ui_window_cb init;
    ui_window_cb draw;
} dwin_window;



void dwin_loop();
void dwin_lcd_model_init(void);
void dwin_init(void);
void dwin_send(const dwin_ui_element *element, int value);
void dwin_send(const dwin_ui_element *element, const char *value);

uint8_t dwin_init_language(void);
uint8_t dwin_get_language();
void dwin_set_language(uint8_t lang_id);

uint8_t dwin_init_auto_level_status();
void dwin_auto_level_start(const char *page_name_to_display);
void dwin_set_probe_offset(float z);

uint8_t dwin_get_auto_level_status();
void dwin_set_auto_level_status(bool is_enabled);
void dwin_auto_level_complete();

void dwin_init_volume();
uint8_t dwin_get_volume();
void dwin_set_volume(uint8_t volume);
void dwin_settings_load();

bool dwin_get_fan();
void dwin_set_fan(bool enabled);

void dwin_plan_current_position(AxisEnum axis);

void dwin_homing_complete();
void dwin_set_homing(bool is_homing);
bool dwin_get_homing();
void dwin_home(const char *page_name_to_display, const char *page_name_to_return_to, bool absolute);
void dwin_home(const char *page_name_to_display, const char *page_name_to_return_to);
void dwin_home(const char *page_name_to_display);

void ui_register_windows(const dwin_window *windows, int window_count);
dwin_window *ui_window_get_by_name(const char *name);
dwin_window *ui_window_get_current_window();

void ui_input_lock();
void ui_input_unlock();
bool ui_input_is_locked();
void ui_window_change_window(const char *name);
void ui_window_change_window(const char *name, bool dont_navigate);

void dwin_filament_was_removed();
void dwin_start_print(const char *window_to_nav_to);
void dwin_print_file_selected(int index);
bool dwin_is_paused();
void dwin_pause_print();
void dwin_stop_print();
void dwin_resume_print();
uint8_t dwin_percent_done();
void dwin_send_filename();

// wrapper
void DWIN_Update(void);
void HMI_Init(void);
void DWIN_CompletedHoming(void);
void DWIN_CompletedLeveling(void);
void HMI_StartFrame(const bool with_update); // startup screen
void DWIN_SetHomeFlag(bool flag);
void DWIN_SetHeatFlag(bool flag, millis_t heat_time);
void DWIN_FilamentRunout();
void Popup_Window_Temperature(const bool toohigh);


/*********************************/
#define FHONE   (0x5A)
#define FHTWO   (0xA5)
#define TEXTBYTELEN     18
#define MaxFileNumber   20

#define	CEIconGrap      12
#define	FileNum	MaxFileNumber
#define	FileNameLen	TEXTBYTELEN

#define SizeofDatabuf		40

/*************Register and Variable addr*****************/
#define	RegAddr_W	0x80
#define	RegAddr_R	0x81
#define	VarAddr_W	0x82
#define	VarAddr_R	0x83
#define	ExchangePageBase    ((unsigned long)0x5A010000)     // the first page ID. other page = first page ID + relevant num;
#define	StartSoundSet       ((unsigned long)0x060480A0)     // 06,start-music; 04, 4 musics; 80, the volume value; 04, return value about music number.

#define	FanOn            255
#define	FanOff           0

/*variable addr*/
#define	ExchangepageAddr	0x0084
#define	SoundAddr			0x00A0
#define	StartIcon			0x1000
#define	StartIcon1			0x1001
#define	FeedrateDisplay		0x1006
#define	Stopprint			0x1008
#define	Pauseprint			0x100A
#define	Resumeprint			0x100C
#define	PrintscheduleIcon	0x100E
#define	Timehour			0x1010
#define	Timemin				0x1012
#define	IconPrintstatus		0x1014
#define	Percentage			0x1016
#define	FanKeyIcon			0x101E

#define	HeatPercentIcon		0x1024

#define	NzBdSet				0x1032
#define	NozzlePreheat		0x1034
#define	NozzleTemp			0x1036
#define	BedPreheat			0x103A
#define	Bedtemp				0x103C

#define	AutoZeroIcon		0x1042
#define	AutoLevelMode		0x1045
#define	AutoZero			0x1046
#define	DisplayXaxis		0x1048
#define	DisplayYaxis		0x104A
#define	DisplayZaxis		0x104C

#define	FilementUnit1		0x1054
#define	Exchfilement		0x1056
#define	FilementUnit2		0x1058

#define	MacVersion			0x1060
#define	SoftVersion			0x106A
#define	PrinterSize			0x1074
#define	CorpWebsite			0x107E
#define	VolumeIcon			0x108A
#define	SoundIcon			0x108C
#define	AutolevelIcon		0x108D
#define	ExchFlmntIcon		0x108E
#define	AutolevelVal		0x1100

#define	FilenameIcon		0x1200
#define	FilenameIcon1		0x1220
#define	Printfilename		0x2000
#define	SDFILE_ADDR			0x200A
#define	FilenamePlay		0x20D2
#define	FilenameChs			0x20D3
#define	Choosefilename		0x20D4
#define	FilenameCount		0x20DE
#define	FilenameNature		0x6003

/************struct**************/
typedef struct DataBuf
{
    unsigned char len;  
    unsigned char head[2];
    unsigned char command;
    unsigned long addr;
    unsigned long bytelen;
    union cdata {
      uint16_t data[32];
      uint8_t bdata[64];
    } cdata;
    unsigned char reserv[4];
} DB;

typedef struct CardRecord
{
    uint8_t recordcount;
    int Filesum;
    unsigned long addr[FileNum];
    char Cardshowfilename[FileNum][FileNameLen];
    char Cardfilename[FileNum][FileNameLen];
}CRec;

class RTSSHOW {
  public:
    RTSSHOW();
    int RTS_RecData();
    void RTS_SDCardInit(void);
    void RTS_SDCardUpate(void);
    int RTS_CheckFilement(int);
    void RTS_SndData(void);
    void RTS_SndData(const String &, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(const char[], unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(char, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(unsigned char*, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(int, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(float, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(unsigned int,unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(long,unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(unsigned long,unsigned long, unsigned char = VarAddr_W);
    void RTS_SDcard_Stop();
    int _handle_packet();
    void RTS_HandleData();
    void RTS_Init();
    
    DB recdat;
    DB snddat;
    unsigned char databuf[SizeofDatabuf];
  private:
    
  };

extern RTSSHOW rtscheck;


extern void RTSUpdate();
extern void RTSInit();
