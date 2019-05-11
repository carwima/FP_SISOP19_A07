#include <ao/ao.h>
#include <mpg123.h>
#define BITS 8
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

char *queue[];
int k;

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
		printf("Now Playing : %s", queue[k]);
		printf("Previous : Press 4\n", queue[k-1]);
		printf("Next : Press 2\n", queue[k+1]);
		printf("Pause : Press 3\n");
		switch(input){
		case 1:
			
		break;
		case 2:
			
		break;
		case 3:
			
		break;
		case 4:
			exit(0);
		break;
		}
	}
}

//thread 3
void *play(void *arg){
  while(1){
    if(pflag!=1) continue;
    else{
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

      if(argc < 2)
          exit(0);

      /* initializations */
      ao_initialize();
      driver = ao_default_driver_id();
      mpg123_init();
      mh = mpg123_new(NULL, &err);
      buffer_size = mpg123_outblock(mh);
      buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

      /* open the file and get the decoding format */
      mpg123_open(mh, queue[k]);
      mpg123_getformat(mh, &rate, &channels, &encoding);

      /* set the output format and open the output device */
      format.bits = mpg123_encsize(encoding) * BITS;
      format.rate = rate;
      format.channels = channels;
      format.byte_format = AO_FMT_NATIVE;
      format.matrix = 0;
      dev = ao_open_live(driver, &format, NULL);

      /* decode and play */
      while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
          ao_play(dev, buffer, done);

      /* clean up */
      free(buffer);
      ao_close(dev);
      mpg123_close(mh);
      mpg123_delete(mh);
      mpg123_exit();
      ao_shutdown();
    }
  }
}

//thread 4
void *pause(){
  while(1){
    
  }
}

//thread 5
void *dir_to_queue(){
  
}

int main(int argc, char *argv[])
{
    dir_to_queue(){
    }
    return 0;
}
