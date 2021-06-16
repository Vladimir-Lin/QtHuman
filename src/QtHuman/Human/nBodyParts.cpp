#include <qthuman.h>

N::BodyParts:: BodyParts (QWidget * parent,Plan * p)
             : TreeDock  (          parent,       p)
             , organ     (0                        )
             , sorting   (Qt::AscendingOrder       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::BodyParts::~BodyParts(void)
{
}

QSize N::BodyParts::sizeHint(void) const
{
  return QSize ( 240 , 240 ) ;
}

bool N::BodyParts::FocusIn(void)
{
  AssignAction ( Label      , windowTitle ( ) ) ;
  LinkAction   ( Refresh    , List        ( ) ) ;
  LinkAction   ( Insert     , New         ( ) ) ;
  LinkAction   ( Paste      , Paste       ( ) ) ;
  LinkAction   ( Export     , Export      ( ) ) ;
  LinkAction   ( Copy       , Copy        ( ) ) ;
  LinkAction   ( SelectNone , SelectNone  ( ) ) ;
  LinkAction   ( SelectAll  , SelectAll   ( ) ) ;
  LinkAction   ( Font       , setFont     ( ) ) ;
  return true                                   ;
}

bool N::BodyParts::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::BodyParts::dropText(QWidget * source,QPointF pos,const QString & text)
{ Q_UNUSED ( source ) ;
  Q_UNUSED ( pos    ) ;
  Paste    ( text   ) ;
  return true         ;
}

void N::BodyParts::setHeader(QString title,QString head)
{
  NewTreeWidgetItem ( h        ) ;
  h -> setText      ( 0 , head ) ;
  setWindowTitle    ( title    ) ;
  assignHeaderItems ( h        ) ;
  plan->setFont     ( this     ) ;
}

void N::BodyParts::Configure(void)
{
  PassDragDrop = false                          ;
  ///////////////////////////////////////////////
  NewTreeWidgetItem       ( head              ) ;
  head -> setText         ( 0 , tr("Part")    ) ;
  setWindowTitle          ( tr("Parts")       ) ;
  setFocusPolicy          ( Qt::WheelFocus    ) ;
  setDragDropMode         ( DragDrop          ) ;
  setAlternatingRowColors ( true              ) ;
  setRootIsDecorated      ( false             ) ;
  setSelectionMode        ( ExtendedSelection ) ;
  setColumnCount          ( 1                 ) ;
  assignHeaderItems       ( head              ) ;
  MountClicked            ( 1                 ) ;
  MountClicked            ( 2                 ) ;
  setDropFlag             ( DropText , true   ) ;
  plan->setFont           ( this              ) ;
  ///////////////////////////////////////////////
  if (!plan->Booleans["Desktop"])               {
    allowGesture = true                         ;
    grabGesture ( Qt::TapAndHoldGesture )       ;
    grabGesture ( Qt::PanGesture        )       ;
  }                                             ;
}

UUIDs N::BodyParts::LoadUuids(SqlConnection & connection)
{
  return connection . Uuids                            (
           PlanTable(Body)                             ,
           "uuid"                                      ,
           QString ( "where organ = %1 %2"             )
           .arg ( organ                                )
           .arg ( connection.OrderBy("id",sorting) ) ) ;
}

void N::BodyParts::LoadNames(SqlConnection & connection,UUIDs & Uuids)
{
  int  index = 0                      ;
  SUID uuid  = 0                      ;
  QTreeWidgetItem * it                ;
  foreach (uuid,Uuids)                {
    QString name = connection.getName (
                     PlanTable(Names) ,
                     "uuid"           ,
                     vLanguageId      ,
                     uuid           ) ;
    it = addItem ( name , uuid , 0  ) ;
    setCheckable ( it               ) ;
    index++                           ;
    if (nMod(index,100))              {
      plan->processEvents()           ;
    }                                 ;
  }                                   ;
}

SUID N::BodyParts::AppendUuid(SqlConnection & connection)
{
  QString Q                                ;
  SUID    uuid                             ;
  uuid = connection.Unique                 (
           PlanTable(MajorUuid)            ,
           "uuid"                          ,
           3697                          ) ;
  connection.assureUuid                    (
    PlanTable(MajorUuid)                   ,
    uuid                                   ,
    Types::Body                          ) ;
  Q = connection.sql.InsertInto            (
        PlanTable(Body)                    ,
        2                                  ,
        "uuid"                             ,
        "organ"                          ) ;
  connection . Prepare ( Q               ) ;
  connection . Bind    ( "uuid"  , uuid  ) ;
  connection . Bind    ( "organ" , organ ) ;
  connection . Exec    (                 ) ;
  return uuid                              ;
}

bool N::BodyParts::AppendName(SqlConnection & connection,SUID uuid,QString name)
{
  return connection.assureName(PlanTable(Names),uuid,vLanguageId,name) ;
}

QTreeWidgetItem * N::BodyParts::AppendItem (void)
{
  NewTreeWidgetItem ( IT                   ) ;
  IT->setData       ( 0 , Qt::UserRole , 0 ) ;
  addTopLevelItem   ( IT                   ) ;
  setCheckable      ( IT                   ) ;
  return IT                                  ;
}

void N::BodyParts::setCheckable(QTreeWidgetItem * item)
{ Q_UNUSED ( item ) ;
}

bool N::BodyParts::List(void)
{
  VirtualGui::Mutex.lock  ()        ;
  clear       ()                    ;
  setEnabled (false)                ;
  SqlConnection SC(plan->sql)       ;
  if (SC.open("OrganLists","List")) {
    UUIDs Uuids = LoadUuids(SC)     ;
    LoadNames(SC,Uuids)             ;
    SC.close()                      ;
  }                                 ;
  SC.remove()                       ;
  setEnabled (true)                 ;
  VirtualGui::Mutex.unlock()        ;
  Alert ( Done )                    ;
  return true                       ;
}

void N::BodyParts::singleClicked(QTreeWidgetItem * item,int column)
{
}

void N::BodyParts::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * le = NULL                  ;
  switch (column)                        {
    case 0                               :
      le = setLineEdit                   (
             item                        ,
             0                           ,
             SIGNAL(returnPressed  ())   ,
             SLOT  (editingFinished()) ) ;
      le -> setFocus(Qt::TabFocusReason) ;
    break                                ;
  }                                      ;
}

void N::BodyParts::New(void)
{
  QTreeWidgetItem * IT = NULL ;
  removeOldItem   (         ) ;
  IT = AppendItem (         ) ;
  scrollToItem    ( IT      ) ;
  doubleClicked   ( IT , 0  ) ;
}

void N::BodyParts::removeOldItem(void)
{
  TreeWidget :: removeOldItem ( true , 0 ) ;
}

void N::BodyParts::editingFinished(void)
{
  SUID uuid = nTreeUuid(ItemEditing,ItemColumn)     ;
  QLineEdit * name = Casting(QLineEdit,ItemWidget)  ;
  if (NotNull(name))                                {
    QString task = name->text()                     ;
    if (task.length()>0)                            {
      VirtualGui::Mutex.lock  ()                    ;
      SqlConnection SC(plan->sql)                   ;
      if (SC.open("OrganLists","editingFinished"))  {
        if (uuid<=0) uuid = AppendUuid(SC)          ;
        if (uuid>0 && AppendName(SC,uuid,task))     {
          ItemEditing->setData(0,Qt::UserRole,uuid) ;
          ItemEditing->setText(0,task)              ;
          emit Update ( this , uuid )               ;
        }                                           ;
        SC.close()                                  ;
      }                                             ;
      SC.remove()                                   ;
      VirtualGui::Mutex.unlock()                    ;
    }                                               ;
  }                                                 ;
  removeOldItem ( )                                 ;
}

void N::BodyParts::Paste(QString text)
{
  if (text.length()<=0) return               ;
  QStringList sl = text.split("\n")          ;
  VirtualGui::Mutex.lock  ()                 ;
  SqlConnection SC(plan->sql)                ;
  if (SC.open("nItemEditor","Paste"))        {
    for (int i=0;i<sl.count();i++)           {
      QString s = sl[i]                      ;
      s = s.replace("\r","")                 ;
      s = s.replace("\n","")                 ;
      if (s.length()>0)                      {
        SUID uuid = AppendUuid(SC)           ;
        if (uuid>0 && AppendName(SC,uuid,s)) {
          addItem (s,uuid,0)                 ;
        }                                    ;
      }                                      ;
    }                                        ;
    SC.close()                               ;
  }                                          ;
  SC.remove()                                ;
  VirtualGui::Mutex.unlock()                 ;
}

void N::BodyParts::Paste(void)
{
  QString text = qApp->clipboard()->text() ;
  Paste(text) ;
}

void N::BodyParts::Export(void)
{
  QString filename = QFileDialog::getSaveFileName(this) ;
  if (filename.length()<=0) return ;
  QFile F(filename) ;
  if (!F.open(QIODevice::WriteOnly)) return;
  for (int i=0;i<topLevelItemCount();i++) {
    QString n = topLevelItem(i)->text(0) ;
    n += "\r\n" ;
    F.write(n.toUtf8()) ;
  };
  F.close() ;
}

void N::BodyParts::Copy(void)
{
  QStringList s;
  for (int i=0;i<topLevelItemCount();i++)
    if (topLevelItem(i)->isSelected()) s << topLevelItem(i)->text(0) ;
  QString v = s.join("\r\n") ;
  qApp->clipboard()->setText(v) ;
}

void N::BodyParts::Translation(void)
{
  UUIDs Uuids = itemUuids ( 0 )              ;
  emit Translations ( windowTitle() , Uuids) ;
}

bool N::BodyParts::Menu(QPoint pos)
{
  QMdiSubWindow *   mdi    = Casting(QMdiSubWindow,parent())       ;
  QDockWidget   *   dock   = Casting(QDockWidget  ,parent())       ;
  MenuManager       mm (this)                                      ;
  QMenu         *   me                                             ;
  QMenu         *   ma                                             ;
  QMenu         *   ms                                             ;
  QMenu         *   mc                                             ;
  QAction       *   a                                              ;
  mm . add ( 101 , tr("Refresh") )                                 ;
  mm . add ( 102 , tr("Insert" ) )                                 ;
  mm . add ( 103 , tr("Export" ) )                                 ;
  mm . add ( 104 , tr("Paste"  ) )                                 ;
  mm . add ( 105 , tr("Copy"   ) )                                 ;
  mm . addSeparator (                   )                          ;
  me = mm . addMenu ( tr("Edit"       ) )                          ;
  ma = mm . addMenu ( tr("Adjustments") )                          ;
  ms = mm . addMenu ( tr("Sorting"    ) )                          ;
  mc = mm . addMenu ( tr("Selections" ) )                          ;
  AdjustMenu     ( mm , ma )                                       ;
  SortingMenu    ( mm , ms )                                       ;
  SelectionsMenu ( mm , mc )                                       ;
  mm . addSeparator ( ma                     ) ;
  mm . add          ( ma , 201 , tr("Multilingual translations") ) ;
  if (plan->Booleans["Desktop"])                                   {
    mm . addSeparator ( )                                          ;
    nIfSafe(dock) mm.add(1000001,tr("Move to window area"))        ;
    nIfSafe(mdi ) mm.add(1000002,tr("Move to dock area"  ))        ;
  }                                                                ;
  mm . setFont(plan)                                               ;
  if (plan->Booleans["Desktop"])                                   {
    pos = QCursor::pos()                                           ;
  } else                                                           {
    pos = mapToGlobal ( pos )                                      ;
  }                                                                ;
  a = mm.exec(pos)                                                 ;
  if (!plan->Booleans["Desktop"])                                  {
    allowGesture = true                                            ;
  }                                                                ;
  if (IsNull(a)) return true                                       ;
  switch (mm[a])                                                   {
    nFastCast ( 101 , List   ( ) )                                 ;
    nFastCast ( 102 , New    ( ) )                                 ;
    nFastCast ( 103 , Export ( ) )                                 ;
    nFastCast ( 104 , Paste  ( ) )                                 ;
    nFastCast ( 105 , Copy   ( ) )                                 ;
    nFastCast ( 201 , Translation ( ) )                            ;
    case 1000001                                                   :
      emit attachMdi (this,Qt::Vertical)                           ;
    break                                                          ;
    case 1000002                                                   :
      emit attachDock                                              (
        this                                                       ,
        windowTitle()                                              ,
        Qt::RightDockWidgetArea                                    ,
        Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea           ) ;
    break                                                          ;
    default                                                        :
      if (RunAdjustment(mm,a)) return true                         ;
      if (RunSorting   (mm,a)) return true                         ;
      if (RunSelections(mm,a)) return true                         ;
    return true                                                    ;
  }                                                                ;
  return true                                                      ;
}
