#include <qthuman.h>

N::PeopleMerge:: PeopleMerge ( QWidget * parent , N::Plan * p )
               : TreeDock    (           parent ,           p )
               , GroupItems  (                              p )
               , Deletion    ( true                           )
               , Execute     ( true                           )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleMerge::~PeopleMerge(void)
{
}

QSize N::PeopleMerge::sizeHint(void) const
{
  return QSize ( 320 , 240 ) ;
}

void N::PeopleMerge::Configure(void)
{
  dockingOrientation = Qt::Vertical                        ;
  //////////////////////////////////////////////////////////
  setAccessibleName       ( "N::PeopleMerge"    )          ;
  //////////////////////////////////////////////////////////
  NewTreeWidgetItem       ( head                )          ;
  setWindowTitle          ( tr("Merge People" ) )          ;
  head->setText           ( 0 , tr("Name")      )          ;
  head->setText           ( 1 , tr("UUID")      )          ;
  //////////////////////////////////////////////////////////
  setFocusPolicy          ( Qt::WheelFocus      )          ;
  setDragDropMode         ( DragDrop            )          ;
  setAlternatingRowColors ( true                )          ;
  setRootIsDecorated      ( false               )          ;
  setSelectionMode        ( ExtendedSelection   )          ;
  setColumnCount          ( 2                   )          ;
  assignHeaderItems       ( head                )          ;
  MountClicked            ( 2                   )          ;
  plan -> setFont         ( this                )          ;
  //////////////////////////////////////////////////////////
  setDropFlag             ( DropPeople , true   )          ;
  //////////////////////////////////////////////////////////
  LocalIcons [ "Insert" ] = QIcon(":/images/plus.png"    ) ;
  LocalIcons [ "Delete" ] = QIcon(":/images/delete.png"  ) ;
  LocalIcons [ "Merge"  ] = QIcon(":/images/minimize.png") ;
  LocalIcons [ "Clear"  ] = QIcon(":/images/blank.png"   ) ;
  //////////////////////////////////////////////////////////
  PassDragDrop = false                                     ;
}

bool N::PeopleMerge::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::PeopleMerge::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::PeopleMerge::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return true                              ;
}

bool N::PeopleMerge::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut         ( nEqual(source,this) , false ) ;
  return dropItems ( source , mime , pos         ) ;
}

bool N::PeopleMerge::FocusIn(void)
{
  AssignAction ( Label  , windowTitle ( ) ) ;
  LinkAction   ( Insert , New         ( ) ) ;
  LinkAction   ( Delete , Delete      ( ) ) ;
  return true                               ;
}

void N::PeopleMerge::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( column > 0 )                 ;
  removeOldItem()                         ;
  /////////////////////////////////////////
  QComboBox * combo                       ;
  SUID        uuid  = nTreeUuid(item,0)   ;
  combo = setComboBox                     (
            item                          ,
            column                        ,
            SIGNAL(activated     (int))   ,
            SLOT  (peopleFinished(int)) ) ;
  /////////////////////////////////////////
  combo->blockSignals( true )             ;
  combo->setEditable ( true )             ;
  N::PeopleManager PM( plan )             ;
  EnterSQL ( SC , plan->sql )             ;
    PM . PeopleComboBox ( SC , combo )    ;
  LeaveSQL ( SC , plan->sql )             ;
  combo->blockSignals(false )             ;
  /////////////////////////////////////////
  if (uuid>0) (*combo) <= uuid            ;
  combo->showPopup()                      ;
}

void N::PeopleMerge::New(void)
{
  NewTreeWidgetItem ( IT )      ;
  IT->setData(0,Qt::UserRole,0) ;
  addTopLevelItem ( IT     )    ;
  scrollToItem    ( IT     )    ;
  doubleClicked   ( IT , 0 )    ;
}

void N::PeopleMerge::Delete(void)
{
  QTreeWidgetItem * item    ;
  item = currentItem()      ;
  nDropOut ( IsNull(item) ) ;
  takeItem ( item         ) ;
}

void N::PeopleMerge::peopleFinished(int)
{
  nDropOut               ( IsNull(ItemEditing)     ) ;
  QComboBox * combo = Casting(QComboBox,ItemWidget ) ;
  nDropOut               ( IsNull(combo      )     ) ;
  ////////////////////////////////////////////////////
  SUID uuid = N::GetUuid ( combo                   ) ;
  ItemEditing->setText   ( 0,combo->currentText()  ) ;
  ItemEditing->setText   ( 1,QString::number(uuid) ) ;
  ItemEditing->setData   ( 0,Qt::UserRole,uuid     ) ;
  ItemEditing->setToolTip( 0,QString::number(uuid) ) ;
  removeOldItem          (                         ) ;
  Alert                  ( Done                    ) ;
}

bool N::PeopleMerge::dropPeople(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                                  ;
  Q_UNUSED ( pos    )                                  ;
  UUIDs Puids = itemUuids(0)                           ;
  N::PeopleManager PM ( plan  )                        ;
  EnterSQL ( SC , plan->sql )                          ;
    QString Q                                          ;
    SUID    uuid                                       ;
    foreach (uuid,Uuids)                               {
      if (!Puids.contains(uuid))                       {
        QStringList names                              ;
        QString     n = ""                             ;
        names = PM.Names(SC,uuid)                      ;
        if (names.count()>0) n = names[0]              ;
        NewTreeWidgetItem(item)                        ;
        item->setData    ( 0 , Qt::UserRole,uuid     ) ;
        item->setText    ( 0 , n                     ) ;
        item->setText    ( 1 , QString::number(uuid) ) ;
        item->setToolTip ( 0 , QString::number(uuid) ) ;
        addTopLevelItem  ( item                      ) ;
        Puids << uuid                                  ;
      }                                                ;
    }                                                  ;
  LeaveSQL ( SC , plan->sql )                          ;
  Alert    ( Done           )                          ;
  return true                                          ;
}

bool N::PeopleMerge::DeleteMerged(N::SqlConnection & SC,SUID deletion)
{
  QString Q                                                           ;
  Q = SC.sql.DeleteFrom(PlanTable(PeopleUuid),SC.WhereUuid(deletion)) ;
  if (!SC.Query(Q))                                                   {
    PlanDebug(30,tr("Can not delete %1 from table %2"                 )
                .arg(deletion                                         )
                .arg(PlanTable(PeopleUuid)                        ) ) ;
    PlanDebug(30,SC.q.lastError().text())                             ;
    return false                                                      ;
  }                                                                   ;
  plan -> processEvents ( )                                           ;
  Q = SC.sql.DeleteFrom(PlanTable(MajorUuid),SC.WhereUuid(deletion))  ;
  if (!SC.Query(Q))                                                   {
    PlanDebug(30,tr("Can not delete %1 from table %2"                 )
                .arg(deletion                                         )
                .arg(PlanTable(MajorUuid)                         ) ) ;
    PlanDebug(30,SC.q.lastError().text())                             ;
    return false                                                      ;
  }                                                                   ;
  plan -> processEvents ( )                                           ;
  return true                                                         ;
}

bool N::PeopleMerge::NamesMerger(SqlConnection & SC,SUID master,SUID deletion)
{
  if ( master   <= 0               ) return false             ;
  if ( deletion <= 0               ) return false             ;
  if ( master   == deletion        ) return false             ;
  /////////////////////////////////////////////////////////////
  NameMapItems DeleteMaps                                     ;
  DeleteMaps . where ( SC , PlanTable(NameMaps) , deletion )  ;
  if ( DeleteMaps . count ( ) <= 0 ) return false             ;
  plan -> processEvents ( )                                   ;
  /////////////////////////////////////////////////////////////
  NameMapItems MasterMaps                                     ;
  MasterMaps . where ( SC , PlanTable(NameMaps) , master   )  ;
  plan -> processEvents ( )                                   ;
  /////////////////////////////////////////////////////////////
  int prefer = 0                                              ;
  if ( MasterMaps.count() > 0      )                          {
    prefer = MasterMaps.last()->prefer                        ;
  }                                                           ;
  prefer++                                                    ;
  DeleteMaps . remove ( SC , PlanTable(NameMaps) )            ;
  plan -> processEvents ( )                                   ;
  /////////////////////////////////////////////////////////////
  QString Q                                                   ;
  Q = SC . sql . InsertInto                                   (
        PlanTable(NameMaps)                                   ,
        4                                                     ,
        "uuid"                                                ,
        "name"                                                ,
        "prefer"                                              ,
        "usage"                                             ) ;
  for (int i=0;i<DeleteMaps.count();i++)                      {
    SC . Prepare ( Q                               )          ;
    SC . Bind    ( "uuid"   , master               )          ;
    SC . Bind    ( "name"   , DeleteMaps[i]->name  )          ;
    SC . Bind    ( "prefer" , prefer               )          ;
    SC . Bind    ( "usage"  , DeleteMaps[i]->usage )          ;
    if ( SC . Exec ( ) ) prefer++                             ;
    plan -> processEvents ( )                                 ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  return true                                                 ;
}

bool N::PeopleMerge::GroupsRemove (SqlConnection & SC,QList<Group> & groups)
{
  for (int i=0;i<groups.count();i++) {
    ExactDetach ( SC , groups[i] )   ;
    plan -> processEvents ( )        ;
  }                                  ;
  return true                        ;
}

bool N::PeopleMerge::GroupsJoin (SqlConnection & SC,QList<Group> & groups)
{
  QString Q                                              ;
  for (int i=0;i<groups.count();i++)                     {
    bool exists = false                                  ;
    //////////////////////////////////////////////////////
    Q = SC.sql.SelectFrom                                (
          "id"                                           ,
          PlanTable(Groups)                              ,
          ExactItem                                      (
            groups [ i ] . first                         ,
            groups [ i ] . second                        ,
            groups [ i ] . t1                            ,
            groups [ i ] . t2                            ,
            groups [ i ] . relation                  ) ) ;
    if (SC.Fetch(Q)) exists = true                       ;
    if (exists) continue                                 ;
    //////////////////////////////////////////////////////
    UUIDs U                                              ;
    U << groups[i].second                                ;
    GroupItems :: Join                                   (
      SC                                                 ,
      groups[i].first                                    ,
      groups[i].t1                                       ,
      groups[i].t2                                       ,
      groups[i].relation                                 ,
      0                                                  ,
      U                                                  ,
      false                                            ) ;
    plan -> processEvents ( )                            ;
    Q = SC.sql.Update                                    (
          PlanTable(Groups)                              ,
          ExactItem                                      (
            groups[i].first                              ,
            groups[i].second                             ,
            groups[i].t1                                 ,
            groups[i].t2                                 ,
            groups[i].relation                         ) ,
          2                                              ,
          "membership"                                   ,
          "reversal"                                   ) ;
    SC . Prepare ( Q                                   ) ;
    SC . Bind    ( "membership" , groups[i].membership ) ;
    SC . Bind    ( "reversal"   , groups[i].reversal   ) ;
    SC . Exec    (                                     ) ;
    plan -> processEvents ( )                            ;
  }                                                      ;
  return true                                            ;
}

bool N::PeopleMerge::GroupsMerger(SqlConnection & SC,SUID master,SUID deletion)
{
  if ( master   <= 0               ) return false             ;
  if ( deletion <= 0               ) return false             ;
  if ( master   == deletion        ) return false             ;
  /////////////////////////////////////////////////////////////
  QList<Group> G                                              ;
  QString         Q                                           ;
  Q = SC.sql.SelectFrom                                       (
        SC . Columns                                          (
          8                                                   ,
          "first"                                             ,
          "second"                                            ,
          "t1"                                                ,
          "t2"                                                ,
          "relation"                                          ,
          "membership"                                        ,
          "position"                                          ,
          "reversal"                                        ) ,
        PlanTable(Groups)                                     ,
        QString("where `first` = %1").arg(deletion)           ,
        SC.OrderByAsc("id")                                 ) ;
  SqlLoopNow ( SC , Q )                                       ;
    Group M                                                   ;
    M.first      = SC . Uuid   ( 0 )                          ;
    M.second     = SC . Uuid   ( 1 )                          ;
    M.t1         = SC . Int    ( 2 )                          ;
    M.t2         = SC . Int    ( 3 )                          ;
    M.relation   = SC . Int    ( 4 )                          ;
    M.membership = SC . Double ( 5 )                          ;
    M.position   = SC . Int    ( 6 )                          ;
    M.reversal   = SC . Int    ( 7 )                          ;
    G << M                                                    ;
  SqlLoopErr ( SC , Q )                                       ;
  SqlLoopEnd ( SC , Q )                                       ;
  if (G.count()<=0) return false                              ;
  GroupsRemove ( SC , G )                                     ;
  /////////////////////////////////////////////////////////////
  for (int i=0;i<G.count();i++) G[i].first = master           ;
  GroupsJoin ( SC , G )                                       ;
  /////////////////////////////////////////////////////////////
  return true                                                 ;
}

bool N::PeopleMerge::OwnersMerger(SqlConnection & SC,SUID master,SUID deletion)
{
  if ( master   <= 0               ) return false             ;
  if ( deletion <= 0               ) return false             ;
  if ( master   == deletion        ) return false             ;
  /////////////////////////////////////////////////////////////
  QList<Group> G                                              ;
  QString      Q                                              ;
  Q = SC.sql.SelectFrom                                       (
        SC . Columns                                          (
          8                                                   ,
          "first"                                             ,
          "second"                                            ,
          "t1"                                                ,
          "t2"                                                ,
          "relation"                                          ,
          "membership"                                        ,
          "position"                                          ,
          "reversal"                                        ) ,
        PlanTable(Groups)                                     ,
        QString("where `second` = %1").arg(deletion)          ,
        SC.OrderByAsc("id")                                 ) ;
  SqlLoopNow ( SC , Q )                                       ;
    Group M                                                   ;
    M.first      = SC . Uuid   ( 0 )                          ;
    M.second     = SC . Uuid   ( 1 )                          ;
    M.t1         = SC . Int    ( 2 )                          ;
    M.t2         = SC . Int    ( 3 )                          ;
    M.relation   = SC . Int    ( 4 )                          ;
    M.membership = SC . Double ( 5 )                          ;
    M.position   = SC . Int    ( 6 )                          ;
    M.reversal   = SC . Int    ( 7 )                          ;
    G << M                                                    ;
  SqlLoopErr ( SC , Q )                                       ;
  SqlLoopEnd ( SC , Q )                                       ;
  if (G.count()<=0) return false                              ;
  GroupsRemove ( SC , G )                                     ;
  /////////////////////////////////////////////////////////////
  for (int i=0;i<G.count();i++) G[i].second = master          ;
  GroupsJoin ( SC , G )                                       ;
  /////////////////////////////////////////////////////////////
  return true                                                 ;
}

bool N::PeopleMerge::MergeTriples(N::SqlConnection & SC,SUID master,SUID deletion)
{
  QString Q                                           ;
  /////////////////////////////////////////////////////
  Q = SC.sql.Update                                   (
        PlanTable(Triples)                            ,
        QString("where `first` = %1").arg(deletion)   ,
        1                                             ,
        "first"                                     ) ;
  SC . Prepare ( Q                )                   ;
  SC . Bind    ( "first" , master )                   ;
  SC . Exec    (                  )                   ;
  plan -> processEvents ( )                           ;
  /////////////////////////////////////////////////////
  Q = SC.sql.Update                                   (
        PlanTable(Triples)                            ,
        QString("where `second` = %1").arg(deletion)  ,
        1                                             ,
        "second"                                    ) ;
  SC . Prepare ( Q                 )                  ;
  SC . Bind    ( "second" , master )                  ;
  SC . Exec    (                   )                  ;
  plan -> processEvents ( )                           ;
  /////////////////////////////////////////////////////
  Q = SC.sql.Update                                   (
        PlanTable(Triples)                            ,
        QString("where `third` = %1").arg(deletion)   ,
        1                                             ,
        "third"                                     ) ;
  SC . Prepare ( Q                )                   ;
  SC . Bind    ( "third" , master )                   ;
  SC . Exec    (                  )                   ;
  plan -> processEvents ( )                           ;
  /////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom                               (
        PlanTable(Triples)                            ,
        QString("where `first` = %1").arg(deletion))  ;
  SC.Query(Q)                                         ;
  plan -> processEvents ( )                           ;
  /////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom                               (
        PlanTable(Triples)                            ,
        QString("where `second` = %1").arg(deletion)) ;
  SC.Query(Q)                                         ;
  plan -> processEvents ( )                           ;
  /////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom                               (
        PlanTable(Triples)                            ,
        QString("where `third` = %1").arg(deletion))  ;
  SC.Query(Q)                                         ;
  plan -> processEvents ( )                           ;
  /////////////////////////////////////////////////////
  return true                                         ;
}

bool N::PeopleMerge::MergeFlags(N::SqlConnection & SC,SUID master,SUID deletion)
{
  QString Q                                                            ;
  Q = SC.sql.Update(PlanTable(UFlags),SC.WhereUuid(deletion),1,"uuid") ;
  SC . Prepare ( Q               )                                     ;
  SC . Bind    ( "uuid" , master )                                     ;
  SC . Exec    (                 )                                     ;
  plan -> processEvents ( )                                            ;
  //////////////////////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom(PlanTable(UFlags),SC.WhereUuid(deletion))      ;
  if (SC.Query(Q))                                                     {
    PlanDebug ( 60 , tr("Remove %1 from uflags").arg(deletion)       ) ;
  }                                                                    ;
  plan -> processEvents ( )                                            ;
  //////////////////////////////////////////////////////////////////////
  return true                                                          ;
}

bool N::PeopleMerge::MergeVariables(N::SqlConnection & SC,SUID master,SUID deletion)
{
  QString Q                                                               ;
  CUIDs   N                                                               ;
  CUIDs   R                                                               ;
  bool    correct                                                         ;
  /////////////////////////////////////////////////////////////////////////
  correct = SC.Cuids(N,PlanTable(Variables),"id",SC.WhereUuid(deletion))  ;
  plan -> processEvents ( )                                               ;
            SC.Cuids(R,PlanTable(Variables),"id",SC.WhereUuid(master  ))  ;
  plan -> processEvents ( )                                               ;
  if (!correct)                                                           {
    PlanDebug ( 60                                                        ,
                tr("%1 does not have variables").arg(deletion)          ) ;
    return true                                                           ;
  }                                                                       ;
  plan -> processEvents ( )                                               ;
  /////////////////////////////////////////////////////////////////////////
  if (!Execute)                                                           {
    PlanDebug ( 60                                                        ,
         tr("%1 has %2 variables").arg(deletion).arg(N.count())         ) ;
    return true                                                           ;
  }                                                                       ;
  plan -> processEvents ( )                                               ;
  /////////////////////////////////////////////////////////////////////////
  QStringList V                                                           ;
  QString     VN                                                          ;
  Q = SC . sql . SelectFrom                                               (
        "name"                                                            ,
        PlanTable(Variables)                                              ,
        SC.WhereUuid(deletion)                                            ,
        SC.OrderByAsc("id")                                             ) ;
  SqlLoopNow ( SC , Q )                                                   ;
    VN = SC . String ( 0 )                                                ;
    if (VN.length()>0 && !V.contains(VN)) V << VN                         ;
  SqlLoopErr ( SC , Q )                                                   ;
  SqlLoopEnd ( SC , Q )                                                   ;
  /////////////////////////////////////////////////////////////////////////
  foreach (VN,V)                                                          {
    Q = SC.sql.Update                                                     (
          PlanTable(Variables)                                            ,
          QString ( "where `uuid` = %1 and `name` = '%2'"                 )
          . arg ( deletion                                                )
          . arg ( VN                                                    ) ,
          1                                                               ,
          "uuid"                                                        ) ;
    SC . Prepare ( Q               )                                      ;
    SC . Bind    ( "uuid" , master )                                      ;
    SC . Exec    (                 )                                      ;
  }                                                                       ;
  plan -> processEvents ( )                                               ;
  /////////////////////////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom(PlanTable(Variables),SC.WhereUuid(deletion))      ;
  if (SC.Query(Q))                                                        {
    PlanDebug ( 60 , tr("Remove %1 from variables").arg(deletion)       ) ;
  }                                                                       ;
  plan -> processEvents ( )                                               ;
  /////////////////////////////////////////////////////////////////////////
  return true                                                             ;
}

bool N::PeopleMerge::MergeIdentifier(SqlConnection & SC,SUID master,SUID deletion)
{
  QString Q                                                                 ;
  Q = SC.sql.Update(PlanTable(Identifiers),SC.WhereUuid(deletion),1,"uuid") ;
  SC . Prepare ( Q               )                                          ;
  SC . Bind    ( "uuid" , master )                                          ;
  SC . Exec    (                 )                                          ;
  plan -> processEvents ( )                                                 ;
  ///////////////////////////////////////////////////////////////////////////
  Q = SC.sql.DeleteFrom(PlanTable(Identifiers),SC.WhereUuid(deletion))      ;
  if (SC.Query(Q))                                                          {
    PlanDebug ( 60 , tr("Remove %1 from identifiers").arg(deletion)       ) ;
  }                                                                         ;
  plan -> processEvents ( )                                                 ;
  ///////////////////////////////////////////////////////////////////////////
  return true                                                               ;
}

void N::PeopleMerge::Merge(void)
{
  nDropOut ( topLevelItemCount() < 2 )     ;
  UUIDs Uuids = itemUuids(0)               ;
  nDropOut ( Uuids . count()     < 2 )     ;
  SUID  puid  = Uuids[0]                   ;
  SUID  uuid                               ;
  Uuids . takeAt      ( 0                ) ;
  //////////////////////////////////////////
  setEnabled          ( false            ) ;
  EnterSQL            ( SC , plan->sql   ) ;
    foreach           ( uuid , Uuids     ) {
      NamesMerger     ( SC , puid , uuid ) ;
      GroupsMerger    ( SC , puid , uuid ) ;
      OwnersMerger    ( SC , puid , uuid ) ;
      MergeTriples    ( SC , puid , uuid ) ;
      MergeFlags      ( SC , puid , uuid ) ;
      MergeVariables  ( SC , puid , uuid ) ;
      MergeIdentifier ( SC , puid , uuid ) ;
      if (Execute)                         {
        DeleteMerged  ( SC ,        uuid ) ;
      }                                    ;
    }                                      ;
  ErrorSQL            ( SC , plan->sql   ) ;
  LeaveSQL            ( SC , plan->sql   ) ;
  setEnabled          ( true             ) ;
  //////////////////////////////////////////
  TreeWidgetItems Items                    ;
  for (int i=1;i<topLevelItemCount();i++)  {
    Items << topLevelItem(i)               ;
  }                                        ;
  for (int i=0;i<Items.count();i++)        {
    takeItem(Items[i])                     ;
  }                                        ;
  //////////////////////////////////////////
  Alert ( Done  )                          ;
}

void N::PeopleMerge::run(int Type,ThreadData * data)
{ Q_UNUSED  ( data ) ;
  switch    ( Type ) {
    case 10001       :
      Merge (      ) ;
    break            ;
  }                  ;
}

bool N::PeopleMerge::Menu(QPoint pos)
{
  N::MenuManager mm ( this )                                  ;
  QTreeWidgetItem * item = itemAt  ( pos )                    ;
  QAction       * aa                                          ;
  mm   . add ( 101 , LocalIcons [ "Insert" ] , tr("New")    ) ;
  if (NotNull(item))                                          {
    mm . add ( 102 , LocalIcons [ "Delete" ] , tr("Delete") ) ;
  }                                                           ;
  if (topLevelItemCount()>1)                                  {
    mm . add ( 103 , LocalIcons [ "Merge"  ] , tr("Merge")  ) ;
  }                                                           ;
  mm   . add ( 104 , LocalIcons [ "Clear"  ] , tr("Clear")  ) ;
  mm   . addSeparator ( )                                     ;
  mm   . add ( 201 , tr("Deletion" ) , true , Deletion )      ;
  mm   . add ( 202 , tr("Execution") , true , Execute  )      ;
  mm . addSeparator (    )                                    ;
  DockingMenu       ( mm )                                    ;
  mm   . setFont(plan)                                        ;
  aa = mm.exec()                                              ;
  nKickOut ( IsNull(aa) , true )                              ;
  if ( RunDocking ( mm , aa ) ) return true                   ;
  switch ( mm [ aa ] )                                        {
    case 101                                                  :
      New    (       )                                        ;
    break                                                     ;
    case 102                                                  :
      Delete (       )                                        ;
    break                                                     ;
    case 103                                                  :
      start  ( 10001 )                                        ;
    break                                                     ;
    case 104                                                  :
      clear  (       )                                        ;
    break                                                     ;
    case 201                                                  :
      Deletion = Deletion ? false : true                      ;
    break                                                     ;
    case 202                                                  :
      Execute  = Execute  ? false : true                      ;
    break                                                     ;
  }                                                           ;
  return true                                                 ;
}
