extends Camera3D

@export var display_density_ppi: float
@export var display_panel: Panel
@export var parent_subviewport: SubViewport
@export var anaglyph_panel: Panel
@export var is_left_eye: bool

var display_size: Vector2

var lenticule_shader_material: ShaderMaterial
var anaglyph_shader_material: ShaderMaterial

var did_ready_run = false

func _ready() -> void:
	await RenderingServer.frame_post_draw
	
	lenticule_shader_material = display_panel.material
	anaglyph_shader_material = anaglyph_panel.material
	
	did_ready_run = true


func _process(delta: float) -> void:
	if not did_ready_run:
		return
	
	display_size = display_panel.size/display_density_ppi
	
	var homography: Basis = get_homography(position, display_size, fov, rotation.y, -rotation.x, parent_subviewport.size);
		
	lenticule_shader_material.set_shader_parameter("homography_inv", homography)

	if is_left_eye:
		anaglyph_shader_material.set_shader_parameter("left_homography_inv", homography)
	else:
		anaglyph_shader_material.set_shader_parameter("right_homography_inv", homography)
	

# Units of virtual camera position and display size must be in same units. That's it. You don't need
# to do anything else
# Axis should be: left is positive x, up is positive y, backwards is positive z
# Angles should be in degrees
# Position should be relative to the center of the plane/display
# You can feed in any sort of type of camera feed, position, camera fov, whatever as long as you 
# give the right info in
#
# Basically this creates a homography that converts what this virtual camera, or eye, or any camera
# image into a "wallpaper" that needs to be displayed on the display. Then this wallpaper would look
# like a 3D object is there, even though it's a flat image
#
# The sort of magic part is that you can put in one camera's view into here, apply homography, 
# display it on the wallpaper, and it will look correct for every single other camera, as long as
# the other camera is also at the same position. so you can have arbitrary FOV for the input camera
# (aside from clipping problems)
#
# Input image, or subviewport, can be any aspect ratio. just input what size.
#
# The homography transforms the input camera view into a NEW camera view. This new camera position
# is at the same position as the input camera (it has to be because of homography limitations).
# BUT, this new camera has a horizontal FOV that perfectly matches the horizontal size of the display
# and vertical FOV also perfectly match vertical size of the display. this is achieved through the
# a intrinsic constant for the k_display intrinsic matrix, see visionbook for more details. in
# addition to this FOV, the new camera is also always pointed parallel to the normal of the display
# so that's why we have the rotation matrix there as well.
# 
# finally, although the FOV sizese match, the camera might be transformed. to fix this problem is 
# really easy, we just add a translatoin matrix that translates in 2D homogenous space.
#
# normal homographies work by converting the ORIGINAL, input homogenous coordinate into the output
# this can cause problems. instead, we want to get an output homogenous coordinate, figure out 
# which input homogenous coordinate would map to it, and copy the pixel at that input coordinate.
# this is why we invert the homography matrix, and send it to the shader. also, the shader (more
# precisely the godot game engine) does some smoothing itself, so there really isnt' any problems of
# artifacts 
#
# the shader that goes along with this is pretty dumb. all it does is apply a homography matrix.
# all it needs is the matrix. all it knows is the matrix. which means all the important settings are
# in this gd code here, which is good because we can test stuff, print stuff, easier
#
func get_homography(virtual_camera_position: Vector3, display_size: Vector2, virtual_camera_fov: float, yaw_angle: float, pitch_angle: float, input_image_size_pixels: Vector2) -> Basis:
	var k_display = Basis(
		Vector3(-virtual_camera_position.z/display_size.x, 0, 0),
		Vector3(0, -virtual_camera_position.z/display_size.y, 0),
		Vector3(0.5, 0.5, 1)
	)
	
	var yaw_rotation_mat = Basis(
		Vector3(cos(yaw_angle), 0, -sin(yaw_angle)),
		Vector3(0, 1, 0),
		Vector3(sin(yaw_angle), 0, cos(yaw_angle))
	)
	
	var pitch_rotation_mat = Basis(
		Vector3(1, 0, 0),
		Vector3(0, cos(pitch_angle), sin(pitch_angle)),
		Vector3(0, -sin(pitch_angle), cos(pitch_angle))
	)
	
	var translation_mat = Basis(
		Vector3(1, 0, 0),
		Vector3(0, 1, 0),
		Vector3(virtual_camera_position.x/display_size.x, -virtual_camera_position.y/display_size.y, 1)
	)
	
	# For the horizontal intrinsic component, it is based on also the horizontal FOV calculation,
	# found on the Wikipedia page https://en.wikipedia.org/wiki/Field_of_view_in_video_games 
	var k_camera = Basis(
		Vector3(-1.0/2.0/tan(deg_to_rad(virtual_camera_fov/2.0))/ (input_image_size_pixels.x/input_image_size_pixels.y), 0, 0),
		Vector3(0, -1.0/2.0/tan(deg_to_rad(virtual_camera_fov/2.0)), 0),
		Vector3(0.5, 0.5, 1)
	)
	
	var homography = translation_mat * k_display * yaw_rotation_mat * pitch_rotation_mat * k_camera.inverse()
	return homography.inverse()
