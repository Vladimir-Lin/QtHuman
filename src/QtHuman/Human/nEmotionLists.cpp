#include <qthuman.h>

N::EmotionLists:: EmotionLists ( QWidget * parent , Plan * p )
                : ItemEditor   (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::EmotionLists::~EmotionLists(void)
{
}

void N::EmotionLists::Configure(void)
{
  ItemTable = PlanTable(Emotions)         ;
  ItemType  = Types::Emotion              ;
  NewTreeWidgetItem ( head              ) ;
  head->setText     ( 0 , tr("Emotion") ) ;
  setWindowTitle    ( tr("Emotions")    ) ;
  assignHeaderItems ( head              ) ;
  plan -> setFont   ( this              ) ;
}
