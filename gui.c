#include "args.h"
#define IMP
#include "pith.h"
#include <GL/glx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef int(glSwapInterval_t)(Display *, GLXDrawable, int);

uint32_t nProcGen = 0;
uint32_t rnd() {
  nProcGen += 0xe120fc15;
  uint64_t tmp;
  tmp = (uint64_t)nProcGen * 0x4a39b70d;
  uint32_t m1 = (tmp >> 32) ^ tmp;
  tmp = (uint64_t)m1 * 0x12fad5c9;
  uint32_t m2 = (tmp >> 32) ^ tmp;
  return m2;
}
struct wind {
  Display *display;
  Window window;
  int width;
  int height;
  uint32_t *b;
  uint32_t *a;
  uint32_t *e;
};
sargo(draw) {
  struct wind *s = (void *)o[7];
  glClear(GL_COLOR_BUFFER_BIT);

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->width, s->height, GL_RGBA,
                  GL_UNSIGNED_BYTE, s->b);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(-1.0f, -1.0f, 0.0f);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-1.0f, 1.0f, 0.0f);
  glTexCoord2f(1.0, 0.0);
  glVertex3f(1.0f, 1.0f, 0.0f);
  glTexCoord2f(1.0, 1.0);
  glVertex3f(1.0f, -1.0f, 0.0f);
  glEnd();
  glXSwapBuffers(s->display, s->window);
}
sargo(put) {
  struct wind *s = (void *)o[7];
  for (; begin < advance; begin += sizeof(uint32_t), s->a++) {
    if (s->a == s->e) {
      s->a = s->b;
      C0(draw, o);
    }
    *s->a = *(uint32_t *)begin;
  }
}

sargo(skip) {
  struct wind *s = (void *)o[7];
  parg(nt, next);
  parg(uint64_t, skip);
  s->a += skip % (s->e - s->b);
  if ((void *)o[7] <= (void *)o[6]) {
    o[6] = o[5] + (o[6] - o[7]);
    C0(draw, o);
    C(next, o, args(int, s->width, s->height));
  }
}
sargo(screen) {
  parg(int, height);
  parg(int, width);
  C(o[0], o, args(uint32_t, -1, -1, -1, -1));
  C(o[1], o, {
    arg(uint32_t, width * height - 4);
    arg(void *, screen);
  });
}
sargo(onpith) {
  parg(nt *, pith);
  struct wind *s = (void *)pith[7];
  // C(screen, pith, args(int, s->width, s->height));
  uint64_t frames = 1 << 12;
  while (frames--) {
    for (int i = 0; i < s->width; i += 8)
      for (int j = 0; j < s->height; j += 8)
        s->b[i + j * s->width] = 0xFF000000            //
                                 | (rnd() % 256) << 16 //
                                 | (rnd() % 256) << 8  //
                                 | (rnd() % 256) << 0; //
    C0(draw, pith)
  }
  C(o[0], o, arg(int, 9));
}
sargo(pl) {
  Display *display = XOpenDisplay(NULL);
  Window windowRoot = DefaultRootWindow(display);
  int x = 0;
  int y = 0;
  int width = 256 * 2;
  int height = 240 * 2;

  int attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
  XVisualInfo *visualinfo = glXChooseVisual(display, 0, attribs);
  if (!visualinfo)
    return C0(o[1], o);
  Colormap colourmap =
      XCreateColormap(display, windowRoot, visualinfo->visual, AllocNone);
  if (colourmap == BadAlloc || colourmap == BadMatch || colourmap == BadValue ||
      colourmap == BadWindow)
    return C0(o[1], o);
  XSetWindowAttributes attributes;
  attributes.colormap = colourmap;
  attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                          ButtonPressMask | ButtonReleaseMask |
                          PointerMotionMask | FocusChangeMask |
                          StructureNotifyMask;
  Window window = XCreateWindow(
      display, windowRoot, x, y, width, height, 0, visualinfo->depth,
      InputOutput, visualinfo->visual, CWColormap | CWEventMask, &attributes);
  if (window == BadAlloc || window == BadMatch || window == BadValue ||
      window == BadWindow)
    return C0(o[1], o);
  Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
  XSetWMProtocols(display, window, &wmDelete, 1);
  XMapWindow(display, window);
  XStoreName(display, window, "ABO");
  GLXContext glDeviceContext =
      glXCreateContext(display, visualinfo, NULL, GL_TRUE);
  glXMakeCurrent(display, window, glDeviceContext);

  // -----

  XWindowAttributes gwa;
  XGetWindowAttributes(display, window, &gwa);
  glViewport(0, 0, gwa.width, gwa.height);

  glSwapInterval_t *glSwapIntervalEXT = NULL;
  glSwapIntervalEXT = (glSwapInterval_t *)glXGetProcAddress(
      (unsigned char *)"glXSwapIntervalEXT");
  if (glSwapIntervalEXT != NULL)
    glSwapIntervalEXT(display, window, 0);

  GLuint glBuffer;
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &glBuffer);
  glBindTexture(GL_TEXTURE_2D, glBuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  uint32_t *buff = malloc(sizeof(*buff) * gwa.width * gwa.height);
  for (int i = 0; i < gwa.width * gwa.height; i++)
    buff[i] = 0xFF000000 + i % 256;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gwa.width, gwa.height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, buff);
  struct wind windata = {.window = window,
                         .display = display,
                         .width = gwa.width,
                         .height = gwa.height,
                         .b = buff,
                         .a = buff,
                         .e = buff + sizeof(*buff) * gwa.width * gwa.height};
  void *pith[8] = {put, skip, 0, 0, 0, 0, 0, &windata};
  C(o[0], o, arg(void *, pith));
  free(buff);
}
int main() {
  Ma(1 << 12, {
    void *oa[8] = {hexdump, hexdump, [7] = (void *)oa};
    C(mb, oa, args(nt, pl, onpith, 0));
  });
}

// while (true) {
//  for (int i = 0; i < width; i += 8)
//    for (int j = 0; j < height; j += 8)
//      buff[i + j * width] = 0xFF000000            //
//                            | (rnd() % 256) << 16 //
//                            | (rnd() % 256) << 8  //
//                            | (rnd() % 256) << 0; //
//  glClear(GL_COLOR_BUFFER_BIT);

//  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gwa.width, gwa.height, GL_RGBA,
//                  GL_UNSIGNED_BYTE, buff);
//  glBegin(GL_QUADS);
//  glTexCoord2f(0.0, 1.0);
//  glVertex3f(-1.0f, -1.0f, 0.0f);
//  glTexCoord2f(0.0, 0.0);
//  glVertex3f(-1.0f, 1.0f, 0.0f);
//  glTexCoord2f(1.0, 0.0);
//  glVertex3f(1.0f, 1.0f, 0.0f);
//  glTexCoord2f(1.0, 1.0);
//  glVertex3f(1.0f, -1.0f, 0.0f);
//  glEnd();
//  glXSwapBuffers(display, window);
//};
