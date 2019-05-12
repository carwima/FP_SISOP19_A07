#define FUSE_USE_VERSION 28
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/carwima/";

void cek_path(char* fpath, const char*path){
	if(strcmp(path,"/") == 0)
	{
		sprintf(fpath,"%s",dirpath);
	}	
	else sprintf(fpath,"%s%s",dirpath,path);
}

int cek_file(const char *fpath,const char *filename){
	char root[1000];
	strcpy(root,"/home/carwima/");
        if(strncmp(filename,".mp3",4)==0){
		strcat(root,filename);
		rename(fpath,root);
		return 0;
        }
	return 1;
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;

	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_access(const char *path, int mask)
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;

	res = access(fpath, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;

	res = readlink(fpath, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	cek_path(fpath,path);

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		if(strcmp(de->d_name,".")!=0 && strcmp(de->d_name,"..")!=0){
			struct stat st;
			memset(&st, 0, sizeof(st));
			st.st_ino = de->d_ino;
			st.st_mode = de->d_type << 12;
			if(de->d_type!=DT_REG) continue;
			if (filler(buf, de->d_name, &st, 0))
				break;
		}
	}

	closedir(dp);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
	char fpath[1000];
	cek_path(fpath,path);
	int res;

	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	char fpath[1000];
	cek_path(fpath,path);
	int res;

	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;

	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{

	char fpath[1000];
	cek_path(fpath,path);

	int res;

	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{

	char fpath[1000];
	cek_path(fpath,path);

	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{

	char fpath[1000];
	cek_path(fpath,path);

	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	char fpath[1000];
	cek_path(fpath,path);

	int res;

	res = statvfs(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

	char fpath[1000];
	cek_path(fpath,path);

    (void) fi;

    int res;
    res = creat(fpath, mode);
    if(res == -1)
	return -errno;

    close(res);

    return 0;
}


static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	char fpath[1000];
	cek_path(fpath,path);
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) fpath;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	char fpath[1000];
	cek_path(fpath,path);

	(void) fpath;
	(void) isdatasync;
	(void) fi;
	return 0;
}

struct queue{
   char file[100];
};

struct queue stack[1000000];
int front=0;
int isi=0;

void push(char str[]){
	strcpy(stack[front+isi].file,str);
	isi++;
}

void pop(){
	front++;
	isi--;
}

int isEmpty(){
	if(stack[front].file==NULL) return 1;
}

char starting_array[1000]="/home/carwima/";

void pindah_mp3(){
	push(starting_array);
	while(!isEmpty()){
		DIR *dir;
		printf("%s\n",stack[front].file);
		struct dirent *ent;
		dir = opendir(stack[front].file);

		if (dir == NULL){
			pop();
			continue;	
		}

		while((ent = readdir(dir)) != NULL) {
		      if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
			 if(ent->d_type==DT_REG){
				  if(strncmp(ent->d_name,".mp3",4)==0){
					char others[1000];
					strcpy(others, stack[front].file);
					strcat(others, ent->d_name);				
					char path[1000];
					strcpy(path, "/home/carwima/");
					strcat(path, ent->d_name);
//					printf("%s\n",path);;
					rename(others,path);
				  }
			}
			else {
				char others[1000];
				strcpy(others, stack[front].file);
				strcat(others, ent->d_name);
				strcat(others, "/");
				
				push(others);
			}
		     }
		}
		closedir(dir);
		pop();

	}
}

void* xmp_init(struct fuse_conn_info *conn){
	pindah_mp3();
	return 0;
}

static struct fuse_operations xmp_oper = {
	.init		= xmp_init,
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create         = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
