#include <qthuman.h>

N::ActorView:: ActorView     ( QWidget * parent,Plan * p )
             : PeopleView    (           parent,       p )
{
  WidgetClass        ;
  t1 = Types::Album  ;
}

N::ActorView::~ActorView (void)
{
}

bool N::ActorView::startup(void)
{
  if ( isLoading ( ) ) return false ;
  ResumeLanguage      (       )     ;
  clear               (       )     ;
  if ( first == 0 )                 {
    addSequence       ( 50002 )     ;
  } else                            {
    addSequence       ( 10002 )     ;
  }                                 ;
  emit TriggerCommand (       )     ;
  return true                       ;
}
