RGB_MATRIX_EFFECT(SOLID_REACTIVE_SPLASH)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// Configuration constants
#define SPLASH_RADIUS 40          // Radius to affect surrounding keys
#define HUE_SHIFT_AMOUNT 85       // How much to shift hue (85 = 1/3 color wheel)
#define SPLASH_FADE_DURATION 1000

bool SOLID_REACTIVE_SPLASH(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    bool has_keypress = g_last_hit_tracker.count > 0;
    uint16_t time_since_hit = 0;
    uint8_t j = 0;

    HSV base_hsv = rgb_matrix_config.hsv;

    if (has_keypress) {
        j = g_last_hit_tracker.count - 1;

        // For some reason if I uncomment the code below, has_keypress always gets set to false.  Either g_last_hit_tracker.tick or g_rgb_timer isn't doing what it should.

//        time_since_hit = g_rgb_timer - g_last_hit_tracker.tick[g_last_hit_tracker.count - 1];
//        time_since_hit = timer_elapsed(g_last_hit_tracker.tick[j]);
//        if (time_since_hit > SPLASH_FADE_DURATION) {
//            has_keypress = false;
//        }
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        // Start with solid background color
        HSV hsv = base_hsv;

        if (has_keypress) {
            // Check if this LED position matches the keypress position
            if (g_led_config.point[i].x == g_last_hit_tracker.x[j] &&
                g_led_config.point[i].y == g_last_hit_tracker.y[j]) {
                // Make this LED bright white
                hsv.s = 0;   // No saturation = white
                hsv.v = 255; // Full brightness
            }
        } else {
            // No keypress, set everything blue
            hsv.h = 170;
        }

        // Always set the LED color (background + any effects)
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return led_max < RGB_MATRIX_LED_COUNT;
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
