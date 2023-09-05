#pragma once

#include <array>

namespace Xroads
{
    const std::array<u32,256> DEFAULT_PALETTE = {
        0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
        0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
        0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
        0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
        0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
        0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
        0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
        0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
        0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
        0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
        0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
        0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
        0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
        0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
        0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
        0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
    };

    template<typename T>
    T Read(const std::vector<u8>& v, int& pos)
    {
        T ret = *((T*)((&v[0])+pos));
        pos += sizeof(T);
        return ret;
    }

    template<typename T>
    T ReadAbs(const std::vector<u8>& v, int pos)
    {
        T ret = *((T*)((&v[0])+pos));
        return ret;
    }

    template<typename T>
    std::vector<T> ReadMany(const std::vector<u8>& v, int& pos, int N)
    {
        std::vector<T> ret(N,T());
        for(int i=0; i<N; ++i)
            ret[i] = Read<T>(v, pos);
        return ret;
    }

    //FIXME: i'd like this to go inside VoxLoader but it cannot be done.
    inline consteval u32 charptr_to_int(const char* c)
    {
        u32 ret=0;
        ret += u32(c[0]);
        ret += u32(c[1])<<8;
        ret += u32(c[2])<<16;
        ret += u32(c[3])<<24;
        return ret;
    }

    #include "xroads/Shapes.h"

    enum struct CENTERING
    {
        XY, XYZ
    };
    struct VoxModel
    {
        std::vector<GeneralQuad> quads;
        std::vector<Color> colors;

        Color ToColor(u32 color)
        {
            Color ret;
            //powf to bring this into sRGB
            ret.r = powf((color%0x100)/255.0f,2.2f);
            ret.g = powf(((color>>8)%0x100)/255.0f,2.2f);
            ret.b = powf(((color>>16)%0x100)/255.0f,2.2f);
            ret.a = ((color>>24)%0x100)/255.0f; //no gamma correction for alpha channel
            return ret;
        }

        void FinalizeModel(CENTERING centering)
        {
            //float maxs = //std::max(std::max(size.x,size.y),size.z);
            f32 maxs = [this,centering]()->f32
            {
                if (centering == CENTERING::XY)
                    return std::max(size.x,size.y);
                return std::max(std::max(size.x,size.y),size.z);
            }();
            //const C3 offset = C3(0.5f,0.5f,(centering==CENTERING::XYZ?0.5f:0.0f));

            C3 offset = C3{f32(size.x),f32(size.y),f32(size.z)}*(0.5f/maxs);
            if (centering == CENTERING::XY)
            {
                offset.z = 0.0f;
            }

            quads.clear();
            colors.clear();


            for(int z=0; z<size.z; ++z)
            {
                for(int y=0; y<size.y; ++y)
                {
                    for(int x=0; x<size.x; ++x)
                    {
                        if (Get({x,y,z}) == 0)
                            continue;

                        Color current_color = ToColor(palette[Get({x,y,z})-2]);

                        if (Get({x-1,y,z}) == 0)
                        {
                            C3 midpoint = C3{float(x), float(y)+0.5f, float(z)+0.5f} / maxs - offset;
                            quads.push_back({midpoint, 1.0f/maxs, {0.0f,-1.0f,0.0f}, {0.0f,0.0f,1.0f}});
                            colors.push_back(current_color);
                        }
                        if (Get({x+1,y,z}) == 0)
                        {
                            C3 midpoint = C3{float(x+1), float(y)+0.5f, float(z)+0.5f} / maxs - offset;
                            quads.push_back({midpoint, 1.0f/maxs, {0.0f,1.0f,0.0f}, {0.0f,0.0f,1.0f}});
                            colors.push_back(current_color);
                        }

                        if (Get({x,y-1,z}) == 0)
                        {
                            C3 midpoint = C3{float(x)+0.5f, float(y), float(z)+0.5f} / maxs - offset;
                            quads.push_back({midpoint, 1.0f/maxs, {0.0f,0.0f,-1.0f}, {1.0f,0.0f,0.0f}});
                            colors.push_back(current_color*0.92f);
                        }
                        if (Get({x,y+1,z}) == 0)
                        {
                            C3 midpoint = C3{float(x)+0.5f, float(y+1), float(z)+0.5f} / maxs - offset;
                            quads.push_back({midpoint, 1.0f/maxs, {0.0f,0.0f,1.0f}, {1.0f,0.0f,0.0f}});
                            colors.push_back(current_color*0.92f);
                        }
                        if (Get({x,y,z-1}) == 0)
                        {
                            C3 midpoint = C3{float(x)+0.5f, float(y)+0.5f, float(z)} / maxs - offset;
                            quads.push_back({midpoint, 1.0f/maxs, {-1.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}});
                            colors.push_back(current_color*0.84f);
                        }
                        if (Get({x,y,z+1}) == 0)
                        {
                            C3 midpoint = C3{float(x)+0.5f, float(y)+0.5f, float(z+1)} / maxs - offset;
                            quads.push_back({midpoint, 1.0f/maxs, {1.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}});
                            colors.push_back(current_color*0.84f);
                        }
                    }
                }
            }
        }


        std::array<unsigned int,256> palette = DEFAULT_PALETTE;

        std::vector<u8> modeldata;

        C3i size;

        void SetSize(C3i c)
        {
            size = c;
            modeldata = std::vector<u8>(c.z*c.y*c.x,0);
        }

        bool InBounds(C3i c)
        {
            if (c.x < 0 or c.x >= size.x) return false;
            if (c.y < 0 or c.y >= size.y) return false;
            if (c.z < 0 or c.z >= size.z) return false;
            return true;
        }

        void Set(C3i c, u8 value)
        {
            if (!InBounds(c))
                return;
            int index = c.z*size.y*size.x + c.y*size.x + c.x;
            modeldata[index] = value;
        }

        u8 Get(C3i c)
        {
            if (!InBounds(c))
                return 0;
            return modeldata[c.z*size.y*size.x + c.y*size.x + c.x];
        }

    };

    struct VoxLoader
    {
        static constexpr u32 VOX = charptr_to_int("VOX ");
        static constexpr u32 PACK = charptr_to_int("PACK");
        static constexpr u32 SIZE = charptr_to_int("SIZE");
        static constexpr u32 XYZI = charptr_to_int("XYZI");
        static constexpr u32 RGBA = charptr_to_int("RGBA");
        static constexpr u32 MAIN = charptr_to_int("MAIN");

        struct Chunk
        {
            u32 name;
            std::vector<u8> data, child;
        };

        static VoxModel Load(const std::string& voxname, CENTERING centering, const std::string& voxdir="vox")
        {
            std::string filename = voxdir+"/"+voxname+".vox";
            std::vector<u8> data = ReadFile(filename);

            int pos=0;

            [[maybe_unused]] u32 chunkname = Read<u32>(data, pos);
            [[maybe_unused]] u32 voxversion = Read<u32>(data, pos);


            /*if (chunkname != VOX)
                cout << "Error loading vox, magic number not found." << endl;*/

            Chunk mainchunk = LoadChunk(data, pos);

            /*if (mainchunk.name != MAIN)
                cout << "Error loading vox, first chunk is not MAIN" << endl;*/

            //cout << mainchunk.data.size() << " - " << mainchunk.child.size() << endl;

            std::vector<Chunk> chunks;

            i32 mainpos=0;
            while(true)
            {
                if (mainpos >= mainchunk.child.size())
                    break;
                chunks.push_back(LoadChunk(mainchunk.child, mainpos));
            }

            VoxModel m;
            for(int ch_i=0; ch_i<chunks.size(); ++ch_i)
            {
                if (chunks[ch_i].name != SIZE)
                    continue;

                Chunk& size_c = chunks[ch_i];
                Chunk& model_c = chunks[ch_i+1];

                if (model_c.name != XYZI)
                    continue;

                C3i size;
                size.x = ReadAbs<u32>(size_c.data,0);
                size.y = ReadAbs<u32>(size_c.data,4);
                size.z = ReadAbs<u32>(size_c.data,8);

                //cout << "Model found, size " << size.x << "x" << size.y << "x" << size.z << endl;


                m.SetSize(size);

                int model_c_pos=0;

                int n_vertices = Read<u32>(model_c.data, model_c_pos);

                for(int v_i=0; v_i<n_vertices; ++v_i)
                {
                    u8 x = Read<u8>(model_c.data,model_c_pos);
                    u8 y = Read<u8>(model_c.data,model_c_pos);
                    u8 z = Read<u8>(model_c.data,model_c_pos);
                    u8 c = Read<u8>(model_c.data,model_c_pos);

                    m.Set(C3i(x,y,z), c+1);
                }


                break;
            }
            for(int ch_i=0; ch_i<chunks.size(); ++ch_i)
            {
                if (chunks[ch_i].name != RGBA)
                    continue;
                auto& pal = chunks[ch_i];
                if (pal.data.size() != 1024)
                    continue;
                for(int p_i=0; p_i<256; ++p_i)
                    m.palette[p_i] = ReadAbs<u32>(pal.data, p_i*4);
            }

            //auto quads = m.GetModel();

            //cout << quads.size() << " quads done." << endl;

            m.FinalizeModel(centering);

            return m;
        }


        static Chunk LoadChunk(std::vector<u8>& data, int& pos)
        {
            Chunk chunk;
            chunk.name = Read<u32>(data, pos);

            u32 datasize = Read<u32>(data,pos);
            u32 childsize = Read<u32>(data,pos);

            /*cout << char((chunk.name)&0xFF);
            cout << char((chunk.name>>8)&0xFF);
            cout << char((chunk.name>>16)&0xFF);
            cout << char((chunk.name>>24)&0xFF);
            cout << ": " << datasize << ", " << childsize << endl;*/


            chunk.data = ReadMany<u8>(data,pos,datasize);
            chunk.child = ReadMany<u8>(data,pos,childsize);

            return chunk;
        }
    };

}
