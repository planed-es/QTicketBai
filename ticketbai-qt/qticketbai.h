#ifndef  QTICKETBAI_H
# define QTICKETBAI_H

# include <QObject>
# include "companydata.h"

class QTicketBai
{
public:
  explicit QTicketBai() { instance = this; }
  QTicketBai(const QTicketBai&) = delete;
  ~QTicketBai() { instance = nullptr; }

  void withUser(const CompanyData& value) { m_user = value; }
  void withDeveloper(const CompanyData& value) { m_developer = value; }

  static const CompanyData& user() { return instance->m_user; }
  static const CompanyData& developer() { return instance->m_developer; }

private:
  static QTicketBai* instance;
  CompanyData m_user;
  CompanyData m_developer;
};


#endif
