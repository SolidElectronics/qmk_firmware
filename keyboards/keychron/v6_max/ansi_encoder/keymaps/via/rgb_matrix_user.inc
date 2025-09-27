RGB_MATRIX_EFFECT(SOLID_REACTIVE_SPLASH)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#define SPLASH_RADIUS 30
#define HUE_SHIFT_AMOUNT 128
#define SPLASH_FADE_DURATION 200

bool SOLID_REACTIVE_SPLASH(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    HSV base_hsv = rgb_matrix_config.hsv;

    for (uint8_t i = led_min; i < led_max; i++) {
        // Start with solid background color
        HSV hsv = base_hsv;

        uint16_t total_hue_shift = 0;  // Accumulate hue shifts from all active splashes
        bool has_active_splash = false;

        // Loop through all keypresses in the tracker
        for (uint8_t j = 0; j < g_last_hit_tracker.count; j++) {

            // Skip if this key appears later in the tracker (use only the most recent)
            bool has_newer_press = false;
            for (uint8_t k = j + 1; k < g_last_hit_tracker.count; k++) {
                if (g_last_hit_tracker.index[k] == g_last_hit_tracker.index[j]) {
                    has_newer_press = true;
                    break;
                }
            }
            if (has_newer_press) {
                continue;
            }

            // Calculate speed-adjusted duration
            uint16_t adjusted_duration = SPLASH_FADE_DURATION * 255 / (rgb_matrix_config.speed + 1);

            // Skip if this keypress has expired
            if (g_last_hit_tracker.tick[j] > adjusted_duration) {
                continue;
            }

            // Calculate distance from this keypress
            int16_t dx = g_led_config.point[i].x - g_led_config.point[g_last_hit_tracker.index[j]].x;
            int16_t dy = g_led_config.point[i].y - g_led_config.point[g_last_hit_tracker.index[j]].y;
            uint8_t dist = sqrt16(dx * dx + dy * dy);

            // Skip if outside splash radius
            if (dist > SPLASH_RADIUS) {
                continue;
            }

            has_active_splash = true;

            // Calculate time-based fade factor (255 to 0) with speed adjustment
            uint16_t time_factor = 255 - ((uint16_t)g_last_hit_tracker.tick[j] * 255 / adjusted_duration);

            // Calculate distance-based fade factor (255 to 0)
            uint16_t distance_factor = 255 - ((uint16_t)dist * 255 / SPLASH_RADIUS);

            // Combine both factors (result is 0-255 range after scaling)
            uint16_t intensity = (time_factor * distance_factor) >> 8;

            // Apply hue shift based on intensity and add to total
            uint16_t hue_shift = (HUE_SHIFT_AMOUNT * intensity) >> 8;
            total_hue_shift += hue_shift;
        }

        if (has_active_splash) {
            // Clamp total hue shift to prevent overflow
            if (total_hue_shift > 255) {
                total_hue_shift = 255;
            }
            hsv.h += (uint8_t)total_hue_shift;
        }

        // Always set the LED color (background + any effects)
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}
#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
