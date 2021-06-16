#include <qthuman.h>

typedef struct     {
  N::Plan * plan   ;
  QWidget * widget ;
} ExtraPacket      ;

N::PeopleDispatcher:: PeopleDispatcher ( QObject * parent )
                    : QObject          (           parent )
                    , SyntaxDispatcher (                  )
                    , UuidSyntax       (                  )
                    , plan             ( NULL             )
{
  nConnect ( this , SIGNAL ( ViewPeople ( ) )   ,
             this , SLOT   ( SendPeople ( ) ) ) ;
}

N::PeopleDispatcher::~PeopleDispatcher (void)
{
}

int N::PeopleDispatcher::type(void) const
{
  return CiosMenuId ( 78 , 0 , 1 ) ;
}

void N::PeopleDispatcher::Help(QStringList & help)
{
  help << tr("===== People dispatcher commands =====") ;
  help << QString("set people variable name value : %1").arg(tr("set variable into name")) ;
  help << QString("show people variables : %1").arg(tr("show variables")) ;
  help << QString("search people by name ... syntax ... : %1").arg(tr("search people")) ;
  help << QString("search people by tags ... syntax ... : %1").arg(tr("Using tags to search people")) ;
  help << QString("search people by names (people names) : %1").arg(tr("search people")) ;
}

int N::PeopleDispatcher::matching(QString cmd)
{
  QStringList C = CommandTokens ( cmd )                                      ;
  ////////////////////////////////////////////////////////////////////////////
  command    = cmd                                                           ;
  arguments  . clear ( )                                                     ;
  arguments <= C                                                             ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) > 4 ) && ( "search" == C [ 0 ] . toLower ( ) ) )      {
    if ( "people" == C [ 1 ] . toLower ( ) )                                 {
      if ( "by" == C [ 2 ] . toLower ( ) )                                   {
        if ( "name"  == C [ 3 ] . toLower ( ) )                              {
          return CiosMenuId ( 78 , 1 ,  1 )                                  ;
        }                                                                    ;
        if ( "names" == C [ 3 ] . toLower ( ) )                              {
          return CiosMenuId ( 78 , 1 ,  2 )                                  ;
        }                                                                    ;
        if ( "tags"  == C [ 3 ] . toLower ( ) )                              {
          return CiosMenuId ( 78 , 1 ,  3 )                                  ;
        }                                                                    ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) == 5 ) && ( "set" == C [ 0 ] . toLower ( ) )        ) {
    if ( "people" == C [ 1 ] . toLower ( )                                 ) {
      if ( ( "variable" == C [ 2 ] . toLower ( ) )  )                        {
        return CiosMenuId ( 78 , 2 , 1 )                                     ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ( C . count ( ) == 3 ) && ( "show" == C [ 0 ] . toLower ( ) )       ) {
    if ( "people" == C [ 1 ] . toLower ( )                                 ) {
      if ( ( "variables" == C [ 2 ] . toLower ( ) )  )                       {
        return CiosMenuId ( 78 , 2 , 2 )                                     ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return 0                                                                   ;
}

bool N::PeopleDispatcher::execute(int id)
{
  switch ( id )                     {
    case CiosMenuId ( 78 , 1 ,  1 ) :
      SearchPeople  (             ) ;
    return true                     ;
    case CiosMenuId ( 78 , 1 ,  2 ) :
      SearchNames   (             ) ;
    return true                     ;
    case CiosMenuId ( 78 , 1 ,  3 ) :
      SearchTags    (             ) ;
    return true                     ;
    case CiosMenuId ( 78 , 2 ,  1 ) :
      setVariable   (             ) ;
    return true                     ;
    case CiosMenuId ( 78 , 2 ,  2 ) :
      showVariables (             ) ;
    return true                     ;
  }                                 ;
  return false                      ;
}

void N::PeopleDispatcher::setExtras(void * data)
{
  ExtraPacket * ep = (ExtraPacket *) data                   ;
  QWidget     * ww = ep -> widget                           ;
  plan = ep -> plan                                         ;
  nConnect ( this , SIGNAL ( ListPeople(QString,UUIDs&) )   ,
             ww   , SLOT   ( ListPeople(QString,UUIDs&) ) ) ;
}

bool N::PeopleDispatcher::FilterType(SqlConnection & SC,int Type,UUIDs & U,UUIDs & P)
{
  return UuidSyntax::FilterType ( SC , PlanTable(MajorUuid) , Type , U , P ) ;
}

bool N::PeopleDispatcher::NamesOwners(SqlConnection & SC,UUIDs & U,UUIDs & P)
{
  return UuidSyntax::NamesOwners ( SC , PlanTable(NameMaps) , U , P ) ;
}

bool N::PeopleDispatcher::FetchPeople(SqlConnection & SC,UUIDs & U,QString K)
{
  if ( K . contains ( "%" ) )                     {
    FetchLike   ( SC , PlanTable(Names) , U , K ) ;
  } else
  if ( K . contains ( "*" ) )                     {
    FetchRegexp ( SC , PlanTable(Names) , U , K ) ;
  } else                                          {
    FetchEqual  ( SC , PlanTable(Names) , U , K ) ;
  }                                               ;
  /////////////////////////////////////////////////
  return ( U . count ( ) > 0 )                    ;
}

bool N::PeopleDispatcher::FetchByTag(SqlConnection & SC,UUIDs & U,QString Key)
{
  TagsManager TM ( plan )                         ;
  GroupItems  GI ( plan )                         ;
  UUIDs       T                                   ;
  UUIDs       P                                   ;
  UUIDs       L                                   ;
  SUID        u                                   ;
  /////////////////////////////////////////////////
  TM . GetTags ( SC , T , Key )                   ;
  if ( T . count ( ) <= 0 ) return false          ;
  /////////////////////////////////////////////////
  GI . AutoMap    = true                          ;
  GI . GroupTable = GI . LookTable                (
                      Types::Tag                  ,
                      Types::People               ,
                      Groups::Subordination     ) ;
  foreach ( u , T )                               {
    P = GI . Subordination                        (
          SC                                      ,
          u                                       ,
          Types::Tag                              ,
          Types::People                           ,
          Groups::Subordination                   ,
          SC . OrderByAsc ( "position" )        ) ;
    L << P                                        ;
  }                                               ;
  /////////////////////////////////////////////////
  P . clear      (   )                            ;
  P = Uniqueness ( L )                            ;
  foreach ( u , P )                               {
    if ( ! U . contains ( u ) ) U << u            ;
  }                                               ;
  /////////////////////////////////////////////////
  return ( U . count ( ) > 0 )                    ;
}

void N::PeopleDispatcher::SendPeople(void)
{
  while ( Paddings . count ( ) > 0 )  {
    QString p = Paddings . first ( )  ;
    UUIDs   U = Found [ p ]           ;
    Paddings . takeFirst (       )    ;
    Found    . remove    ( p     )    ;
    emit ListPeople      ( p , U )    ;
  }                                   ;
}

void N::PeopleDispatcher::SearchPeople(void)
{
  QStringList s = Syntax ( command )                    ;
  QStringList k                                         ;
  QStringList t                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search" != l ) c = false                        ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "people" != l ) c = false                        ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"     != l ) c = false                        ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "name"   != l ) c = false                        ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  ///////////////////////////////////////////////////////
  int left  = 0                                         ;
  int right = 0                                         ;
  if ( ! isSyntax ( left , right , s , k ) )            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=left;i<=right;i++) t << s [ i ]            ;
  l = t . join ( " " )                                  ;
  Report ( tr("Syntax : %1") . arg ( l )   )            ;
  Report ( "\n"                            )            ;
  ///////////////////////////////////////////////////////
  QMap<QString,UUIDs> UM                                ;
  UUIDs               X                                 ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs U                                             ;
    UUIDs P                                             ;
    /////////////////////////////////////////////////////
    foreach          ( l , k               )            {
      UM [ l ] = U                                      ;
      FetchPeople    ( SC , UM [ l ] , l   )            ;
    }                                                   ;
    /////////////////////////////////////////////////////
    if ( Operate ( U , t , UM ) )                       {
      if ( NamesOwners ( SC , U , P ) )                 {
        FilterType ( SC , Types::People , P , X )       ;
      }                                                 ;
    }                                                   ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 people") . arg ( X . count ( ) )   ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = t . join ( " " )                                ;
    Paddings  << l                                      ;
    Found [ l ] = X                                     ;
    emit ViewPeople ( )                                 ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::PeopleDispatcher::SearchNames(void)
{
  QStringList s = Syntax ( command )                    ;
  QStringList k                                         ;
  QStringList t                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search" != l ) c = false                        ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "people" != l ) c = false                        ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"     != l ) c = false                        ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "names"  != l ) c = false                        ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  k = s                                                 ;
  t = s                                                 ;
  l = t . join ( " " )                                  ;
  Report ( tr("Syntax : %1") . arg ( l )   )            ;
  Report ( "\n"                            )            ;
  ///////////////////////////////////////////////////////
  QMap<QString,UUIDs> UM                                ;
  UUIDs               X                                 ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs U                                             ;
    UUIDs P                                             ;
    /////////////////////////////////////////////////////
    foreach          ( l , k               )            {
      UM [ l ] = U                                      ;
      FetchPeople    ( SC , UM [ l ] , l   )            ;
    }                                                   ;
    /////////////////////////////////////////////////////
    U . clear ( )                                       ;
    P . clear ( )                                       ;
    foreach ( l , k ) P << UM [ l ]                     ;
    U = Uniqueness ( P )                                ;
    P . clear      (   )                                ;
    /////////////////////////////////////////////////////
    if ( NamesOwners ( SC , U , P ) )                   {
      FilterType ( SC , Types::People , P , X )         ;
    }                                                   ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 people") . arg ( X . count ( ) )   ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = t . join ( " " )                                ;
    Paddings  << l                                      ;
    Found [ l ] = X                                     ;
    emit ViewPeople ( )                                 ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::PeopleDispatcher::SearchTags(void)
{
  QStringList s = Syntax ( command )                    ;
  QStringList k                                         ;
  QStringList t                                         ;
  QString     l                                         ;
  bool        c = true                                  ;
  ///////////////////////////////////////////////////////
  l = s [ 0 ] . toLower ( )                             ;
  if ( "search" != l ) c = false                        ;
  l = s [ 1 ] . toLower ( )                             ;
  if ( "people" != l ) c = false                        ;
  l = s [ 2 ] . toLower ( )                             ;
  if ( "by"     != l ) c = false                        ;
  l = s [ 3 ] . toLower ( )                             ;
  if ( "tags"   != l ) c = false                        ;
  ///////////////////////////////////////////////////////
  if ( ! c )                                            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=0;i<4;i++) s . takeFirst ( )               ;
  ///////////////////////////////////////////////////////
  int left  = 0                                         ;
  int right = 0                                         ;
  if ( ! isSyntax ( left , right , s , k ) )            {
    Report ( tr("Syntax error") )                       ;
    Report ( "\n"               )                       ;
    return                                              ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  for (int i=left;i<=right;i++) t << s [ i ]            ;
  l = t . join ( " " )                                  ;
  Report ( tr("Syntax : %1") . arg ( l )   )            ;
  Report ( "\n"                            )            ;
  ///////////////////////////////////////////////////////
  QMap<QString,UUIDs> UM                                ;
  UUIDs               X                                 ;
  SqlConnection SC   ( plan -> sql         )            ;
  if ( SC . open     ( FunctionString )    )            {
    UUIDs U                                             ;
    /////////////////////////////////////////////////////
    foreach          ( l , k               )            {
      UM [ l ] = U                                      ;
      FetchByTag     ( SC , UM [ l ] , l   )            ;
    }                                                   ;
    /////////////////////////////////////////////////////
    Operate ( U , t , UM )                              ;
    X = Uniqueness ( U )                                ;
    /////////////////////////////////////////////////////
    SC . close       (                     )            ;
  }                                                     ;
  SC   . remove      (                     )            ;
  ///////////////////////////////////////////////////////
  if ( X . count ( ) > 0 )                              {
    QString m                                           ;
    m = tr("Found %1 people") . arg ( X . count ( ) )   ;
    Report ( m    )                                     ;
    Report ( "\n" )                                     ;
    l = t . join ( " " )                                ;
    Paddings  << l                                      ;
    Found [ l ] = X                                     ;
    emit ViewPeople ( )                                 ;
  } else                                                {
    Report ( tr("Found nothing") )                      ;
    Report ( "\n"                )                      ;
  }                                                     ;
}

void N::PeopleDispatcher::setVariable(void)
{
  if ( arguments . count ( ) != 5 ) return       ;
  QString n = arguments [ 3 ] . toString ( )     ;
  QString v = arguments [ 4 ] . toString ( )     ;
  Variables [ n ] = arguments [ 4 ]              ;
  Report ( QString("%1 => %2\n").arg(n).arg(v) ) ;
}

void N::PeopleDispatcher::showVariables(void)
{
  QStringList K = Variables . keys ( )             ;
  QString     k                                    ;
  QString     v                                    ;
  foreach ( k , K )                                {
    v = Variables [ k ] . toString ( )             ;
    Report ( QString("%1 => %2\n").arg(k).arg(v) ) ;
  }                                                ;
}
