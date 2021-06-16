#include <qthuman.h>

N::EyesList:: EyesList      (QWidget * parent,Plan * p)
            : ComboBox      (          parent,       p)
            , PeopleManager (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::EyesList::~EyesList (void)
{
}

void N::EyesList::Configure(void)
{
  setEditable     ( true             ) ;
  plan -> setFont ( this             ) ;
  setToolTip      ( tr("Human eyes") ) ;
}

void N::EyesList::List (void)
{
  AbstractGui::Mutex.lock  ()                                          ;
  clear      (     )                                                   ;
  setEnabled (false)                                                   ;
  SqlConnection SC ( plan->sql  )                                      ;
  if (SC.open("EyesList","List"))                                      {
    UUIDs Uuids = SC.Uuids(PlanTable(Eyes),"uuid",SC.OrderByAsc("id")) ;
    addItems ( SC , Uuids )                                            ;
    SC.close()                                                         ;
  }                                                                    ;
  SC.remove  (    )                                                    ;
  setEnabled (true)                                                    ;
  AbstractGui::Mutex.unlock()                                          ;
}
