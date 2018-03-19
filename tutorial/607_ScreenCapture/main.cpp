 #include <igl/readOBJ.h>
 #include <igl/readPLY.h>
#include <igl/viewer/Viewer.h>
#include <iostream>
#include "tutorial_shared_path.h"
#include <igl/png/writePNG.h>
#include <igl/png/readPNG.h>
#include <string>
#include <igl/per_vertex_normals.h>
#include <igl/jet.h>
#include <math.h>       /* cos */
#define PI 3.14159265
#include <igl/get_seconds.h>
#include <igl/read_triangle_mesh.h>
#include <igl/viewer/Viewer.h>
#include <Eigen/Core>
#include <thread>
#include <future>
 int angle = 0;

 int theta;
 Eigen::MatrixXd V;
 Eigen::MatrixXd rot(3,3);
 Eigen::MatrixXi F;
 Eigen::MatrixXd N_faces;
 // This file propagates attributes through normal coordinates
std::string inputFilename;
// std::string outputFilename;
// This function is called every time a keyboard button is pressed


 bool post_draw(igl::viewer::Viewer& viewer)
{

    Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> R(1280,800);
    Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> G(1280,800);
    Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> B(1280,800);
    Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> A(1280,800);

    // Draw the scene in the buffers
    viewer.core.draw_buffer(viewer.data,viewer.opengl,false,R,G,B,A);

    // Save it to a PNG
    auto str = std::to_string(angle);
    igl::png::writePNG(R,G,B,A,"/home/thibault/tmp/out" + str + ".png");
    std::cout << angle << std::endl;
    viewer.data.clear();
    angle = angle + 1;
    V = V*rot;


    viewer.data.set_mesh(V, F);
    viewer.data.set_colors(N_faces);
    if(angle>360){
        std::_Exit(0);    // return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cout << "usage: ./607_ScreenCapture_bin meshin.ply";
    return -1;
  }

  const std::string inputFilename = argv[1];

    theta  = 1.0;
  // Load a mesh in OFF format
  rot(0,0) = cos(theta* PI / 180.0);
  rot(0,1) = 0;
  rot(0,2) = sin(theta* PI / 180.0);
  rot(1,0) = 0;
  rot(1,1) = 1;
  rot(1,2) = 0;
  rot(2,0) = -sin(theta* PI / 180.0);
  rot(2,1) = 0;
  rot(2,2) = cos(theta* PI / 180.0);



  Eigen::MatrixXd UV_faces;

  Eigen::MatrixXd C;

  std::cout << "loading ";
  std::cout << inputFilename;

  igl::readPLY(inputFilename, V, F, N_faces, UV_faces);
  std::cout << " loaded!\n";
  std::cout <<  F.rows();
  std::cout << " loaded!\n";
  std::cout <<  V.rows();
  std::cout << " loaded!\n";
  std::cout << "The matrix V is of size "
            << V.rows() << "x" << V.cols() << std::endl;
  std::cout << "The matrix F is of size "
            << F.rows() << "x" << F.cols() << std::endl;

   for (size_t i=0; i<V.rows(); i++)
   {
       N_faces(i, 0) = N_faces(i, 0)/255.0;
       N_faces(i, 1) = N_faces(i, 1)/255.0;
       N_faces(i, 2) = N_faces(i, 2)/255.0;
   }

//    V = V*rot;
  // Plot the mesh and register the callback
  igl::viewer::Viewer viewer;

  float dist = 4;
  // It all happens here !
  Eigen::Vector3f camera_center = Eigen::Vector3f(0,0,0);
  Eigen::Vector3f camera_eye    = Eigen::Vector3f(dist,dist,dist).normalized()*dist;
  Eigen::Vector3f camera_up     = Eigen::Vector3f(0,1,0);


    viewer.core.camera_center = camera_center;
    viewer.core.camera_up = camera_up;
    viewer.core.camera_eye = camera_eye;
    viewer.core.show_lines = 0;

    viewer.callback_post_draw = &post_draw;
    V = V*rot;
    viewer.data.set_mesh(V, F);
    viewer.data.set_colors(N_faces);
    viewer.launch_init(true,false);

    viewer.opengl.shader_mesh.free();

    {
        std::string mesh_vertex_shader_string =
                R"(#version 150
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
in vec3 position;
in vec3 normal;
out vec3 position_eye;
out vec3 normal_eye;
in vec4 Ka;
in vec4 Kd;
in vec4 Ks;
in vec2 texcoord;
out vec2 texcoordi;
out vec4 Kai;
out vec4 Kdi;
out vec4 Ksi;

void main()
{
  position_eye = vec3 (view * model * vec4 (position, 1.0));
  normal_eye = vec3 (view * model * vec4 (normal, 0.0));
  normal_eye = normalize(normal_eye);
  gl_Position = proj * vec4 (position_eye, 1.0); //proj * view * model * vec4(position, 1.0);
  Kai = Ka;
  Kdi = Kd;
  Ksi = Ks;
  texcoordi = texcoord;
})";

        std::string mesh_fragment_shader_string =
                R"(#version 150
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec4 fixed_color;
in vec3 position_eye;
in vec3 normal_eye;
uniform vec3 light_position_world;
vec3 Ls = vec3 (1, 1, 1);
vec3 Ld = vec3 (1, 1, 1);
vec3 La = vec3 (1, 1, 1);
in vec4 Ksi;
in vec4 Kdi;
in vec4 Kai;
in vec2 texcoordi;
uniform sampler2D tex;
uniform float specular_exponent;
uniform float lighting_factor;
uniform float texture_factor;
out vec4 outColor;
void main()
{
vec3 Ia = La * vec3(Kai);    // ambient intensity

vec3 light_position_eye = vec3 (view * vec4 (light_position_world, 1.0));
vec3 vector_to_light_eye = light_position_eye - position_eye;
vec3 direction_to_light_eye = normalize (vector_to_light_eye);
float dot_prod = dot (direction_to_light_eye, normal_eye);
float clamped_dot_prod = max (dot_prod, -dot_prod);
vec3 Id = Ld * vec3(Kdi) * clamped_dot_prod;    // Diffuse intensity

vec3 reflection_eye = reflect (-direction_to_light_eye, normal_eye);
vec3 surface_to_viewer_eye = normalize (-position_eye);
float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
dot_prod_specular = float(abs(dot_prod)==dot_prod) * max (dot_prod_specular, 0.0);
float specular_factor = pow (dot_prod_specular, specular_exponent);
vec3 Kfi = 0.5*vec3(Ksi);
vec3 Lf = Ls;
float fresnel_exponent = 2*specular_exponent;
float fresnel_factor = 0;
{
  float NE = max( 0., dot( normal_eye, surface_to_viewer_eye));
  fresnel_factor = pow (max(sqrt(1. - NE*NE),0.0), fresnel_exponent);
}
vec3 Is = Ls * vec3(Ksi) * specular_factor;    // specular intensity
vec3 If = Lf * vec3(Kfi) * fresnel_factor;     // fresnel intensity
vec4 color = vec4(lighting_factor * (If + Is + Id) + Ia +
  (1.0-lighting_factor) * vec3(Kdi),(Kai.a+Ksi.a+Kdi.a)/3);
outColor = mix(vec4(1,1,1,1), texture(tex, texcoordi), texture_factor) * color;
if (fixed_color != vec4(0.0)) outColor = fixed_color;
})";
        viewer.opengl.shader_mesh.init(
                mesh_vertex_shader_string,
                mesh_fragment_shader_string,
                "outColor");
    }

    viewer.launch_rendering(true);
    viewer.launch_shut();
}