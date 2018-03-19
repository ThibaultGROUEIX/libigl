# libigl - A simple C++ geometry processing library

# EDIT BY THIBAULT

# screen capture

### 1

in 607_screen_capture : main-double_face allows you to do an automatic screenshot, with double sided faces.

Usage :

```shell
607_screen_capture.bin myplyfile.ply
```

It outputs out.png in ~/tmp/ which you can use however you like.

### 2

in 607_screen_capture : main allows you to 360 screenshots around an object to make a video around it.

Usage :

```shell
607_screen_capture.bin myplyfile.ply #make sure color attributes are passed as normal attributes by replacing "blue" by "nx", "red" by "ny", "green" by "nz" in PLY file.
```

It outputs out[1-360].png in ~/tmp/ which you can use however you like.

Though I recommend using ffmpeg to make a video out of it.

It heavily not optimised because i don't use it much and I don't know libigl well enough to do better in a short time.

### 3

```shell
ffmpeg -framerate 20 -i %4d.ply.obj.png output.mp4
```



## Coding guidelines for future libigl edits

* I can directly edit the vertex and fragment shader in "main.cpp"
* ReadPLY only loads vertex position, normals and texture coordinates. It's hard to edit to load color attributes and other type of attributes. As attributes can't be stored in *.obj* and *.off* , i pass them as normal by editing the PLY file when i need them. It's dirty, I know (but fast!). 
* I use global variables to pass attributes to callback function post_draw.



## Copyright

2017 Alec Jacobson, Daniele Panozzo, Christian Schüller, Olga Diamanti, Qingnan Zhou, Sebastian Koch, Jeremie Dumas, Amir Vaxman, Nico Pietroni, Stefan Brugger, Kenshi Takayama, Wenzel Jakob, Nikolas De Giorgis, Luigi Rocca, Leonardo Sacht, Kevin Walliman, Olga Sorkine-Hornung, and others.

Please see individual files for appropriate copyright notices.
