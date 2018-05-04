#include <vector>
#include <memory>
#include <fstream>

#include "spot_light.hpp"
#include "proj_light.hpp"
#include "dir_light.hpp"

const int width = 1024;
const int height = 1024;

std::vector<double>
build_image( const std::vector<std::unique_ptr<lightsource>>& lights )
{
  std::vector<double> image(3*width*height);
  double scalex = 2./(width-1);
  double scaley = 2./(height-1);
  double zp = -1;
  for ( int i = 0; i < height; ++i ) {
    double yp = -1. + scaley * i;
    for ( int j = 0; j < width; ++j ) {
      double xp = -1. + scalex*j;
      double red = 0., green = 0., blue = 0.;
      for ( const auto& l : lights ){
	auto col = l->contrib( {xp,yp,zp}, {0.,0.,1.} );
	image[3*(i*width+j)+0] -= col[0];
	image[3*(i*width+j)+1] -= col[1];
	image[3*(i*width+j)+2] -= col[2];	
      }
    }
  }
  return image;
}

void save_image( const char* name, const std::vector<double>& image )
{
  std::ofstream ofs(name, std::ios::out|std::ios::binary);
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for ( double x : image )
    ofs << (unsigned char)(255*std::min(x,1.));
  ofs.close();
}

int main()
{
  std::vector<std::unique_ptr<lightsource>> lights;
  lights.push_back(std::make_unique<spot_light>(std::array<double,3>{0.5,0.5,1.},
						std::array<double,3>{1.,0.,0.}));
  lights.push_back(std::make_unique<spot_light>(std::array<double,3>{-0.5,0.5,1.},
						std::array<double,3>{0.,1.,0.}));
  lights.push_back(std::make_unique<spot_light>(std::array<double,3>{0.5,-0.5,1.},
						std::array<double,3>{0.,0.,1.}));
  auto img1 = build_image(lights);
  save_image("spotlight.ppm",img1);
  lights.resize(0);
  lights.push_back(std::make_unique<dir_light>(std::array<double,3>{0.,0.,-1.},
					       std::array<double,3>{1.,0.,0.}));
  lights.push_back(std::make_unique<dir_light>(std::array<double,3>{0.,0.,-1.},
					       std::array<double,3>{0.,1.,0.}));
  lights.push_back(std::make_unique<dir_light>(std::array<double,3>{0.,0.,-1.},
					       std::array<double,3>{0.,0.,1.}));
  auto img2 = build_image(lights);
  save_image("dirlight.ppm",img2);
  lights.resize(0);
  double deg2rad_coef = 0.03490658503988659;
  double max_angl = 15*deg2rad_coef;
  lights.push_back(std::make_unique<proj_light>(std::array<double,3>{0.5,0.5,1.},
						std::array<double,3>{0.0,0.0,-1.},
						max_angl,
						std::array<double,3>{1.,0.,0.}));
  lights.push_back(std::make_unique<proj_light>(std::array<double,3>{-0.5,0.5,1.},
						std::array<double,3>{0.1,0.0,-1.},
						max_angl,
						std::array<double,3>{0.,1.,0.}));
  lights.push_back(std::make_unique<proj_light>(std::array<double,3>{0.5,-0.5,1.},
						std::array<double,3>{0.,0.1,-1.},
						max_angl,
						std::array<double,3>{0.,0.,1.}));
  
  auto img3 = build_image(lights);
  save_image("projlight.ppm",img3);
  max_angl = 30*deg2rad_coef;
  lights.resize(0);
  lights.push_back(std::make_unique<dir_light>(std::array<double,3>{0.,0.,-1.},
					       std::array<double,3>{1.,0.,0.}));
  lights.push_back(std::make_unique<spot_light>(std::array<double,3>{-0.5,0.5,0.25},
						std::array<double,3>{0.,1.,0.}));
  lights.push_back(std::make_unique<proj_light>(std::array<double,3>{0.5,-0.5,0.125},
						std::array<double,3>{-0.3,0.1,-1.},
						max_angl,
						std::array<double,3>{0.,0.,1.}));
  
  auto img4 = build_image(lights);
  save_image("comp_light.ppm",img4);

  return 0;
}
