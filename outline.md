There are two programs. The **controller**, and the **renderer**.

The controller is a C++ program that uses GTK for graphics. The controller
handles

- Webcam inputs
- Face detection
- Interlacing algorithm
- Renderer control and directions
- Switching displayed subject

The renderer is a Godot program that handles 3D rendering. It receieves
imperative instructions from the controller to manipulate the rendered image. As
little processing as possible is placed on the renderer, because most of the
code will be in GDScript, which is not as fast as the renderer.

# Overall Architecture

Main Controller

- UI
- Coordinates processes
- Gets pixel coordinates of face from CV, uses it to get angles, that angles is
  then sent to Renderer

CV

- Gets webcam info
- Identifies location of face
- Sends face (x, y) pixel coordinates to the main controller thru a pipe
- Also puts the image data into a sharedmemory which is used by the UI to
  display
- Starts in SEARCHING state. Searches entire area for a face. If face is
  detected then put a point at that lovcation. next frame look for faces. do the
  point thing again for every point, if no face overlaps with that point, delete
  it. otherwise, increment some associated counter by one. once counter reaches
  5 go to TRACKING state
- get the first face detected on that point. our next search area will only be
  in the area x2 of the last face. makes things faster and makes sure we only
  look at one person. make search area box blue and face box green. everytime we
  detect a face look for eyes only in that face green box. make eye boxes red.
- if we are in tracking state and we dont see a face in the area for 5 frames
  reset to searching

Renderer

- Godot program
- Communicates through sockets with the main controller

# Calibration

The program needs to know several pieces of information to function correctly:

1. Horizontal angle of view of the webcam
2. Density of the lenticular lens in lenticules per inch
3. Density of the pixels on the display in pixels per inch

From these three initial settings, the program can calculate everything else it
needs to know.

## Horizontal angle of view of the webcam

A paper and cardboard calibration tool is required.

A QR code attached to a card with a known width is placed at some distance from
the webcam.

We use a QR code because many libraries already exist to quickly detect QR
codes.

Once the QR code is detected, the user will press a "Capture" button to confirm
the image for calibration. The user is then prompted to measure the distance of
the QR code from the webcam.

Now that we have a known distance and a known width, we can calculate the
horizontal angle of view of the webcam.

Since we have the known distance and known width, we can calculate the angular
size of the QR code in the webcam image.

We take the width of the QR code in pixels, then divide the angular size of the
QR code by the pixel width to get the angle per pixel. We can then multiply the
angle per pixel by the total pixel width of the webcam image to get the total
horizontal angle of view of the webcam.

## Density of the lenticular lens in lenticules per inch

The user will measure the width of a lenticule with a ruler, then input that
value into the program. The program will then calculate the density of the
lenticules in lenticules per inch.

## Density of the pixels on the display in pixels per inch

To get the pixel density of the display, the user will click on a button to get
to the pixel density calibration screen. The below image shows the calibration
setup.

![Calibration Setup](calibration.png)

The user will place a ruler against the display. The user will align the ruler
along the red line, and place one edge of the ruler against the green line. The
user will then identify where 6 inches is on the ruler, and click on the screen
at that point.

From this we know the pixel distance of 6 inches in pixels. Dividing the pixel
distance by 6 gives us the pixel density in pixels per inch.

# Set up, post-calibration

Starting to display a subject.

# Main Loop

After everything has been set up

most of the processing occurs, including webcam processing, face detection,
interlacing algorithms, program

# Timeline

- Sep 25 - Oct 9 (2 weeks, at the same time as other tasks): Buy lens sheet
- Sep 25 - Oct 2 (1 week): UI for controller window/program. This will control
  the 3D renderer and computer vision program.
- Oct 2 - Oct 16 (2 weeks): Write face detection and eye tracking in C++ with
  OpenCV. This will be a separate program that communicates with the controller
  program.
- Oct 16 - Oct 30 (2 weeks): Inter-process communication between controller and
  CV program. I will be using pipes to coordinate between the two programs, and
  shared memory to share the webcam images. By the end of this week, a button on
  the controller will be able to enable and disable webcam processing on the CV
  program.
- Oct 30 - Nov 14 (2 weeks): Calibration process for the controller software.
  For now, only store the values in a file and in memory. This part will also
  include more CV-controller coordination, as one portion of the calibration
  will require the webcam.
- Nov 14 - Nov 21 (1 week): Begin 3D rendering of currently fixed virtual object
  using the Godot game engine. Program should be able to render a 3D object and
  display an interlaced image based on interlacing instructions stored in a
  variable. The interlacing instructions will be updated later so that they will
  update based on instructions from the controller program.
- Nov 21 - Nov 28 (1 week): Using face data from the CV program and calibration
  data, calculation proper interlacing instructions for the 3D renderer. Don't
  transmit to renderer yet.
- Nov 28 - Dec 12 (2 weeks): Implement inter-process communication between the
  controller program and the 3D renderer. Use socket communication, because
  Godot only has support for sockets. Will need to implement a socket server on
  the controller program, which may be difficult. The connection from Godot to
  socket server should be easy; I have experience with this before. By the end
  of this, the 3D renderer will be able to move around the virtual camera based
  on face data from the CV program.
- Dec 12 - Jan 1 (3 weeks): Optimize face and eye detection. Look into
  hyperparameters, or train my own model. There is both face and eye datasets on
  Kaggle and other sites, and I can always augment these datasets by introducing
  noise, rotation, occlusions, and other transformations.
