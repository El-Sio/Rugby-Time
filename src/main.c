
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "http.h"
#include "util.h"

#define ANDROID false
 
#if ANDROID
#define MY_UUID { 0xA3, 0x4E, 0x23, 0x60, 0xF2, 0x77, 0x4E, 0xF2, 0x99, 0x2D, 0xF1, 0x1E, 0xA7, 0xCB, 0xEF, 0x10 }
#else
#define MY_UUID HTTP_UUID
#endif

#define HTTP_COOKIE 42691234	
	
//PBL_APP_INFO_SIMPLE(MY_UUID, "Rugby Time", "CANAL+", 1 /* App version */);
PBL_APP_INFO(MY_UUID, "Rugby Time", "CANAL+", 1, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_STANDARD_APP);


//Declarations :

void handle_init(AppContextRef ctx);
void http_success(int32_t request_id, int http_status, DictionaryIterator* received, void* context);
void http_failure(int32_t request_id, int http_status, void* context);
void window_appear(Window* me);
void window_appear2(Window* me);
void window_appear3(Window* me);
void httpebble_error(int error_code);
void click_config_provider(ClickConfig **config, Window *window);
void click_config_provider2(ClickConfig **config, Window *window);
void click_config_provider3(ClickConfig **config, Window *window);

/*ActionBarLayer *main_action_bar;
ActionBarLayer *ranking_action_bar;
ActionBarLayer *result_action_bar;*/

Window window;
Window resultwindow;
Window rankingwindow;
TextLayer layer_text1;
TextLayer layer_text2;
TextLayer layer_text3;
TextLayer layer_text4;
static int nbjournee=1;
static int nummatch=1;
static int numrang =1;


//Handles all API returns
// Shoud probably split this into different functions...

void http_success(int32_t request_id, int http_status, DictionaryIterator* received, void* context) {
  if (request_id != HTTP_COOKIE) {
    return;
  }
text_layer_set_text(&layer_text1,"success !");
text_layer_set_text(&layer_text3,"No Data");
text_layer_set_text(&layer_text4,"No Data");
text_layer_set_text(&layer_text2, "No Data");
Tuple* tuple1 = dict_find(received, 1);
Tuple* tuple2 = dict_find(received, 2);

	//Case for Ranking
	if(strcmp(tuple1->value->cstring,"B")==0)
	{
		Tuple* tuple3 = dict_find(received, 3);
		text_layer_set_text(&layer_text1,tuple3->value->cstring);
		text_layer_set_text(&layer_text3,"at day :");
		text_layer_set_text(&layer_text4,itoa(nbjournee));
		text_layer_set_text(&layer_text2, tuple2->value->cstring);
	}
	
	//Case for Results
	if(strcmp(tuple1->value->cstring,"A")==0)
	{
	
		Tuple* tuple3 = dict_find(received, 3);
		Tuple* tuple4 = dict_find(received, 4);
		Tuple* tuple5 = dict_find(received, 5);
	
		text_layer_set_text(&layer_text1,tuple2->value->cstring);
		text_layer_set_text(&layer_text3,tuple3->value->cstring);
		text_layer_set_text(&layer_text2,tuple4->value->cstring);
		text_layer_set_text(&layer_text4, tuple5->value->cstring);
	}

}

//Handles HTTP Error codes (see below for code correspondance)

void http_failure(int32_t request_id, int http_status, void* context) {
  httpebble_error(http_status >= 1000 ? http_status - 1000 : http_status);
}

//Windows Handlers

void window_disappear(Window* me) {
	//Not sure this is usefull
	window_set_click_config_provider(me, (ClickConfigProvider) NULL);
}

void window_disappear2(Window* me) {
	//Not sure this is usefull
	window_set_click_config_provider(me, (ClickConfigProvider) NULL);
}

void window_disappear3(Window* me) {
	//Not sure this is usefull
	window_set_click_config_provider(me, (ClickConfigProvider) NULL);
}

void window_load(Window* me) {

  /*
  
  Trying to figure out Action Bar Layer...
  This crashes the Pebble !
  
  action_bar_layer_init(main_action_bar);
  action_bar_layer_add_to_window(main_action_bar, me);
  action_bar_layer_set_click_config_provider(main_action_bar, (ClickConfigProvider) click_config_provider);	
  */  	
}

void window_appear(Window* me) {

  window_set_click_config_provider(me, (ClickConfigProvider) click_config_provider);
  text_layer_init(&layer_text1, GRect(0, 10, 144, 30));
  text_layer_set_text_color(&layer_text1, GColorBlack);
  text_layer_set_background_color(&layer_text1, GColorClear);
  text_layer_set_font(&layer_text1, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(&layer_text1, GTextAlignmentCenter);
  text_layer_set_text(&layer_text1, "Long click: results");	
  layer_add_child(&window.layer, &layer_text1.layer);

  text_layer_init(&layer_text2, GRect(0, 110, 144, 30));
  text_layer_set_text_color(&layer_text2, GColorBlack);
  text_layer_set_background_color(&layer_text2, GColorClear);
  text_layer_set_font(&layer_text2, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(&layer_text2, GTextAlignmentCenter);
  text_layer_set_text(&layer_text2, itoa(nbjournee));
  layer_add_child(&window.layer, &layer_text2.layer);

 text_layer_init(&layer_text3, GRect(0, 40, 144, 30));
  text_layer_set_text_color(&layer_text3, GColorBlack);
  text_layer_set_background_color(&layer_text3, GColorClear);
  text_layer_set_font(&layer_text3, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(&layer_text3, GTextAlignmentCenter);
  text_layer_set_text(&layer_text3, "Short click: standings");
  layer_add_child(&window.layer, &layer_text3.layer);
	
  text_layer_init(&layer_text4, GRect(0, 70, 144, 30));
  text_layer_set_text_color(&layer_text4, GColorBlack);
  text_layer_set_background_color(&layer_text4, GColorClear);
  text_layer_set_font(&layer_text4, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(&layer_text4, GTextAlignmentCenter);
  text_layer_set_text(&layer_text4, "Select Match Day :");
  layer_add_child(&window.layer, &layer_text4.layer);
	
}

void window_appear2(Window* me) {
	
  window_set_click_config_provider(me, (ClickConfigProvider) click_config_provider2);
  layer_add_child(&resultwindow.layer, &layer_text1.layer);
  layer_add_child(&resultwindow.layer, &layer_text2.layer);
  layer_add_child(&resultwindow.layer, &layer_text3.layer);
  layer_add_child(&resultwindow.layer, &layer_text4.layer);
}

void window_appear3(Window* me) {
	
  window_set_click_config_provider(me, (ClickConfigProvider) click_config_provider3);
  layer_add_child(&rankingwindow.layer, &layer_text1.layer);
  layer_add_child(&rankingwindow.layer, &layer_text2.layer);
  layer_add_child(&rankingwindow.layer, &layer_text3.layer);
  layer_add_child(&rankingwindow.layer, &layer_text4.layer);
}

//API call to get match results for Match Day "journee",returns match N° "numeromatch" and allows to cycle through all 7 matches of the day.
//Call format is {"1":matchday,"2":"match","3":match number}
//Return format is {"1":"A",2":"home team","3":"home score","4":"away score","5":"away team"}

void get_match(int journee, int numeromatch) {
	
  DictionaryIterator* dict;
  HTTPResult  result = http_out_get("http://japansio.info/api/testAPI.php", HTTP_COOKIE, &dict);
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
  
  dict_write_int32(dict, 1, journee);
  dict_write_cstring(dict, 2, "match");
  dict_write_int32(dict, 3, numeromatch);

  result = http_out_send();
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
}

//API call to get ranking for match Day "journee", returns rank "rang" for the Day and allows to cycle through all 14 teams.
//Call format is {"1":matchday,"2":"","3":rank}
//Return format is {"1":"B",2":"Team Name","3":"rank"}

void get_result(int journee, int rang) {
	
  DictionaryIterator* dict;
  HTTPResult  result = http_out_get("http://japansio.info/api/testAPI.php", HTTP_COOKIE, &dict);
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
  
  dict_write_int32(dict, 1, journee);
  dict_write_cstring(dict, 2, "");
  dict_write_int32(dict, 3, rang);

  result = http_out_send();
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
}

// Button handlers (one for each window)

//main window Up : increase and display Matchday
void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
text_layer_set_text(&layer_text3, "Short click : rankings");
text_layer_set_text(&layer_text1, "Long click : results");
text_layer_set_text(&layer_text4, "Select Match Day :");
	nbjournee +=1;
	if(nbjournee>26) {nbjournee =26;}
	text_layer_set_text(&layer_text2, itoa(nbjournee));
}

// result window Up : ask for next match result (cycles from 0 to 6)
void up_single_click_handler2(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)resultwindow;
	nummatch +=1;
	text_layer_set_text(&layer_text1,"loading");
	text_layer_set_text(&layer_text3,"next");
	text_layer_set_text(&layer_text4,"game");
	text_layer_set_text(&layer_text2, "result");
	if(nummatch>6) {nummatch =0;}
	get_match(nbjournee,nummatch);
}

// ranking window Up : ask for next in ranking (cycles from 1 to 14)
void up_single_click_handler3(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)rankingwindow;
	numrang +=1;
	if(numrang>14) {numrang =1;}
	text_layer_set_text(&layer_text1,"next");
	text_layer_set_text(&layer_text3,"at day :");
	text_layer_set_text(&layer_text4,"loading");
	text_layer_set_text(&layer_text2, "loading");
	get_result(nbjournee,numrang);
}

// result window down : ask for previous match result (cycles from 6 to 0)
void down_single_click_handler2(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)resultwindow;
	nummatch -=1;
	text_layer_set_text(&layer_text1,"loading");
	text_layer_set_text(&layer_text3,"previous");
	text_layer_set_text(&layer_text4,"game");
	text_layer_set_text(&layer_text2, "result");
	if(nummatch<0) {nummatch =6;}
	get_match(nbjournee,nummatch);
}

// ranking window down : ask for previous in ranking (cycles from 14 to 0)
void down_single_click_handler3(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)rankingwindow;

	numrang -=1;
	if(numrang<1) {numrang =14;}
	text_layer_set_text(&layer_text1,"previous");
	text_layer_set_text(&layer_text3,"at day :");
	text_layer_set_text(&layer_text4,"loading");
	text_layer_set_text(&layer_text2, "loading");
	get_result(nbjournee,numrang);
}

//main window down : decrese and display Match Day
void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
text_layer_set_text(&layer_text3, "Short click : rankings");
text_layer_set_text(&layer_text1, "Long click : results");
text_layer_set_text(&layer_text4, "Select Match Day :");
	nbjournee -=1;
	if(nbjournee<1) {nbjournee =1;}
	text_layer_set_text(&layer_text2, itoa(nbjournee));
}

//main window single select click -short- : calls the ranking window up and asks for rank 1 in current Match Day
void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
text_layer_set_text(&layer_text1, "ranking at day : ");
text_layer_set_text(&layer_text3, "loading...");
text_layer_set_text(&layer_text4, "");
text_layer_set_text(&layer_text2, "loading...");
window_init(&rankingwindow, "Ranking");
  window_stack_push(&rankingwindow, true);
  window_set_window_handlers(&rankingwindow, (WindowHandlers){
  .appear  = window_appear3,
  .disappear = window_disappear3
  });
  
  get_result(nbjournee,1);
	
}

//main window single select click -long- : calls the result window up and asks for match N°1 in current Match Day
void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
text_layer_set_text(&layer_text1, "result for day : ");
text_layer_set_text(&layer_text3, itoa(nbjournee));
text_layer_set_text(&layer_text4, "");
text_layer_set_text(&layer_text2, "loading...");
window_init(&resultwindow, "Result");
  window_stack_push(&resultwindow, true);
  window_set_window_handlers(&resultwindow, (WindowHandlers){
  .appear  = window_appear2,
  .disappear = window_disappear2
  });
  
  get_match(nbjournee,0);
	
}


//Config Provider for each window

//main
void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

//results
void click_config_provider2(ClickConfig **config, Window *window) {
  (void)resultwindow;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler2;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler2;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

//ranking
void click_config_provider3(ClickConfig **config, Window *window) {
  (void)rankingwindow;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler3;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler3;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

//App init

void handle_init(AppContextRef ctx) {
  http_set_app_id(76782702);

  http_register_callbacks((HTTPCallbacks) {
    .success = http_success,
    .failure = http_failure
  }, NULL);

  window_init(&window, "Rugby Time");
  window_stack_push(&window, true);
  window_set_window_handlers(&window, (WindowHandlers){
  .appear  = window_appear,
  .disappear = window_disappear,
  .load = window_load
  });
}

//Error handling

void httpebble_error(int error_code) {

	text_layer_set_text(&layer_text1, "Error");
  static char error_message[] = "UNKNOWN_HTTP_ERRROR_CODE_GENERATED";

  switch (error_code) {
    case HTTP_SEND_TIMEOUT:
      strcpy(error_message, "HTTP_SEND_TIMEOUT");
    break;
    case HTTP_SEND_REJECTED:
      strcpy(error_message, "HTTP_SEND_REJECTED");
    break;
    case HTTP_NOT_CONNECTED:
      strcpy(error_message, "HTTP_NOT_CONNECTED");
    break;
    case HTTP_BRIDGE_NOT_RUNNING:
      strcpy(error_message, "HTTP_BRIDGE_NOT_RUNNING");
    break;
    case HTTP_INVALID_ARGS:
      strcpy(error_message, "HTTP_INVALID_ARGS");
    break;
    case HTTP_BUSY:
      strcpy(error_message, "HTTP_BUSY");
    break;
    case HTTP_BUFFER_OVERFLOW:
      strcpy(error_message, "HTTP_BUFFER_OVERFLOW");
    break;
    case HTTP_ALREADY_RELEASED:
      strcpy(error_message, "HTTP_ALREADY_RELEASED");
    break;
    case HTTP_CALLBACK_ALREADY_REGISTERED:
      strcpy(error_message, "HTTP_CALLBACK_ALREADY_REGISTERED");
    break;
    case HTTP_CALLBACK_NOT_REGISTERED:
      strcpy(error_message, "HTTP_CALLBACK_NOT_REGISTERED");
    break;
    case HTTP_NOT_ENOUGH_STORAGE:
      strcpy(error_message, "HTTP_NOT_ENOUGH_STORAGE");
    break;
    case HTTP_INVALID_DICT_ARGS:
      strcpy(error_message, "HTTP_INVALID_DICT_ARGS");
    break;
    case HTTP_INTERNAL_INCONSISTENCY:
      strcpy(error_message, "HTTP_INTERNAL_INCONSISTENCY");
    break;
    case HTTP_INVALID_BRIDGE_RESPONSE:
      strcpy(error_message, "HTTP_INVALID_BRIDGE_RESPONSE");
    break;
    default: {
      strcpy(error_message, "HTTP_ERROR_UNKNOWN");
    }
  }

  text_layer_set_text(&layer_text2, error_message);
	vibes_short_pulse();
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	  .messaging_info = {
      .buffer_sizes = {
        .inbound = 256,
        .outbound = 256,
      }
    }
  };
  app_event_loop(params, &handlers);
}