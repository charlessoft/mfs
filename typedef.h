#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_
struct mfs_state
{
    FILE* logfile;
    char* rootdir;
};

#define MFS_DATA ((struct mfs_state *) fuse_get_context()->private_data)
#endif
