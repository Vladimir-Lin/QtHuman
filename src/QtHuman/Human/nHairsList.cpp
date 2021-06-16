#include <qthuman.h>

N::HairsList:: HairsList     (QWidget * parent,Plan * p)
             : ComboBox      (          parent,       p)
             , PeopleManager (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::HairsList::~HairsList (void)
{
}

void N::HairsList::Configure(void)
{
  setEditable     ( true              ) ;
  plan -> setFont ( this              ) ;
  setToolTip      ( tr("Human hairs") ) ;
}

void N::HairsList::List (void)
{
  AbstractGui::Mutex.lock  ()                                           ;
  clear      (     )                                                    ;
  setEnabled (false)                                                    ;
  SqlConnection SC ( plan->sql  )                                       ;
  if (SC.open("HairsList","List"))                                      {
    UUIDs Uuids = SC.Uuids(PlanTable(Hairs),"uuid",SC.OrderByAsc("id")) ;
    addItems ( SC , Uuids )                                             ;
    SC.close()                                                          ;
  }                                                                     ;
  SC.remove  (    )                                                     ;
  setEnabled (true)                                                     ;
  AbstractGui::Mutex.unlock()                                           ;
}
