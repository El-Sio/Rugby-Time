#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "http.h"
#include "util.h"

//Set this to true to compile for Android
#define ANDROID false
 
#if ANDROID
#define MY_UUID { 0xA3, 0x4E, 0x23, 0x60, 0xF2, 0x77, 0x4E, 0xF2, 0x99, 0x2D, 0xF1, 0x1E, 0xA7, 0xCB, 0xEF, 0x10 }
#else
#define MY_UUID HTTP_UUID
#endif

//Unique Random Stuff to identify one app from another with Httpebble
#define HTTP_COOKIE 42691234

PBL_APP_INFO(MY_UUID, "Rugby Time", "CANAL+", 1, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_STANDARD_APP);


//Declarations :

void handle_init(AppContextRef ctx);
void http_success(int32_t request_id, int http_status, DictionaryIterator* received, void* context);
void http_failure(int32_t request_id, int http_status, void* context);
void menuwindow_appear(Window* me);
void window_unload3(Window* me);
void window_unload2(Window* me);
void menuwindow_disappear(Window* me);
void window_load2(Window* me);
void window_load3(Window* me);
void httpebble_error(int error_code);
void click_config_provider(ClickConfig **config, Window *window);
void click_config_provider2(ClickConfig **config, Window *window);
void click_config_provider3(ClickConfig **config, Window *window);
static void open_ranking(int index, void* context);
static void open_results(int index, void* context);
void get_match(int journee, int numeromatch);
void get_result(int journee, int rang);

ActionBarLayer ranking_action_bar;
ActionBarLayer result_action_bar;

NumberWindow selectday;
Window resultwindow;
Window rankingwindow;
Window menuwindow;
TextLayer layer_text1;
TextLayer layer_text2;
TextLayer layer_text3;
TextLayer layer_text4;
static int nbjournee=1;
static int nummatch=1;
static int numrang =1;

//icons
static HeapBitmap icon_plus;
static HeapBitmap icon_minus;
static HeapBitmap icon_refresh;

//Main Menu Stuff
static SimpleMenuItem main_menu_items[] = {
	{
		.title = "Select Data"
	},
	{
		.title = "Ranking",
		.callback = open_ranking,
	},
	{
		.title = "Results",
		.callback = open_results,
	}
};

static SimpleMenuSection section = {
    .items = main_menu_items,
    .num_items = ARRAY_LENGTH(main_menu_items),
};

static SimpleMenuLayer main_menu_layer;

void main_menu_init(Window* window) {
    simple_menu_layer_init(&main_menu_layer, GRect(0, 0, 144, 152), window, &section, 1, NULL);
    layer_add_child(window_get_root_layer(window), simple_menu_layer_get_layer(&main_menu_layer));
}

static void open_ranking(int index, void* context) {
	
    text_layer_set_text(&layer_text1, "ranking at day : ");
	text_layer_set_text(&layer_text3, "loading...");
	text_layer_set_text(&layer_text4, "");
	text_layer_set_text(&layer_text2, "loading...");	
	window_init(&rankingwindow, "Ranking");
	window_stack_push(&rankingwindow, true);
	window_set_window_handlers(&rankingwindow, (WindowHandlers){
  	.load = window_load3,
	.unload = window_unload3
  });
	get_result(nbjournee,1);
}

static void open_results(int index, void* context) {
    text_layer_set_text(&layer_text1, "result for day : ");
    text_layer_set_text(&layer_text3, itoa(nbjournee));
    text_layer_set_text(&layer_text4, "");
    text_layer_set_text(&layer_text2, "loading...");
	window_init(&resultwindow, "Result");
  	window_set_window_handlers(&resultwindow, (WindowHandlers){
  	.load = window_load2,
	.unload = window_unload2
  });
	window_stack_push(&resultwindow, true);
	get_match(nbjournee,0);	
}

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

void menuwindow_appear(Window* me) {
	//Not sure this is usefull
}

void menuwindow_disappear(Window* me) {
	//Not sure this is usefull
}

void window_unload2(Window* me) {
	
  heap_bitmap_deinit(&icon_plus);
  heap_bitmap_deinit(&icon_minus);
  heap_bitmap_deinit(&icon_refresh);
  action_bar_layer_remove_from_window(&result_action_bar);
  layer_remove_child_layers(window_get_root_layer(me));

}

void window_unload3(Window* me) {

  heap_bitmap_deinit(&icon_plus);
  heap_bitmap_deinit(&icon_minus);
  action_bar_layer_remove_from_window(&ranking_action_bar);
  layer_remove_child_layers(window_get_root_layer(me));

	
}

void window_load2(Window* me) {

  heap_bitmap_init(&icon_plus, RESOURCE_ID_IMAGE_ICON_PLUS);
  heap_bitmap_init(&icon_minus, RESOURCE_ID_IMAGE_ICON_MINUS);
  heap_bitmap_init(&icon_refresh, RESOURCE_ID_IMAGE_ICON_REFRESH);
  action_bar_layer_init(&result_action_bar);
  action_bar_layer_add_to_window(&result_action_bar, me);
  action_bar_layer_set_click_config_provider(&result_action_bar, (ClickConfigProvider) click_config_provider2);	
  action_bar_layer_set_icon(&result_action_bar, BUTTON_ID_DOWN, &icon_plus.bmp);
  action_bar_layer_set_icon(&result_action_bar, BUTTON_ID_UP, &icon_minus.bmp);
  action_bar_layer_set_icon(&result_action_bar, BUTTON_ID_SELECT, &icon_refresh.bmp);
	layer_add_child(window_get_root_layer(me), &layer_text1.layer);
  layer_add_child(window_get_root_layer(me), &layer_text2.layer);
  layer_add_child(window_get_root_layer(me), &layer_text3.layer);
  layer_add_child(window_get_root_layer(me), &layer_text4.layer);
}

void window_load3(Window* me) {

  heap_bitmap_init(&icon_plus, RESOURCE_ID_IMAGE_ICON_PLUS);
  heap_bitmap_init(&icon_minus, RESOURCE_ID_IMAGE_ICON_MINUS);
  action_bar_layer_init(&ranking_action_bar);
  action_bar_layer_set_click_config_provider(&ranking_action_bar, (ClickConfigProvider) click_config_provider3);
	action_bar_layer_add_to_window(&ranking_action_bar, me);
	action_bar_layer_set_icon(&ranking_action_bar, BUTTON_ID_DOWN, &icon_plus.bmp);
    action_bar_layer_set_icon(&ranking_action_bar, BUTTON_ID_UP, &icon_minus.bmp);
	layer_add_child(window_get_root_layer(me), &layer_text1.layer);
	layer_add_child(window_get_root_layer(me), &layer_text2.layer);
	layer_add_child(window_get_root_layer(me), &layer_text3.layer);
	layer_add_child(window_get_root_layer(me), &layer_text4.layer);

	
}

void window_appear2(Window* me) {
  
}

void window_appear3(Window* me) {
	
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

// result window Up : ask for next match result (cycles from 0 to 6)
void up_single_click_handler2(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)resultwindow;
	nummatch +=1;
	text_layer_set_text(&layer_text1,"loading");
	text_layer_set_text(&layer_text3,"previous");
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
	text_layer_set_text(&layer_text1,"previous");
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
	text_layer_set_text(&layer_text3,"next");
	text_layer_set_text(&layer_text4,"game");
	text_layer_set_text(&layer_text2, "result");
	if(nummatch<0) {nummatch =6;}
	get_match(nbjournee,nummatch);
}

void select_single_click_handler2(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)resultwindow;
	text_layer_set_text(&layer_text1,"Refreshing");
	text_layer_set_text(&layer_text3,"current");
	text_layer_set_text(&layer_text4,"game");
	text_layer_set_text(&layer_text2, "result");
	get_match(nbjournee,nummatch);
	
}

// ranking window down : ask for previous in ranking (cycles from 14 to 0)
void down_single_click_handler3(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)rankingwindow;

	numrang -=1;
	if(numrang<1) {numrang =14;}
	text_layer_set_text(&layer_text1,"next");
	text_layer_set_text(&layer_text3,"at day :");
	text_layer_set_text(&layer_text4,"loading");
	text_layer_set_text(&layer_text2, "loading");
	get_result(nbjournee,numrang);
}

void nbjourneeselect() {

	nbjournee= number_window_get_value(&selectday);
	//Todo : call the main menu window
	window_init(&menuwindow, "Select Data");
	main_menu_init(&menuwindow);
	window_set_window_handlers(&menuwindow, (WindowHandlers) {.appear = menuwindow_appear,.disappear = menuwindow_disappear});
	window_stack_push(&menuwindow, true);
	
}

//Config Provider for each window

//results
void click_config_provider2(ClickConfig **config, Window *window) {
  (void)resultwindow;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler2;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler2;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
	
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler2;
  config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 100;
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
	resource_init_current_app(&APP_RESOURCES);
  http_register_callbacks((HTTPCallbacks) {
    .success = http_success,
    .failure = http_failure
  }, NULL);

	text_layer_init(&layer_text1, GRect(0, 10, 120, 30));
  	text_layer_set_text_color(&layer_text1, GColorBlack);
 	text_layer_set_background_color(&layer_text1, GColorClear);
  	text_layer_set_font(&layer_text1, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  	text_layer_set_text_alignment(&layer_text1, GTextAlignmentCenter);
	
	text_layer_init(&layer_text2, GRect(0, 110, 120, 30));
  text_layer_set_text_color(&layer_text2, GColorBlack);
  text_layer_set_background_color(&layer_text2, GColorClear);
  text_layer_set_font(&layer_text2, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(&layer_text2, GTextAlignmentCenter);
	
	text_layer_init(&layer_text3, GRect(0, 40, 120, 30));
  text_layer_set_text_color(&layer_text3, GColorBlack);
  text_layer_set_background_color(&layer_text3, GColorClear);
  text_layer_set_font(&layer_text3, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(&layer_text3, GTextAlignmentCenter);

 text_layer_init(&layer_text4, GRect(0, 70, 120, 30));
  text_layer_set_text_color(&layer_text4, GColorBlack);
  text_layer_set_background_color(&layer_text4, GColorClear);
  text_layer_set_font(&layer_text4, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(&layer_text4, GTextAlignmentCenter);	
	
	number_window_init(&selectday, "Match Day :", (NumberWindowCallbacks) {.selected = nbjourneeselect}, "");
	number_window_set_max(&selectday, 26);
	number_window_set_min(&selectday, 1);
	number_window_set_step_size(&selectday, 1);
	window_stack_push((Window *)&selectday, true);
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