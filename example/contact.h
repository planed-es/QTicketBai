#ifndef  CONTACT_H
# define CONTACT_H

# include <ticketbai-qt/tbaicontact.h>

class Contact : public TbaiContact
{
  Q_OBJECT
public:
  Contact(QObject* parent = nullptr);

  QString toString() const;
  void fromString(const QString&);
};

#endif
