shader_type canvas_item;

uniform sampler2D left_eye;
uniform sampler2D right_eye;
uniform mat3 left_homography_inv;
uniform mat3 right_homography_inv;

void fragment() {
	// Code based on lenticular_bars.gdshader
	
	vec3 uv_h = vec3(UV, 1.0); // Homogenous
	
	vec3 src_left_uv_h = left_homography_inv * uv_h;
	vec3 src_right_uv_h = right_homography_inv * uv_h;
	
	vec2 src_left_uv = src_left_uv_h.xy / src_left_uv_h.z;
	vec2 src_right_uv = src_right_uv_h.xy / src_right_uv_h.z;
	
	if (src_left_uv.x >= 0.0 && src_left_uv.x < 1.0 && src_left_uv.y >= 0.0 && src_left_uv.y < 1.0) {
		COLOR.r = texture(left_eye, src_left_uv).r;
	}

	if (src_right_uv.x >= 0.0 && src_right_uv.x < 1.0 && src_right_uv.y >= 0.0 && src_right_uv.y < 1.0) {
		COLOR.g = texture(right_eye, src_right_uv).g;
		COLOR.b = texture(right_eye, src_right_uv).b;
	}
}