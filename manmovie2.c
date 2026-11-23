/*
*Author: Mikey johnston
*Desc: make up to 50 fractal images and combines them into a movie
*Notes: after several try and error, a full restart was done hence the 2 monicker
*
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
    //variable for the fractel images
    //current state just alter scale and name
    char c;
    const char *outfile = "mandel1.bmp";
	double xcenter =  0.286932;
	double ycenter =  0.014287;
	double scale = 2;
	int    image_width = 10000;
	int    image_height = 10000;
	int    max = 10000;
    //processors
    //given processes as arguements
    int procs = 1;
    //threads
    int threads = 1;
    //just stole getopt from mandel program
    while((c = getopt(argc,argv,"x:y:s:W:H:m:o:h:p:t:"))!=-1) {
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
            case 't':
                threads = atoi(optarg);
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
        // will wait for current process to end instead of starting another
        if (active_children >=procs) {
            wait(NULL);
            active_children--;
        }
        //changing arguements filename (mandel(x).bmp and the scale going from 2 to 0.0002)
        char filename[64];
        char scale_str[32];
        char thread_str[32];
        //need string to give as arguments but also have it be variable number
        //snprintf allows this
        //snprintf(char target[], sizeof(target),string, varaibles, ...);
        snprintf(filename, sizeof(filename), "mandel%d.bmp", frames_started+1);
        snprintf(scale_str, sizeof(scale_str), "%lf", scale);
        snprintf(thread_str, sizeof(thread_str), "%d", threads);

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
                "./mandel", //file path (run this progam)
                "-x 0.286932 -y 0.014287", //set x, y flags
                "-s", scale_str, // set scale flag
                "-t", thread_str, // set thread flag
                "-o", filename, // set file flag
                NULL
            };
            //call ./mandel with give arguments.
            //execvp to just give arguments instead of setting in call (ie execl)
            execvp(args[0], args);
            
            //execvp should terminate the child unless it fails
            perror("execvp failed");
            return 0;
        } else {
            //lets know a frame is currently being made
            active_children++;
            //lets know frame has been started
            frames_started++;
            
            //steps down scale by (2 - 0.0002)/99
            scale -= scale_step;
        }
    }

    //cleanup
    while (active_children > 0) {
        //have parent wait for childen to end before having program end
        wait(NULL);
        active_children--;
    }
    return 0;
}
   