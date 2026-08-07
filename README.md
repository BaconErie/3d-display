![banner](docs/banner.png)

# Low Cost Retrofit 3D Display

This is a program that, in conjunction with a lenticular lens panel or 3D anaglyph glasses, converts any computer display into a stereoscopic 3D display. This means the user sees a virtual 3D object when they look at the display which looks no different than a real, physical 3D object. I also keep the cost of running this project low: if you already have a computer screen and a webcam (e.g. as part of a laptop), then you will only need to spend at most 50 dollars for the required new parts. 

As a demonstration of the 3D display, when the user moves around, the view of the 3D object changes, as if the screen is a window into a virtual world:

<video controls>
  <source src="docs/head-tracking.mp4" type="video/mp4">
</video>

The 3D display is stereoscopic, meaning that the left and right eyes see different perspectives of the same object. This is how we perceive depth of physical objects. In the below video, head tracking is disabled, and the change in perspectives is a result of the display simultaneously showing the left and right eye perspectives, with the help of a lenticular lens.

<video controls>
  <source src="docs/stereoscopic-miku.mp4" type="video/mp4">
</video>

Instead of a lenticular lens, users may also use 3D anaglyph glasses (those old red-blue 3D movie glasses). This video shows head tracking in 3D anaglyph mode (notice the red/cyan halos)...

<video controls>
  <source src="docs/head-tracking-anaglyph.mp4" type="video/mp4">
</video>

...and this video shows how each eye sees different perspectives depending on which color filter the eye is looking through.

<video controls>
  <source src="docs/anaglyph-colors.mp4" type="video/mp4">
</video>

This repo contains two main folders. The `controller` folder is a C++ project that builds to a directory containing a `3d_display_program` executable. The `renderer` program is a Godot project that should be built into a single executable named `renderer` and then moved into the same directory as the `3d_display_program`. With both sub-programs built, the `3d_display_program` should be run to start the system.

Details on how this program works, especially the mathematics behind the homographies that drive much of the realism of this project, is available in the `docs/low-cost-retrofit-display.pdf` file. I recommend anyone trying to create their own 3D display to really focus on section 3.6, which explains what a homography is, why it is necessary, and how to do it. In addition, I have also made a [Google Slides I presented to a class](https://docs.google.com/presentation/d/18xh9L-4q5a0BtSvI9O-ukv0M-eG8DvCXooS9wi7Og_I/edit), although I don't think it's as useful as the PDF file.

This project depends on OpenCV for head tracking, Boost for IPC, GTK for UI, and Godot for 3D rendering. Also prominently featured in documentation is a 3D Miku model, created by **revsworks** and used under a CC Attribution license. The model is available at [https://sketchfab.com/3d-models/hatsune-miku-plushie-22e25b5fbd444102a175c088e14eb541](https://sketchfab.com/3d-models/hatsune-miku-plushie-22e25b5fbd444102a175c088e14eb541).

I chose to write this for fun to learn GTK and C++ as well as exercise my OpenCV skills as part of a senior capstone project. I learned a lot about idiomatic C++ (which would've made me do some things differently had I known from the start), and it was fun to incorporate Miku throughout the project.

# License Notice

> Copyright 2026 gakuseh
>
> This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
> 
> This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
> 
> You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. 