#include <qthuman.h>

N::EyesView:: EyesView      ( QWidget * parent , Plan * p )
            : ListDock      (           parent ,        p )
            , PeopleManager (                           p )
            , dropAction    ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::EyesView::~EyesView(void)
{
}

void N::EyesView::Configure(void)
{
  setWindowTitle               ( tr("Eye colors")     ) ;
  setViewMode                  (IconMode              ) ;
  setIconSize                  (QSize(64,64)          ) ;
  setGridSize                  (QSize(80,96)          ) ;
  setMovement                  (Snap                  ) ;
  setResizeMode                (Adjust                ) ;
  setSelectionMode             (SingleSelection       ) ;
  setWordWrap                  (true                  ) ;
  setMouseTracking             (true                  ) ;
  setWrapping                  (true                  ) ;
  setTextElideMode             (Qt::ElideNone         ) ;
  setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   (Qt::ScrollBarAsNeeded ) ;
  setMinimumSize               (QSize(96,100)         ) ;
  setDragDropMode              (DragDrop              ) ;
  setDropFlag                  (DropTag      , true   ) ;
  setDropFlag                  (DropPicture  , true   ) ;
  setDropFlag                  (DropPeople   , true   ) ;
  plan -> setFont              (this                  ) ;
  if (plan->Booleans["Phone"])                          {
    setViewMode ( QListView::ListMode )                 ;
  }                                                     ;
}

bool N::EyesView::FocusIn(void)
{
  AssignAction ( Label   , windowTitle ( ) ) ;
  LinkAction   ( Refresh , startup     ( ) ) ;
  LinkAction   ( Copy    , Copy        ( ) ) ;
  return true                                ;
}

QMimeData * N::EyesView::dragMime(void)
{
  QListWidgetItem * IT = currentItem()            ;
  if (IsNull(IT)) return NULL                     ;
  SUID  uuid  = nListUuid(IT)                     ;
  QMimeData * mime = new QMimeData()              ;
  setMime ( mime , "eyes/uuid"  , uuid )          ;
  if (NotNull(IT))                                {
    QIcon icon  = IT->icon()                      ;
    if (!icon.isNull())                           {
      QSize  s     = iconSize()                   ;
      QImage image = icon . pixmap(s) . toImage() ;
      if (!image.isNull())                        {
        mime -> setImageData(image)               ;
      }                                           ;
    }                                             ;
    mime -> setText ( IT->text() )                ;
  }                                               ;
  return mime                                     ;
}

bool N::EyesView::hasItem(void)
{
  QListWidgetItem * item = currentItem() ;
  return NotNull ( item )                ;
}

bool N::EyesView::startDrag(QMouseEvent * event)
{
  QListWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return false                                               ;
}

bool N::EyesView::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::EyesView::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::EyesView::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::EyesView::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

QString N::EyesView::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType         (
           mime                        ,
           "picture/uuid;picture/uuids;"
           "tag/uuid;tag/uuids;"
           "people/uuid;people/uuids"
         )                             ;
}

UUIDs N::EyesView::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="picture/uuid")          {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="picture/uuids")         {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="tag/uuid")              {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="tag/uuids")             {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="people/uuid")           {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="people/uuids")          {
    Uuids  = GetUuids ( data )           ;
  }                                      ;
  return Uuids                           ;
}

bool N::EyesView::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  QString    mtype                  ;
  UUIDs      Uuids                  ;
  if (source==this) return false    ;
  mtype = MimeType  (mime      )    ;
  Uuids = MimeUuids (mime,mtype)    ;
  if (mtype=="picture/uuid"        ||
      mtype=="picture/uuids"        )
    plan->showMessage               (
      tr("Assign %1 icon from %2"   )
      .arg(Uuids.count()            )
      .arg(source->windowTitle()) ) ;
  if (mtype=="tag/uuid"            ||
      mtype=="tag/uuids"            )
    plan->showMessage               (
      tr("Assign %1 tags to %2"     )
      .arg(Uuids.count()            )
      .arg(source->windowTitle()) ) ;
  if (mtype=="people/uuid"         ||
      mtype=="people/uuids"         )
    plan->showMessage               (
    tr("Copy %1 people from %2"     )
      .arg(Uuids.count()            )
      .arg(source->windowTitle()) ) ;
  return true                       ;
}

bool N::EyesView::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false                                ;
  if (source==this)                                           {
    QListWidgetItem * atItem = itemAt(pos)                    ;
    if (IsNull (atItem)) return false                         ;
    if (NotNull(atItem) && atItem->isSelected()) return false ;
  }                                                           ;
  return true                                                 ;
}

bool N::EyesView::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false             ;
  return dropItems ( source , mime , pos ) ;
}

bool N::EyesView::startup(void)
{
  clear ( )                                                              ;
  GroupItems     GI ( plan )                                             ;
  PictureManager PM ( plan )                                             ;
  EnterSQL ( SC , plan->sql )                                            ;
    QString name                                                         ;
    SUID    uuid                                                         ;
    UUIDs   Uuids = SC.Uuids(PlanTable(Eyes),"uuid",SC.OrderByAsc("id")) ;
    foreach (uuid,Uuids)                                                 {
      QIcon icon                                                         ;
      name = Name(SC,uuid,vLanguageId)                                   ;
      SUID puid = GI.FindSecond                                          (
        SC                                                               ,
        uuid                                                             ,
        Types  :: Eyes                                                   ,
        Types  :: Picture                                                ,
        Groups :: Icon                                                   ,
        "order by position asc limit 0,1"                              ) ;
      QImage * image   = PM.Thumb  (SC,puid)                             ;
      if (NotNull(image))                                                {
        icon = PM.Icon(image,QSize(64,64))                               ;
        delete image                                                     ;
        image = NULL                                                     ;
      } else                                                             {
        icon = QIcon(":/images/eyeshadows.png")                          ;
      }                                                                  ;
      addItem ( uuid , icon , name , name )                              ;
    }                                                                    ;
  LeaveSQL ( SC , plan->sql )                                            ;
  Alert ( Done )                                                         ;
  return true                                                            ;
}

void N::EyesView::Copy(void)
{
  QMimeData * mime = dragMime    (      ) ;
  if (IsNull(mime)) return                ;
  qApp->clipboard()->setMimeData ( mime ) ;
}

bool N::EyesView::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                                      ;
  QMdiSubWindow   * mdi    = Casting(QMdiSubWindow,parent())         ;
  QDockWidget     * dock   = Casting(QDockWidget  ,parent())         ;
  QListWidgetItem * item   = itemAt(pos)                             ;
  QPoint            global = mapToGlobal(pos)                        ;
  QAction       * a                                                  ;
  if (NotNull(item))                                                 {
    mm.add(101,tr("People" ))                                        ;
    mm.add(102,tr("Gallery"))                                        ;
  }                                                                  ;
  if (!plan->Booleans["Phone"])                                      {
    if (viewMode()==QListView::IconMode) mm.add(301,tr("List view")) ;
    if (viewMode()==QListView::ListMode) mm.add(302,tr("Icon view")) ;
  }                                                                  ;
  nIfSafe(dock) mm.add(1000001,tr("Move to window area"))            ;
  nIfSafe(mdi ) mm.add(1000002,tr("Move to dock area"  ))            ;
  mm . setFont ( plan )                                              ;
  a = mm.exec(global)                                                ;
  if (IsNull(a)) return true                                         ;
  switch (mm[a])                                                     {
    case 101                                                         :
      emit People  ( nListUuid(item) , item->text() )                ;
    break                                                            ;
    case 102                                                         :
      emit Gallery ( item->text() , nListUuid(item) )                ;
    break                                                            ;
    case 301                                                         :
      setViewMode  ( QListView::ListMode )                           ;
    break                                                            ;
    case 302                                                         :
      setViewMode  ( QListView::IconMode )                           ;
    break                                                            ;
    case 1000001                                                     :
      emit attachMdi (this,Qt::Vertical)                             ;
    break                                                            ;
    case 1000002                                                     :
      emit attachDock                                                (
        this                                                         ,
        windowTitle()                                                ,
        Qt::RightDockWidgetArea                                      ,
        Qt::TopDockWidgetArea                                        |
        Qt::BottomDockWidgetArea                                     |
        Qt::LeftDockWidgetArea                                       |
        Qt::RightDockWidgetArea                                    ) ;
    break                                                            ;
    default                                                          :
    break                                                            ;
  }                                                                  ;
  return true                                                        ;
}

bool N::EyesView::dropTags(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return true          ;
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (IsNull(atItem)) return true        ;
  SUID eyes = nListUuid(atItem)          ;
  dropAction = true                      ;
  UUIDs Puids                            ;
  Puids << eyes                          ;
  GroupItems GI ( plan           )       ;
  EnterSQL      ( SC , plan->sql )       ;
    SUID uuid                            ;
    foreach (uuid,Uuids)                 {
      GI.Join                            (
        SC                               ,
        uuid                             ,
        Types::Tag                       ,
        Types::Eyes                      ,
        Groups::Subordination            ,
        0                                ,
        Puids                          ) ;
    }                                    ;
  LeaveSQL      ( SC , plan->sql )       ;
  Alert         ( Done           )       ;
  dropAction = false                     ;
  return true                            ;
}

bool N::EyesView::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return true          ;
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (IsNull(atItem)) return true        ;
  if (Uuids.count()<=0) return true      ;
  SUID eyes  = nListUuid(atItem)         ;
  SUID puid  = Uuids[0]                  ;
  dropAction = true                      ;
  EnterSQL(SC,plan->sql)                 ;
    QString Q                            ;
    Q = SC.sql.DeleteFrom                (
          plan->Tables[Tables::Groups]   ,
          QString("where first = %1 and t1 = %2 and t2 = %3 and relation = %4")
          .arg(eyes                      )
          .arg(Types::Eyes               )
          .arg(Types::Picture            )
          .arg(Groups::Icon              )
        )                                ;
    SC.Query(Q)                          ;
    Q = SC.sql.InsertInto                (
          plan->Tables[Tables::Groups]   ,
          5                              ,
          "first"                        ,
          "second"                       ,
          "t1"                           ,
          "t2"                           ,
          "relation"                   ) ;
    SC.Prepare ( Q                     ) ;
    SC.Bind ("first"   ,eyes           ) ;
    SC.Bind ("second"  ,puid           ) ;
    SC.Bind ("t1"      ,Types::Eyes    ) ;
    SC.Bind ("t2"      ,Types::Picture ) ;
    SC.Bind ("relation",Groups::Icon   ) ;
    SC.Exec (                          ) ;
  LeaveSQL(SC,plan->sql)                 ;
  Alert ( Done )                         ;
  dropAction = false                     ;
  startup()                              ;
  return true                            ;
}

bool N::EyesView::dropPeople(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (source==this) return true          ;
  if (IsNull(atItem)) return true        ;
  dropAction = true                      ;
  SUID  auid  = nListUuid(atItem)        ;
  UUIDs AUIDs = Uuids                    ;
  GroupItems GI(plan)                    ;
  EnterSQL(SC,plan->sql)                 ;
    GI.Join                              (
      SC , auid                          ,
      Types::Eyes                        ,
      Types::People                      ,
      Groups::Subordination              ,
      0 , AUIDs                        ) ;
  LeaveSQL(SC,plan->sql)                 ;
  Alert ( Done )                         ;
  dropAction = false                     ;
  return true                            ;
}
