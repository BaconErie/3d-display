shader_type canvas_item;

uniform float pixels_per_lens;

float show_pixel_for_left_eye(float x) {
	// If this pixel is for the left eye, returns 1. Else, 0. Requires pixels_per_lens.
	return ceil(mod(x, pixels_per_lens)/pixels_per_lens - 0.5);
}

void fragment() {
    COLOR.r = show_pixel_for_left_eye(FRAGCOORD.x);
	COLOR.b = 1.0-show_pixel_for_left_eye(FRAGCOORD.x);
}