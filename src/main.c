#include <pebble.h>

Window *my_window;
TextLayer *s_time_layer;
static RotBitmapLayer *image_layer;
static GBitmap *image;
InverterLayer *inver;
int prevSinceMid = -1;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  struct tm *gmtTime = gmtime(&temp); //gmtTime does not appear to work quite yet, see map image placement below
  
  static char buffer[] = "00:00";
  
  if (clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  }
  else {
    strftime(buffer, sizeof("00:00"), "%l:%M", tick_time);
  }
  
  text_layer_set_text(s_time_layer, buffer);
  
  
  int sinceMid = gmtTime->tm_hour;
  int wedge_size = TRIG_MAX_ANGLE/24;
  if (prevSinceMid != sinceMid) {
        rot_bitmap_layer_set_angle(image_layer, wedge_size*sinceMid);
        prevSinceMid = sinceMid;
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(); }

void handle_init(void) {
  my_window = window_create();

  s_time_layer = text_layer_create(GRect(0, 0, 144, 24));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  image = gbitmap_create_with_resource(RESOURCE_ID_START_MAP); //when gmtTime does work, use GMT_12AM instead of START_MAP
  image_layer = rot_bitmap_layer_create(image);
  //GRect starts at -60 because rot_layer offsets it this much, rotating the square pic. 204 is the diameter of the circle rot_layer spins the image in.
  layer_set_frame((Layer *)image_layer, GRect(-30, 24-30+1, 204, 204)); 

  

  layer_add_child(window_get_root_layer(my_window),  (Layer *)image_layer);
  
  
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(s_time_layer));
  update_time();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  inver = inverter_layer_create	(GRect(0, 72+24, 144, 72));
  layer_add_child(window_get_root_layer(my_window), inverter_layer_get_layer(inver));	

  
  window_stack_push(my_window, true);
}



void handle_deinit(void) {
  gbitmap_destroy(image);
  rot_bitmap_layer_destroy(image_layer);
  text_layer_destroy(s_time_layer);
  inverter_layer_destroy(inver);
  window_destroy(my_window); 
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
