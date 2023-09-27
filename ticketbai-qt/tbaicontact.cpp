#include "tbaicontact.h"

TbaiContact::TbaiContact(QObject* parent) : QObject(parent)
{
}

TbaiContact& TbaiContact::fromData(const CompanyData& src)
{
  CompanyData::operator=(src);
  emit changed();
  return *this;
}
