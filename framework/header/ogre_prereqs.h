#ifndef QTOGREPREREQS_H
#define QTOGREPREREQS_H

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#define QT_5
#endif

#ifdef QT_5
#   include <QtWidgets/QtWidgets>
#else
#   include <QtGui>
#endif

#if defined(Q_OS_WIN)
  #define GL_CONTEXT HGLRC
#else
  #include <GL/glx.h>
  #define GL_CONTEXT GLXContext

  //Hack: Xlib.h (indirectly added by glx.h) do a '#define Bool int', 
  //breaking rapidjson, define 'None' to a long, breaking lots of Ogre 
  //enumerations and also a 'KeyPress', breaking the QEvent::KeyPress.
  //FIXME: maybe there's a better way to workaround it than doing those 
  //undef's bellow.
  #ifdef Bool
     #undef Bool
  #endif
  #ifdef None
     #undef None
  #endif
  #ifdef KeyPress
     #undef KeyPress
  #endif

#endif




#endif // QTOGREPREREQS_H
