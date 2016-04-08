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
        datapack(long DataSize,const void* DataMem)
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
        int setsize(long DataSize)
        {
            if(size<0)
            {
                size=DataSize;
                data=malloc(size);
                if(data==nullptr)
                {
                    size=-1;
                    return -1;
                }
                memset(data,0,size);
                return 0;
            }
            else
            {
                free(data);
                size=DataSize;
                data=malloc(size);
                if(data==nullptr)
                {
                    size=-1;
                    return -1;
                }
                memset(data,0,size);
                return 0;
            }
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
        OK=0,
        NO_MEM=-1,
        FILE_EXIST=-2,
        CANNOT_WRITE=-3,
        FILE_NOT_EXIST=-4,
        VERSION_TOO_HIGH=-5,
        VERSION_TOO_LOW=-6,
        NULL_HANDLE=-7,
        BAD_COL_NUM=-8,
        BAD_LINE_NUM=-9,
        EMPTY_TABLE=-10,
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
        FILE* fp=fopen(FileAddress,"rb");
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

        setcallret(callret,callbackret::OK);
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
        FILE* fp=fopen(FileAddress,"rb");
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
        fp=fopen(FileAddress,"rb");
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
            ppack=new _internal_mytable::datapack(sz);
            if(ppack==nullptr)
            {
                setcallret(callret,callbackret::NO_MEM);
                delete handle;
                return nullptr;
            }
            fread(ppack->getdata(),sz,1,fp);
            dpk.push_back(*ppack);
            delete ppack;
            for(size_t i=1;i<handle->colname.size();i++)
            {
                fread(&sz,sizeof(long),1,fp);
                ppack=new _internal_mytable::datapack(sz);
                if(ppack==nullptr)
                {
                    setcallret(callret,callbackret::NO_MEM);
                    delete handle;
                    return nullptr;
                }
                fread(ppack->getdata(),sz,1,fp);
                dpk.push_back(*ppack);
                delete ppack;
            }
            handle->data.push_back(dpk);
        }
        fclose(fp);
        handle->saved=true;
        setcallret(callret,callbackret::OK);
        return handle;
    }

    void save_table(TABLEHANDLE handle)
    {
        if(handle->saved) return;
        FILE* fp=fopen(handle->dbaddr.c_str(),"wb");
        fwrite(&handle->version,sizeof(double),1,fp);
        long sz=handle->colname.size();
        fwrite(&sz,sizeof(long),1,fp);
        for(int i=0;i<sz;i++)
        {
            long length=strlen(handle->colname.at(i).c_str());
            fwrite(&length,sizeof(long),1,fp);
            fwrite(handle->colname.at(i).c_str(),length,1,fp);
        }
        int lsz=handle->data.size();
        for(int i=0;i<lsz;i++)
        {
            int wsz=handle->data.at(i).size();
            for(int j=0;j<wsz;j++)
            {
                long datasize=handle->data.at(i).at(j).getsize();
                fwrite(&datasize,sizeof(long),1,fp);
                fwrite(handle->data.at(i).at(j).getdata(),datasize,1,fp);
            }
        }
        handle->saved=true;
    }

    void close_table(TABLEHANDLE handle)
    {
        save_table(handle);
        delete handle;
    }

    int setcolname(TABLEHANDLE handle,int ColumnID,const string& TargetName)
    {
        if(handle==nullptr)
        {
            return static_cast<int>(callbackret::NULL_HANDLE);
        }
        if(ColumnID<0)
        {
            return static_cast<int>(callbackret::BAD_COL_NUM);
        }
        int sz=handle->colname.size();
        if(ColumnID>=sz)
        {
            return static_cast<int>(callbackret::BAD_COL_NUM);
        }
        else
        {
            handle->colname.at(ColumnID)=TargetName;
            handle->saved=false;
            return 0;
        }
    }

    int addcol(TABLEHANDLE handle,int AddAfterCol,long Sizeof_DataToFill,void* DataToFill,const string& NewColName)
    {
        if(handle==nullptr)
        {
            return static_cast<int>(callbackret::NULL_HANDLE);
        }

        int bsz=handle->data.size();
        int colsz=handle->colname.size();
        if(AddAfterCol<-1||AddAfterCol>=colsz)
        {
            return static_cast<int>(callbackret::BAD_COL_NUM);
        }
        _internal_mytable::datapack dpk(Sizeof_DataToFill,DataToFill);
        if(bsz==0)
        {
            /// Originally empty ( bsz==0 && colsz==0 )
            vector<_internal_mytable::datapack> vec;
            vec.push_back(dpk);
            handle->data.push_back(vec);
            handle->colname.push_back(NewColName);
            handle->saved=false;
            return 0;
        }
        for(int i=0;i<bsz;i++)
        {
            handle->data.at(i).insert(handle->data.at(i).begin()+1+AddAfterCol,dpk);
        }
        handle->colname.insert(handle->colname.begin()+1+AddAfterCol,NewColName);
        handle->saved=false;
        return 0;
    }

    int addline(TABLEHANDLE handle,int AddAfterLine,long Sizeof_DataToFill,void* DataToFill)
    {
        if(handle==nullptr)
        {
            return static_cast<int>(callbackret::NULL_HANDLE);
        }
        int colsz=handle->colname.size();
        if(colsz==0)
        {
            return static_cast<int>(callbackret::EMPTY_TABLE);
        }
        int bsz=handle->data.size();
        /**
        if colsz>=1 ,then bsz >= 1
        */
        if(AddAfterLine<-1||AddAfterLine>=bsz)
        {
            return static_cast<int>(callbackret::BAD_LINE_NUM);
        }

        _internal_mytable::datapack dpk(Sizeof_DataToFill,DataToFill);
        vector<_internal_mytable::datapack> vec;
        for(int i=0;i<colsz;i++)
        {
            vec.push_back(dpk);
        }

        handle->data.insert(handle->data.begin()+1+AddAfterLine,vec);
        handle->saved=false;
        return 0;
    }

    void* getrawdata(TABLEHANDLE handle,int Line,int Col,long* Sizeof_TargetData,void* DataBuffer,int* callret)
    {
        if(handle==nullptr)
        {
            setcallret(callret,callbackret::NULL_HANDLE);
            return nullptr;
        }
        int colsz=handle->colname.size();
        int bsz=handle->data.size();
        if(Line>=bsz)
        {
            setcallret(callret,callbackret::BAD_LINE_NUM);
            return nullptr;
        }
        if(Col>=colsz)
        {
            setcallret(callret,callbackret::BAD_COL_NUM);
            return nullptr;
        }
        *Sizeof_TargetData=handle->data.at(Line).at(Col).getsize();
        memcpy(DataBuffer,handle->data.at(Line).at(Col).getdata(),*Sizeof_TargetData);
        setcallret(callret,callbackret::OK);
        return DataBuffer;
    }

    void* setrawdata(TABLEHANDLE handle,int Line,int Col,long Sizeof_Data,const void* SourceBuffer,int* callret)
    {
        if(handle==nullptr)
        {
            setcallret(callret,callbackret::NULL_HANDLE);
            return nullptr;
        }
        int colsz=handle->colname.size();
        int bsz=handle->data.size();
        if(Line>=bsz)
        {
            setcallret(callret,callbackret::BAD_LINE_NUM);
            return nullptr;
        }
        if(Col>=colsz)
        {
            setcallret(callret,callbackret::BAD_COL_NUM);
            return nullptr;
        }
        _internal_mytable::datapack dpk(Sizeof_Data,SourceBuffer);
        handle->data.at(Line).at(Col)=dpk;
        setcallret(callret,callbackret::OK);
        return const_cast<void*>(SourceBuffer);
    }

}/// End of namespace mytable




#endif /// End of HC_TECH_GITHUB_PROJECT_MYTABLE_CPP
