#ifndef  SETTINGS_H
# define SETTINGS_H

# include <ticketbai-qt/context.h>

class Settings : public TbaiContext
{
  Q_OBJECT
  Q_PROPERTY(QString companyId MEMBER _companyId NOTIFY companyChanged)
  Q_PROPERTY(QString developerId MEMBER _developerId NOTIFY developerChanged)
public:
  Settings(QObject* parent = nullptr);
  ~Settings();

  Q_INVOKABLE void save();

signals:
  void companyChanged();
  void developerChanged();
  void afterDeveloperChanged();

private slots:
  void onCompanyChanged();
  void onDeveloperChanged();

private:
  QString _companyId, _developerId;
};

#endif
