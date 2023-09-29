#ifndef  CONTACTCONTROLLER_H
# define CONTACTCONTROLLER_H

# include "contact.h"
# include <QQmlListProperty>

class ContactController : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QQmlListProperty<Contact> list READ qmlList NOTIFY listChanged)
public:
  static ContactController* self;

  ContactController(QObject* parent = nullptr);
  ~ContactController();

  Q_INVOKABLE Contact* createContact();
  Q_INVOKABLE void remove(Contact*);
  Q_INVOKABLE void save();

  const Contact* find(const QString& id) const;

  QQmlListProperty<Contact> qmlList() { return QQmlListProperty<Contact>(this, &_contacts); }

signals:
  void listChanged();

private:
  QList<Contact*> _contacts;
};

#endif
