#include <stdio.h>
#include <stdlib.h>
#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/xattr.h>
//#include "typedef.h"
#include "params.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <pthread.h>
#include <pthread.h>
#include "cservice.h"
#include <string>
#include <vector>
using namespace std;
#include "log.h"

vector<string> g_cache;

struct bb_state* bb_data;

bool exist_file( const char* path )
{

    vector<string>::iterator iter;  
    for (iter=g_cache.begin();iter!=g_cache.end();iter++)  
    {  
        string str = *iter;
        if( str == path )
            return true;
        //cout<<*iter<<'\0';  
        //filler(buf,(*iter).c_str(),null,0);
    }  
    return false;
}

int get_file_content( const char* path, unsigned char** buf, int * nLen )
{
    if( access(path,F_OK) !=1 )
    {
        FILE* fp = fopen( path, "rb" );
        if ( fp )
        {
            fseek( fp, 0, SEEK_END );
            *nLen = ftell( fp );
            fseek( fp, 0, SEEK_SET );
            if( *nLen > 0 )
            {
                *buf = new unsigned char[*nLen];
                memset( *buf, 0 ,sizeof(*nLen));
                fread(*buf,1,*nLen,fp);
                fclose(fp);
                fp = NULL;
            }
            return *nLen;
        }
    }
    return -1;
}

int get_file( const char* path )
{
    log_msg("\nget_file()\n    path=%s\n",path);
    //Service CService;
    //CService.HttpRequest("GET", BB_DATA->httpgeturl,)
    return -1;
}

int post_file( const char* path )
{
    string postFileData;
    string strBoundary = "----WebKitFormBoundaryx386qjAs2dAgshUG";
    string formMultipart = "Content-Type: multipart/form-data; boundary=" + strBoundary;
    vector<string> custom_headers;
    custom_headers.push_back( formMultipart );
    postFileData = postFileData + "--" + strBoundary + "\r\n";
    postFileData += "Content-Disposition: form-data; name=\"file\"; filename=\"10K.txt\"\r\n";
    postFileData += "Content-Type: text/plain\r\n";
    postFileData += "\r\n";

    unsigned char* buf = NULL;
    int nLen = -1;
    int nres = -1;
    nres = get_file_content( path, &buf, &nLen );
    if ( nLen > 0 ) {
       postFileData.append( (char*)buf, nLen ); 
    }
    else 
        postFileData.append( path, strlen(path) );

    postFileData += "\r\n";

    postFileData = postFileData + "--" + strBoundary + "--\r\n";

    HTTPDATA postFormData;
    memset( &postFormData, 0, sizeof(postFormData) );
    postFormData.data = (char*)postFileData.data();
    postFormData.datalen = postFileData.size();
    postFormData.totallen = postFileData.size();
    Service CService;


    nres = CService.HttpRequest( "POST", BB_DATA->httpposturl, &postFormData,custom_headers,&CService);
    log_msg("\npost_file()\n    respbuf=%s\n",CService.m_resp_buffer.c_str());
    if ( nres ==0 && nLen>0 ) {
        return nLen;
    }
    return 0;
}

void *mfs_init(struct fuse_conn_info *conn)
{
    
    log_conn( conn );
    return BB_DATA;
}

static int mfs_getattr(const char *path, struct stat *stbuf)
{
	log_msg("\nmfs_getattr\n    path=%s\n",path);
	//get_file(path);
	log_stat(stbuf);
	//return 0;
	int res = 0;
	res = lstat(path, stbuf);
	log_msg("\nmfs_getattr--lstat()\n    res=%d",res);
	log_stat(stbuf);
	if (res == -1)
		return -errno;

	return 0;
}


int mfs_opendir(const char *path, struct fuse_file_info *fi)
{
    log_msg("\nmfs_opendir()\n    path=%s\n",path);
    DIR *dp;
    int retstat = 0;
    
    dp = opendir(path);
    
    fi->fh = (intptr_t) dp;
    
    return retstat;
}



static int mfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{

    log_msg("\nmfs_readdir()\n    path=%s\n    buf=0x%08x\n",path,buf);

    
    vector<string>::iterator iter;  
    for (iter=g_cache.begin();iter!=g_cache.end();iter++)  
    {  
        //cout<<*iter<<'\0';  
        filler(buf,(*iter).c_str(),NULL,0);
    }  
    return 0;
	DIR *dp;
	struct dirent *de;

    dp = opendir(path);
    //printf("path===%s\n",path);
    //dp = opendir("/home/charles");
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}


static int mfs_mkdir(const char *path, mode_t mode)
{
    int res =0 ;
    res = mkdir(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}


static int mfs_rmdir(const char *path)
{
    int res =0;
    res = rmdir(path);
    if (res == -1)
        return -errno;

    return 0;
}



static int mfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    log_msg("\nmfs_create()\n    path=%s\n    mode=%d",path,mode);
    int fd = 0;
    fd = open(path, fi->flags, mode);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;


    //int fd = 0;
    //log_msg("\nmfs_create()\n    path=%s\n    mode=%d\n", path, mode );
    //log_fi(fi);
    //fd = open( path, fi->flags, mode );
    //if( fd == -1 )
    //return -errno;
    //fi->fh = fd;
    //return 0;
}

static int mfs_unlink(const char *path)
{
    log_msg("\nmfs_unlink()\n    path=%s\n",path);
    int res = 0;
    res = unlink(path);
    if (res == -1)
        return -errno;

    return 0;
}


static int mfs_open(const char *path, struct fuse_file_info *fi)
{
    int res = 0;
    res = open(path, fi->flags);
    log_msg("\nmfs_open()\n    path=%s\nres=%d\n",path,res);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

static int mfs_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
    log_msg("\nmfs_write()\n    path=%s\n    buf=0x%08x\n    size=%d\n    offset=%d\n",path,buf,size,offset);
    //int fd;
    int res;

    log_fi(fi);
    log_msg("-------\n");
    //-------------修改post-----
    //res=11;
    res = post_file(path);
    log_msg("===postfile:res=%d\n",res);
    if(res>0)
    {
        if(!exist_file(path+1))
            g_cache.push_back(path+1);
    }
    return res;


}



static int mfs_utimens(const char *path, const struct timespec ts[2])
{
    log_msg("\nmfs_utimens()\n    path=%s\n");
    int res;
    struct timeval tv[2];

    tv[0].tv_sec = ts[0].tv_sec;
    tv[0].tv_usec = ts[0].tv_nsec / 1000;
    tv[1].tv_sec = ts[1].tv_sec;
    tv[1].tv_usec = ts[1].tv_nsec / 1000;

    res = utimes(path, tv);
    if (res == -1)
        return -errno;

    return 0;
}

static int mfs_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    log_msg("\n----mfs_read()\n    path=%s\n    buf=0x%08x\n    size=%d\n    offset=%d\n",
            path,buf,size,offset);
    log_fi(fi);
    //int fd;
    int res = 0;

    char szfileurl[256] = {0};
    strcpy(szfileurl,BB_DATA->httpgeturl);
    strcat(szfileurl,path+1);

    log_msg("\n------download url = %s\n", szfileurl );

    //curl_easy_setopt(easy_handle, CURLOPT_URL, szfileurl);

    //(void) fi;
    //fd = open(path, O_RDONLY);
    //if (fd == -1)
    //return -errno;

    //res = pread(fd, buf, size, offset);
    //if (res == -1)
    //res = -errno;

    //close(fd);
    return res;
}

static int mfs_truncate(const char *path, off_t size)
{
    int res;

    res = truncate(path, size);
    if (res == -1)
        return -errno;

    return 0;
}

static struct vsfuse_oper : fuse_operations  {
    vsfuse_oper()
    {   
        //readdir = bb_readdir;
        //getattr = bb_getattr;
        //init = bb_init;
        //destroy = bb_destroy;
        ////opdir = xmp_opendir;

        init       = mfs_init;
        //目录操作
        getattr	= mfs_getattr;
        readdir	= mfs_readdir;
        mkdir		= mfs_mkdir;
        rmdir		= mfs_rmdir;

        //创建文件操作回调
        create		= mfs_create; 
        open		= mfs_open;
        utimens	= mfs_utimens;

        //删除文件
        unlink		= mfs_unlink;

        //文件读写回调
        read		= mfs_read;
        write		= mfs_write;

        //rename		= mfs_rename,
        truncate	= mfs_truncate;
        //opendir    = mfs_opendir;
        //access		= mfs_access;
        //readlink	= mfs_readlink;
        //mknod		= mfs_mknod;
        //symlink	= mfs_symlink;
        //link		= mfs_link;
        //chmod		= mfs_chmod;
        //chown		= mfs_chown;
        //statfs		= mfs_statfs;
        //release	= mfs_release;
        //fsync		= mfs_fsync;

    }   
}mfs_oper;

//static struct fuse_operations mfs_oper = {
//.init       = mfs_init,
////目录操作
//.getattr	= mfs_getattr,
//.readdir	= mfs_readdir,
//.mkdir		= mfs_mkdir,
//.rmdir		= mfs_rmdir,

////创建文件操作回调
//.create		= mfs_create, 
//.open		= mfs_open,
//.utimens	= mfs_utimens,

////删除文件
//.unlink		= mfs_unlink, 

////文件读写回调
//.read		= mfs_read,
//.write		= mfs_write,

////.rename		= mfs_rename,
////.truncate	= mfs_truncate,
////.opendir    = mfs_opendir,
////.access		= mfs_access,
////.readlink	= mfs_readlink,
////.mknod		= mfs_mknod,
////.symlink	= mfs_symlink,
////.link		= mfs_link,
////.chmod		= mfs_chmod,
////.chown		= mfs_chown,
////.statfs		= mfs_statfs,
////.release	= mfs_release,
////.fsync		= mfs_fsync,
//};



int get_file_size(char *filename)
{
    int file_len = 0;
    int fd = 0;

    fd = open(filename, O_RDONLY);
    if(fd < 0)
    {
        perror("open");
        exit(-1);
    }

    file_len = lseek(fd, 0, SEEK_END);
    if(file_len < 0)
    {
        perror("lseek");
        exit(-1);
    }

    return file_len;
}


int main(int argc, char *argv[])
{

    bb_data = (bb_state*)malloc(sizeof(struct bb_state));
    memset(bb_data, 0, sizeof(bb_data));
    bb_data->logfile = log_open();
    bb_data->httpgeturl = "http://10.211.5.8/getdata.php?key=";
    bb_data->httpposturl = "http://10.142.49.238:9080/udsfs/uploadfile";
    //
    return fuse_main(argc, argv, &mfs_oper, bb_data);
}
