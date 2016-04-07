/** MyTable On GitHub
    https://github.com/Kiritow/MyTable
*/
#ifndef HC_TECH_GITHUB_PROJECT_MYTABLE_CPP
#define HC_TECH_GITHUB_PROJECT_MYTABLE_CPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <vector>
using namespace std;

#include "MyTable.h"

namespace _internal_mytable
{
    struct datapack
    {
    private:
        long size;
        void* data;
    public:
        datapack()
        {
            size=-1;
            data=nullptr;
        }
        ~datapack()
        {
            free(data);
        }
        datapack(long DataSize)
        {
            size=DataSize;
            data=malloc(size);
            if(data==nullptr)
            {
                size=-1;
            }
            else
            {
                memset(data,0,size);
            }
        }
        datapack(long DataSize,void* DataMem)
        {
            size=DataSize;
            data=malloc(size);
            if(data==nullptr)
            {
                size=-1;
            }
            else
            {
                memcpy(data,DataMem,size);
            }
        }
        datapack(const datapack& inc)
        {
            size=inc.size;
            if(size<0)
            {
                data=nullptr;
                return;
            }
            data=malloc(size);
            if(data==nullptr)
            {
                size=-1;
            }
            else
            {
                memcpy(data,inc.data,size);
            }
        }
        datapack& operator = (const datapack& inc)
        {
            if(this==&inc)
            {
                return *this;
            }
            free(data);
            data=nullptr;
            size=inc.size;
            if(size<0)
            {
                return *this;
            }
            data=malloc(size);
            if(data==nullptr)
            {
                size=-1;
                return *this;
            }
            memcpy(data,inc.data,size);
            return *this;
        }
        long getsize()
        {
            return size;
        }
        void* getdata()
        {
            return data;
        }
    };
    struct table
    {
        string dbaddr;
        double version;
        vector<string> colname;
        //long col; // col == colname.size()
        vector<vector<datapack>> data;
        bool saved;
    };
}/// End of namespace _internal_mytable

namespace mytable
{
    const double MYTABLE_VERSION = 1.0;

    using TABLE = _internal_mytable::table;
    using TABLEHANDLE = _internal_mytable::table*;
    enum class callbackret
    {
        NO_MEM=-1,
        FILE_EXIST=-2,
        CANNOT_WRITE=-3,
        FILE_NOT_EXIST=-4,
        VERSION_TOO_HIGH=-5,
        VERSION_TOO_LOW=-6,
    };

    inline void setcallret(int* callret,callbackret status)
    {
        if(callret!=nullptr)
        {
            *callret=static_cast<int>(status);
        }
    }


    TABLEHANDLE create_table(const char* FileAddress,bool overwrite,int* callret)
    {
        TABLEHANDLE handle=new TABLE;
        if(handle==nullptr)
        {
            setcallret(callret,callbackret::NO_MEM);
            return nullptr;
        }
        FILE* fp=fopen(FileAddress,"r");
        if(fp!=nullptr)
        {
            fclose(fp);
            if(!overwrite)
            {
                setcallret(callret,callbackret::FILE_EXIST);
                delete handle;
                return nullptr;
            }
        }

        fp=fopen(FileAddress,"wb");
        fwrite(&MYTABLE_VERSION,sizeof(double),1,fp);
        long sz=0;
        fwrite(&sz,sizeof(long),1,fp);
        fclose(fp);
        handle->dbaddr=FileAddress;
        handle->version=MYTABLE_VERSION;
        handle->saved=true;
        return handle;
    }

    TABLEHANDLE open_table(const char* FileAddress,int* callret)
    {
        TABLEHANDLE handle=new TABLE;
        if(handle==nullptr)
        {
            setcallret(callret,callbackret::NO_MEM);
            return nullptr;
        }
        FILE* fp=fopen(FileAddress,"r");
        if(fp==nullptr)
        {
            setcallret(callret,callbackret::FILE_NOT_EXIST);
            delete handle;
            return nullptr;
        }
        fclose(fp);
        fp=fopen(FileAddress,"a");
        if(fp==nullptr)
        {
            setcallret(callret,callbackret::CANNOT_WRITE);
            delete handle;
            return nullptr;
        }
        fclose(fp);

        handle->dbaddr=FileAddress;
        fp=fopen(FileAddress,"r");
        fread(&handle->version,sizeof(double),1,fp);
        if(handle->version-MYTABLE_VERSION>0.01)
        {
            /// version too high
            delete handle;
            fclose(fp);
            setcallret(callret,callbackret::VERSION_TOO_HIGH);
            return nullptr;
        }
        else if(handle->version-MYTABLE_VERSION<0.01)
        {
            /// version too low
            delete handle;
            fclose(fp);
            setcallret(callret,callbackret::VERSION_TOO_LOW);
            return nullptr;
        }
        /// version pass.

        long col;
        fread(&col,sizeof(long),1,fp);
        if(col>0)
        {
            thread_local char buffer[256];
            long sz;
            for(long thiscol=0;thiscol<col;thiscol++)
            {
                fread(&sz,sizeof(long),1,fp);
                memset(buffer,0,256);
                if(sz==0)
                {
                    handle->colname.push_back("");
                }
                else
                {
                    fread(buffer,sz,1,fp);
                    handle->colname.push_back((string)buffer);
                }
            }
        }

        long sz;
        while(fread(&sz,sizeof(long),1,fp)>0)
        {
            vector<_internal_mytable::datapack> dpk;
            _internal_mytable::datapack* ppack;
        }
    }
}/// End of namespace mytable




#endif /// End of HC_TECH_GITHUB_PROJECT_MYTABLE_CPP
