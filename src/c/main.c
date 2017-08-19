#include <pebble.h>
#define GOLDEN_RATIO 1.618033

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_minute_layer;
static GFont s_time_font;
static Layer *s_timeline_layer;
static int minute_labels[12] = {00, 05, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55};

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void timeline_update_proc(Layer *layer, GContext *ctx) {
  // Custom drawing happens here!
  GRect bounds = layer_get_bounds(layer);
  
  // Draw a filled rectangle
  int timeline_x = 0;
  int timeline_y = 90;
  GRect rect_bounds = GRect(timeline_x, timeline_y, bounds.size.w, bounds.size.h);
  graphics_fill_rect(ctx, rect_bounds, 0, GCornersAll);
  
  // Draw lines
  graphics_context_set_stroke_color(ctx, GColorClear);
  int height_minute = 8;
  int correct_minute  = -1 * height_minute;
  for (int x= 0 ; x<bounds.size.w*2; x+=8){
      GPoint start = GPoint(x, bounds.size.h);
      GPoint end = GPoint(x, bounds.size.h - height_minute);
      
      if(x >= correct_minute){
          if((x-correct_minute)%40 == 0){
              end = GPoint(x, bounds.size.h - height_minute*3);    
          } 
      }
      graphics_draw_line(ctx, start, end);
  }
 
    // Master line
  GPoint start = GPoint(bounds.size.w/2, 90);
  GPoint end = GPoint(bounds.size.w/2, bounds.size.h);
  graphics_draw_line(ctx, start, end);
  
    
//     TIMELINE NUMBERS
    
     // Create the TextLayer with specific bounds
  s_minute_layer = text_layer_create(
      GRect(0, timeline_y, bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_minute_layer, GColorBlack);
  text_layer_set_text_color(s_minute_layer, GColorClear);
  
  // Apply to s_minute_layer
//   text_layer_set_font(s_minute_layer, fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS));
//   text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);
    
  static char m_buffer[13]="StudyTonight"; 
//   strftime(m_buffer, sizeof(s_buffer), clock_is_24h_style() ?
//                                           "%H:%M" : "%I:%M", tick_time);

  text_layer_set_text(s_minute_layer, m_buffer);

}


static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(38, 32), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  
  // Create GFont
  //s_time_font = fonts_load_custom_font(resource_get_handle(FONT_KEY_LECO_36_BOLD_NUMBERS));
  
  // Apply to TextLayer
  //text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    
  // Create timeline layer
  s_timeline_layer = layer_create(bounds);
    
  // Assign the custom drawing procedure
  layer_set_update_proc(s_timeline_layer, timeline_update_proc);


  // Add time as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
   
  // Add timeline to Window
  layer_add_child(window_get_root_layer(window), s_timeline_layer);
}

static void main_window_unload(Window *window) {
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void init() {
// Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();      
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  app_event_loop();
  deinit();
}