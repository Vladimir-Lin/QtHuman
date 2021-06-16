#include <qthuman.h>

N::PeopleList:: PeopleList    (QWidget * parent,Plan * p)
              : ComboBox      (          parent,       p)
              , PeopleManager (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleList::~PeopleList (void)
{
}

void N::PeopleList::Configure(void)
{
  setEditable                  ( true                        ) ;
  setInsertPolicy              ( NoInsert                    ) ;
  setMaxVisibleItems           ( 30                          ) ;
  DoProcessEvents                                              ;
  LineEdit * le = new LineEdit ( this                        ) ;
  le -> setPlaceholderText     ( tr ( "Search people name" ) ) ;
  setLineEdit                  ( le                          ) ;
  //////////////////////////////////////////////////////////////
  QFont f = plan -> fonts [ Fonts :: Status ]                  ;
  //////////////////////////////////////////////////////////////
  int   w = plan->screen.widthPixels (45)                      ;
  QSize s = plan->status->size()                               ;
  QSize m (w,s.height()-4)                                     ;
  setMinimumSize     ( m                   )                   ;
  setMaximumSize     ( m                   )                   ;
  f   . setPixelSize ( s . height ( ) - 10 )                   ;
  le -> setFont      ( f                   )                   ;
  show               (                     )                   ;
  //////////////////////////////////////////////////////////////
  nConnect ( le   , SIGNAL ( returnPressed   (           ) )   ,
             this , SLOT   ( Search          (           ) ) ) ;
  nConnect ( this , SIGNAL ( assignCompleter (QStringList) )   ,
             this , SLOT   ( setCompleter    (QStringList) ) ) ;
}

void N::PeopleList::setCompleter(QStringList words)
{
  LineEdit   * le = Casting        ( LineEdit , lineEdit ( )      ) ;
  QCompleter * lc = new QCompleter ( words , le                   ) ;
  lc -> setCompletionMode          ( QCompleter::InlineCompletion ) ;
  le -> setCompleter               ( lc                           ) ;
}

void N::PeopleList::run(int type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch ( type )   {
    case 200021     :
      List   ( )    ;
    break           ;
  }                 ;
}

bool N::PeopleList::startup(void)
{
  clear (        ) ;
  start ( 200021 ) ;
  return true      ;
}

void N::PeopleList::List(void)
{
  setEnabled ( false )                     ;
  SqlConnection SC ( plan->sql )           ;
  if (SC.open("PeopleList","List"))        {
    QStringList words                      ;
    SUID  uuid                             ;
    UUIDs Uuids = Lists(SC)                ;
    foreach (uuid,Uuids)                   {
      QStringList names = Names(SC,uuid)   ;
      QString n                            ;
      foreach (n,names)                    {
        addItem(n,uuid)                    ;
        if (!words.contains(n)) words << n ;
      }                                    ;
    }                                      ;
    lineEdit()->setText( ""    )           ;
    emit setCompleter  ( words )           ;
    SC.close()                             ;
  }                                        ;
  SC.remove()                              ;
  setEnabled ( true  )                     ;
}

void N::PeopleList::Search(void)
{
  QString name = lineEdit()->text()          ;
  nDropOut ( name.length() <= 0 )            ;
  int index = findText(name)                 ;
  if (index<0)                               {
    index = findText(name,Qt::MatchContains) ;
    if (index>0)                             {
      if (currentIndex()<0)                  {
        name  = itemText        ( index )    ;
        lineEdit ( ) -> setText ( name  )    ;
        return                               ;
      } else                                 {
        int match = -1                       ;
        index = currentIndex ( )             ;
        while (match<0 && index < count() )  {
          QString ins = itemText  ( index )  ;
          if (ins.contains(name))            {
            match = index                    ;
          }                                  ;
          index++                            ;
        }                                    ;
        if (match>=0)                        {
          name  = itemText        ( match )  ;
          lineEdit ( ) -> setText ( name  )  ;
          return                             ;
        }                                    ;
      }                                      ;
    }                                        ;
  }                                          ;
  nDropOut ( index < 0 )                     ;
  SUID u = toUuid ( index )                  ;
  emit People ( name , u )                   ;
}
