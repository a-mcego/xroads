#pragma once

namespace Xroads
{
    struct TextureDef
    {
        TextureDef(){};
        TextureDef(GLuint id):texture_id(id) {}
        TextureDef(GLuint id, const Rect2D<float>& rect):texture_id(id)
        {
            SetUVFromRect(rect);
        }
        TextureDef(GLuint id, const Rect2D<float>& rect, GLuint id2):texture_id(id),texture2_id(id2)
        {
            SetUVFromRect(rect);
        }
        void SetUVFromRect(const Rect2D<float>& rect)
        {
            UV[0] = rect.GetTopLeft();
            UV[1] = rect.GetTopRight();
            UV[2] = rect.GetBottomRight();
            UV[3] = rect.GetBottomLeft();
        }
        GLuint texture_id{0};
        std::array<Coord2D<float>,4> UV = {{{0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f}}};
        GLuint texture2_id{0};
        //Rect2D<float> UV = {{0.f,0.f},{1.f,1.f}};
    };

    struct VertexUV
    {
        float x=0.0f, y=0.0f, z=0.0f, u=0.0f, v=0.0f;
    };

    struct Quad
    {
        Rect2D<float> position;
        float z=0.f;
        std::array<VertexUV,6> GetVertices(const TextureDef& s) const
        {
            return std::array<VertexUV,6>{{
                {position.topleft.x,     position.topleft.y,     z, s.UV[0].x, s.UV[0].y}, //0
                {position.bottomright.x, position.bottomright.y, z, s.UV[2].x, s.UV[2].y}, //2
                {position.bottomright.x, position.topleft.y,     z, s.UV[1].x, s.UV[1].y}, //1
                {position.topleft.x,     position.topleft.y,     z, s.UV[0].x, s.UV[0].y}, //0
                {position.topleft.x,     position.bottomright.y, z, s.UV[3].x, s.UV[3].y}, //3
                {position.bottomright.x, position.bottomright.y, z, s.UV[2].x, s.UV[2].y}, //2
            }};
        }
    };
    using UIQuad = Quad; //for zero vortex. fix later.

    struct GeneralQuad
    {
        C3 center{};
        float size{1.0f};
        C3 x_axis{1.0,0.0,0.0};
        C3 y_axis{0.0,1.0,0.0};

        GeneralQuad() = default;
        GeneralQuad(const C3& center_, float size_, const C3& x_axis_, const C3& y_axis_)
        :center(center_),size(size_),x_axis(x_axis_),y_axis(y_axis_){}

        GeneralQuad(const Quad& q)
        {
            center.z = q.z;
            center.y = (q.position.topleft.y + q.position.bottomright.y)*0.5f;
            center.x = (q.position.topleft.x + q.position.bottomright.x)*0.5f;

            x_axis.x = q.position.bottomright.x - q.position.topleft.x;
            y_axis.y = q.position.bottomright.y - q.position.topleft.y;

            size = 1.0f;
        }

        GeneralQuad(const C2& center_, float z, float size_, const C2& x_axis_)
        {
            center.x = center_.x;
            center.y = center_.y;
            center.z = z;

            size = size_;

            x_axis.z = 0.0f;
            y_axis.z = 0.0f;

            x_axis.x = x_axis_.x;
            x_axis.y = x_axis_.y;
            y_axis.x = -x_axis_.y;
            y_axis.y = x_axis_.x;
            x_axis = x_axis.Normalize();
            y_axis = y_axis.Normalize();
        }

        std::array<VertexUV,6> GetVertices(const TextureDef& s) const
        {
            C3 topleft     = center+(-x_axis-y_axis)*(size*0.5f);
            C3 topright    = center+( x_axis-y_axis)*(size*0.5f);
            C3 bottomleft  = center+(-x_axis+y_axis)*(size*0.5f);
            C3 bottomright = center+( x_axis+y_axis)*(size*0.5f);

            return std::array<VertexUV,6>{{
                {topleft.x,     topleft.y,     topleft.z,     s.UV[0].x, s.UV[0].y}, //0
                {bottomright.x, bottomright.y, bottomright.z, s.UV[2].x, s.UV[2].y}, //2
                {topright.x,    topright.y,    topright.z,    s.UV[1].x, s.UV[1].y}, //1
                {topleft.x,     topleft.y,     topleft.z,     s.UV[0].x, s.UV[0].y}, //0
                {bottomleft.x,  bottomleft.y,  bottomleft.z,  s.UV[3].x, s.UV[3].y}, //3
                {bottomright.x, bottomright.y, bottomright.z, s.UV[2].x, s.UV[2].y}, //2
            }};
        }
    };

    struct FullTriangle
    {
        std::array<C3,3> points;
        std::array<VertexUV,3> GetVertices(const TextureDef& s) const
        {
            return std::array<VertexUV,3>{{
                {points[0].x, points[0].y, points[0].z, s.UV[0].x, s.UV[0].y}, //0
                {points[2].x, points[2].y, points[2].z, s.UV[2].x, s.UV[2].y}, //2
                {points[1].x, points[1].y, points[1].z, s.UV[1].x, s.UV[1].y}, //1
            }};
        }
    };
    struct FullQuad
    {
        std::array<C3,4> points;
        std::array<VertexUV,6> GetVertices(const TextureDef& s) const
        {
            const C3& topleft     = points[0];
            const C3& topright    = points[1];
            const C3& bottomleft  = points[2];
            const C3& bottomright = points[3];

            return std::array<VertexUV,6>{{
                {topleft.x,     topleft.y,     topleft.z,     s.UV[0].x, s.UV[0].y}, //0
                {bottomright.x, bottomright.y, bottomright.z, s.UV[2].x, s.UV[2].y}, //2
                {topright.x,    topright.y,    topright.z,    s.UV[1].x, s.UV[1].y}, //1
                {topleft.x,     topleft.y,     topleft.z,     s.UV[0].x, s.UV[0].y}, //0
                {bottomleft.x,  bottomleft.y,  bottomleft.z,  s.UV[3].x, s.UV[3].y}, //3
                {bottomright.x, bottomright.y, bottomright.z, s.UV[2].x, s.UV[2].y}, //2
            }};
        }
    };
}
