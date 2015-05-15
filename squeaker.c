#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <X11/Xlib.h>
#include <math.h>

int squeak(Mix_Chunk* sample) {
  int channel;
  channel = Mix_PlayChannel(0, sample, 0);
  if (channel == -1) {
    fprintf(stderr, "nope: %s", Mix_GetError());
  }
  return channel;
}



int main(int argc, char** argv) {
  /*
    Set up the audio system and load
    the sample into memory
   */

  // initialize audio subsystem
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    fprintf(stderr,
            "\nUnable to initialize SDL:  %s\n",
            SDL_GetError());
    exit(2);
  }

  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16SYS;
  int audio_channels = 1;
  int audio_buffers = 4096;
 
  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
    fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
    exit(1);
  }

  Mix_Chunk* sample;
  sample = Mix_LoadWAV("/home/ian/squeaky_cursor/squeaky_mouse.wav");
  if (!sample) {
    printf("Mix_LoadWAV: %s\n", Mix_GetError());
    exit(1);
  }


  /*
    Do X11 things
  */

  Display *display;
  XEvent xevent;
  Window window;


  if ( (display = XOpenDisplay(NULL)) == NULL ) {
    exit(3);
  }

  window = DefaultRootWindow(display);
  XAllowEvents(display, AsyncBoth, CurrentTime);

  XGrabPointer(display,
               window,
               false,
               PointerMotionMask,
               GrabModeAsync,
               GrabModeAsync,
               None,
               None,
               CurrentTime);

  int x_last = 0;
  int y_last = 0;
  double delta = 0;
  while(1) {
    XNextEvent(display, &xevent);
    if (xevent.type == MotionNotify) {
      delta = sqrt((pow((double)xevent.xmotion.x_root - (double)x_last, 2)+
                    pow((double)xevent.xmotion.y_root - (double)y_last, 2))
                   /2.0);
      x_last = xevent.xmotion.x_root;
      y_last = xevent.xmotion.y_root;

      printf("x_last: %d\n", x_last);
      printf("y_last: %d\n", y_last);
      printf("xevent.xmotion.xroot: %d\n", xevent.xmotion.x_root);
      printf("xevent.xmotion.yroot: %d\n", xevent.xmotion.y_root);
      printf("delta: %f\n", delta);
      if (delta < 100.0) {
        squeak(sample);
      }
    }
  }
  /*
    Listen for mouse events
    and play the sample when the delta is low?
   */

  int channel = squeak(sample);

  // cleanup
  while(Mix_Playing(channel) != 0);
  Mix_FreeChunk(sample);
  Mix_CloseAudio();
  SDL_Quit();
}
