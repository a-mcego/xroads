#pragma once

#include "Strutil.h"
#include "Vox.h"
#include "Shapes.h"

#include <fstream>
#include <set>

namespace Xroads
{
    struct ObjModel
    {
        std::vector<FullTriangle> quads;
        std::vector<std::vector<C2>> uvs;
        std::vector<Color> colors;
    };

    struct ObjLoader
    {
        static bool ObjExists(const std::string& objname, const std::string& objdir="vox")
        {
            return FileExists(objdir+"/"+objname+".obj");
        }

        static Color ColorTransform(Color c)
        {
            Color total = Color{0.5f,0.2960f,0.2568f}*c.r + Color{0.2568f,0.5f,0.2568f}*c.g + Color{0.2568f,0.2960f,0.5f}*c.b;
            return total*(1.0f/1.5f);
        }

        static ObjModel Load(const std::string& objname, const std::string& objdir="vox")
        {
            ObjModel objmodel;
            std::vector<C3> vertices;
            std::vector<Color> color_indices;
            std::vector<C2> UVs;

            std::ifstream objfile(objdir+"/"+objname+".obj");
            Log("Loading obj model! "+(objdir+"/"+objname+".obj"));;
            while(true)
            {
                std::string line;
                std::getline(objfile, line);
                if(!objfile.good() or objfile.eof())
                    break;
                if (line.size() < 3)
                    continue;
                while(true)
                {
                    if (line.empty())
                        break;
                    if (line.back() == '\r')
                        line.pop_back();
                    else if (line.back() == '\n')
                        line.pop_back();
                    else
                        break;
                }

                auto params = Explode(line, ' ');
                if (params[0] == "v")
                {
                    f32 x = FromString<f32>(params[1]);
                    f32 z = -FromString<f32>(params[2]);
                    f32 y = FromString<f32>(params[3]);
                    vertices.push_back(C3{x,y,z});
                    //load vertex
                }
                else if (params[0] == "vn");//vertex normals can be ignored!
                else if (params[0] == "vt")
                {
                    UVs.push_back({FromString<f32>(params[1]),1.0f-FromString<f32>(params[2])});

                    i32 palette_index = i32(FromString<f32>(params[1])*256.0f)+1;
                    color_indices.push_back(ColorTransform(ToColor(DEFAULT_PALETTE[palette_index])));
                }
                else if (params[0] == "f")
                {
                    float biggest_extent=0.0f;
                    for(auto& v: vertices)
                    {
                        biggest_extent = std::max(biggest_extent,std::abs(v.x));
                        biggest_extent = std::max(biggest_extent,std::abs(v.y));
                        biggest_extent = std::max(biggest_extent,std::abs(v.z));
                    }

                    int n_triangles = params.size()-3;
                    for(int i=0; i<n_triangles; ++i)
                    {
                        std::array<int,3> tri_indices{{1,i+3,i+2}};
                        FullTriangle tri;
                        i32 uv_id=0;

                        objmodel.uvs.push_back(std::vector<C2>());
                        auto& uvback = objmodel.uvs.back();

                        for(int tri_i=0; tri_i<3; ++tri_i)
                        {
                            auto vert_info = Explode(params.at(tri_indices.at(tri_i)), '/');
                            i32 vert_id = FromString<i32>(vert_info.at(0))-1; //-1 because .obj is 1-based indexing

                            uv_id = FromString<i32>(vert_info[1])-1; //-1 because .obj is 1-based indexing
                            //vert_info[2] has normals but we don't need them
                            tri.points[tri_i] = vertices.at(vert_id)/biggest_extent*0.5f;
                            uvback.push_back(UVs[uv_id]);
                        }

                        std::swap(uvback[1],uvback[2]);

                        Color objcolor{1.0f,1.0f,1.0f};

                        if (uvback[1].y < 0.2f)
                            objcolor *= 2.5f;

                        objmodel.colors.push_back(objcolor);//color_indices[uv_id]);
                        objmodel.quads.push_back(tri);
                    }
                }
            }
            return objmodel;
        }
    };
}
