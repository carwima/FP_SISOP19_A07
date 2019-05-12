#include <ao/ao.h>
#include <mpg123.h>
#define BITS 8
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <dirent.h>

struct queue{
   char file[1000];
};

struct queue arr[1000];

char c;
int k, pstats, input, total, screen;
pthread_t th[4];

//thread 1
void* kbhit(void *arg){
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    while((c=getchar()))      
    {	
	switch(c){
			case '1':
				input = 1;
				break;
			case '2':
				input = 2;
				break;
			case '3':
				input = 3;
				break;
			case '4':
				input = 4;
				break;
			default:
				input = 0;
				break;
    	
	}
    }
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

//thread 2
void *menu(void *arg){
	while(1){
	if(screen==0){
		system("clear");
		printf("Now Playing : %s\n", arr[k].file);
		if(k-1<0) printf("1. Previous : %s\n", arr[total].file);
		else printf("1. Previous : %s\n", arr[k-1].file);
		if(k+1>total) printf("2. Next : %s\n", arr[0].file);
		else printf("2. Next : %s\n", arr[k+1].file);
		printf("3. List\n");
		printf("4. Keluar\n");
		printf("%d  input %c %ds\n",k,c,screen);
		switch(input){
		case 1:
			pstats=1;
			if(k-1<0) k=total+1;
			k--;
		break;
		case 2:
			pstats=1;
			if(k+1>total) k=-1;
			k++;
		break;
		case 3:
			screen=1;
		break;
		case 4:
			exit(0);
		break;
		}
		input=0;
	}
		sleep(1);
	}
}

void *list(void *arg){
	while(1){
	if(screen==1){
		system("clear");
		int a;
		for(a=0;a<=total;a++){
			int f=a+1;
			printf("%d. %s\n",f,arr[a].file);
		}
		printf("\n1. Menu awal\n");
		printf("%d  input %c\n",k,c);
		switch(input){
		case 1:
			screen=0;
		break;
		}
		input=0;
	}
		sleep(1);
	}
}

//thread 3
void *play(void *arg){
	while(1){
		mpg123_handle *mh;
		unsigned char *buffer;
		size_t buffer_size;
		size_t done;
		int err;

		int driver;
		ao_device *dev;

		ao_sample_format format;
		int channels, encoding;
		long rate;
		
		/* initializations */
		ao_initialize();
		driver = ao_default_driver_id();
		mpg123_init();
		mh = mpg123_new(NULL, &err);
		buffer_size = mpg123_outblock(mh);
		buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

		/* open the file and get the decoding format */
		mpg123_open(mh, arr[k].file);
		mpg123_getformat(mh, &rate, &channels, &encoding);

		/* set the output format and open the output device */
		format.bits = mpg123_encsize(encoding) * BITS;
		format.rate = rate;
		format.channels = channels;
		format.byte_format = AO_FMT_NATIVE;
		format.matrix = 0;
		dev = ao_open_live(driver, &format, NULL);

		/* decode and play */
		while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK && pstats==0)
		  ao_play(dev, buffer, done);
		pstats=0;
		/* clean up */
		free(buffer);
		ao_close(dev);
		mpg123_close(mh);
		mpg123_delete(mh);
		mpg123_exit();
		ao_shutdown();
	}
}

//init
/*
void debug(){
	queue[0]="/home/carwima/FP/mp3/Samsung GALAXY S4 Alarms - Morning Flower.mp3";
	queue[1]="/home/carwima/FP/mp3/Morning Flower.mp3";
	total=2;
	k=0;
	screen=0;
}
*/

void dir_to_queue(){
	k=0;
	DIR *dir;
	struct dirent *ent;
	dir = opendir("/home/carwima/FP/mp3");
	if (dir != NULL) {
	      while((ent = readdir(dir)) != NULL) {
		if(ent->d_type==DT_REG){
			char path[1000];
			strcpy(path, "/home/carwima/FP/mp3/");
		        strcat(path, ent->d_name);
			strcpy(arr[k].file,path);
			k++;
		}
	      }
	      total=k-1;
	      k=0;
	      closedir(dir);
	}
}

int main(){
	dir_to_queue();
//	debug();
	pthread_create( &th[0], NULL, kbhit, NULL);
	pthread_create( &th[1], NULL, menu, NULL);
	pthread_create( &th[2], NULL, play, NULL);
	pthread_create( &th[3], NULL, list, NULL);
	int i;
	for(i=0; i<4;i++) pthread_join(th[0],NULL);
    return 0;
}
