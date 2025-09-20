RGB_MATRIX_EFFECT(SOLID_REACTIVE_SPLASH)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// Configuration constants
#define SPLASH_RADIUS 30          // Radius to affect surrounding keys
#define HUE_SHIFT_AMOUNT 85       // How much to shift hue (85 = 1/3 color wheel)
#define SPLASH_FADE_DURATION 1000

bool SOLID_REACTIVE_SPLASH(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    bool has_keypress = g_last_hit_tracker.count > 0;
    uint8_t j = 0;

    HSV base_hsv = rgb_matrix_config.hsv;

    if (has_keypress) {
        j = g_last_hit_tracker.count - 1;
        // Unset keypress flag if delay is exceeded
        if (g_last_hit_tracker.tick[j] > SPLASH_FADE_DURATION) {
            has_keypress = false;
        }
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        // Start with solid background color
        HSV hsv = base_hsv;
        //hsv.v = 255;

        if (has_keypress) {
            // Calculate distance from most recent pressed key
            int16_t dx = g_led_config.point[i].x - g_led_config.point[g_last_hit_tracker.index[j]].x;
            int16_t dy = g_led_config.point[i].y - g_led_config.point[g_last_hit_tracker.index[j]].y;
            uint8_t dist = sqrt16(dx * dx + dy * dy);

            // If keypress was within the affected area
            if (dist <= SPLASH_RADIUS) {

                // Calculate time-based fade factor (255 to 0)
                uint16_t time_factor = 255 - ((uint16_t)g_last_hit_tracker.tick[j] * 255 / SPLASH_FADE_DURATION);

                // Calculate distance-based fade factor (255 to 0)
                uint16_t distance_factor = 255 - ((uint16_t)dist * 255 / SPLASH_RADIUS);

                // Combine both factors (result is 0-255 range after scaling)
                uint16_t intensity = (time_factor * distance_factor) >> 8;

                // Apply hue shift based on intensity
                uint8_t hue_shift = (HUE_SHIFT_AMOUNT * intensity) >> 8;
                hsv.h += hue_shift;
            }

//        } else {
//            // No keypress, set everything blue
//            hsv.h = 170;
        }

        // Always set the LED color (background + any effects)
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return led_max < RGB_MATRIX_LED_COUNT;
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
