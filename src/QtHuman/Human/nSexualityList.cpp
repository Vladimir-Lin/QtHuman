#include <qthuman.h>

N::SexualityList:: SexualityList (QWidget * parent,Plan * p)
                 : ComboBox      (          parent,       p)
                 , PeopleManager (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SexualityList::~SexualityList (void)
{
}

void N::SexualityList::Configure(void)
{
  setEditable     ( true                             ) ;
  plan -> setFont ( this                             ) ;
  setToolTip      ( tr("Human biologic sexualities") ) ;
}

void N::SexualityList::List (void)
{
  AbstractGui::Mutex.lock  ()           ;
  clear      (     )                    ;
  setEnabled (false)                    ;
  SqlConnection SC ( plan->sql  )       ;
  if (SC.open("nSexualityTree","List")) {
    UUIDs Uuids = HumanSexualities(SC)  ;
    addItems ( SC , Uuids )             ;
    SC.close()                          ;
  }                                     ;
  SC.remove  (    )                     ;
  setEnabled (true)                     ;
  AbstractGui::Mutex.unlock()           ;
}
