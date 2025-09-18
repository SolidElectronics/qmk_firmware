RGB_MATRIX_EFFECT(SOLID_REACTIVE_SPLASH)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// Configuration constants
#define SPLASH_RADIUS 40          // Radius to affect surrounding keys
#define HUE_SHIFT_AMOUNT 85       // How much to shift hue (85 = 1/3 color wheel)

// Test function - let's see what parameters we actually get
static HSV solid_reactive_splash_math(HSV hsv, int16_t dx, int16_t dy, uint8_t dist, uint16_t tick) {
    HSV result = rgb_matrix_config.hsv;  // Always use base color
/*
    // Visual debug - make any keypress area bright white to see if it's working
    if (tick < 100) {  // Recent keypress
        result.s = 0;   // Remove saturation = white
        result.v = 255; // Full brightness
    }
*/

    // Only apply effects to LEDs that are close to a recent keypress
    if (tick < 255 && dist <= SPLASH_RADIUS) {
        // Calculate fade progress
        uint8_t fade_progress = scale16by8(tick, rgb_matrix_config.speed);

        // Distance-based intensity (closer = more intense)
        uint8_t distance_intensity = 255;
        if (dist > 0) {
            distance_intensity = scale8(SPLASH_RADIUS - dist, 255);
        }

        // Calculate hue shift amount
        uint8_t hue_shift = scale8(
            255 - fade_progress,                    // Fade over time
            scale8(distance_intensity,              // Scale by distance
                   HUE_SHIFT_AMOUNT)               // Maximum hue shift
        );

        // Apply hue shift (wraps around naturally)
        result.h += hue_shift;

        // Optional brightness boost
        uint8_t brightness_boost = scale8(
            255 - fade_progress,
            scale8(distance_intensity, 30)
        );
        result.v = qadd8(result.v, brightness_boost);
    }

    return result;
}

bool SOLID_REACTIVE_SPLASH(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV base_hsv = rgb_matrix_config.hsv;

    // Manually set background for all LEDs
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB rgb = hsv_to_rgb(base_hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    // Try to add reactive effects - but this might override our background
    // Let's see if we get white flashes when pressing keys
    effect_runner_reactive_splash(0, params, &solid_reactive_splash_math);

    return led_max < RGB_MATRIX_LED_COUNT;
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
