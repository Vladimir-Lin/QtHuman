#include <qthuman.h>

N::RaceLists:: RaceLists  ( QWidget * parent , Plan * p )
             : ItemEditor (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::RaceLists::~RaceLists(void)
{
}

void N::RaceLists::Configure(void)
{
  ItemTable = PlanTable(Races)         ;
  ItemType  = Types::Race              ;
  NewTreeWidgetItem ( head           ) ;
  head->setText     ( 0 , tr("Race") ) ;
  setWindowTitle    ( tr("Races")    ) ;
  assignHeaderItems ( head           ) ;
  plan -> setFont   ( this           ) ;
}
