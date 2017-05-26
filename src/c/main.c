#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static Layer *s_text_layer;
// Declare globally
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;



static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_time_buffer[8];
  
  // Write the current date into a buffer
  static char s_date_buffer[10];
  
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  strftime(s_date_buffer, sizeof(s_date_buffer), "%a%e", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_time_buffer);
  
  // Display this date on the TextLayer
  text_layer_set_text(s_date_layer, s_date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  layer_mark_dirty(s_text_layer);
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
  // Custom drawing happens here

  // Load the font
  GFont font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ITALIC_LOWERCASE_10));
  // Set the color
  graphics_context_set_text_color(ctx, GColorRichBrilliantLavender);
  
  char *text = "codes <3 chuan";
  // Determine a reduced bounding box
  GRect layer_bounds = layer_get_bounds(layer);
  GRect text_bounds = GRect(layer_bounds.origin.x, 155,
                       layer_bounds.size.w, 155);
  
  
  // Calculate the size of the text to be drawn, with restricted space
  GSize text_size = graphics_text_layout_get_content_size(text, font, text_bounds,
                                GTextOverflowModeWordWrap, GTextAlignmentCenter);
  
  // Draw the text
  graphics_draw_text(ctx, text, font, text_bounds, GTextOverflowModeWordWrap, 
                                            GTextAlignmentCenter, NULL);
 
}

static void main_window_load(Window *window) {
  
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UW_LOGO);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, 5, bounds.size.w, 50));

  s_text_layer = layer_create(bounds);
  
  layer_set_update_proc(s_text_layer, layer_update_proc);
  
  s_date_layer = text_layer_create(GRect(0, 120, 144, 155));
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorYellow);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorYellow);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGITAL_30));
 
  
  // Apply time to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_date_layer, s_time_font); // same font because same text layer
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, s_text_layer);
}

static void main_window_unload(Window *window) {
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  //Set the background color of the window
  window_set_background_color(s_main_window, GColorIndigo);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Make sure the time is displayed from the start
  update_time();
  
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}