#include <qthuman.h>

#ifdef QT_OPENGL_LIB

N::CrowdsShelf:: CrowdsShelf ( QObject * parent , Plan * p )
               : ViewShelf   (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::CrowdsShelf:: CrowdsShelf ( QObject * parent , Plan * p , SceneDestructor * des )
               : ViewShelf   (           parent ,        p ,                   des )
{
  Configure ( ) ;
}

N::CrowdsShelf::~CrowdsShelf (void)
{
}

void N::CrowdsShelf::Configure(void)
{
  QObject::setProperty ( "Type"             , "CrowdsShelf"               ) ;
  setVariable          ( "Type"             , "CrowdsShelf"               ) ;
  setVariable          ( "Menu"             , true                        ) ;
  ///////////////////////////////////////////////////////////////////////////
  setVariable          ( "Access"           , "Divisions"                 ) ;
  setVariable          ( "T1"               , Types::People               ) ;
  setVariable          ( "T2"               , Types::People               ) ;
  setVariable          ( "T3"               , Types::People               ) ;
  setVariable          ( "T4"               , Types::Division             ) ;
  setVariable          ( "T5"               , Types::People               ) ;
  setVariable          ( "Sorting"          , Qt::DescendingOrder         ) ;
  setVariable          ( "AutoRefresh"      , false                       ) ;
  setVariable          ( "ReplaceIcon"      , ":/images/peoplegroups.png" ) ;
  setVariable          ( "ArrangeBy"        , "Icon"                      ) ;
  setVariable          ( "ScrollBy"         , "Current"                   ) ;
  ///////////////////////////////////////////////////////////////////////////
  setVariable          ( "Editable"         , true                        ) ;
//  setVariable          ( "FollowWindow"     , true                        ) ;
  ///////////////////////////////////////////////////////////////////////////
  setVariable          ( "NewGroup"         , tr("New crowd")             ) ;
  setVariable          ( "ItemCount"        , tr("%1 people")             ) ;
  ///////////////////////////////////////////////////////////////////////////
  setDropFlag          ( DropText           , false                       ) ;
  setDropFlag          ( DropUrls           , false                       ) ;
  setDropFlag          ( DropImage          , false                       ) ;
  setDropFlag          ( DropHtml           , false                       ) ;
  setDropFlag          ( DropColor          , false                       ) ;
  setDropFlag          ( DropTag            , false                       ) ;
  setDropFlag          ( DropPicture        , true                        ) ;
  setDropFlag          ( DropPeople         , true                        ) ;
  setDropFlag          ( DropVideo          , false                       ) ;
  setDropFlag          ( DropAlbum          , false                       ) ;
  setDropFlag          ( DropGender         , false                       ) ;
  setDropFlag          ( DropDivision       , true                        ) ;
}

bool N::CrowdsShelf::FocusIn(void)
{
  if ( IsNull ( plan ) ) return false  ;
  LinkAction ( Insert , NewCrowd ( ) ) ;
  return true                          ;
}

bool N::CrowdsShelf::hasItem (
       ControlPoint & From    ,
       ControlPoint & To      ,
       ControlPoint & At      )
{
  UUIDs U = SelectedItems ( )  ;
  return ( U . count ( ) > 0 ) ;
}

QMimeData * N::CrowdsShelf::dragMime (
              ControlPoint & From     ,
              ControlPoint & To       ,
              ControlPoint & At       )
{
  UUIDs        U    = SelectedItems ( )           ;
  QMimeData  * mime = new QMimeData ( )           ;
  if ( 1 == U . count ( ) )                       {
    SUID       t    = U [ 0 ]                     ;
    QByteArray data = CreateByteArray ( t )       ;
    mime -> setData ( "division/uuid"  , data )   ;
  } else                                          {
    QByteArray data = CreateByteArray ( U )       ;
    mime -> setData ( "division/uuids" , data )   ;
  }                                               ;
  QString tooltip                                 ;
  tooltip = tr ( "%1 crowds selected"             )
            .arg ( U . count ( )                ) ;
  QToolTip :: showText ( QCursor::pos(),tooltip ) ;
  return mime                                     ;
}

bool N::CrowdsShelf::dropPeople (
       QWidget     * widget     ,
       ControlPoint  pos        ,
       const UUIDs & Uuids      )
{
  UUIDs   U = Uuids                                  ;
  SUID    p = Value ( "DropUUID" ) . toULongLong ( ) ;
  VarArgs args                                       ;
  if ( U . count ( ) <= 0 ) return true              ;
  args << p                                          ;
  toVariants ( U , args )                            ;
  start ( CiosMenuId ( 85 , 12 , 31 ) , args )       ;
  return true                                        ;
}

bool N::CrowdsShelf::dropPictures (
       QWidget     * widget        ,
       ControlPoint  pos           ,
       const UUIDs & Uuids         )
{
  UUIDs   U = Uuids                                  ;
  SUID    p = Value ( "DropUUID" ) . toULongLong ( ) ;
  VarArgs args                                       ;
  if ( U . count ( ) <= 0 ) return true              ;
  args << p                                          ;
  toVariants ( U , args )                            ;
  start ( CiosMenuId ( 85 , 12 , 33 ) , args )       ;
  return true                                        ;
}

bool N::CrowdsShelf::dropDivisions (
       QWidget     * source         ,
       ControlPoint  psf            ,
       const UUIDs & Uuids          )
{
  UUIDs   U = Uuids                                  ;
  SUID    p = Value ( "DropUUID" ) . toULongLong ( ) ;
  VarArgs args                                       ;
  if ( U . count ( ) <= 0 ) return true              ;
  args << p                                          ;
  toVariants ( U , args )                            ;
  start ( CiosMenuId ( 85 , 12 , 32 ) , args )       ;
  return true                                        ;
}

// PickView::ItemRunner    => 10001, 10002,  10003, 10004, 10011, 73002, 98011, 98012, 851226, 851227, 851228, 851229, 851230, 851231, 851232
// CrowdsShelf::ItemRunner => 73007, 73008
bool N::CrowdsShelf::ItemRunner(int type,ThreadData * data)
{
  if ( PickView::ItemRunner ( type , data ) ) return true ;
  switch ( type )                                         {
    case 73007                                            :
      CrowdConstraints   ( data -> Arguments )            ;
    return true                                           ;
    case 73008                                            :
      CrowdRules         ( data -> Arguments )            ;
    return true                                           ;
  }                                                       ;
  return false                                            ;
}

void N::CrowdsShelf::CrowdConstraints(VarArgs & args)
{
  if ( args . count ( ) <  2 ) return         ;
  /////////////////////////////////////////////
  SUID nuid = args [ 0 ] . toULongLong ( )    ;
  SUID uuid = args [ 1 ] . toULongLong ( )    ;
  /////////////////////////////////////////////
  if ( nuid             <= 0 ) return         ;
  if ( uuid             <= 0 ) return         ;
  /////////////////////////////////////////////
#pragma message("Crowd constraints are required to be completed")
}

void N::CrowdsShelf::CrowdRules(VarArgs & args)
{
  if ( args . count ( ) <  2 ) return         ;
  /////////////////////////////////////////////
  SUID nuid = args [ 0 ] . toULongLong ( )    ;
  SUID uuid = args [ 1 ] . toULongLong ( )    ;
  /////////////////////////////////////////////
  if ( nuid             <= 0 ) return         ;
  if ( uuid             <= 0 ) return         ;
  /////////////////////////////////////////////
#pragma message("Crowd rule tables are required to be completed")
}

int N::CrowdsShelf::Indexing(RangeBar * rbu,int direction,QVariants & v)
{
//  qint64 s = v [ 0 ] . toLongLong ( )     ; // start
//  qint64 e = v [ 1 ] . toLongLong ( )     ; // end
//  qint64 p = v [ 2 ] . toLongLong ( )     ; // page
  qint64 i = v [ 3 ] . toLongLong ( )     ; // index
  UUIDs  U = CurrentItems                 ;
  setVariable  ( "CurrentPageIndex" , i ) ;
  PickItem::ArrangeItems ( i , U )        ;
  return 0                                ;
}

bool N::CrowdsShelf::Relocation(QRectF viewport)
{
  return CentralRelocation ( viewport ) ;
}

void N::CrowdsShelf::AllPeople(void)
{
  emit ShowAll ( ) ;
}

void N::CrowdsShelf::NewCrowd(void)
{
  start ( CiosMenuId ( 85 , 12 , 28 ) ) ;
}

void N::CrowdsShelf::SelectNothing(void)
{
  UUIDs U = CurrentItems                          ;
  SUID  u                                         ;
  foreach ( u , U )                               {
    SelectItem * item = ChildOf<SelectItem> ( u ) ;
    if ( NotNull ( item ) )                       {
      item -> setChecked ( false )                ;
    }                                             ;
  }                                               ;
  setVariable ( "CurrentNode" , 0 )               ;
}

void N::CrowdsShelf::SelectAll(void)
{
  UUIDs U = CurrentItems                          ;
  SUID  u                                         ;
  foreach ( u , U )                               {
    SelectItem * item = ChildOf<SelectItem> ( u ) ;
    if ( NotNull ( item ) )                       {
      item -> setChecked ( true )                 ;
    }                                             ;
  }                                               ;
}

void N::CrowdsShelf::atDoubleClicked(SelectItem * item)
{
  PushCommand ( CiosMenuId ( 89 , 44 , 62 ) , item ) ;
}

bool N::CrowdsShelf::Menu    (
       QWidget      * widget ,
       QPointF        pos    ,
       ControlPoint & From   ,
       ControlPoint & To     )
{
  AllowPickItemMenu                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  MenuManager  mm ( widget )                                                 ;
  QAction    * aa                                                            ;
  QMenu      * ma                                                            ;
  QMenu      * ms                                                            ;
  QMenu      * md                                                            ;
  QMenu      * me                                                            ;
  QMenu      * mc                                                            ;
  SelectItem * item = NearestItem ( From , To )                              ;
  QLineEdit  * line = NULL                                                   ;
  QLineEdit  * utem = NULL                                                   ;
  VarArgs      args                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull ( item ) ) item = CurrentItem ( )                              ;
  ////////////////////////////////////////////////////////////////////////////
  if ( hasFunction ( "Editable" ) )                                          {
    mm      . add          ( CiosMenuId(89,44,14) , tr("Open crowd"  ) )     ;
    mm      . add          ( CiosMenuId(89,44,11) , tr("All people")   )     ;
    mm      . add          ( CiosMenuId(89,44,12) , tr("Refresh"     ) )     ;
    mm      . addSeparator (                                           )     ;
    mm      . add          ( CiosMenuId(89,44,13) , tr("New crowd"   ) )     ;
    mm      . addSeparator (                                           )     ;
    me = mm . addMenu      ( tr("Edit")                                )     ;
    mm      . add          ( me                                              ,
                             CiosMenuId(89,44,41)                            ,
                             tr("Count people in all crowds")          )     ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( hasFunction ( "Selectable" ) && NotNull ( item ) )                    {
    md = mm . addMenu      ( tr("Details")                                 ) ;
    mc = mm . addMenu      ( tr("Categorize")                              ) ;
    NameEditor             ( mm , md , item , &line                        ) ;
    MountUuid              ( mm , md , item                                ) ;
    mm      . addSeparator ( md                                            ) ;
    mm      . addSeparator (                                               ) ;
    //////////////////////////////////////////////////////////////////////////
    mm      . add          ( md                                              ,
                             CiosMenuId(89,44,51)                            ,
                             tr("Count people in this crowd")              ) ;
    mm      . addSeparator ( md                                            ) ;
    mm      . add          ( md                                              ,
                             CiosMenuId(89,44,71)                            ,
                             tr("Delete this crowd")                       ) ;
    mm      . add          ( md                                              ,
                             CiosMenuId(89,44,72)                            ,
                             tr("Delete all people in this crowd")         ) ;
    //////////////////////////////////////////////////////////////////////////
    mm      . add          ( mc , CiosMenuId(89,44,81) , tr("Constraints") ) ;
    mm      . add          ( mc , CiosMenuId(89,44,82) , tr("Rule tables") ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  ma   = mm . addMenu  ( tr("Adjustments")    )                              ;
  ms   = mm . addMenu  ( tr("Sorting")        )                              ;
  AdjustMenu           ( mm , ma              )                              ;
  SortingMenu          ( mm , ms              )                              ;
  ////////////////////////////////////////////////////////////////////////////
  mm . addSeparator    (                      )                              ;
  mm . add             ( CiosMenuId(89,44,99)                                ,
                         tr("Close crowds explorer")                       ) ;
  ////////////////////////////////////////////////////////////////////////////
  RunPickItemMenu                                                            ;
  if ( IsNull ( aa ) )                                                       {
    ModifyName ( item , line )                                               ;
    return true                                                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( AdjustProcessor  ( mm , aa        ) ) return true                     ;
  if ( SortingProcessor ( mm , aa        ) ) return true                     ;
  if ( 0 != CallToMenu  ( mm , aa , true ) ) return true                     ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case CiosMenuId    ( 89 , 44 , 99 )                                      : // Close this crowds explorer
      emit CloseObject ( node         )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 11 )                                      : // View all people
      AllPeople        (              )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 12 )                                      : // Refresh
      startup          (              )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 13 )                                      : // New crowd
      start            ( CiosMenuId ( 85 , 12 , 28 ) )                       ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 14 )                                      : // Open crowd
      if ( NotNull ( item ) )                                                {
        emit DoubleClicked ( item )                                          ;
        atDoubleClicked    ( item )                                          ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 41 )                                      : // Count people in all crowds
      start            ( CiosMenuId ( 85 , 12 , 26 ) )                       ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 51 )                                      : // Count people in this crowd
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( CiosMenuId ( 85 , 12 , 27 ) , args )                ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 71 )                                      : // Delete this crowd
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( CiosMenuId ( 85 , 12 , 29 ) , args )                ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 72 )                                      : // Delete all people in this gallery
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( CiosMenuId ( 85 , 12 , 30 ) , args )                ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 81 )                                      : // Crowd categorize constraints
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( 73007 , args )                                      ;
      }                                                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 82 )                                      : // Crowd categorize rule tables
      if ( NotNull ( item ) )                                                {
        SUID itemUuid = item -> Value ( "UUID" ) . toULongLong ( )           ;
        args << item -> node                                                 ;
        args << itemUuid                                                     ;
        start          ( 73008 , args )                                      ;
      }                                                                      ;
    break                                                                    ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

#endif
