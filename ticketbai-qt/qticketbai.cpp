#include "qticketbai.h"

QTicketBai* QTicketBai::_instance = nullptr;

static const char* multipleInstanceError =
  "Cannot create two instances of QTicketBai simultaneously.";

static const char* noInstanceError =
  "No QTicketBai instance available at the moment."
  "When calling ticketbai-qt methods without manually specifying which TbaiContext to use,"
  "you must have a QTicketBai instance living somewhere in your application to define the"
  "default settings.";

QTicketBai::QTicketBai(QObject* parent) : TbaiContext(parent)
{
  if (_instance)
    throw std::logic_error(multipleInstanceError);
  _instance = this;
}

QTicketBai::~QTicketBai()
{
  _instance = nullptr;
}

TbaiContext& QTicketBai::context()
{
  if (!_instance)
    throw std::logic_error(noInstanceError);
  return *_instance;
}
