#ifndef  QTICKETBAI_H
# define QTICKETBAI_H

# include <QObject>
# include "companydata.h"
# include "context.h"

class TICKETBAIQT_EXPORT QTicketBai : public TbaiContext
{
public:
  explicit QTicketBai(QObject* parent = nullptr);
  ~QTicketBai();

  static TbaiContext& context();

private:
  static QTicketBai* _instance;
};


#endif
