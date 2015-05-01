#include <pebble.h>
/* TODO
    *Move date up; add day of week: http://www.cplusplus.com/reference/ctime/strftime/
        *Make date font smaller
    *Move time up to center it between the split, and the top of the watch.
*/
static Window *s_main_window;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char buffer[] = "00:00";
    if (clock_is_24h_style() == true) {
        strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    }
    else {
        strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
    text_layer_set_text(s_time_layer, buffer);
}

static void update_date() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    static char buffer[] = "00-00 DOW";
    strftime(buffer, sizeof("00-00"), "%m-%d", tick_time);
    text_layer_set_text(s_date_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    update_date();
}

static void main_window_load(Window *window) {
    // ** background layer ** //
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_IMAGE);
    s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    // ** time layer ** //
    s_time_layer = text_layer_create(GRect(0, 15, 144, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    // ** date layer ** //
    s_date_layer = text_layer_create(GRect(0, 85, 144, 50));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text(s_date_layer, "00-00");
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
    gbitmap_destroy(s_background_bitmap);
    bitmap_layer_destroy(s_background_layer);
    
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
}

static void init() {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true);
}
static void deinit() {
    window_destroy(s_main_window);
}
int main(void) {
    init();
    app_event_loop();
    deinit();
}