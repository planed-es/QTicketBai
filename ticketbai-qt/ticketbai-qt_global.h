#ifndef  TICKETBAIQT_GLOBAL_H
# define TICKETBAIQT_GLOBAL_H

# include <QtCore/qglobal.h>

# if defined(TICKETBAIQT_LIBRARY)
#  define TICKETBAIQT_EXPORT Q_DECL_EXPORT
# else
#  define TICKETBAIQT_EXPORT Q_DECL_IMPORT
#endif

#endif
