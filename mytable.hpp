/** MyTable On GitHub
    https://github.com/Kiritow/MyTable
*/

#ifndef HC_TECH_GITHUB_PROJECT_MYTABLE_HEAD
#define HC_TECH_GITHUB_PROJECT_MYTABLE_HEAD

/// Declarations

namespace _internal_mytable
{
struct table;
}// End of namespace _internel_mytable



namespace mytable
{
using TABLEHANDLE = _internal_mytable::table*;

/// Create a table
TABLEHANDLE create_table(const char* FileAddress,bool overwrite,int* callret=nullptr);

/// Open a table
TABLEHANDLE open_table(const char* FileAddress,int* callret=nullptr);

/// Save changes to disk
void save_table(TABLEHANDLE handle);

/// Close a table. Changes will be saved automatically
void close_table(TABLEHANDLE handle);

/// Add a new column to a table
int addcol(TABLEHANDLE handle,int AddAfterCol,long Sizeof_DataToFill,void* DataToFill);

/// Add a new line to a table
int addline(TABLEHANDLE handle,int AddAfterLine,long Sizeof_DataToFill,void* DataToFill);

/// Change the title of a column
int setcolname(TABLEHANDLE handle,int ColumnID,const char* TargetName);

/// Get raw data by position
void* getrawdata(TABLEHANDLE handle,int Line,int Col,long* Sizeof_TargetData,void* DataBuffer);

/// Set raw data by position
void* setrawdata(TABLEHANDLE handle,int Line,int Col,long Sizeof_Data,const void* SourceBuffer);

/// Search the same col in target lines
void foreach_col(TABLEHANDLE handle,int Col,long FromLine,long ToLine,FUNC_PLPV callback_func);

}// End of namespace mytable

/// Implements
#include "MyTable.cpp"

#endif // End of HC_TECH_GITHUB_PROJECT_MYTABLE_HEAD
