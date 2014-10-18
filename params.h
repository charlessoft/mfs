/*
  Copyright (C) 2012 Joseph J. Pfeiffer, Jr., Ph.D. <pfeiffer@cs.nmsu.edu>

  This program can be distributed under the terms of the GNU GPLv3.
  See the file COPYING.

  There are a couple of symbols that need to be #defined before
  #including all the headers.
*/

#ifndef _PARAMS_H_
#define _PARAMS_H_

// The FUSE API has been changed a number of times.  So, our code
// needs to define the version of the API that we assume.  As of this
// writing, the most current API version is 26
#define FUSE_USE_VERSION 26

// need this to get pwrite().  I have to use setvbuf() instead of
// setlinebuf() later in consequence.
#define _XOPEN_SOURCE 500

// maintain bbfs state in here
#include <limits.h>
#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>
struct bb_state {
    char* httpgeturl;
    char* httpposturl;
    FILE *logfile;
    char *rootdir;
    CURL* connection;
    int attached_to_multi;
    /*struct ftpfs_file* current_fh;*/
    unsigned blksize;
    int verbose;
    int debug;
    curl_version_info_data* curl_version;
    int transform_symlinks;
    int disable_epsv;
    int skip_pasv_ip;
    char* ftp_method;
    char* custom_list;
    int tcp_nodelay;
    char* ftp_port;
    int disable_eprt;
    int connect_timeout;
    int use_ssl;
    int no_verify_hostname;
    int no_verify_peer;
    char* cert;
    char* cert_type;
    char* key;
    char* key_type;
    char* key_password;
    char* engine;
    char* cacert;
    char* capath;
    char* ciphers;
    char* interface;
    char* krb4;
};
#define BB_DATA ((struct bb_state *) fuse_get_context()->private_data)

#endif
