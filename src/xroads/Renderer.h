#pragma once

#include "Shapes.h"
#include <unordered_map>
#include <map>
#include "Textures.h"
#include "Shaders.h"
#include "Strutil.h"
#include "Vox.h"
#include "Obj.h"

namespace Xroads
{
    //TODO: make packing cross-platform
    #pragma pack(push,4)
    struct Vertex
    {
        float x{}, y{}, z{};
        float r{}, g{}, b{}, a{};
        float u{}, v{};
        float nx{1.0f}, ny{}, nz{};
    };
    #pragma pack(pop)


    inline std::vector<TextureDef> CutAtlas(GLuint texture_id, const C2i& size)
    {
        std::vector<TextureDef> sprites;
        for(int y=0; y<size.y; ++y)
        {
            for(int x=0; x<size.x; ++x)
            {
                TextureDef s;

                const float bleeding_preventer = 0.009f;

                s.texture_id = texture_id;
                Rect2D<float> uv_rect;
                uv_rect.topleft.x = (float(x)+bleeding_preventer)/float(size.x);
                uv_rect.topleft.y = (float(y)+bleeding_preventer)/float(size.y);
                uv_rect.bottomright.x = (float(x+1)-bleeding_preventer)/float(size.x);
                uv_rect.bottomright.y = (float(y+1)-bleeding_preventer)/float(size.y);

                s.UV[0] = uv_rect.GetTopLeft();
                s.UV[1] = uv_rect.GetTopRight();
                s.UV[2] = uv_rect.GetBottomRight();
                s.UV[3] = uv_rect.GetBottomLeft();

                sprites.push_back(s);
            }
        }
        return sprites;
    }

    struct hash_pair
    {
        template <class T1, class T2>
        size_t operator()(const std::pair<T1, T2>& p) const
        {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);

            if (hash1 != hash2)
            {
                return hash1 ^ hash2;
            }

            // If hash1 == hash2, their XOR is zero.
            return hash1;
        }
    };

    //MakeStrong(ModelID, u64);
    using ModelID = u64;


    //this struct changes the bf16 type to f32 so we can use bf16 with opengl
    //used in the Uniform function
    template<typename TYPE>
    struct ToOpenglType
    {
        using T = TYPE;
    };
    template<>
    struct ToOpenglType<bf16>
    {
        using T = f32;
    };


    struct Renderer
    {
        enum struct CAMERA
        {
            PERSPECTIVE,
            ORTHO,
            N
        };

        enum struct STAGE
        {
            TERRAIN,
            SHADOW,
            ACTOR,
            HEALTHBAR,
            GUI,
            GUITEXT,
            N
        };

        static constexpr CAMERA STAGE_TO_CAMERA[i32(STAGE::N)] = {CAMERA::PERSPECTIVE, CAMERA::PERSPECTIVE, CAMERA::PERSPECTIVE, CAMERA::PERSPECTIVE, CAMERA::ORTHO, CAMERA::ORTHO};

        struct RenderList
        {
            GLuint texture=-1, texture2=-1, shader=-1;
            std::vector<Vertex> vertices;
        };

        static std::array<std::vector<RenderList>,i32(STAGE::N)> renderlists;

        /*struct LightDef
        {
            C3 pos;
            Color color;
            f32 intensity{};
        };*/
        //static i32 n_lights_last_frame;
        //static std::array<std::vector<LightDef>,3> lights;
        static void QueueLight(const C3& pos, const Color& color, f32 intensity);
        static void Flash(Color color, f32 amount)
        {
            options.flash_color = color;
            options.flash_amount = amount;
        }

        static i64 trianglecount;
        static i32 searches, getvecs;

        static const u32 MAX_VERTICES_PER_FRAME = (1<<20);

        static GLuint vertexarray_id, vertexbuffer_id;

        static std::array<glm::mat4,i32(CAMERA::N)> Vs, Ps; //transforms for each camera
        static std::array<C3,i32(CAMERA::N)> Vs_normal;

        static C2i current_resolution;


        struct Model
        {
            GLuint vertexbuffer=0;
            std::vector<Vertex> vertices;
            TextureDef texturedef;

            template<typename ModelType>
            Model(const ModelType& vm, const TextureDef& texturedef_):texturedef(texturedef_)
            {
                auto& faces = vm.quads;
                auto& uvs = vm.uvs;
                auto& colors = vm.colors;

                XrAssert(faces.size(), ==, colors.size());
                for(i32 q_i=0; q_i<faces.size(); ++q_i)
                {
                    auto& face = faces[q_i];
                    auto& uv = uvs[q_i];
                    auto& color = colors[q_i];
                    auto verts = face.GetVertices(texturedef);

                    for(i32 v_i=0; v_i<verts.size(); ++v_i)
                    {
                        Vertex v;

                        v.x = verts[v_i].x;
                        v.y = verts[v_i].y;
                        v.z = verts[v_i].z;
                        v.u = uv.at(v_i).x;
                        v.v = uv[v_i].y;

                        v.r = color.r;
                        v.g = color.g;
                        v.b = color.b;
                        v.a = 1.0f;//color.a;

                        vertices.push_back(v);
                        if (v_i%3 == 2)
                        {
                            i32 last = vertices.size()-1;
                            auto normal = TriangleNormal(vertices[last-2],vertices[last-1],vertices[last]);

                            vertices[last-2].nx = normal.x;
                            vertices[last-2].ny = normal.y;
                            vertices[last-2].nz = normal.z;

                            vertices[last-1].nx = normal.x;
                            vertices[last-1].ny = normal.y;
                            vertices[last-1].nz = normal.z;

                            vertices[last-0].nx = normal.x;
                            vertices[last-0].ny = normal.y;
                            vertices[last-0].nz = normal.z;
                        }

                    }
                }

                i32 n_vertices = vertices.size();
                //std::cout << "Generating buffer for " << ToString(faces.size()) << " faces, " << n_vertices << " vertices." << std::endl;
                glGenBuffers(1, &vertexbuffer);
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, n_vertices*sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
        };

        static std::vector<Model> models;
        static std::map<std::string, ModelID, StringLessThan> model_ids;

        static CENTERING current_centering;

        static ModelID GetModelID(std::string_view modelname_view, const GLuint texture_id)
        {
            if (auto iter = model_ids.find(modelname_view); iter!=model_ids.end())
                return iter->second;

            std::string modelname{modelname_view};
            if (ObjLoader::ObjExists(modelname))
            {
                ObjModel vm = ObjLoader::Load(modelname, "vox");
                Model m = Model(vm, TextureDef(Textures::Get("_tex_"+modelname)));
                models.push_back(m);
            }
            else
            {
                VoxModel vm = VoxLoader::Load(modelname, current_centering);
                Model m = Model(vm, TextureDef(texture_id));
                models.push_back(m);
            }
            model_ids[modelname] = ModelID{models.size()-1};
            return ModelID{models.size()-1};
        }


        struct ModelQueueData
        {
            std::vector<Color> colors;
            std::vector<glm::mat4> matrices;

            void clear()
            {
                colors.clear();
                matrices.clear();
            }
        };

        static std::map<ModelID, ModelQueueData> modelqueue, modelqueueUI, modelqueue_lightvolume;

        static void QueueModel(std::map<ModelID, ModelQueueData>& mqd_origin, std::string_view modelname, const GLuint texture_id, const glm::mat4& M_matrix, const Color& color)
        {
            auto& mqd = mqd_origin[GetModelID(modelname, texture_id)];
            mqd.colors.push_back(color);
            mqd.matrices.push_back(M_matrix);
        }
        static void Queue(std::string_view modelname, const GLuint texture_id, const glm::mat4& M_matrix, const Color& color)
        {
            QueueModel(modelqueue,modelname,texture_id,M_matrix,color);
            //auto& mqd = modelqueue[GetModelID(modelname, texture_id)];
            //mqd.colors.push_back(color);
            //mqd.matrices.push_back(M_matrix);
        }
        static void QueueUI(std::string_view modelname, const GLuint texture_id, const glm::mat4& M_matrix, const Color& color)
        {
            QueueModel(modelqueueUI,modelname,texture_id,M_matrix,color);
            //auto& mqd = modelqueueUI[GetModelID(modelname, texture_id)];
            //mqd.colors.push_back(color);
            //mqd.matrices.push_back(M_matrix);
        }

        static void ChangeBuffer(int buffer_id)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0 * sizeof(f32)));
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(f32)));
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7 * sizeof(f32)));
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9 * sizeof(f32)));
        }

        //static std::vector<std::string> light_uniform_names;

        static void Init()
        {
            glGenVertexArrays(1, &quadVAO);
            glGenVertexArrays(1, &vertexarray_id);
            glBindVertexArray(vertexarray_id);
            glGenBuffers(1, &vertexbuffer_id);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_id);
            glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_PER_FRAME*sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
            glGenBuffers(2, &modelVBO[0]);
            {
                glBindBuffer(GL_ARRAY_BUFFER, modelVBO[0]);
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(5);
                glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)16);
                glEnableVertexAttribArray(6);
                glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)32);
                glEnableVertexAttribArray(7);
                glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)48);
                glVertexAttribDivisor(4, 1);
                glVertexAttribDivisor(5, 1);
                glVertexAttribDivisor(6, 1);
                glVertexAttribDivisor(7, 1);
            }
            {
                glBindBuffer(GL_ARRAY_BUFFER, modelVBO[1]);
                glEnableVertexAttribArray(8);
                glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
                glVertexAttribDivisor(8, 1);
            }

            ChangeBuffer(vertexbuffer_id);

            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_BLEND);

            glEnable(GL_CULL_FACE);

            /*light_uniform_names.clear();
            for(int i=0; i<200; ++i)
            {
                light_uniform_names.push_back("lights[" + std::to_string(i) + "].Position");
                light_uniform_names.push_back("lights[" + std::to_string(i) + "].Color");
            }*/
        }

        static void SetV(CAMERA camera, const glm::mat4& mat, const C3& normal)
        {
            Vs_normal[i32(camera)] = normal;
            Vs[i32(camera)] = mat;
        }
        static void SetP(CAMERA camera, const glm::mat4& mat)
        {
            Ps[i32(camera)] = mat;
        }

        static void Quit()
        {
            glDeleteVertexArrays(1, &vertexarray_id);
        }

        static std::vector<Vertex>& GetVec(STAGE stage, GLuint texture, GLuint texture2, GLuint shader)
        {
            getvecs += 1;
            auto& stage_lists = renderlists[i32(stage)];

            for(auto& stage_list: stage_lists)
            {
                searches += 1;
                if (stage_list.texture == texture && stage_list.shader == shader && stage_list.texture2 == texture2)
                {
                    return stage_list.vertices;
                }
            }

            //not found, add.
            stage_lists.push_back(RenderList{ .texture = texture, .texture2 = texture2, .shader = shader, .vertices{} });
            return stage_lists.back().vertices;
        }

        static void Queue(const IsShape auto& quad, const C3& normal, const Color& color, const TextureDef& texturedef, GLuint shader_id, STAGE stage)
        {
            auto verts = quad.GetVertices(texturedef);
            //auto camnormal = Vs_normal[int(STAGE_TO_CAMERA[int(stage)])];
            auto& vec = GetVec(stage, texturedef.texture_id, texturedef.texture2_id, shader_id);
            for(int i=0; i<verts.size(); i+=3)
            {
                vec.emplace_back(verts[i  ].x,verts[i  ].y, verts[i  ].z, color.r, color.g, color.b, color.a, verts[i  ].u, verts[i  ].v, normal.x, normal.y, normal.z);
                vec.emplace_back(verts[i+1].x,verts[i+1].y, verts[i+1].z, color.r, color.g, color.b, color.a, verts[i+1].u, verts[i+1].v, normal.x, normal.y, normal.z);
                vec.emplace_back(verts[i+2].x,verts[i+2].y, verts[i+2].z, color.r, color.g, color.b, color.a, verts[i+2].u, verts[i+2].v, normal.x, normal.y, normal.z);
            }
        }

        static void Queue(const IsShape auto& quad, const Color& color, const TextureDef& texturedef, GLuint shader_id, STAGE stage)
        {
            auto verts = quad.GetVertices(texturedef);

            auto& vec = GetVec(stage, texturedef.texture_id, texturedef.texture2_id, shader_id);
            //auto camnormal = Vs_normal[int(STAGE_TO_CAMERA[int(stage)])];
            for(int i=0; i<verts.size(); i+=3)
            {
                auto normal = TriangleNormal(verts[i],verts[i+1],verts[i+2]);

                vec.emplace_back(verts[i  ].x,verts[i  ].y, verts[i  ].z, color.r, color.g, color.b, color.a, verts[i  ].u, verts[i  ].v, normal.x, normal.y, normal.z);
                vec.emplace_back(verts[i+1].x,verts[i+1].y, verts[i+1].z, color.r, color.g, color.b, color.a, verts[i+1].u, verts[i+1].v, normal.x, normal.y, normal.z);
                vec.emplace_back(verts[i+2].x,verts[i+2].y, verts[i+2].z, color.r, color.g, color.b, color.a, verts[i+2].u, verts[i+2].v, normal.x, normal.y, normal.z);
            }
        }

        template<typename... Ts>
        static void Uniform(GLuint shader_id, std::string_view name, Ts... data) //OBS: make sure *name* is null-terminated!
        {
            constexpr int SIZE = sizeof...(Ts);
            static_assert(AllSameType<Ts...>, "uniform operands should be the same type");
            static_assert(SIZE > 0, "must have some uniform operands");
            static_assert(SIZE <= 4, "too many uniform operands");

            using T = typename std::tuple_element<0, std::tuple<Ts...>>::type;
            static_assert(IsOneOf<T, int, float, glm::mat4, bf16>, "data doesn't have a supported type");

            static_assert(!(std::is_same_v<T,glm::mat4> && sizeof...(Ts) != 1), "if passing a matrix, only pass one");

            std::array<typename ToOpenglType<T>::T,SIZE> arr;
            int index=0;
            ((arr[index] = data, ++index),...);
            if constexpr(std::is_same_v<T,int>)
            {
                if constexpr(SIZE == 1)
                    glUniform1iv(glGetUniformLocation(shader_id,name.data()), 1, arr.data());
                if constexpr(SIZE == 2)
                    glUniform2iv(glGetUniformLocation(shader_id,name.data()), 1, arr.data());
                if constexpr(SIZE == 3)
                    glUniform3iv(glGetUniformLocation(shader_id,name.data()), 1, arr.data());
                if constexpr(SIZE == 4)
                    glUniform4iv(glGetUniformLocation(shader_id,name.data()), 1, arr.data());
            }
            if constexpr(std::is_same_v<T,float>)
            {
                if constexpr(SIZE == 1)
                    glUniform1fv(glGetUniformLocation(shader_id,name.data()), 1, arr.data());
                if constexpr(SIZE == 2)
                    glUniform2fv(glGetUniformLocation(shader_id,name.data()), 1, arr.data());
                if constexpr(SIZE == 3)
                    glUniform3fv(glGetUniformLocation(shader_id,name.data()), 1, arr.data());
                if constexpr(SIZE == 4)
                    glUniform4fv(glGetUniformLocation(shader_id,name.data()), 1, arr.data());
            }
            if constexpr(std::is_same_v<T,glm::mat4>)
            {
                if constexpr(SIZE == 1)
                    glUniformMatrix4fv(glGetUniformLocation(shader_id,name.data()), 1, false, &arr[0][0][0]);
            }

        }

        struct Options
        {
            enum struct LIGHTING
            {
                LOW,
                MEDIUM,
                HIGH,
                N
            } lighting{LIGHTING::MEDIUM};

            f32 aberration{0.0f};
            i32 maxlights{16};
            f32 fog_intensity{1.0f};
            Color fog_color{1.0f,1.0f,1.0f};
            i32 normal_smoothing_n{1};
            i32 occlusion_n{0};
            f32 flash_amount{};
            Color flash_color{};
            i32 bloom_amount{0};

            int GetLightDivisor()
            {
                if (lighting == LIGHTING::LOW)
                    return 16;
                if (lighting == LIGHTING::MEDIUM)
                    return 4;
                return 1;
            }

            std::string GetShaderDefines()
            {
                std::string ret;
                if (lighting == LIGHTING::LOW)
                    ret += "#define LIGHTING_LOW\n";
                else if (lighting == LIGHTING::MEDIUM)
                    ret += "#define LIGHTING_MEDIUM\n";
                else if (lighting == LIGHTING::HIGH)
                    ret += "#define LIGHTING_HIGH\n";
                return ret;
            }
        } static options;


        static void Aberration(float param) { options.aberration = param; }
        struct FBTex
        {
            GLuint FBO{}, buffer{};
        };

        static FBTex pingpong[2];
        static GLuint gBuffer, gPosition, gNormal, gColorSpec, rboDepth, gAlbedoSpec, gBright, gNormalOut, gLighting, fbSecond, fbLighting;
        static GLuint modelVBO[2];
        static void Render()
        {
            /*auto GetLightAmount = []()->int
            {
                int total=0;
                for(auto& l: lights)
                    total += l.size();
                return total;
            };

            auto GetLight = [](int index)->LightDef&
            {
                for(auto& l: lights)
                {
                    if (index < l.size())
                        return l[index];
                    index -= l.size();
                }
                std::unreachable();
            };*/

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(vertexarray_id);
            glDisable(GL_BLEND);

            ChangeBuffer(vertexbuffer_id);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);

            for(int r_i=0; r_i<int(STAGE::HEALTHBAR); ++r_i)
            {
                auto& renderlist = renderlists[r_i];

                switch(CAMERA(STAGE_TO_CAMERA[r_i]))
                {
                case CAMERA::ORTHO:
                    glDisable(GL_DEPTH_TEST);
                    break;
                case CAMERA::PERSPECTIVE:
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LESS);
                    break;
                default:;
                }

                if (STAGE(r_i) == STAGE::SHADOW)
                {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE); // Additive blending
                    glBlendEquation(GL_FUNC_ADD);
                    glDepthMask(GL_FALSE);
                }
                else if (STAGE(r_i) == STAGE::HEALTHBAR)
                {
                    glDisable(GL_BLEND);
                    glDepthMask(GL_TRUE);
                    glDisable(GL_DEPTH_TEST);
                }
                else
                {
                    glDisable(GL_BLEND);
                    glDepthMask(GL_TRUE);
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LESS);
                }

                for (auto& a: renderlist)
                {
                    if (a.vertices.empty())
                        continue;

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, a.texture);

                    glUseProgram(a.shader);

                    Uniform(a.shader, "texture1", 0);
                    Uniform(a.shader, "texture2", 1);

                    if(a.texture2 > 0)
                    {
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, a.texture2);
                        Uniform(a.shader, "has_texture2", int(1));
                        glActiveTexture(GL_TEXTURE0);
                    }
                    else
                    {
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        Uniform(a.shader, "has_texture2", int(0));
                        glActiveTexture(GL_TEXTURE0);
                    }

                    Uniform(a.shader, "blendcolor", 1.0f, 1.0f, 1.0f);
                    Uniform(a.shader, "current_time", float(fmod(glfwGetTime(),1000.0f)));
                    //Uniform(a.shader, "water_effect", 1.0f);

                    //auto loc = glGetUniformLocation(a.shader, "modelviewpers");
                    //glUniformMatrix4fv(loc, 1, GL_FALSE, &VPs[int(STAGE_TO_CAMERA[r_i])][0][0]);
                    Uniform(a.shader, "Vs", Vs[int(STAGE_TO_CAMERA[r_i])]);
                    Uniform(a.shader, "Ps", Ps[int(STAGE_TO_CAMERA[r_i])]);

                    /*if (options.lighting == Options::LIGHTING::LOW)
                    {
                        std::cout << "Lights low!" << std::endl;
                        const int N_MAX_LIGHTS = options.maxlights;

                        const int lights_this_frame = Min(N_MAX_LIGHTS,GetLightAmount());

                        // send light relevant uniforms
                        for (unsigned int i = 0; i < lights_this_frame; i++)
                        {
                            auto& light = GetLight(i);
                            Uniform(a.shader, light_uniform_names[i*2], light.pos.x, light.pos.y, light.pos.z, light.intensity);
                            Uniform(a.shader, light_uniform_names[i*2+1], light.color.r, light.color.g, light.color.b);
                        }
                        Uniform(a.shader, "n_lights",lights_this_frame);
                    }*/

                    glBufferSubData(GL_ARRAY_BUFFER, 0, a.vertices.size()*sizeof(Vertex),(void*)a.vertices.data());
                    glDrawArrays(GL_TRIANGLES, 0, a.vertices.size());
                    trianglecount += a.vertices.size()/3;
                    a.vertices.clear();
                }

                if (STAGE(r_i) == STAGE::ACTOR)
                {
                    auto XrglBufferDataFromVector = [](GLuint buf, const auto& vec)
                    {
                        glBindBuffer(GL_ARRAY_BUFFER, buf);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(typename std::remove_cvref<decltype(vec)>::type::value_type)*vec.size(), vec.data(), GL_DYNAMIC_DRAW);
                    };

                    GLuint default_shader_id = Shaders::Get("default_model");
                    glUseProgram(default_shader_id);
                    Uniform(default_shader_id, "Ps", Ps[int(STAGE_TO_CAMERA[r_i])]);
                    Uniform(default_shader_id, "Vs", Vs[int(STAGE_TO_CAMERA[r_i])]);

                    for(auto& [id, mdq]: modelqueue)
                    {
                        if (mdq.colors.empty())
                            continue;
                        Renderer::Model& m = models[id];
                        ChangeBuffer(m.vertexbuffer);

                        GLuint texture_id = m.texturedef.texture_id;
                        glBindTexture(GL_TEXTURE_2D, texture_id);

                        XrglBufferDataFromVector(modelVBO[0], mdq.matrices);
                        XrglBufferDataFromVector(modelVBO[1], mdq.colors);

                        glDrawArraysInstanced(GL_TRIANGLES, 0, m.vertices.size(), mdq.colors.size());
                        trianglecount += m.vertices.size()/3ULL*mdq.colors.size();
                        mdq.clear();
                    }
                    ChangeBuffer(vertexbuffer_id);
                    //glUseProgram(Shaders::Get("default2_color"));
                    //Uniform(default_shader_id, "blendcolor", 1.0f, 1.0f, 1.0f);
                }

            }


            {
                glViewport(0, 0, current_resolution.x/options.GetLightDivisor(), current_resolution.y/options.GetLightDivisor());
                glBindFramebuffer(GL_FRAMEBUFFER, fbLighting);
                glDisable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE); // Additive blending
                glBlendEquation(GL_FUNC_ADD);

                glClear(GL_COLOR_BUFFER_BIT);

                auto XrglBufferDataFromVector = [](GLuint buf, const auto& vec)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, buf);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(typename std::remove_cvref<decltype(vec)>::type::value_type)*vec.size(), vec.data(), GL_DYNAMIC_DRAW);
                };

                GLuint lv_shader_id = Shaders::Get("lightvolume");
                glUseProgram(lv_shader_id);
                Uniform(lv_shader_id, "gPosition_sampler", 0);
                Uniform(lv_shader_id, "gNormal_sampler", 1);
                Uniform(lv_shader_id, "gDiffuse_sampler", 2);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, gPosition);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, gNormal);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, gColorSpec);

                Uniform(lv_shader_id, "Ps", Ps[int(CAMERA::PERSPECTIVE)]);
                Uniform(lv_shader_id, "Vs", Vs[int(CAMERA::PERSPECTIVE)]);
                Uniform(lv_shader_id, "inv_texsize", 1.0f/f32(current_resolution.x/options.GetLightDivisor()), 1.0f/f32(current_resolution.y/options.GetLightDivisor()));
                for(auto& [id, mdq]: modelqueue_lightvolume)
                {
                    Renderer::Model& m = models[id];
                    ChangeBuffer(m.vertexbuffer);

                    XrglBufferDataFromVector(modelVBO[0], mdq.matrices);
                    XrglBufferDataFromVector(modelVBO[1], mdq.colors);

                    glDrawArraysInstanced(GL_TRIANGLES, 0, m.vertices.size(), mdq.colors.size());
                    trianglecount += m.vertices.size()/3ULL*mdq.colors.size();
                    mdq.clear();
                }
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glViewport(0, 0, current_resolution.x, current_resolution.y);
            }


            glBindFramebuffer(GL_FRAMEBUFFER, fbSecond);

            // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
            // -----------------------------------------------------------------------------------------------------------------------
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //shaderLightingPass.use();

            GLuint deferred_program_id = Shaders::Get("deferred");

            glUseProgram(deferred_program_id);

            Uniform(deferred_program_id, "gPosition", 0);
            Uniform(deferred_program_id, "gNormal", 1);
            Uniform(deferred_program_id, "gAlbedoSpec", 2);
            Uniform(deferred_program_id, "gLighting", 3);
            Uniform(deferred_program_id, "fog_attributes", options.fog_color.r, options.fog_color.g, options.fog_color.b, options.fog_intensity);
            Uniform(deferred_program_id, "normal_smoothing_n", options.normal_smoothing_n);
            Uniform(deferred_program_id, "occlusion_n", options.occlusion_n);
            Uniform(deferred_program_id, "inv_texsize", 1.0f/f32(current_resolution.x/options.GetLightDivisor()), 1.0f/f32(current_resolution.y/options.GetLightDivisor()));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, gLighting);

            Uniform(deferred_program_id, "aberration", options.aberration);

            //if (options.lighting == Options::LIGHTING::HIGH)
            //{
                /*const int N_MAX_LIGHTS = options.maxlights;
                // send light relevant uniforms
                Uniform(deferred_program_id, "Vs", Vs[int(CAMERA::PERSPECTIVE)]);
                n_lights_last_frame = 0;
                for (unsigned int i = 0; i < Min(N_MAX_LIGHTS,GetLightAmount()); i++)
                {
                    auto& light = GetLight(i);
                    Uniform(deferred_program_id, light_uniform_names[i*2], light.pos.x, light.pos.y, light.pos.z, light.intensity);
                    Uniform(deferred_program_id, light_uniform_names[i*2+1], light.color.r, light.color.g, light.color.b);
                    ++n_lights_last_frame;
                }
                Uniform(deferred_program_id, "n_lights", Min(N_MAX_LIGHTS,GetLightAmount()));*/
            //}
            //shaderLightingPass.setVec3("viewPos", camera.Position);
            //for(auto& l: lights)
            //    l.clear();
            // finally render quad
            renderQuad();




            // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
            // ----------------------------------------------------------------------------------
            /*glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
            // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
            // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
            // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
            int display_w, display_h;
            glfwGetFramebufferSize(glfw_window, &display_w, &display_h);
            glBlitFramebuffer(0, 0, display_w, display_h, 0, 0, display_w, display_h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

            {
                //bool horizontal = true, first_iteration = true;
                bool first_iteration = true;
                int horizontal = 1;

                GLuint blur_program = Shaders::Get("blur");
                glUseProgram(blur_program);
                for (int i = 0; i < options.bloom_amount; ++i)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, pingpong[horizontal%2].FBO);
                    Uniform(blur_program,"horizontal",horizontal);
                    Uniform(blur_program,"image",0);
                    glActiveTexture(GL_TEXTURE0);
                    ++horizontal;
                    glBindTexture(GL_TEXTURE_2D, first_iteration ? gBright : pingpong[horizontal%2].buffer);  // bind texture of other framebuffer (or scene if first iteration)
                    renderQuad();
                    //horizontal = !horizontal;
                    if (first_iteration)
                        first_iteration = false;
                }
                ++horizontal;

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                GLuint bf = Shaders::Get("bloomfinal");
                glUseProgram(bf);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, gNormalOut);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, pingpong[horizontal%2].buffer);
                int bloom = 1;
                float exposure = 1.0f;
                Uniform(bf,"scene",0);
                Uniform(bf,"bloomBlur",1);
                Uniform(bf,"bloom",bloom);
                Uniform(bf,"exposure",exposure);
                Uniform(bf,"flash",options.flash_color.r,options.flash_color.g,options.flash_color.b,options.flash_amount);
                glClear(GL_DEPTH_BUFFER_BIT);
                renderQuad();
            }

            glBindVertexArray(vertexarray_id);
            ChangeBuffer(vertexbuffer_id);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glActiveTexture(GL_TEXTURE0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glBlendColor(1.0f,1.0f,1.0f,1.0f);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
            glBlendEquation(GL_FUNC_ADD);

            for(int r_i=int(STAGE::HEALTHBAR); r_i<int(STAGE::N); ++r_i)
            {
                auto& renderlist = renderlists[r_i];

                /*switch(CAMERA(STAGE_TO_CAMERA[r_i]))
                {
                case CAMERA::ORTHO:
                    glDisable(GL_DEPTH_TEST);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    break;
                case CAMERA::PERSPECTIVE:
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LESS);
                    break;
                default:;
                }*/

                if (STAGE(r_i) == STAGE::HEALTHBAR)
                {
                    glDisable(GL_DEPTH_TEST);
                }
                else
                {
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LESS);
                }

                for (auto& a: renderlist)
                {
                    if (a.vertices.empty())
                        continue;

                    GLuint texture_id = a.texture;//first.first;
                    glBindTexture(GL_TEXTURE_2D, texture_id);
                    GLuint shader_id = Shaders::Get("gui");//first.second;
                    glUseProgram(shader_id);

                    Uniform(shader_id, "Vs", Vs[int(STAGE_TO_CAMERA[r_i])]);
                    Uniform(shader_id, "Ps", Ps[int(STAGE_TO_CAMERA[r_i])]);

                    glBufferSubData(GL_ARRAY_BUFFER, 0, a.vertices.size()*sizeof(Vertex),(void*)a.vertices.data());
                    glDrawArrays(GL_TRIANGLES, 0, a.vertices.size());
                    trianglecount += a.vertices.size()/3;
                    a.vertices.clear();
                }
            }

            //modelqueueUI
            /*
            {
                //glClear(GL_DEPTH_BUFFER_BIT);
                GLuint default_shader_id = Shaders::Get("gui");
                glUseProgram(default_shader_id);
                Uniform(default_shader_id, "has_texture2", int(0));
                for(auto& [id, matrices]: modelqueueUI)
                {
                    Renderer::Model& m = models[id];
                    ChangeBuffer(m.vertexbuffer);

                    GLuint texture_id = m.texturedef.texture_id;
                    glBindTexture(GL_TEXTURE_2D, texture_id);

                    for(auto& [color, matrix]: matrices)
                    {
                        glm::mat4 tempmat = Vs[int(STAGE_TO_CAMERA[int(STAGE::GUI)])] * matrix;
                        Uniform(default_shader_id, "Ps", Ps[int(STAGE_TO_CAMERA[int(STAGE::GUI)])]);
                        Uniform(default_shader_id, "Vs", tempmat);
                        Uniform(default_shader_id, "blendcolor", color.r, color.g, color.b);
                        glDrawArrays(GL_TRIANGLES, 0, m.vertices.size());
                        trianglecount += m.vertices.size()/3;
                    }
                    matrices.clear();
                }
                ChangeBuffer(vertexbuffer_id);
                Uniform(default_shader_id, "blendcolor", 1.0f, 1.0f, 1.0f);
            }*/
        }

        static GLuint quadVAO, quadVBO;
        static void renderQuad()
        {
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        static void ResolutionChange(const C2i& new_resolution)
        {
            current_resolution = new_resolution;
            //glfwGetFramebufferSize(glfw_window, &display_x, &display_y);
            //std::cout << "Resolution change: " << display_x << "x" << display_y << std::endl;
            glViewport(0, 0, current_resolution.x, current_resolution.y);

            GLuint attachments[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };

            for (int i=0; i<2; ++i)
            {
                glDeleteFramebuffers(1, &pingpong[i].FBO);
                glGenFramebuffers(1, &pingpong[i].FBO);
                glDeleteTextures(1, &pingpong[i].buffer);
                glGenTextures(1, &pingpong[i].buffer);

                glBindFramebuffer(GL_FRAMEBUFFER, pingpong[i].FBO);
                glBindTexture(GL_TEXTURE_2D, pingpong[i].buffer);
                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGBA16F, current_resolution.x, current_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL
                );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpong[i].buffer, 0
                );
            }

            glDeleteFramebuffers(1, &gBuffer);
            glGenFramebuffers(1, &gBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

            // - position color buffer
            glDeleteTextures(1, &gPosition);
            glGenTextures(1, &gPosition);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, current_resolution.x, current_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

            // - normal color buffer
            glDeleteTextures(1, &gNormal);
            glGenTextures(1, &gNormal);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, current_resolution.x, current_resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

            // - color + specular color buffer
            glDeleteTextures(1, &gAlbedoSpec);
            glGenTextures(1, &gAlbedoSpec);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, current_resolution.x, current_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
            glDrawBuffers(3, attachments);

            // create and attach depth buffer (renderbuffer)
            glDeleteRenderbuffers(1, &rboDepth);
            glGenRenderbuffers(1, &rboDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, current_resolution.x, current_resolution.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
            // finally check if framebuffer is complete
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                Kill("Framebuffer 1 not complete!");

            glDeleteFramebuffers(1, &fbLighting);
            glGenFramebuffers(1, &fbLighting);
            glBindFramebuffer(GL_FRAMEBUFFER, fbLighting);

            // - lighting buffer
            glDeleteTextures(1, &gLighting);
            glGenTextures(1, &gLighting);
            glBindTexture(GL_TEXTURE_2D, gLighting);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, current_resolution.x/options.GetLightDivisor(), current_resolution.y/options.GetLightDivisor(), 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gLighting, 0);
            glDrawBuffers(1, attachments);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                Kill("Framebuffer 2 not complete!");

            glDeleteFramebuffers(1, &fbSecond);
            glGenFramebuffers(1, &fbSecond);
            glBindFramebuffer(GL_FRAMEBUFFER, fbSecond);
            glDrawBuffers(2, attachments);

            // - bright color buffer
            glDeleteTextures(1, &gBright);
            glGenTextures(1, &gBright);
            glBindTexture(GL_TEXTURE_2D, gBright);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, current_resolution.x, current_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBright, 0);

            // - normie color buffer
            glDeleteTextures(1, &gNormalOut);
            glGenTextures(1, &gNormalOut);
            glBindTexture(GL_TEXTURE_2D, gNormalOut);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, current_resolution.x, current_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalOut, 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                Kill("Framebuffer 3 not complete!");

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        static u32 LoadShader(std::string_view name);
        static u32 LoadTexture(std::string_view name, WRAP wrap);
    };
}
