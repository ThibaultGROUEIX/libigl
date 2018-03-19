 #include <igl/readOBJ.h>
 #include <igl/readPLY.h>
#include <igl/viewer/Viewer.h>
#include <iostream>
#include "tutorial_shared_path.h"
#include <igl/png/writePNG.h>
#include <igl/png/readPNG.h>
#include <string>
#include <igl/per_vertex_normals.h>

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

    igl::png::writePNG(R,G,B,A,"/home/thibault/tmp/out.png");
    std::_Exit(0);    // return false;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cout << "usage: ./607_ScreenCapture_bin meshin.ply";
    return -1;
  }

  const std::string inputFilename = argv[1];
  // Load a mesh in OFF format
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  std::cout << "loading ";
  std::cout << inputFilename;

  igl::readPLY(inputFilename, V, F);
  std::cout << " loaded!\n";
  Eigen::MatrixXi Fnew(F.rows()*2, 3);
  Eigen::MatrixXd Vnew(V.rows()*2, 3);
  // Eigen::MatrixXi FTCnew(FTC.rows()*2, 3);
  //
  Eigen::MatrixXd Nvtx;
  igl::per_vertex_normals(V, F, Nvtx);
  //
  double epsilon = 0.0001;
  for (size_t i=0; i<V.rows(); i++)
  {
     Vnew.row(i) = V.row(i) + Nvtx.row(i) * epsilon;
     Vnew.row(V.rows()+i) = V.row(i) - Nvtx.row(i) * epsilon;
  }
  //
  for (size_t i=0; i<F.rows(); i++)
  {
     Fnew(2*i+0, 0) = F(i,0);
     Fnew(2*i+0, 1) = F(i,1);
     Fnew(2*i+0, 2) = F(i,2);
     Fnew(2*i+1, 0) = V.rows()+F(i,0);
     Fnew(2*i+1, 1) = V.rows()+F(i,2);
     Fnew(2*i+1, 2) = V.rows()+F(i,1);
     //
    //  FTCnew(2*i+0, 0) = FTC(i, 0);
    //  FTCnew(2*i+0, 1) = FTC(i, 1);
    //  FTCnew(2*i+0, 2) = FTC(i, 2);
    //  FTCnew(2*i+1, 0) = FTC(i, 0);
    //  FTCnew(2*i+1, 1) = FTC(i, 2);
    //  FTCnew(2*i+1, 2) = FTC(i, 1);
  }
  //
  // FTC = FTCnew;
  F = Fnew;
  V = Vnew;

  // std::cerr << "Press 1 to render the scene and save it in a png." << std::endl;
  // std::cerr << "Press 2 to load the saved png and use it as a texture." << std::endl;

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
  viewer.data.set_mesh(V, F);
  viewer.launch();
}