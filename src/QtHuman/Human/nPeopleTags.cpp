#include <qthuman.h>

typedef struct   {
  QString Name   ;
  UUIDs   Uuids  ;
} PendingSignals ;

N::PeopleTags:: PeopleTags ( QWidget * parent , Plan * p )
              : TagsEditor (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleTags::~PeopleTags(void)
{
}

void N::PeopleTags::Configure(void)
{
  Trigger       = NULL                         ;
  Scope         = "PeopleTags"                 ;
  TagsCheckable = true                         ;
  ItemType      = Types::People                ;
  //////////////////////////////////////////////
  NewTreeWidgetItem ( head                   ) ;
  setWindowTitle    ( tr("Tags")             ) ;
  head->setText     ( 0 , tr("People tags")  ) ;
  setAccessibleName ( "N::PeopleTags"        ) ;
  setObjectName     ( "N::PeopleTags"        ) ;
  setAllAlignments  ( head , Qt::AlignCenter ) ;
  setFont           ( head , Fonts::ListView ) ;
  setHeaderItem     ( head                   ) ;
  //////////////////////////////////////////////
  setDropFlag       ( DropText    , false    ) ;
  setDropFlag       ( DropUrls    , false    ) ;
  setDropFlag       ( DropImage   , false    ) ;
  setDropFlag       ( DropHtml    , false    ) ;
  setDropFlag       ( DropColor   , false    ) ;
  setDropFlag       ( DropTag     , true     ) ;
  setDropFlag       ( DropPicture , false    ) ;
  setDropFlag       ( DropPeople  , true     ) ;
  setDropFlag       ( DropVideo   , false    ) ;
  setDropFlag       ( DropAlbum   , false    ) ;
  setDropFlag       ( DropGender  , false    ) ;
}

bool N::PeopleTags::addMenuItems(MenuManager & menu,QPoint pos)
{
  Q_UNUSED ( pos )                                   ;
   QTreeWidgetItem * item = currentItem ( )          ;
   UUIDs Uuids                                       ;
   ///////////////////////////////////////////////////
   nFullLoop(i,topLevelItemCount())                  {
     QTreeWidgetItem * item = topLevelItem(i)        ;
     if (item->checkState(0)==Qt::Checked)           {
       Uuids << nTreeUuid    ( item , 0 )            ;
     }                                               ;
   }                                                 ;
   if (Uuids.count()<=0 && IsNull(item)) return true ;
   if (Uuids.count()==0 && NotNull(item))            {
     menu.add(501,tr("Selected people"     ))        ;
   } else
   if (Uuids.count()==1)                             {
     menu.add(501,tr("Checked people"      ))        ;
   } else
   if (Uuids.count()>1)                              {
     menu.add(501,tr("Tags selected people"))        ;
     menu.add(502,tr("Tags union people"   ))        ;
   }                                                 ;
   menu.addSeparator()                               ;
   return true                                       ;
}

bool N::PeopleTags::RunMenuResult(QAction * action,MenuManager & menu,QPoint pos)
{
  UUIDs       Uuids                                        ;
  VarArgs     args                                         ;
  int         id = menu [ action ]                         ;
  QTreeWidgetItem * item = currentItem ( )                 ;
  nFullLoop(i,topLevelItemCount())                         {
    QTreeWidgetItem * it = topLevelItem ( i )              ;
    if ( it -> checkState ( 0 ) == Qt::Checked )           {
      Uuids << nTreeUuid    ( it , 0 )                     ;
    }                                                      ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  if ( Uuids . count ( ) <= 0 )                            {
    if ( IsNull ( item ) )                                 {
      Alert ( Error )                                      ;
      return true                                          ;
    }                                                      ;
    Uuids << nTreeUuid ( item , 0 )                        ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  if ( ( Uuids . count ( ) == 1 ) && NotNull ( item ) )    {
    emit TagSelected ( item->text(0) , Uuids[0] )          ;
    return true                                            ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  toVariants ( Uuids , args )                              ;
  switch ( id )                                            {
    case 501                                               :
      start ( 10102 , args )                               ;
    break                                                  ;
    case 502                                               :
      start ( 10101 , args )                               ;
    break                                                  ;
  }                                                        ;
  return true                                              ;
}

void N::PeopleTags::SendPending(void)
{
  QList<PendingSignals *> PSs                                         ;
  mutex [ "Pending" ] . lock   ( )                                    ;
  while ( PendingItems . count ( ) > 0 )                              {
    PendingSignals * ps = (PendingSignals *) PendingItems . first ( ) ;
    PendingItems . takeFirst ( )                                      ;
    emit ListPeople       ( ps -> Name , ps -> Uuids )                ;
    plan -> processEvents (                          )                ;
    PSs << ps                                                         ;
  }                                                                   ;
  mutex [ "Pending" ] . unlock ( )                                    ;
  for (int i = 0 ; i < PSs . count ( ) ; i++ ) delete PSs [ i ]       ;
  PSs . clear ( )                                                     ;
}

void N::PeopleTags::TagsUnion(ThreadData * d)
{
  if ( ! IsContinue ( d ) ) return                           ;
  UUIDs U                                                    ;
  FromVariants ( d -> Arguments , U )                        ;
  if ( U . count ( ) <= 0 ) return                           ;
  bool              correct = true                           ;
  UUIDs             Z                                        ;
  QStringList       HL                                       ;
  QTreeWidgetItem * it                                       ;
  EnterSQL ( SC , plan -> sql )                              ;
    UUIDs V                                                  ;
    UUIDs X                                                  ;
    SUID  u = U . first ( )                                  ;
    U . takeFirst ( )                                        ;
    it = uuidAt ( u )                                        ;
    if ( NotNull ( it ) ) HL << it -> text ( 0 )             ;
    GetSecondUuids ( SC                                      ,
                     Z                                       ,
                     u                                       ,
                     Types::Tag                              ,
                     Types::People                           ,
                     Groups::Subordination                 ) ;
    V = Uniqueness ( Z )                                     ;
    while ( U . count ( ) > 0 )                              {
      u = U . first  ( )                                     ;
      U . takeFirst  ( )                                     ;
      it = uuidAt ( u )                                      ;
      if ( NotNull ( it ) ) HL << it -> text ( 0 )           ;
      X . clear      ( )                                     ;
      GetSecondUuids ( SC                                    ,
                       X                                     ,
                       u                                     ,
                       Types::Tag                            ,
                       Types::People                         ,
                       Groups::Subordination               ) ;
      Z = V + Uniqueness ( X )                               ;
      V = Z                                                  ;
    }                                                        ;
    Z = Uniqueness ( V )                                     ;
  ErrorSQL ( SC , plan -> sql )                              ;
    correct = false                                          ;
  LeaveSQL ( SC , plan -> sql )                              ;
  ////////////////////////////////////////////////////////////
  if ( Z . count ( ) <= 0 )                                  {
    Alert ( Error )                                          ;
    return                                                   ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  PendingSignals * ps = new PendingSignals ( )               ;
  ps -> Name  = HL . join ( " + " )                          ;
  ps -> Uuids = Z                                            ;
  mutex [ "Pending" ] . lock   ( )                           ;
  PendingItems << ps                                         ;
  mutex [ "Pending" ] . unlock ( )                           ;
  emit DispatchPending ( )                                   ;
}

void N::PeopleTags::TagsIntersection (ThreadData * d)
{
  if ( ! IsContinue ( d ) ) return                           ;
  UUIDs U                                                    ;
  FromVariants ( d -> Arguments , U )                        ;
  if ( U . count ( ) <= 0 ) return                           ;
  bool              correct = true                           ;
  UUIDs             Z                                        ;
  QStringList       HL                                       ;
  QTreeWidgetItem * it                                       ;
  EnterSQL ( SC , plan -> sql )                              ;
    UUIDs V                                                  ;
    UUIDs X                                                  ;
    SUID  u = U . first ( )                                  ;
    U . takeFirst ( )                                        ;
    it = uuidAt ( u )                                        ;
    if ( NotNull ( it ) ) HL << it -> text ( 0 )             ;
    GetSecondUuids ( SC                                      ,
                     Z                                       ,
                     u                                       ,
                     Types::Tag                              ,
                     Types::People                           ,
                     Groups::Subordination                 ) ;
    V = Uniqueness ( Z )                                     ;
    while ( ( Z . count ( ) > 0 ) && ( U . count ( ) > 0 ) ) {
      u = U . first  ( )                                     ;
      U . takeFirst  ( )                                     ;
      it = uuidAt ( u )                                      ;
      if ( NotNull ( it ) ) HL << it -> text ( 0 )           ;
      X . clear      ( )                                     ;
      GetSecondUuids ( SC                                    ,
                       X                                     ,
                       u                                     ,
                       Types::Tag                            ,
                       Types::People                         ,
                       Groups::Subordination               ) ;
      Z = V & Uniqueness ( X )                               ;
      V = Z                                                  ;
    }                                                        ;
    Z = Uniqueness ( V )                                     ;
  ErrorSQL ( SC , plan -> sql )                              ;
    correct = false                                          ;
  LeaveSQL ( SC , plan -> sql )                              ;
  ////////////////////////////////////////////////////////////
  if ( Z . count ( ) <= 0 )                                  {
    Alert ( Error )                                          ;
    return                                                   ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  PendingSignals * ps = new PendingSignals ( )               ;
  ps -> Name  = HL . join ( " & " )                          ;
  ps -> Uuids = Z                                            ;
  mutex [ "Pending" ] . lock   ( )                           ;
  PendingItems << ps                                         ;
  mutex [ "Pending" ] . unlock ( )                           ;
  emit DispatchPending ( )                                   ;
}
