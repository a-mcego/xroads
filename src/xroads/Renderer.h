#pragma once

#include "Shapes.h"
#include <unordered_map>
#include <map>
#include "Textures.h"
#include "Shaders.h"
#include "Vox.h"

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


    inline std::vector<Sprite> CutAtlas(GLuint texture_id, const C2i& size)
    {
        std::vector<Sprite> sprites;
        for(int y=0; y<size.y; ++y)
        {
            for(int x=0; x<size.x; ++x)
            {
                Sprite s;

                const float bleeding_preventer = 0.009f;

                s.texture_id = texture_id;
                s.UV.topleft.x = (float(x)+bleeding_preventer)/float(size.x);
                s.UV.topleft.y = (float(y)+bleeding_preventer)/float(size.y);
                s.UV.bottomright.x = (float(x+1)-bleeding_preventer)/float(size.x);
                s.UV.bottomright.y = (float(y+1)-bleeding_preventer)/float(size.y);

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
            N
        };

        static constexpr CAMERA STAGE_TO_CAMERA[int(STAGE::N)] = {CAMERA::PERSPECTIVE, CAMERA::PERSPECTIVE, CAMERA::PERSPECTIVE, CAMERA::PERSPECTIVE, CAMERA::ORTHO};

        struct RenderList
        {
            GLuint texture=-1, shader=-1;
            std::vector<Vertex> vertices;
        };

        static std::array<std::vector<RenderList>,int(STAGE::N)> renderlists;

        struct LightDef
        {
            C3 pos;
            Color color;
            float linear{};
            float quadratic{};
        };
        static int n_lights_last_frame;
        static std::vector<LightDef> lights;
        static void QueueLight(const C3& pos, const Color& color, float linear, float quadratic)
        {
            lights.push_back({pos, color*2.0f, linear*0.1f, quadratic*0.01f});
        }
        static Color flash_color;
        static float flash_amount;
        static void Flash(Color color, float amount)
        {
            flash_color = color;
            flash_amount = amount;
        }

        static int trianglecount;
        static int searches, getvecs;

        static const u32 MAX_VERTICES_PER_FRAME = (1<<20);

        static GLuint vertexarray_id, vertexbuffer_id;

        static std::array<glm::mat4,int(CAMERA::N)> Vs, Ps; //transforms for each camera


        struct Model
        {
            GLuint vertexbuffer=0;
            std::vector<Vertex> vertices;
            Sprite sprite;

            Model(const VoxModel& vm, const Sprite& sprite_):sprite(sprite_)
            {
                auto& quads = vm.quads;
                auto& colors = vm.colors;

                XrAssert(quads.size(), ==, colors.size());

                for(int q_i=0; q_i<quads.size(); ++q_i)
                {
                    auto& quad = quads[q_i];
                    auto& color = colors[q_i];
                    auto verts = quad.GetVertices(sprite);

                    for(int v_i=0; v_i<verts.size(); ++v_i)
                    {
                        Vertex v;

                        v.x = verts[v_i].x;
                        v.y = verts[v_i].y; //swap y and z and handedness
                        v.z = verts[v_i].z; //swap y and z and handedness
                        v.u = verts[v_i].u;
                        v.v = verts[v_i].v;

                        v.r = color.r;
                        v.g = color.g;
                        v.b = color.b;
                        v.a = color.a;

                        vertices.push_back(v);
                        if (v_i%3 == 2)
                        {
                            int last = vertices.size()-1;
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

                int n_vertices = vertices.size();
                glGenBuffers(1, &vertexbuffer);
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, n_vertices*sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
        };

        //vector<std::pair<ModelID, glm::mat4>> modelqueue;

        static std::vector<Model> models;
        static std::map<std::string, ModelID> model_ids;

        static ModelID GetModelID(const std::string& modelname)
        {
            if (model_ids.contains(modelname))
                return model_ids[modelname];
            VoxModel vm = VoxLoader::Load(modelname, CENTERING::XYZ);
            Model m = Model(vm, Sprite{Textures::Get("white")});
            models.push_back(m);
            model_ids[modelname] = ModelID{models.size()-1};
            return ModelID{models.size()-1};
        }

        static std::map<ModelID, std::vector<std::pair<Color,glm::mat4>>> modelqueue;
        static void Queue(const std::string& modelname, const glm::mat4& M_matrix, const Color& color)
        {
            modelqueue[GetModelID(modelname)].push_back(std::make_pair(color,M_matrix));
        }

        static void ChangeBuffer(int buffer_id)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0 * sizeof(float)));
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7 * sizeof(float)));
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9 * sizeof(float)));
        }

        static void Init()
        {
            glGenVertexArrays(1, &quadVAO);
            glGenVertexArrays(1, &vertexarray_id);
            glBindVertexArray(vertexarray_id);
            glGenBuffers(1, &vertexbuffer_id);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_id);
            glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES_PER_FRAME*sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

            ChangeBuffer(vertexbuffer_id);

            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_BLEND);

             glEnable(GL_CULL_FACE);
        }

        static void SetV(CAMERA camera, const glm::mat4& mat)
        {
            Vs[int(camera)] = mat;
        }
        static void SetP(CAMERA camera, const glm::mat4& mat)
        {
            Ps[int(camera)] = mat;
        }

        static void Quit()
        {
            glDeleteVertexArrays(1, &vertexarray_id);
        }

        static std::vector<Vertex>& GetVec(STAGE stage, GLuint texture, GLuint shader)
        {
            getvecs += 1;
            auto& stage_lists = renderlists[int(stage)];

            for(auto& stage_list: stage_lists)
            {
                searches += 1;
                if (stage_list.texture == texture && stage_list.shader == shader)
                {
                    return stage_list.vertices;
                }
            }

            //not found, add.

            stage_lists.push_back(RenderList{ .texture = texture, .shader = shader, .vertices{} });
            return stage_lists.back().vertices;
        }

        static void Queue(const IsQuadType auto& quad, const Color& color, const Sprite& sprite, GLuint shader_id, STAGE stage)
        {
            auto verts = quad.GetVertices(sprite);

            //auto vert_order = quad.GetVertexOrder();

            for(int i=0; i<verts.size(); ++i)
            {
                Vertex v;

                v.x = verts[i].x;
                v.y = verts[i].y; //swap y and z
                v.z = verts[i].z; //swap y and z
                v.u = verts[i].u;
                v.v = verts[i].v;

                v.r = color.r;
                v.g = color.g;
                v.b = color.b;
                v.a = color.a;

                auto& vec = GetVec(stage, sprite.texture_id, shader_id);
                vec.push_back(v);

                if (vec.size()%3 == 0)
                {
                    int v_i = vec.size()-1;
                    auto normal = TriangleNormal(vec[v_i-2],vec[v_i-1],vec[v_i]);

                    vec[v_i-2].nx = normal.x;
                    vec[v_i-2].ny = normal.y;
                    vec[v_i-2].nz = normal.z;

                    vec[v_i-1].nx = normal.x;
                    vec[v_i-1].ny = normal.y;
                    vec[v_i-1].nz = normal.z;

                    vec[v_i-0].nx = normal.x;
                    vec[v_i-0].ny = normal.y;
                    vec[v_i-0].nz = normal.z;
                }
            }
        }

        template<typename... Ts>
        static void Uniform(GLuint shader_id, std::string name, Ts... data)
        {
            constexpr int SIZE = sizeof...(Ts);
            static_assert(AllSameType<Ts...>, "uniform operands should be the same type");
            static_assert(SIZE > 0, "must have some uniform operands");
            static_assert(SIZE <= 4, "too many uniform operands");

            using T = typename std::tuple_element<0, std::tuple<Ts...>>::type;
            static_assert(IsOneOf<T, int, float, glm::mat4, bf16>, "data doesn't have a supported type");

            static_assert(!(std::is_same_v<T,glm::mat4> && sizeof...(Ts) != 1), "if passing a matrix, only pass one");

            std::array<T,SIZE> arr;
            int index=0;
            ((arr.at(index) = data, ++index),...);
            if constexpr(std::is_same_v<T,int>)
            {
                if constexpr(SIZE == 1)
                    glUniform1iv(glGetUniformLocation(shader_id,name.c_str()), 1, arr.data());
                if constexpr(SIZE == 2)
                    glUniform2iv(glGetUniformLocation(shader_id,name.c_str()), 1, arr.data());
                if constexpr(SIZE == 3)
                    glUniform3iv(glGetUniformLocation(shader_id,name.c_str()), 1, arr.data());
                if constexpr(SIZE == 4)
                    glUniform4iv(glGetUniformLocation(shader_id,name.c_str()), 1, arr.data());
            }
            if constexpr(std::is_same_v<T,float>)
            {
                if constexpr(SIZE == 1)
                    glUniform1fv(glGetUniformLocation(shader_id,name.c_str()), 1, arr.data());
                if constexpr(SIZE == 2)
                    glUniform2fv(glGetUniformLocation(shader_id,name.c_str()), 1, arr.data());
                if constexpr(SIZE == 3)
                    glUniform3fv(glGetUniformLocation(shader_id,name.c_str()), 1, arr.data());
                if constexpr(SIZE == 4)
                    glUniform4fv(glGetUniformLocation(shader_id,name.c_str()), 1, arr.data());
            }
            if constexpr(std::is_same_v<T,glm::mat4>)
            {
                if constexpr(SIZE == 1)
                    glUniformMatrix4fv(glGetUniformLocation(shader_id,name.c_str()), 1, false, &arr[0][0][0]);
            }

        }
        static float aberration;
        static void Aberration(float param) { aberration = param; }
        struct FBTex
        {
            GLuint FBO{}, buffer{};
        };
        static FBTex pingpong[2];


        static GLuint gBuffer, gPosition, gNormal, gColorSpec, rboDepth, gAlbedoSpec, gBright, gNormalOut, fbSecond;
        static void Render()
        {


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
                    glDepthMask(GL_FALSE);
                else
                    glDepthMask(GL_TRUE);

                if (STAGE(r_i) == STAGE::HEALTHBAR)
                    glDisable(GL_DEPTH_TEST);
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
                    GLuint shader_id = a.shader;//first.second;
                    glUseProgram(shader_id);
                    Uniform(shader_id, "blendcolor", 1.0f, 1.0f, 1.0f);
                    Uniform(shader_id, "current_time", float(fmod(glfwGetTime(),1000.0f)));

                    //auto loc = glGetUniformLocation(shader_id, "modelviewpers");
                    //glUniformMatrix4fv(loc, 1, GL_FALSE, &VPs[int(STAGE_TO_CAMERA[r_i])][0][0]);
                    Uniform(shader_id, "Vs", Vs[int(STAGE_TO_CAMERA[r_i])]);
                    Uniform(shader_id, "Ps", Ps[int(STAGE_TO_CAMERA[r_i])]);

                    glBufferSubData(GL_ARRAY_BUFFER, 0, a.vertices.size()*sizeof(Vertex),(void*)a.vertices.data());
                    glDrawArrays(GL_TRIANGLES, 0, a.vertices.size());
                    trianglecount += a.vertices.size()/3;
                    a.vertices.clear();
                }

                if (STAGE(r_i) == STAGE::ACTOR)
                {
                    GLuint default_shader_id = Shaders::Get("default2_color");
                    glUseProgram(default_shader_id);
                    for(auto& [id, matrices]: modelqueue)
                    {
                        Renderer::Model& m = models[id];
                        ChangeBuffer(m.vertexbuffer);

                        GLuint texture_id = m.sprite.texture_id;
                        glBindTexture(GL_TEXTURE_2D, texture_id);

                        for(auto& [color, matrix]: matrices)
                        {
                            glm::mat4 tempmat = Vs[int(STAGE_TO_CAMERA[r_i])] * matrix;
                            Uniform(default_shader_id, "Ps", Ps[int(STAGE_TO_CAMERA[r_i])]);
                            Uniform(default_shader_id, "Vs", tempmat);
                            Uniform(default_shader_id, "blendcolor", color.r, color.g, color.b);
                            glDrawArrays(GL_TRIANGLES, 0, m.vertices.size());
                            trianglecount += m.vertices.size()/3;
                        }
                        matrices.clear();
                    }
                    modelqueue.clear();
                    ChangeBuffer(vertexbuffer_id);
                    Uniform(default_shader_id, "blendcolor", 1.0f, 1.0f, 1.0f);

                }

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

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

            Uniform(deferred_program_id, "aberration", aberration);


            int N_MAX_LIGHTS = 96;
            // send light relevant uniforms
            Uniform(deferred_program_id, "Vs", Vs[int(CAMERA::PERSPECTIVE)]);
            for (unsigned int i = 0; i < Min((unsigned long long)(N_MAX_LIGHTS),lights.size()); i++)
            {
                auto& light = lights[i];
                Uniform(deferred_program_id, "lights[" + std::to_string(i) + "].Position", light.pos.x, light.pos.y, light.pos.z);
                Uniform(deferred_program_id, "lights[" + std::to_string(i) + "].Color", light.color.r, light.color.g, light.color.b);
                //const float linear = 0.7f;
                //const float quadratic = 1.8f;
                Uniform(deferred_program_id, "lights[" + std::to_string(i) + "].Linear", light.linear);
                Uniform(deferred_program_id, "lights[" + std::to_string(i) + "].Quadratic", light.quadratic);

                //shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", light.pos[0], light.pos[1], light.pos[2]);
                //shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", light.color[0], light.color[1], light.color[2]);
                // update attenuation parameters and calculate radius
                //shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
                //shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
            }
            Uniform(deferred_program_id, "n_lights", int(Min((unsigned long long)(N_MAX_LIGHTS),lights.size())));
            //shaderLightingPass.setVec3("viewPos", camera.Position);
            n_lights_last_frame = lights.size();
            lights.clear();
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
                unsigned int amount = 6;

                GLuint blur_program = Shaders::Get("blur");
                glUseProgram(blur_program);
                for (unsigned int i = 0; i < amount; i++)
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
                Uniform(bf,"flash",flash_color.r,flash_color.g,flash_color.b,flash_amount);
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
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            for(int r_i=int(STAGE::HEALTHBAR); r_i<int(STAGE::N); ++r_i)
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
        }

        static GLuint quadVAO, quadVBO;
        static void renderQuad()
        {
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        static void ResolutionChange(const C2i& new_resolution)
        {
            int display_x=new_resolution.x, display_y=new_resolution.y;
            //glfwGetFramebufferSize(glfw_window, &display_x, &display_y);
            //std::cout << "Resolution change: " << display_x << "x" << display_y << std::endl;
            glViewport(0, 0, display_x, display_y);

            for (unsigned int i = 0; i < 2; i++)
            {
                glGenFramebuffers(1, &pingpong[i].FBO);
                glGenTextures(1, &pingpong[i].buffer);

                glBindFramebuffer(GL_FRAMEBUFFER, pingpong[i].FBO);
                glBindTexture(GL_TEXTURE_2D, pingpong[i].buffer);
                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGBA16F, display_x, display_y, 0, GL_RGBA, GL_FLOAT, NULL
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

            glGenFramebuffers(1, &gBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

            // - position color buffer
            glGenTextures(1, &gPosition);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, display_x, display_y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

            // - normal color buffer
            glGenTextures(1, &gNormal);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, display_x, display_y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

            // - color + specular color buffer
            glGenTextures(1, &gAlbedoSpec);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, display_x, display_y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

            {
                GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
                glDrawBuffers(3, attachments);
            }

            // create and attach depth buffer (renderbuffer)
            glGenRenderbuffers(1, &rboDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, display_x, display_y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
            // finally check if framebuffer is complete
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Framebuffer not complete!" << std::endl;


            glGenFramebuffers(1, &fbSecond);
            glBindFramebuffer(GL_FRAMEBUFFER, fbSecond);
            {
                GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
                glDrawBuffers(2, attachments);
            }

            // - bright color buffer
            glGenTextures(1, &gBright);
            glBindTexture(GL_TEXTURE_2D, gBright);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, display_x, display_y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBright, 0);

            // - normie color buffer
            glGenTextures(1, &gNormalOut);
            glBindTexture(GL_TEXTURE_2D, gNormalOut);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, display_x, display_y, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalOut, 0);


            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        static u32 LoadShader(std::string name);
        static u32 LoadTexture(std::string name, WRAP wrap);
    };
}
