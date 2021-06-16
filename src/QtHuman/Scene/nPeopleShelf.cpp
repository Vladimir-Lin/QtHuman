#include <qthuman.h>

#ifdef QT_OPENGL_LIB

N::PeopleShelf:: PeopleShelf ( QObject * parent , Plan * p )
               : ViewShelf   (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleShelf:: PeopleShelf ( QObject * parent , Plan * p , SceneDestructor * des )
               : ViewShelf   (           parent ,        p ,                   des )
{
  Configure ( ) ;
}

N::PeopleShelf::~PeopleShelf (void)
{
}

void N::PeopleShelf::Configure(void)
{
  QObject::setProperty ( "Type"             , "PeopleShelf"         ) ;
  setVariable          ( "Type"             , "PeopleShelf"         ) ;
  setVariable          ( "Menu"             , true                  ) ;
  /////////////////////////////////////////////////////////////////////
  setVariable          ( "Access"           , "First"               ) ;
  setVariable          ( "T1"               , Types::Division       ) ;
  setVariable          ( "T2"               , Types::People         ) ;
  setVariable          ( "T3"               , Types::Division       ) ;
  setVariable          ( "T4"               , Types::Division       ) ;
  setVariable          ( "T5"               , Types::People         ) ;
  setVariable          ( "Sorting"          , Qt::AscendingOrder    ) ;
  setVariable          ( "Items"            , 0                     ) ;
  setVariable          ( "ByItem"           , "id"                  ) ;
  setVariable          ( "AutoRefresh"      , true                  ) ;
  setVariable          ( "ReplaceIcon"      , ":/images/face.png"   ) ;
  setVariable          ( "ArrangeBy"        , "Names"               ) ;
  setVariable          ( "ScrollBy"         , "Start"               ) ;
  setVariable          ( "NameMaps"         , PlanTable(PersonMaps) ) ;
  setVariable          ( "NameTable"        , PlanTable(Names)      ) ;
  /////////////////////////////////////////////////////////////////////
  setVariable          ( "Editable"         , true                  ) ;
//  setVariable          ( "FollowWindow"     , true                  ) ;
  setVariable          ( "Existance"        , true                  ) ;
  /////////////////////////////////////////////////////////////////////
  setDropFlag          ( DropText           , false                 ) ;
  setDropFlag          ( DropUrls           , false                 ) ;
  setDropFlag          ( DropImage          , false                 ) ;
  setDropFlag          ( DropHtml           , false                 ) ;
  setDropFlag          ( DropColor          , false                 ) ;
  setDropFlag          ( DropTag            , false                 ) ;
  setDropFlag          ( DropPicture        , true                  ) ;
  setDropFlag          ( DropPeople         , true                  ) ;
  setDropFlag          ( DropVideo          , false                 ) ;
  setDropFlag          ( DropAlbum          , false                 ) ;
  setDropFlag          ( DropGender         , false                 ) ;
  setDropFlag          ( DropDivision       , false                 ) ;
}

bool N::PeopleShelf::FocusIn(void)
{
  if ( IsNull ( plan ) ) return false    ;
  return true                            ;
}

// PickView::ItemRunner     => 10001 , 10002 , 10003 , 10004 , 10011, 73002, 98011, 98012, 851226, 851227, 851228, 851229, 851230, 851231, 851232
// GalleryShelf::ItemRunner => 73007 , 73008
bool N::PeopleShelf::ItemRunner(int type,ThreadData * data)
{
  if ( PickView::ItemRunner ( type , data ) ) return true                    ;
  switch ( type )                                                            {
    case CiosMenuId   ( 89 , 44 , 14      )                                  :
    return true                                                              ;
    case CiosMenuId   ( 89 , 44 , 15      )                                  :
    return true                                                              ;
  }                                                                          ;
  return false                                                               ;
}

void N::PeopleShelf::SelectNothing(void)
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

void N::PeopleShelf::SelectAll(void)
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

int N::PeopleShelf::Indexing(RangeBar * rbu,int direction,QVariants & v)
{
//  qint64 s = v [ 0 ] . toLongLong ( ) ; // start
//  qint64 e = v [ 1 ] . toLongLong ( ) ; // end
//  qint64 p = v [ 2 ] . toLongLong ( ) ; // page
  qint64 i = v [ 3 ] . toLongLong ( ) ; // index
  setVariable ( "StartId" , i )       ;
  start       ( 10011         )       ;
  return 0                            ;
}

bool N::PeopleShelf::Relocation(QRectF viewport)
{
  return CentralRelocation ( viewport ) ;
}

void N::PeopleShelf::PeopleMenu(MenuManager & mm,QMenu * mp,SelectItem * item)
{
  if ( IsNull ( mp ) ) return                                                ;
  ////////////////////////////////////////////////////////////////////////////
  if ( NotNull ( item ) )                                                    {
    MountUuid            ( mm , mp  , item                                 ) ;
    mm    . add          ( mp                                                ,
                           CiosMenuId(83,22,31)                              ,
                           tr("Remove this person")                        ) ;
    mm    . addSeparator ( mp                                              ) ;
    mm    . add          ( mp                                                ,
                           CiosMenuId(83,22,41)                              ,
                           tr("Edit details")                              ) ;
    mm    . add          ( mp                                                ,
                           CiosMenuId(83,22,42)                              ,
                           tr("Edit person names")                         ) ;
    mm    . addSeparator ( mp                                              ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  mm . add ( mp                                                              ,
             CiosMenuId(83,22,51)                                            ,
             tr("Check existance before append people")                      ,
             true                                                            ,
             hasFunction ( "Existance" )                                   ) ;
}

void N::PeopleShelf::ProfileMenu(MenuManager & mm,QMenu * mp,SelectItem * item)
{
  if ( IsNull ( mp ) ) return                                                ;
  mm . add          ( mp , CiosMenuId(83,22,71) , tr("Personal documents") ) ;
  mm . add          ( mp , CiosMenuId(83,22,72) , tr("Personal gallery")   ) ;
  mm . add          ( mp , CiosMenuId(83,22,73) , tr("Video clips")        ) ;
  mm . addSeparator ( mp                                                   ) ;
  mm . add          ( mp , CiosMenuId(83,22,74) , tr("Potential pictures") ) ;
  mm . add          ( mp , CiosMenuId(83,22,75) , tr("Subgroups")          ) ;
  mm . addSeparator ( mp                                                   ) ;
  mm . add          ( mp , CiosMenuId(83,22,76) , tr("All related items")  ) ;
}

void N::PeopleShelf::FeaturesMenu(MenuManager & mm,QMenu * mf,SelectItem * item)
{
  if ( IsNull ( mf ) ) return                                                ;
  mm . add          ( mf , CiosMenuId(83,22,81) , tr("Personality")        ) ;
  mm . addSeparator ( mf                                                   ) ;
  mm . add          ( mf , CiosMenuId(83,22,82) , tr("3D Models")          ) ;
  mm . addSeparator ( mf                                                   ) ;
  mm . add          ( mf , CiosMenuId(83,22,83) , tr("Faces")              ) ;
  mm . add          ( mf , CiosMenuId(83,22,84) , tr("Measurements")       ) ;
  mm . add          ( mf , CiosMenuId(83,22,85) , tr("Body")               ) ;
  mm . add          ( mf , CiosMenuId(83,22,86) , tr("Tatto")              ) ;
  mm . addSeparator ( mf                                                   ) ;
  mm . add          ( mf , CiosMenuId(83,22,87) , tr("Possible faces")     ) ;
}

void N::PeopleShelf::InternetMenu(MenuManager & mm,QMenu * mi,SelectItem * item)
{
  if ( IsNull ( mi ) ) return                                                ;
  mm . add          ( mi                                                     ,
                      CiosMenuId(83,22,61)                                   ,
                      tr("Search name via Google")                         ) ;
  mm . add          ( mi                                                     ,
                      CiosMenuId(83,22,62)                                   ,
                      tr("Image search via Google image")                  ) ;
  mm . addSeparator ( mi                                                   ) ;
  mm . add          ( mi                                                     ,
                      CiosMenuId(83,22,63)                                   ,
                      tr("Search name by customized engine")               ) ;
  mm . add          ( mi                                                     ,
                      CiosMenuId(83,22,64)                                   ,
                      tr("Search image by customized engine")              ) ;
  mm . addSeparator ( mi                                                   ) ;
  mm . add          ( mi                                                     ,
                      CiosMenuId(83,22,65)                                   ,
                      QIcon(":/images/bookmarks.png")                        ,
                      tr("Personal bookmarks")                             ) ;
}

void N::PeopleShelf::StatisticsMenu(MenuManager & mm,QMenu * ms,SelectItem * item)
{
  if ( IsNull ( ms ) ) return                                                ;
  mm . add          ( ms                                                     ,
                      CiosMenuId(83,22,91)                                   ,
                      tr("Pictures")                                       ) ;
  mm . add          ( ms                                                     ,
                      CiosMenuId(83,22,92)                                   ,
                      tr("Possible pictures")                              ) ;
  mm . addSeparator ( ms                                                   ) ;
  mm . add          ( ms                                                     ,
                      CiosMenuId(83,22,93)                                   ,
                      tr("Export people names to file")                    ) ;
  mm . add          ( ms                                                     ,
                      CiosMenuId(83,22,94)                                   ,
                      tr("Export people names in all languages")           ) ;
}

bool N::PeopleShelf::PeopleProcessor(MenuManager & mm,QAction * aa,SelectItem * item)
{
  VarArgs args                                                               ;
  switch ( mm [ aa ] )                                                       {
    case CiosMenuId ( 83 , 22 , 31 )                                         : // Remove this person
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 41 )                                         : // Edit details
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 42 )                                         : // Edit person names
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 51 )                                         : // Check existance before append people
    return true                                                              ;
  }                                                                          ;
  return false                                                               ;
}

bool N::PeopleShelf::ProfileProcessor(MenuManager & mm,QAction * aa,SelectItem * item)
{
  VarArgs args                                                               ;
  switch ( mm [ aa ] )                                                       {
    case CiosMenuId ( 83 , 22 , 71 )                                         : // Personal documents
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 72 )                                         : // Personal gallery
      if ( NotNull ( item ) )                                                {
        emit PeopleGallery  ( item )                                         ;
        PushCommand ( CiosMenuId ( 89 , 44 , 61 ) , item )                   ;
      }                                                                      ;
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 73 )                                         : // Video clips
      if ( NotNull ( item ) )                                                {
        // t1 = Album , t2 = People , requires LoadSecond
      }                                                                      ;
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 74 )                                         : // Potential pictures
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 75 )                                         : // Subgroups
      if ( NotNull ( item ) )                                                {
        emit PeopleSubgroup ( item )                                         ;
        PushCommand ( CiosMenuId ( 89 , 44 , 68 ) , item )                   ;
      }                                                                      ;
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 76 )                                         : // All related items
    return true                                                              ;
  }                                                                          ;
  return false                                                               ;
}

bool N::PeopleShelf::FeaturesProcessor(MenuManager & mm,QAction * aa,SelectItem * item)
{
  VarArgs args                                                               ;
  switch ( mm [ aa ] )                                                       {
    case CiosMenuId ( 83 , 22 , 81 )                                         : // Personality
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 82 )                                         : // 3D Models
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 83 )                                         : // Faces
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 84 )                                         : // Measurements
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 85 )                                         : // Body
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 86 )                                         : // Tatto
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 87 )                                         : // Possible faces
    return true                                                              ;
  }                                                                          ;
  return false                                                               ;
}

bool N::PeopleShelf::InternetProcessor(MenuManager & mm,QAction * aa,SelectItem * item)
{
  VarArgs args                                                               ;
  switch ( mm [ aa ] )                                                       {
    case CiosMenuId ( 83 , 22 , 61 )                                         : // Search name via Google
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 62 )                                         : // Image search via Google image
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 63 )                                         : // Search name by customized engine
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 64 )                                         : // Search image by customized engine
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 65 )                                         : // Personal bookmarks
    return true                                                              ;
  }                                                                          ;
  return false                                                               ;
}

bool N::PeopleShelf::StatisticsProcessor(MenuManager & mm,QAction * aa,SelectItem * item)
{
  VarArgs args                                                               ;
  switch ( mm [ aa ] )                                                       {
    case CiosMenuId ( 83 , 22 , 91 )                                         : // Pictures
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 92 )                                         : // Possible pictures
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 93 )                                         : // Export people names to file
    return true                                                              ;
    case CiosMenuId ( 83 , 22 , 94 )                                         : // Export people names in all languages
    return true                                                              ;
  }                                                                          ;
  return false                                                               ;
}

bool N::PeopleShelf::Menu    (
       QWidget      * widget ,
       QPointF        pos    ,
       ControlPoint & From   ,
       ControlPoint & To     )
{
  AllowPickItemMenu                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  MenuManager  mm ( widget )                                                 ;
  QAction    * aa   = NULL                                                   ;
  QMenu      * ma   = NULL                                                   ;
  QMenu      * ms   = NULL                                                   ;
  QMenu      * mt   = NULL                                                   ;
  QMenu      * mf   = NULL                                                   ;
  QMenu      * me   = NULL                                                   ;
  QMenu      * mi   = NULL                                                   ;
  QMenu      * mp   = NULL                                                   ;
  SelectItem * item = NearestItem   ( From , To )                            ;
  UUIDs        U    = SelectedItems (           )                            ;
  VarArgs      args                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull  ( item ) ) item = CurrentItem ( )                             ;
  MountName ( mm , item )                                                    ;
  ////////////////////////////////////////////////////////////////////////////
  if ( hasFunction ( "Editable" ) )                                          {
    mm      . add          ( CiosMenuId(89,44,12) , tr("New person"      ) ) ;
    mm      . add          ( CiosMenuId(89,44,12) , tr("Reload"          ) ) ;
    mm      . addSeparator (                                               ) ;
    if ( NotNull ( item ) )                                                  {
      me = mm . addMenu    ( tr("Edit")                                    ) ;
      PeopleMenu             ( mm , me , item                              ) ;
      if ( U . count ( ) > 1 )                                               {
        mm . add             ( me                                            ,
                               621                                           ,
                               tr("Merge selected people")                 ) ;
      }                                                                      ;
      mm      . addSeparator (                                             ) ;
      mp = mm . addMenu      ( tr("Profile")                               ) ;
      mf = mm . addMenu      ( tr("Features")                              ) ;
      mi = mm . addMenu      ( tr("Internet")                              ) ;
      mm      . addSeparator (                                             ) ;
      ProfileMenu            ( mm , mp , item                              ) ;
      FeaturesMenu           ( mm , mf , item                              ) ;
      InternetMenu           ( mm , mi , item                              ) ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  mt = mm . addMenu          ( tr("Statistics")                            ) ;
  ////////////////////////////////////////////////////////////////////////////
  if ( hasFunction ( "Editable" ) )                                          {
    if ( "First" == Value ( "Access" ) . toString ( ) )                      {
      mm . add          ( 221 , tr("People listings editor")               ) ;
      mm . addSeparator (                                                  ) ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  ma = mm . addMenu          ( tr("Adjustments")                           ) ;
  ms = mm . addMenu          ( tr("Sorting")                               ) ;
  StatisticsMenu             ( mm , mt , item                              ) ;
  AdjustMenu                 ( mm , ma                                     ) ;
  SortingMenu                ( mm , ms                                     ) ;
  ////////////////////////////////////////////////////////////////////////////
  mm . addSeparator          (                                             ) ;
  mm . add                   ( CiosMenuId(89,44,99)                          ,
                               tr("Close people explorer")                 ) ;
  ////////////////////////////////////////////////////////////////////////////
  RunPickItemMenu                                                            ;
  nKickOut             ( IsNull ( aa ) , true )                              ;
  ////////////////////////////////////////////////////////////////////////////
  if ( AdjustProcessor     ( mm , aa        ) ) return true                  ;
  if ( SortingProcessor    ( mm , aa        ) ) return true                  ;
  if ( PeopleProcessor     ( mm , aa , item ) ) return true                  ;
  if ( ProfileProcessor    ( mm , aa , item ) ) return true                  ;
  if ( FeaturesProcessor   ( mm , aa , item ) ) return true                  ;
  if ( InternetProcessor   ( mm , aa , item ) ) return true                  ;
  if ( StatisticsProcessor ( mm , aa , item ) ) return true                  ;
  if ( 0 != CallToMenu     ( mm , aa , true ) ) return true                  ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case CiosMenuId    ( 89 , 44 , 99 )                                      : // Close this people explorer
      emit CloseObject ( node         )                                      ;
    break                                                                    ;
    case CiosMenuId    ( 89 , 44 , 12 )                                      : // Refresh
      startup          (              )                                      ;
    break                                                                    ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

#endif
