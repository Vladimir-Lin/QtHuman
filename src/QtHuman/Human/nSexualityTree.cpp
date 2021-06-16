#include <qthuman.h>

N::SexualityTree:: SexualityTree (QWidget * parent,Plan * p)
                 : TreeDock      (          parent,       p)
                 , PeopleManager (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SexualityTree::~SexualityTree (void)
{
}

void N::SexualityTree::Configure(void)
{
  PassDragDrop = false                                       ;
  setFocusPolicy          (Qt::WheelFocus                  ) ;
  setDragDropMode         (DragDrop                        ) ;
  setAlternatingRowColors (true                            ) ;
  setRootIsDecorated      (false                           ) ;
  setSelectionMode        (SingleSelection                 ) ;
  setColumnCount          (1                               ) ;
  ////////////////////////////////////////////////////////////
  setWindowTitle          (tr("Human biologic sexualities")) ;
  setHeaderHidden         (true                            ) ;
  plan -> setFont         (this                            ) ;
}

bool N::SexualityTree::FocusIn(void)
{
  AssignAction ( Label   , windowTitle     ( ) ) ;
  LinkAction   ( Refresh , List            ( ) ) ;
  LinkAction   ( Export  , Export          ( ) ) ;
  LinkAction   ( Copy    , CopyToClipboard ( ) ) ;
  return true                                    ;
}

QMimeData * N::SexualityTree::dragMime(void)
{
  QTreeWidgetItem * it = currentItem()               ;
  if (IsNull(it)) return NULL                        ;
  QMimeData * mime = new QMimeData()                 ;
  SUID        type = nTreeUuid(it,0)                 ;
  QByteArray  data((const char *)&type,sizeof(SUID)) ;
  mime->setData("gender/uuid",data)                  ;
  return mime                                        ;
}

QString N::SexualityTree::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType(mime,"people/uuid;people/uuids") ;
}

UUIDs N::SexualityTree::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs Uuids                            ;
  if (mimetype.length()<=0) return Uuids ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="people/uuid")           {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="people/uuids")          {
    Uuids = GetUuids(data)               ;
  }                                      ;
  return Uuids                           ;
}

bool N::SexualityTree::acceptDrop (QWidget * source,const QMimeData * mime)
{
  if (source==this) return false      ;
  QString    mtype                    ;
  UUIDs      Uuids                    ;
  mtype = MimeType  (mime      )      ;
  if (mtype.length()<=0) return false ;
  return true                         ;
}

bool N::SexualityTree::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (source==this) return false     ;
  QString    mtype                   ;
  UUIDs      Uuids                   ;
  mtype = MimeType  (mime      )     ;
  Uuids = MimeUuids (mime,mtype)     ;
  if (Uuids.count()<=0) return false ;
  Alert ( Action )                   ;
  return true                        ;
}

bool N::SexualityTree::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (source==this   ) return false      ;
  if (mime->hasUrls()) return false      ;
  QTreeWidgetItem * atItem = itemAt(pos) ;
  QString    mtype                       ;
  UUIDs      Uuids                       ;
  mtype = MimeType  (mime      )         ;
  Uuids = MimeUuids (mime,mtype)         ;
  if (Uuids.count()<=0) return false     ;
  if (NotNull(atItem))                   {
    plan->showMessage                    (
      tr("%1 people designated as %2"    )
      .arg(Uuids.count()                 )
      .arg(atItem->text(0)))             ;
  }                                      ;
  return true                            ;
}

bool N::SexualityTree::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  setCursor(Qt::ArrowCursor)                          ;
  if (source==this) return false                      ;
  QTreeWidgetItem * atItem = itemAt(pos)              ;
  if (IsNull(atItem)) return false                    ;
  SUID suid = nTreeUuid(atItem,0)                     ;
  QString name = atItem->text(0)                      ;
  if (suid<=0) return false                           ;
  QString mtype                                       ;
  UUIDs   Uuids                                       ;
  setEnabled (false)                                  ;
  source->setEnabled(false)                           ;
  DoProcessEvents                                     ;
  mtype = MimeType  (mime      )                      ;
  Uuids = MimeUuids (mime,mtype)                      ;
  if (Uuids.count()>0) JoinSexuality(name,suid,Uuids) ;
  DoProcessEvents                                     ;
  source->setEnabled(true)                            ;
  setEnabled (true )                                  ;
  DoProcessEvents                                     ;
  Alert ( Done )                                      ;
  return true                                         ;
}

bool N::SexualityTree::removeDrop(void)
{
  return true ;
}

QString N::SexualityTree::Gender(SUID uuid)
{
  QString title = ""                               ;
  for (int i=0                                     ;
       title.length()<=0 && i<topLevelItemCount()  ;
       i++                                       ) {
    QTreeWidgetItem * item                         ;
    item = topLevelItem(i)                         ;
    if (NotNull(item))                             {
      SUID suid = nTreeUuid ( item , 0 )           ;
      if (nEqual(suid,uuid)) title = item->text(0) ;
    }                                              ;
  }                                                ;
  return title                                     ;
}

void N::SexualityTree::List (void)
{
  AbstractGui::Mutex.lock  ()                  ;
  clear       ()                               ;
  setEnabled (false)                           ;
  SqlConnection SC ( plan->sql  )              ;
  if (SC.open("nSexualityTree","List"))        {
    UUIDs Uuids = HumanSexualities(SC)         ;
    SUID  uuid                                 ;
    foreach (uuid,Uuids)                       {
      QString name = Name(SC,uuid,vLanguageId) ;
      addItem ( name , uuid )                  ;
    }                                          ;
    SC.close()                                 ;
  }                                            ;
  SC.remove()                                  ;
  setEnabled (true)                            ;
  AbstractGui::Mutex.unlock()                  ;
}

void N::SexualityTree::GetDocuments(QTreeWidgetItem * item)
{
  nDropOut                  ( IsNull(item) ) ;
  QString name = item->text ( 0            ) ;
  SUID    uuid = nTreeUuid  ( item , 0     ) ;
  emit Documents            ( name , uuid  ) ;
}

void N::SexualityTree::GetPeople(QTreeWidgetItem * item)
{
  nDropOut                  ( IsNull(item) ) ;
  QString name = item->text ( 0            ) ;
  SUID    uuid = nTreeUuid  ( item , 0     ) ;
  UUIDs   Uuids                              ;
  SqlConnection SC ( plan->sql  )            ;
  if (SC.open("nSexualityTree","List"))      {
    Uuids = SexualityCrowd(SC,uuid)          ;
    SC.close()                               ;
  }                                          ;
  SC.remove()                                ;
  emit ListPeople ( name , uuid , Uuids )    ;
}

void N::SexualityTree::JoinSexuality(QString name,SUID sexuality,UUIDs & People)
{
  AbstractGui::Mutex.lock  ()                    ;
  plan->showMessage                              (
    tr("%1 people designated as %2"              )
    .arg(People.count()                          )
    .arg(name))                                  ;
  SqlConnection SC ( plan->sql )                 ;
  if (SC.open("nSexualityTree","JoinSexuality")) {
    QProgressBar * P                             ;
    P  = plan->Progress(tr("%v/%m people"))      ;
    P -> setRange(0,People.count())              ;
    P -> setTextVisible(true)                    ;
    DoProcessEvents                              ;
    for (int i=0;i<People.count();i++)           {
      SUID uuid = People [ i ]                   ;
      P -> setValue        ( i+1 )               ;
      DoProcessEvents                            ;
      AssignSexuality ( SC , uuid , sexuality )  ;
    }                                            ;
    P -> hide        ( )                         ;
    P -> deleteLater ( )                         ;
    SC.close()                                   ;
  }                                              ;
  SC.remove()                                    ;
  AbstractGui::Mutex.unlock()                    ;
}

void N::SexualityTree::Export(void)
{
  QString filename = QFileDialog::getSaveFileName         (
                       this                               ,
                       tr("Export sexualities into file") ,
                       plan->Temporary("")                ,
                       tr("Plain text file(*.txt)")     ) ;
  nDropOut ( filename.length() <= 0 )                     ;
  /////////////////////////////////////////////////////////
  QStringList L = columnStrings ( 0 )                     ;
  nDropOut ( L.count() <= 0 )                             ;
  /////////////////////////////////////////////////////////
  QString    M = L.join("\r\n")                           ;
  QByteArray B = M.toUtf8()                               ;
  QFile      F(filename)                                  ;
  if (F.open(QIODevice::WriteOnly))                       {
    F . write ( B     )                                   ;
    F . close (       )                                   ;
    Alert     ( Done  )                                   ;
  } else                                                  {
    Alert     ( Error )                                   ;
  }                                                       ;
}

bool N::SexualityTree::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                   ;
  QAction * aa                                ;
  QTreeWidgetItem * item = currentItem ( )    ;
  mm . add     ( 101 , tr("Refresh")     )    ;
  nIfSafe(item)                               {
    mm . add   ( 301 , tr("Documents"     ) ) ;
    mm . add   ( 302 , tr("Related people") ) ;
  }                                           ;
  mm . setFont ( plan                       ) ;
  if (plan->Booleans["Desktop"])              {
    pos = QCursor::pos()                      ;
  } else                                      {
    pos = mapToGlobal ( pos )                 ;
  }                                           ;
  aa = mm.exec      ( pos                   ) ;
  if (!plan->Booleans["Desktop"])             {
    allowGesture = true                       ;
  }                                           ;
  nKickOut    ( IsNull(aa) , true           ) ;
  switch (mm[aa])                             {
    nFastCast ( 101 , List         (      ) ) ;
    nFastCast ( 301 , GetDocuments ( item ) ) ;
    nFastCast ( 302 , GetPeople    ( item ) ) ;
  }                                           ;
  return true                                 ;
}
