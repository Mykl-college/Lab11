/*
*Author: Mikey johnston
*Desc: make up to 50 fractal images and combines them into a movie
*
*/

#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc,char *argv[] ){
    char c;
    const char *outfile = "mandel1.bmp";
	double xcenter =  0.286932;
	double ycenter =  0.014287;
	double scale = 2;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;
    //processors
    int procs = 1;
    
    while((c = getopt(argc,argv,"x:y:s:W:H:m:o:h:p:"))!=-1) {
        switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
            case 'p':
                procs = atoi(optarg);
                break;
		}
    }
    if(procs>50){
        procs = 50;
    }

    int frames_started = 0;
    int active_children = 0;
    int frams = 100;
    //double scale_step = 0.04080612244;
    double scale_step = 0.0202;
    while (frames_started < frams) {
        // dont exceed given process amount
        if (active_children >=procs) {
            wait(NULL);
            active_children--;
        }
        //changing arguements filename (mandel(x).bmp and the scale going from 2 to 0.0002)
        char filename[64];
        char scale_str[32];

        snprintf(filename, sizeof(filename), "mandel%d.bmp", frames_started+1);
        snprintf(scale_str, sizeof(scale_str), "%lf", scale);

        //fork and check
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return 1;
        }
        //child
        if (pid == 0) {
            //set argurment
            char *args[] = {
                "./mandel",
                "-x 0.286932 -y 0.014287", //set x, y
                "-s", scale_str,
                "-o", filename,
                NULL
            };
            //call ./mandel with give arguments.
            execvp(args[0], args);
            
            //execvp should terminate the child unless it fails
            perror("execvp failed");
            exit(1);
        } else {
            active_children++;
            frames_started++;
            
            //steps down scale
            scale -= scale_step;
        }
    }
    while (active_children > 0) {
        //have parent wait for childen to end before having program end
        wait(NULL);
        active_children--;
    }
    return 0;
}
   