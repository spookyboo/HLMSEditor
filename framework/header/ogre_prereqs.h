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

#endif // QTOGREPREREQS_H
