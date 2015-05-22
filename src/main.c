#include <pebble.h>
/* TODO
    *Nothing, for now.
*/
static Window *s_main_window;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_dow_layer;

// various layers ^^

static void update_time() { // updates time
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char buffer[] = "00:00";
    if (clock_is_24h_style() == true) {
        strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
    }
    else {
        strftime(buffer, sizeof(buffer), "%I:%M", tick_time);
    }
    text_layer_set_text(s_time_layer, buffer);
}

static void update_date() { // updates date
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char buffer[] = "00-00";
    strftime(buffer, sizeof(buffer), "%m-%d", tick_time);
    text_layer_set_text(s_date_layer, buffer);
}

static void update_dow() { // updates day of week
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char buffer[] = "DOW";
    strftime(buffer, sizeof(buffer), "%A", tick_time);
    text_layer_set_text(s_dow_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) { // handles time ticks
    update_time();
    update_date();
    update_dow();
    bool isCharging = battery_state_service_peek().is_charging;
    if (units_changed & HOUR_UNIT && isCharging == false) { // on the hour change when the watch is not charging...
        vibes_double_pulse(); // ... vibrate!
    }
}

static void main_window_load(Window *window) { // called on window load
    // ** background layer ** //
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_IMAGE);
    s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    // ** time layer ** //
    s_time_layer = text_layer_create(GRect(0, 5, 144, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
	update_time(); // set text to current time
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    // ** date layer ** //
    s_date_layer = text_layer_create(GRect(0, 70, 144, 50));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorWhite);
	update_date(); // set text to current date
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    // ** day of week layer ** //
    s_dow_layer = text_layer_create(GRect(0, 115, 144, 50));
    text_layer_set_background_color(s_dow_layer, GColorClear);
    text_layer_set_text_color(s_dow_layer, GColorWhite);
	update_dow(); // set text to current day of week
    text_layer_set_font(s_dow_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
    text_layer_set_text_alignment(s_dow_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_dow_layer));
}

static void main_window_unload(Window *window) { // called on window unload
    gbitmap_destroy(s_background_bitmap);
    bitmap_layer_destroy(s_background_layer);
    
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_dow_layer);
    
    // ^ destroys layers
}

static void init() { // window initialization
    bool isCharging = battery_state_service_peek().is_charging;
	if (isCharging == false) {
		//vibes_short_pulse(); // alerts the user that the face is enabled with a short vibration, as long as the watch isn't charging.
	}
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true); // pushes new window to the window stack
}
static void deinit() { // called on face quit
    window_destroy(s_main_window); // destroys window
}
int main(void) { // called on face start
    init(); // initializes face
    app_event_loop(); // starts face event loop
    deinit(); // deinitializes face
}