extends Node3D

@export var pixel_density_calibration: Control
@export var offset_calibration: Control
@export var left_camera: Camera3D
@export var right_camera: Camera3D
@export var left_panel: Panel
@export var right_panel: Panel
@export var alignment_panel: Panel
@export var currently_loaded_object: Node

func load_glb_from_path(path: String) -> Variant:
	var gltf_document_load = GLTFDocument.new()
	var gltf_state_load = GLTFState.new()

	var error = gltf_document_load.append_from_file(path, gltf_state_load)

	if error == OK:
		return gltf_document_load.generate_scene(gltf_state_load)
	else:
		print("Couldn't load glTF scene (error code: %s)." % error_string(error))
		return error

var conn
var waiting_for_request: bool = true;
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	conn = StreamPeerTCP.new()

	var result = await conn.connect_to_host('127.0.0.1', 42842)

	if result != OK:
		#print('Failed to connect to controller program, with error code %d' % result)
		get_tree().quit()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if Input.is_action_just_pressed("show_lenticular"):
		$LeftControl.visible = true
		$RightControl.visible = true
		$AnaglyphControl.visible = false
		$Aligment.visible = false
		
	if Input.is_action_just_pressed("show_anaglyph"):
		$LeftControl.visible = false
		$RightControl.visible = false
		$AnaglyphControl.visible = true
		$Aligment.visible = false	
	
	if Input.is_action_just_pressed("show_alignment"):
		$LeftControl.visible = false
		$RightControl.visible = false
		$AnaglyphControl.visible = false
		$Aligment.visible = true
	
	#print('NEW PROCESS.')

	#print('Starting polling')
	conn.poll()
	#print('Polling done.')

	if conn.get_status() != StreamPeerTCP.STATUS_CONNECTED:
		return

	if conn.get_status() == StreamPeerTCP.STATUS_ERROR:
		print("Stream Peer Status is error with no message to cleanly exit. Stopping.")
		get_tree().quit(1)

	var num_bytes_ready_to_read: int = conn.get_available_bytes()
	#print('Num bytes ready: ' + str(num_bytes_ready_to_read))

	if num_bytes_ready_to_read > 0:

		if waiting_for_request:
			#print('Must be request code, so has to be a int 64')
			var request_code: int = conn.get_64()
			

			#print('RENDERER: Request code is ' + str(request_code))

			match request_code:
				0:
					# Show calibration window
					pixel_density_calibration.visible = true
				1:
					# Hide calibration window
					pixel_density_calibration.visible = false
				2: 
					# Getting pixels_per_lens and display density, in pixels per inch
					var pixels_per_lens = conn.get_float()
					var display_density_ppi = conn.get_float()
					left_panel.material.set_shader_parameter("pixels_per_lens", pixels_per_lens)
					right_panel.material.set_shader_parameter("pixels_per_lens", pixels_per_lens)
					alignment_panel.material.set_shader_parameter("pixels_per_lens", pixels_per_lens)
					
					# The camera scripts need the display density to calculate the display size
					left_camera.display_density_ppi = display_density_ppi
					right_camera.display_density_ppi = display_density_ppi
					
					prints("got new pixels per lens.", pixels_per_lens)

				3:
					conn.put_64(get_viewport().get_visible_rect().size.x)
				4:
					# New eye positions
					#print('RENDERER: Got new eye positions.')
					var l_x: float = conn.get_float()
					var l_y: float = conn.get_float()
					var l_z: float = conn.get_float()
					
					var r_x: float = conn.get_float()
					var r_y: float = conn.get_float()
					var r_z: float = conn.get_float()
					
					left_camera.position = Vector3(l_x, l_y, l_z)
					right_camera.position = Vector3(r_x, r_y, r_z)

					left_camera.look_at(Vector3(0, 0, 0), Vector3.UP)
					right_camera.look_at(Vector3(0, 0, 0), Vector3.UP)
					
					#print('Left eye is: ', left_camera.position, 'Right eye is: ', right_camera.position)

				5:
					# Quit code
					#print("Renderer received quit. Stopping.")
					get_tree().quit(0)

				6:
					# Given a new path for a glb object. Load that object.
					var path_string_length = conn.get_64()
					var path_string = conn.get_string(path_string_length)

					var node = load_glb_from_path(path_string)
					if (typeof(node) == typeof(Node)):
						add_child(node)
						remove_child(currently_loaded_object)
						currently_loaded_object = node
					
				7:
					# Show Offset Calibration
					offset_calibration.visible = true
				8:
					# Hide calibration window
					offset_calibration.visible = false
				9:
					# Switch 3D mode
					$LeftControl.visible = not $LeftControl.visible
					$RightControl.visible = not $RightControl.visible
					$AnaglyphControl.visible = not $AnaglyphControl.visible

			#print('End of match statement')
	
	#print('End of process function\n\n\n')
 
