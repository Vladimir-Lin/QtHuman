#include <qthuman.h>

N::PeopleProcessor:: PeopleProcessor   ( QWidget * parent , Plan * p )
                   : InteractiveEditor (           parent ,        p )
                   , Object            ( 0 , Types::People           )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleProcessor::~PeopleProcessor(void)
{
}

void N::PeopleProcessor::Configure(void)
{
  addDispatcher ( new GeneralDispatcher ( this ) ) ;
  addDispatcher ( new PeopleDispatcher  ( this ) ) ;
}

void N::PeopleProcessor::run(int Type,ThreadData * data)
{
  VarArgs V = data->Arguments                                     ;
  switch ( Type )                                                 {
    case CiosMenuId ( 11 , 11 , 11 )                              :
      if ( 2 == V . count ( ) )                                   {
        Dispatching ( V [ 0 ] . toInt ( ) , V [ 1 ] . toInt ( ) ) ;
        Adjust      (                                           ) ;
      }                                                           ;
    break                                                         ;
  }                                                               ;
}

void N::PeopleProcessor::Enter(void)
{
  if ( Commands . length ( ) <= 0 ) return ;
  Report ( Commands )                      ;
  Report ( "\n"     )                      ;
  if ( Interpret ( Commands ) )            {
    Queues  << Commands                    ;
  }                                        ;
  CommandList . prepend ( Commands )       ;
  HistoryIndex = -1                        ;
  Commands     = ""                        ;
}

void N::PeopleProcessor::Enter(QString cmd)
{
  if ( cmd . length ( ) <= 0 ) return ;
  Report ( cmd  )                     ;
  Report ( "\n" )                     ;
  if ( Interpret ( cmd ) )            {
    Queues  << cmd                    ;
  }                                   ;
  CommandList . prepend ( Commands )  ;
  HistoryIndex = -1                   ;
}

void N::PeopleProcessor::Help(void)
{
  QStringList H                                                 ;
  QString     h                                                 ;
  CUIDs       ID = dispatchers . keys ( )                       ;
  int         id                                                ;
  ///////////////////////////////////////////////////////////////
  H << QString("clear : %1").arg(tr("clear history")          ) ;
  H << QString("quit : %1" ).arg(tr("quit application People")) ;
  H << QString("help : %1" ).arg(tr("command syntaxes")       ) ;
  ///////////////////////////////////////////////////////////////
  foreach ( id , ID )                                           {
    dispatchers [ id ] -> Help ( H )                            ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  foreach  ( h , H )                                            {
    Report ( h     )                                            ;
    Report ( "\n"  )                                            ;
  }                                                             ;
}

bool N::PeopleProcessor::Interpret(QString command)
{
  int TID                                        ;
  int ID                                         ;
  if ( canDispatch ( command , TID , ID ) )      {
    Adjust ( )                                   ;
    VarArgs argx                                 ;
    argx << TID                                  ;
    argx << ID                                   ;
    start ( CiosMenuId ( 11 , 11 , 11 ) , argx ) ;
    return false                                 ;
  }                                              ;
  ////////////////////////////////////////////////
  QStringList C = N::CommandTokens ( command )   ;
  int         s = 0                              ;
  if (C.count()==1 && C[0].toLower()=="clear")   {
    s = 10001                                    ;
  } else
  if (C.count()==1 && C[0].toLower()=="help")    {
    s = 10002                                    ;
  } else
  if (C.count()==1 && C[0].toLower()=="quit")    {
    s = 10003                                    ;
  }                                              ;
  switch (s)                                     {
    case 10001                                   :
      Clear     ( )                              ;
    return false                                 ;
    case 10002                                   :
      Help      ( )                              ;
    return false                                 ;
    case 10003                                   :
      emit Quit ( )                              ;
    return false                                 ;
  }                                              ;
  return true                                    ;
}
