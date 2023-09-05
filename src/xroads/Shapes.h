#pragma once

namespace Xroads
{
    struct Sprite
    {
        GLuint texture_id;
        Rect2D<float> UV = {{0.f,0.f},{1.f,1.f}};
    };

    struct VertexUV
    {
        float x=0.0f, y=0.0f, z=0.0f, u=0.0f, v=0.0f;
    };

    struct Quad
    {
        Rect2D<float> position;
        float z=0.f;


        std::array<VertexUV,6> GetVertices(const Sprite& s) const
        {
            return std::array<VertexUV,6>{{
                {position.topleft.x,     position.topleft.y,     z, s.UV.topleft.x,     s.UV.topleft.y    }, //0
                {position.bottomright.x,    position.bottomright.y, z, s.UV.bottomright.x, s.UV.bottomright.y}, //2
                {position.bottomright.x,    position.topleft.y,    z, s.UV.bottomright.x, s.UV.topleft.y    }, //1
                {position.topleft.x,     position.topleft.y,     z, s.UV.topleft.x,     s.UV.topleft.y    }, //0
                {position.topleft.x,  position.bottomright.y,  z, s.UV.topleft.x,     s.UV.bottomright.y}, //3
                {position.bottomright.x, position.bottomright.y, z, s.UV.bottomright.x, s.UV.bottomright.y}, //2
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

        std::array<VertexUV,6> GetVertices(const Sprite& s) const
        {
            C3 topleft     = center+(-x_axis-y_axis)*(size*0.5f);
            C3 topright    = center+( x_axis-y_axis)*(size*0.5f);
            C3 bottomleft  = center+(-x_axis+y_axis)*(size*0.5f);
            C3 bottomright = center+( x_axis+y_axis)*(size*0.5f);

            return std::array<VertexUV,6>{{
                {topleft.x,     topleft.y,     topleft.z,     s.UV.topleft.x,     s.UV.topleft.y    }, //0
                {bottomright.x, bottomright.y, bottomright.z, s.UV.bottomright.x, s.UV.bottomright.y}, //2
                {topright.x,    topright.y,    topright.z,    s.UV.bottomright.x, s.UV.topleft.y    }, //1
                {topleft.x,     topleft.y,     topleft.z,     s.UV.topleft.x,     s.UV.topleft.y    }, //0
                {bottomleft.x,  bottomleft.y,  bottomleft.z,  s.UV.topleft.x,     s.UV.bottomright.y}, //3
                {bottomright.x, bottomright.y, bottomright.z, s.UV.bottomright.x, s.UV.bottomright.y}, //2
            }};
        }
    };

    struct FullQuad
    {
        std::array<C3,4> points;
        std::array<VertexUV,6> GetVertices(const Sprite& s) const
        {
            const C3& topleft     = points[0];
            const C3& topright    = points[1];
            const C3& bottomleft  = points[2];
            const C3& bottomright = points[3];

            return std::array<VertexUV,6>{{
                {topleft.x,     topleft.y,     topleft.z,     s.UV.topleft.x,     s.UV.topleft.y    }, //0
                {bottomright.x, bottomright.y, bottomright.z, s.UV.bottomright.x, s.UV.bottomright.y}, //2
                {topright.x,    topright.y,    topright.z,    s.UV.bottomright.x, s.UV.topleft.y    }, //1
                {topleft.x,     topleft.y,     topleft.z,     s.UV.topleft.x,     s.UV.topleft.y    }, //0
                {bottomleft.x,  bottomleft.y,  bottomleft.z,  s.UV.topleft.x,     s.UV.bottomright.y}, //3
                {bottomright.x, bottomright.y, bottomright.z, s.UV.bottomright.x, s.UV.bottomright.y}, //2
            }};
        }
    };
}
