shader_type canvas_item;

uniform sampler2D input_camera_texture;
uniform mat3 homography_inv; 
uniform bool for_left_eye;
uniform float pixels_per_lens;

float show_pixel_for_left_eye(float x) {
	// If this pixel is for the left eye, returns 1. Else, 0. Requires pixels_per_lens.
	return ceil(mod(x, pixels_per_lens)/pixels_per_lens - 0.5);
}

void fragment() {
	// Apply homography first
	
	// All this thing does it apply a homography on the input_camera_texture
    // It doesn't actually apply the homography directly, instead doing 
    // something equivalent.
    //
    // We need the inverse homography. We take a destination coordinate, and 
    // find the corresponding source coordinate, on the input_camera_texture,
    // that would have mapped to this destination coordinate, if we applied 
    // the homography directly. We copy the color from the input, then apply it
    // to the output.
    //
    // We are completely working in UV coordinates. This means each pixel has
    // An x and y of 0.0 to 1.0, or percentage of screen. Origin is at top left.
    // If you've calculated the homography matrix correctly this should not matter

    // Turn the UV coordinate into a UV homogenous coordinate
    vec3 uv_h = vec3(UV, 1.0); 
    
    // Find what the source UV coordinate would've been
    // Remember this is still homogenous coordinate
    vec3 src_uv_h = homography_inv * uv_h; 
    
    // Divide by the last element in the homogenous coordinate to turn it back
    // into standard UV coordinate.
    vec2 src_uv = src_uv_h.xy / src_uv_h.z;
    

    if (src_uv.x < 0.0 || src_uv.x > 1.0 || src_uv.y < 0.0 || src_uv.y > 1.0) {
        // If the src_uv is out of bounds, just don't draw anything
        COLOR = vec4(0.0);
    } else {
        // Otherwise, copy the color from the source and place it into the destination
        vec4 color = texture(input_camera_texture, src_uv);
        COLOR = color.rgba;
    }
	
    if (for_left_eye) {
        COLOR.a = show_pixel_for_left_eye(FRAGCOORD.x);
    } else {
        COLOR.a = 1.0-show_pixel_for_left_eye(FRAGCOORD.x);
    }
}   