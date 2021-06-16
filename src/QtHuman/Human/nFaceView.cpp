#include <qthuman.h>

#define TABLES(ID) plan->Tables[Tables::ID]

N::FaceView:: FaceView       ( QWidget * parent , Plan * p )
            : ListDock       (           parent ,        p )
            , PictureManager (                           p )
            , Group          ( 0                           )
            , SourceType     ( Types::People               )
            , Relationship   ( Groups::Subordination       )
            , Operation      ( 0                           )
            , Method         ( 0                           )
            , StartId        ( 0                           )
            , Limit          ( 72                          )
            , TotalFaces     ( 0                           )
            , loadingFaces   ( false                       )
            , dropAction     ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::FaceView::~FaceView(void)
{
}

void N::FaceView::Configure(void)
{
  StackSize   = 0                                               ;
  Reservation = false                                           ;
  ///////////////////////////////////////////////////////////////
  setWindowTitle               (tr("Human faces")     )         ;
  setAccessibleName            ("nFaceView"           )         ;
  setViewMode                  (IconMode              )         ;
  setIconSize                  (QSize(128,128)        )         ;
  setGridSize                  (QSize(140,140)        )         ;
  setMovement                  (Snap                  )         ;
  setResizeMode                (Adjust                )         ;
  setSelectionMode             (ExtendedSelection     )         ;
  setWordWrap                  (true                  )         ;
  setMouseTracking             (true                  )         ;
  setWrapping                  (true                  )         ;
  setTextElideMode             (Qt::ElideNone         )         ;
  setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded )         ;
  setVerticalScrollBarPolicy   (Qt::ScrollBarAsNeeded )         ;
  setMinimumSize               (QSize(144,180)        )         ;
  setDragDropMode              (DragDrop              )         ;
  setDropFlag                  (DropFace     , true   )         ;
  setDropFlag                  (DropPeople   , true   )         ;
  plan->setFont                (this                  )         ;
  ///////////////////////////////////////////////////////////////
  connect(this,SIGNAL(setIcon    (QImage*,QListWidgetItem*))    ,
          this,SLOT  (setItemIcon(QImage*,QListWidgetItem*))  ) ;
  ///////////////////////////////////////////////////////////////
  int id                                                        ;
  KeyIDs << addShortcut(Qt::Key_Home    ,this,SLOT(Home    ())) ;
  KeyIDs << addShortcut(Qt::Key_End     ,this,SLOT(End     ())) ;
  KeyIDs << addShortcut(Qt::Key_PageUp  ,this,SLOT(PageUp  ())) ;
  KeyIDs << addShortcut(Qt::Key_PageDown,this,SLOT(PageDown())) ;
  foreach (id,KeyIDs) Shortcuts[id-1]->setEnabled(false)        ;
}

QListWidgetItem * N::FaceView::uuidAt(SUID uuid)
{
  for (int i=0;i<count();i++)      {
    QListWidgetItem * it = item(i) ;
    SUID suid = nListUuid (it)     ;
    if (suid==uuid) return it      ;
  }                                ;
  return NULL                      ;
}

QMimeData * N::FaceView::dragMime(void)
{
  if (Items.count()<=0) return NULL               ;
  QMimeData       * mime    = new QMimeData ( )   ;
  QListWidgetItem * current = currentItem   ( )   ;
  if (Items.count()==1)                           {
    SUID        type = nListUuid(Items[0])        ;
    QByteArray  data = CreateByteArray(type)      ;
    mime -> setData ("face/uuid",data)            ;
  } else                                          {
    UUIDs       Uuids = Selected        (       ) ;
    QByteArray  data  = CreateByteArray ( Uuids ) ;
    mime -> setData ("face/uuids",data)           ;
  }                                               ;
  if (NotNull(current))                           {
    QIcon  icon  = current->icon()                ;
    if (!icon.isNull())                           {
      QSize  s     = iconSize()                   ;
      QImage image = icon . pixmap(s) . toImage() ;
      if (!image.isNull())                        {
        mime -> setImageData(image)               ;
      }                                           ;
    }                                             ;
  }                                               ;
  QString tooltip                                 ;
  tooltip = tr("%1 faces selected"                )
            .arg(Items.count()                  ) ;
  QToolTip :: showText ( QCursor::pos(),tooltip ) ;
  Alert        ( Click   )                        ;
  plan -> Talk ( tooltip )                        ;
  return mime                                     ;
}

UUIDs N::FaceView::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="face/uuid")             {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="face/uuids")            {
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

bool N::FaceView::hasItem(void)
{
  return ( Items.count()>0 ) ;
}

bool N::FaceView::startDrag(QMouseEvent * event)
{
  Items.clear()                                                 ;
  QListWidgetItem * atItem = itemAt(event->pos())               ;
  nKickOut ( IsNull(atItem)                           , false ) ;
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false ) ;
  dragPoint = event->pos    ( )                                 ;
  Items     = selectedItems ( )                                 ;
  nKickOut ( !PassDragDrop                            , true  ) ;
  return false                                                  ;
}

bool N::FaceView::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false ) ;
  QPoint pos = event->pos()                                     ;
  pos -= dragPoint                                              ;
  return ( pos.manhattanLength() > qApp->startDragDistance() )  ;
}

void N::FaceView::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::FaceView::finishDrag(QMouseEvent * event)
{
  if (Items.count()>0) {
    Items.clear()      ;
    return false       ;
  }                    ;
  Items.clear()        ;
  return true          ;
}

bool N::FaceView::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( Group <= 0 , false ) ;
  return dropHandler ( mime )     ;
}

bool N::FaceView::removeDrop(void)
{
  return true ;
}

bool N::FaceView::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut ( Group <= 0 , false )          ;
  QString    mtype                         ;
  UUIDs      Uuids                         ;
  bool       dropIn = false                ;
  mtype = AbstractGui::MimeType            (
            mime                           ,
            "face/uuid;face/uuids;"
            "people/uuid;people/uuids"   ) ;
  if (mtype=="face/uuid"   ) dropIn = true ;
  if (mtype=="face/uuids"  ) dropIn = true ;
  if (mtype=="people/uuid" ) dropIn = true ;
  if (mtype=="people/uuids") dropIn = true ;
  nKickOut ( !dropIn , false )             ;
  Uuids = MimeUuids (mime,mtype)           ;
  nKickOut ( Uuids.count() <= 0 , false )  ;
  return true                              ;
}

bool N::FaceView::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut ( dropAction , false )                             ;
  if (source==this)                                           {
    nKickOut ( Group <= 0 , false )                           ;
    QListWidgetItem * atItem = itemAt(pos)                    ;
    nKickOut ( IsNull (atItem) , false )                      ;
    if (NotNull(atItem) && atItem->isSelected()) return false ;
  }                                                           ;
  return true                                                 ;
}

bool N::FaceView::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  nKickOut ( dropAction , false )          ;
  return dropItems ( source , mime , pos ) ;
}

bool N::FaceView::dropPeople(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  QPoint pos = psf.toPoint()         ;
  QListWidgetItem * it = itemAt(pos) ;
  nKickOut ( IsNull(it) , false )    ;
  SUID uuid = nListUuid(it)          ;
  nKickOut ( uuid <= 0  , false )    ;
  return assignPeople(uuid,Uuids)    ;
}

bool N::FaceView::dropFaces(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  nKickOut ( Group <= 0 , false )         ;
  QPoint pos = psf.toPoint()              ;
  if (source==this)                       {
    QListWidgetItem * it = itemAt(pos)    ;
    nKickOut ( IsNull(it) , false )       ;
    SUID uuid = nListUuid(it)             ;
    nKickOut ( uuid <= 0  , false )       ;
    return MoveFaces(uuid,Uuids)          ;
  } else                                  {
    QListWidgetItem * it = itemAt(pos)    ;
    SUID uuid = 0                         ;
    if (NotNull(it)) uuid = nListUuid(it) ;
    return JoinFaces(uuid,Uuids)          ;
  }                                       ;
  return false                            ;
}

bool N::FaceView::FocusIn(void)
{
  if (KeyIDs.count()>0)                     {
    int id                                  ;
    foreach (id,KeyIDs)                     {
      Shortcuts[id-1]->setEnabled(true)     ;
    }                                       ;
  }                                         ;
  if (Group>0)                              {
    LinkAction ( Delete,DeleteFaces() )     ;
  }                                         ;
  LinkAction ( Refresh    , startup    () ) ;
  LinkAction ( SelectAll  , SelectAll  () ) ;
  LinkAction ( SelectNone , SelectNone () ) ;
  return true                               ;
}

bool N::FaceView::FocusOut(void)
{
  if (KeyIDs.count()>0)                  {
    int id                               ;
    foreach (id,KeyIDs)                  {
      Shortcuts[id-1]->setEnabled(false) ;
    }                                    ;
  }                                      ;
  return true                            ;
}

int N::FaceView::Total(void)
{
  return TotalFaces ;
}

void N::FaceView::setItemIcon(QImage * image,QListWidgetItem * it)
{
  QSize is = iconSize()                            ;
  QIcon icon = PictureManager::Icon ( image , is ) ;
  delete image                                     ;
  it->setIcon(icon)                                ;
}

int N::FaceView::CountFaces(SqlConnection & SC)
{
  int     total = 0                   ;
  QString Q                           ;
  if (Operation==1)                   {
    UUIDs Uuids = BadFaces(SC)        ;
    total = Uuids.count()             ;
  } else
  if (Group==0)                       {
    total = CountFaceDepot(SC)        ;
  } else                              {
    Q = SC.sql.SelectFrom             (
          "count(*)"                  ,
          TABLES(Groups)              ,
          QString("where first = %1 "
                  "and t1 = %2 "
                  "and t2 = %3 "
                  "and relation = %4" )
          .arg(Group                  )
          .arg(SourceType             )
          .arg(Types::Face            )
          .arg(Relationship       ) ) ;
    if (SC.Fetch(Q))                  {
      total = SC . Int ( 0 )          ;
    }                                 ;
  }                                   ;
  return total                        ;
}

UUIDs N::FaceView::FaceLimits(SqlConnection & SC)
{
  if (StartId<0)                                      {
    StartId = TotalFaces - Limit                      ;
    if (StartId<0) StartId = 0                        ;
  }                                                   ;
  return PictureManager::FaceLimits(SC,StartId,Limit) ;
}

UUIDs N::FaceView::FaceGroups(SqlConnection & SC)
{
  UUIDs      Uuids                      ;
  GroupItems GI ( plan )                ;
  Uuids = GI.Subordination              (
            SC                          ,
            Group                       ,
            SourceType                  ,
            Types::Face                 ,
            Relationship                ,
            SC.OrderByAsc("position") ) ;
  if (Limit>0)                          {
    UUIDs Puids                         ;
    int   s = StartId                   ;
    int   t = Uuids.count()             ;
    if (StartId<0)                      {
      StartId = TotalFaces - Limit      ;
      if (StartId<0) StartId = 0        ;
    }                                   ;
    for (int i=0;i<Limit;i++,s++)       {
      if (s<t) Puids << Uuids[s]        ;
    }                                   ;
    Uuids = Puids                       ;
  }                                     ;
  return Uuids                          ;
}

UUIDs N::FaceView::FailFaces(SqlConnection & SC)
{
  UUIDs Uuids = BadFaces(SC)       ;
  if (Limit>0)                     {
    UUIDs Puids                    ;
    int   s = StartId              ;
    int   t = Uuids.count()        ;
    if (StartId<0)                 {
      StartId = TotalFaces - Limit ;
      if (StartId<0) StartId = 0   ;
    }                              ;
    for (int i=0;i<Limit;i++,s++)  {
      if (s<t) Puids << Uuids[s]   ;
    }                              ;
    Uuids = Puids                  ;
  }                                ;
  return Uuids                     ;
}

UUIDs N::FaceView::FaceUuids(SqlConnection & SC)
{
  UUIDs Uuids                  ;
  int   condition = 0          ;
  if (Operation==1)            {
    condition = 3              ;
  } else
  if (Group==0)                {
    if (Limit<0) condition = 0 ;
            else condition = 1 ;
  } else         condition = 2 ;
  switch (condition)           {
    case 0                     :
      Uuids = FullFaces  (SC)  ;
    break                      ;
    case 1                     :
      Uuids = FaceLimits (SC)  ;
    break                      ;
    case 2                     :
      Uuids = FaceGroups (SC)  ;
    break                      ;
    case 3                     :
      Uuids = FailFaces  (SC)  ;
    break                      ;
  }                            ;
  return Uuids                 ;
}

void N::FaceView::MakeFaces(UUIDs & Uuids,QMap<SUID,QListWidgetItem *> & items)
{
  SUID uuid                                 ;
  blockSignals ( true  )                    ;
  foreach (uuid,Uuids)                      {
    NewListWidgetItem(it)                   ;
    it->setData(Qt::UserRole  ,uuid)        ;
    it->setData(Qt::UserRole+1,0   )        ;
    it->setIcon(QIcon(":/images/face.png")) ;
    QListWidget::addItem ( it )             ;
    items [ uuid ] = it                     ;
  }                                         ;
  blockSignals ( false )                    ;
}

void N::FaceView::run(void)
{
  switch (Method)                   {
    nFastCast ( 0 , ListFaces ( ) ) ;
    nFastCast ( 1 , ListUuids ( ) ) ;
  }                                 ;
}

bool N::FaceView::startup(void)
{
  nKickOut ( loadingFaces , false ) ;
  loadingFaces = true               ;
  Fuids  . clear ( )                ;
  Method = 0                        ;
  clear ( )                         ;
  start ( )                         ;
  return true                       ;
}

bool N::FaceView::List(UUIDs & Uuids)
{
  nKickOut ( loadingFaces , false ) ;
  loadingFaces = true               ;
  Fuids  = Uuids                    ;
  Method = 1                        ;
  clear ( )                         ;
  start ( )                         ;
  return true                       ;
}

void N::FaceView::ListFaces(void)
{
  QMap<SUID,QListWidgetItem *> Faces                   ;
  QString m                                            ;
  m = tr("Reading faces ...")                          ;
  setToolTip ( m              )                        ;
  Time::msleep(300)                                    ;
  setEnabled ( false          )                        ;
  loadingFaces = true                                  ;
  EnterSQL   ( SC , plan->sql )                        ;
    int   total                                        ;
    UUIDs Uuids                                        ;
    ////////////////////////////////////////////////////
    if (Operation==1)                                  {
      setWindowTitle(tr("Failure detected `faces`"))   ;
    } else
    if (Group>0)                                       {
      QString name                                     ;
      if (SourceType==Types::People)                   {
        PeopleManager PM ( plan )                      ;
        QStringList N = PM.Names(SC,Group)             ;
        if (N.count()>0) name = N[0]                   ;
      } else                                           {
        name = SC.getName                              (
                 TABLES(Names)                         ,
                 "uuid"                                ,
                 vLanguageId                           ,
                 Group                               ) ;
      }                                                ;
      if (name.length()>0)                             {
        setWindowTitle(tr("Faces of %1").arg(name))    ;
      }                                                ;
    }                                                  ;
    ////////////////////////////////////////////////////
    total = CountFaces(SC)                             ;
    TotalFaces = total                                 ;
    if (total>0)                                       {
      Uuids = FaceUuids ( SC )                         ;
      if (Uuids.count()>0)                             {
        m = tr("Total %1 faces").arg(total)            ;
        if (Uuids.count()!=TotalFaces)                 {
          m += "\n"                                    ;
          m += tr("Start : %1").arg(StartId)           ;
          m += "\n"                                    ;
          m += tr("Displayed : %2").arg(Uuids.count()) ;
        }                                              ;
        MakeFaces(Uuids,Faces)                         ;
        if (Faces.count()>0)                           {
          FetchFaces(SC,Uuids,Faces)                   ;
        }                                              ;
      }                                                ;
    } else                                             {
      setToolTip(tr("No face was found"))              ;
    }                                                  ;
  LeaveSQL   ( SC , plan->sql )                        ;
  setEnabled ( true           )                        ;
  setToolTip ( m              )                        ;
  FocusIn    (                )                        ;
  loadingFaces = false                                 ;
  Alert      ( Done           )                        ;
}

void N::FaceView::ListUuids(void)
{
  loadingFaces = true         ;
  setEnabled ( false        ) ;
  EnterSQL ( SC , plan->sql ) ;
  LeaveSQL ( SC , plan->sql ) ;
  setEnabled ( true         ) ;
  loadingFaces = false        ;
  Alert    ( Done           ) ;
}

bool N::FaceView::FetchFaces                (
       SqlConnection                & SC    ,
       UUIDs                        & Uuids ,
       QMap<SUID,QListWidgetItem *> & items )
{
  QString Q                                                 ;
  SUID    uuid                                              ;
  ///////////////////////////////////////////////////////////
  foreach (uuid,Uuids)                                      {
    QListWidgetItem * it = items[uuid]                      ;
    Q = SC.sql.SelectFrom                                   (
          "picture,rectangle,possibility,rotation"          ,
           TABLES(Faces)                                    ,
           SC.WhereUuid(uuid)                             ) ;
    if (SC.Fetch(Q))                                        {
      SUID   picture     = SC . Uuid ( 0 )                  ;
      SUID   rectangle   = SC . Uuid ( 1 )                  ;
      double possibility = SC.Value(2).toDouble()           ;
      double rotation    = SC.Value(3).toDouble()           ;
      double dr          = rotation                         ;
      QImage * P         = NULL                             ;
      if (picture>0)                                        {
        P = PictureManager::Picture(SC,picture)             ;
      }                                                     ;
      if (dr<0) dr = -dr                                    ;
      if (NotNull(P))                                       {
        QRectF Rect                                         ;
        if (PictureManager::Rectangle(SC,rectangle,Rect))   {
          QRect    R     = Rect.toRect()                    ;
          QImage * image = new QImage()                     ;
          QString  M                                        ;
          M += tr("Face : %1").arg(uuid)                    ;
          M += "\n"                                         ;
          M += tr("Picture : %1").arg(picture)              ;
          M += "\n"                                         ;
          M += tr("Size : <%1,%2>"                          )
               .arg(P->width ()                             )
               .arg(P->height()                           ) ;
          M += "\n"                                         ;
          M += tr("Region : <%1,%2,%3,%4>"                  )
               .arg( R . x      ( )                         )
               .arg( R . y      ( )                         )
               .arg( R . width  ( )                         )
               .arg( R . height ( )                       ) ;
          M += "\n"                                         ;
          M += tr("Possibility : %1%"                       )
               .arg( possibility * 100                      ,
                     6,'f',4,QChar('0')                   ) ;
          if (dr<0.001)                                     {
            *image = P->copy(R)                             ;
            emit setIcon    ( image , it )                  ;
          } else                                            {
            QImage * E = new QImage()                       ;
            QTransform T                                    ;
            T . reset  (          )                         ;
            T . rotate ( rotation )                         ;
            *E = P->transformed(T,Qt::SmoothTransformation) ;
            M += tr("Rotation angle : %1 degree"            )
                 .arg( rotation * 100                       ,
                       6,'f',4,QChar('0')                 ) ;
            *image = E->copy(R)                             ;
            emit setIcon    ( image , it )                  ;
            delete E                                        ;
          }                                                 ;
          it-> setToolTip ( M          )                    ;
          it-> setData    ( Qt::UserRole+1 , picture )      ;
        }                                                   ;
        delete P                                            ;
      }                                                     ;
    }                                                       ;
  }                                                         ;
  return true                                               ;
}

void N::FaceView::Home(void)
{
  nDropOut ( loadingFaces ) ;
  nDropOut ( Limit < 0    ) ;
  StartId = 0               ;
  startup ( )               ;
}

void N::FaceView::End(void)
{
  nDropOut ( loadingFaces ) ;
  nDropOut ( Limit < 0    ) ;
  if (Limit>=TotalFaces)    {
    StartId = 0             ;
  } else                    {
    StartId = -1            ;
  }                         ;
  startup ( )               ;
}

void N::FaceView::PageUp(void)
{
  nDropOut ( loadingFaces ) ;
  nDropOut ( Limit < 0    ) ;
  int Prev = StartId        ;
  Prev -= Limit             ;
  if (Prev<0) Prev = 0      ;
  StartId = Prev            ;
  startup ( )               ;
}

void N::FaceView::PageDown(void)
{
  nDropOut ( loadingFaces ) ;
  nDropOut ( Limit < 0    ) ;
  int Next = StartId        ;
  Next += Limit             ;
  if (Next<TotalFaces)      {
    StartId = Next          ;
  }                         ;
  startup ( )               ;
}

void N::FaceView::DeleteFaces(void)
{
  nDropOut ( loadingFaces       ) ;
  nDropOut ( Group         <= 0 ) ;
  UUIDs Uuids = Selected()        ;
  nDropOut ( Uuids.count() <= 0 ) ;
  GroupItems GI ( plan )          ;
  EnterSQL ( SC , plan->sql     ) ;
    GI.Detach                     (
      SC                          ,
      Group                       ,
      SourceType                  ,
      Types::Face                 ,
      Relationship                ,
      Uuids                     ) ;
  LeaveSQL ( SC,plan->sql       ) ;
  startup  (                    ) ;
}

void N::FaceView::JoinBadFaces(void)
{
  nDropOut ( loadingFaces       ) ;
  UUIDs Uuids = Selected()        ;
  nDropOut ( Uuids.count() <= 0 ) ;
  EnterSQL ( SC , plan->sql     ) ;
    SUID    uuid                  ;
    foreach (uuid,Uuids)          {
      SC.insertUuid               (
        TABLES(BadFaces)          ,
        uuid,"uuid"             ) ;
    }                             ;
  LeaveSQL ( SC , plan->Sql     ) ;
  startup  ( )                    ;
}

void N::FaceView::AwayBadFaces (void)
{
  nDropOut ( loadingFaces       ) ;
  UUIDs Uuids = Selected()        ;
  nDropOut ( Uuids.count() <= 0 ) ;
  EnterSQL(SC,plan->sql)          ;
    QString Q                     ;
    SUID    uuid                  ;
    foreach (uuid,Uuids)          {
      Q = SC.sql.DeleteFrom       (
            TABLES(BadFaces)      ,
            SC.WhereUuid(uuid))   ;
      SC.Query(Q)                 ;
    }                             ;
  LeaveSQL(SC,plan->sql)          ;
  startup ( )                     ;
}

bool N::FaceView::assignPeople(SUID atUuid,const UUIDs & people)
{
  nKickOut ( Group  <= 0 , false )             ;
  nKickOut ( atUuid <= 0 , false )             ;
  dropAction = true                            ;
  //////////////////////////////////////////////
  UUIDs Uuids = people                         ;
  GroupItems GI ( plan )                       ;
  EnterSQL ( SC , plan->sql )                  ;
    SUID  uuid                                 ;
    UUIDs Puids                                ;
    Puids << atUuid                            ;
    foreach (uuid,Uuids)                       {
      GI.Join                                  (
        SC                                     ,
        uuid                                   ,
        Types::People                          ,
        Types::Face                            ,
        Relationship                           ,
        0                                      ,
        Puids                                ) ;
    }                                          ;
  LeaveSQL ( SC , nVirtualGui::Plan->Sql )     ;
  //////////////////////////////////////////////
  Alert    ( Done )                            ;
  dropAction = false                           ;
  return true                                  ;
}

bool N::FaceView::MoveFaces(SUID atUuid,const UUIDs & Uuids)
{
  nKickOut ( Group <= 0 , false )              ;
  QString m                                    ;
  m = tr("Moving %1 faces").arg(Uuids.count()) ;
  plan->Talk(m)                                ;
  //////////////////////////////////////////////
  dropAction = true                            ;
  SUID            uuid                         ;
  UUIDs           ITEMs = ItemUuids ( )        ;
  ListWidgetItems mItems                       ;
  foreach (uuid,Uuids)                         {
    QListWidgetItem * it = uuidAt ( uuid )     ;
    mItems << it                               ;
    takeItem ( row ( it ) )                    ;
    ITEMs . takeAt ( ITEMs . indexOf (uuid) )  ;
  }                                            ;
  QListWidgetItem * rit   = uuidAt ( atUuid )  ;
  int               rid   = row    ( rit    )  ;
  for (int i=0;i<mItems.count();i++,rid++)     {
    insertItem ( rid , mItems [ i ] )          ;
  }                                            ;
  //////////////////////////////////////////////
  setEnabled ( false )                         ;
  //////////////////////////////////////////////
  UUIDs Puids = Uuids                          ;
  GroupItems GI ( plan )                       ;
  EnterSQL ( SC , plan->sql )                  ;
    GI.Join                                    (
      SC                                       ,
      Group                                    ,
      SourceType                               ,
      Types::Face                              ,
      Relationship                             ,
      atUuid                                   ,
      Puids                                  ) ;
  LeaveSQL ( SC , plan->sql )                  ;
  //////////////////////////////////////////////
  setEnabled    ( true  )                      ;
  setSelections ( Puids )                      ;
  Alert         ( Done  )                      ;
  dropAction = false                           ;
  return true                                  ;
}

bool N::FaceView::JoinFaces(SUID uuid,const UUIDs & faces)
{
  nKickOut   ( Group <= 0 , false  )       ;
  QString M                                ;
  M = tr("%1 faces join group %2"          )
      .arg ( faces.count()                 )
      .arg ( windowTitle()               ) ;
  plan->showMessage(M)                     ;
  plan->Talk       (M)                     ;
  DoProcessEvents                          ;
  setEnabled ( false                     ) ;
  GroupItems GI ( plan )                   ;
  UUIDs Puids = faces                      ;
  EnterSQL   ( SC,plan->sql )              ;
    GI.Join  ( SC                          ,
               Group                       ,
               SourceType                  ,
               Types::Face                 ,
               Relationship                ,
               uuid                        ,
               Puids                     ) ;
  LeaveSQL   ( SC,plan->sql )              ;
  M = tr("%1 face join group %2 finished"  )
      .arg ( faces.count()                 )
      .arg ( windowTitle()               ) ;
  plan->showMessage(M)                     ;
  plan->Talk       (M)                     ;
  setEnabled ( true                      ) ;
  startup    (                           ) ;
  return true                              ;
}

void N::FaceView::Relocate(void)
{
  nDropOut   ( Group <= 0   ) ;
  setEnabled ( false        ) ;
  GroupItems GI ( plan      ) ;
  EnterSQL ( SC , plan->sql ) ;
    GI.Relocate               (
      SC                      ,
      Group                   ,
      SourceType              ,
      Types::Face             ,
      Relationship          ) ;
  LeaveSQL ( SC , plan->sql ) ;
  setEnabled ( true  )        ;
  Alert      ( Done  )        ;
}

bool N::FaceView::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                                            ;
  QMdiSubWindow   * mdi    = Casting(QMdiSubWindow,parent())               ;
  QDockWidget     * dock   = Casting(QDockWidget  ,parent())               ;
  QListWidgetItem * item   = itemAt(pos)                                   ;
  QAction         * aa                                                     ;
  UUIDs             Uuids  = Selected()                                    ;
  QMenu           * mv                                                     ;
  QMenu           * me                                                     ;
  mm.add(101,tr("Refresh"))                                                ;
  if (TotalFaces>0)                                                        {
    mv = mm.addMenu(tr("Movement"))                                        ;
    me = mm.addMenu(tr("Edit"    ))                                        ;
    if (NotNull(item))                                                     {
      mm.add(me,301,tr("View source picture"))                             ;
    }                                                                      ;
    if (Uuids.count()>0)                                                   {
      if (Operation==1)                                                    {
        mm.add(me,304,tr("Selections remove from failure detected faces")) ;
      } else                                                               {
        mm.add(me,302,tr("Selections join failure detected faces"))        ;
      }                                                                    ;
    }                                                                      ;
    if (Group>0)                                                           {
      mm.add(me,410,tr("Relocate faces orders"))                           ;
    }                                                                      ;
    if (Group<=0)                                                          {
    } else                                                                 {
      mm.add(mv,501,tr("All faces"))                                       ;
      mm.addSeparator(mv)                                                  ;
      if (Uuids.count()>0)                                                 {
        if (SourceType==Types::People)                           {
          mm.add(me,303,tr("Detach from people"))                          ;
          mm.add(me,311,tr("Facial features extraction"))                  ;
          mm.add(me,312,tr("Geometric model extraction"))                  ;
        }                                                                  ;
      }                                                                    ;
    }                                                                      ;
    mm.add(mv,502,tr("Goto ..."))                                          ;
    mm.add(mv,503,tr("Faces per page"))                                    ;
    mm.addSeparator(mv)                                                    ;
    mm.add(mv,211,tr("Home"         ))                                     ;
    mm.add(mv,212,tr("End"          ))                                     ;
    mm.addSeparator(mv)                                                    ;
    mm.add(mv,213,tr("Previous page"))                                     ;
    mm.add(mv,214,tr("Next page"    ))                                     ;
    AdjustMenu(mm,me)                                                      ;
  }                                                                        ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))             ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))             ;
  mm . setFont ( plan )                                                    ;
  aa = mm.exec (      )                                                    ;
  nKickOut ( IsNull(aa) , true )                                           ;
  bool okay   = false                                                      ;
  int  result = -1                                                         ;
  switch (mm[aa])                                                          {
    case 101                                                               :
      startup ( )                                                          ;
    break                                                                  ;
    case 501                                                               :
      Limit = -1                                                           ;
      startup ( )                                                          ;
    break                                                                  ;
    case 502                                                               :
      result = QInputDialog::getInt                                        (
                 this                                                      ,
                 tr("Go to face index")                                    ,
                 tr("Start Id :")                                          ,
                 StartId                                                   ,
                 0                                                         ,
                 TotalFaces                                                ,
                 1                                                         ,
                 &okay                                                   ) ;
      if (okay)                                                            {
        StartId = result                                                   ;
        startup (       )                                                  ;
      } else                                                               {
        Alert   ( Error )                                                  ;
      }                                                                    ;
    break                                                                  ;
    case 503                                                               :
      result = QInputDialog::getInt                                        (
                 this                                                      ,
                 tr("Faces per page")                                      ,
                 tr("Faces :")                                             ,
                 Limit                                                     ,
                 -1                                                        ,
                 TotalFaces                                                ,
                 1                                                         ,
                 &okay                                                   ) ;
      if (okay && result==0) okay = false                                  ;
      if (okay)                                                            {
        Limit = result                                                     ;
        startup (       )                                                  ;
      } else                                                               {
        Alert   ( Error )                                                  ;
      }                                                                    ;
    break                                                                  ;
    case 211                                                               :
      Home         ( )                                                     ;
    break                                                                  ;
    case 212                                                               :
      End          ( )                                                     ;
    break                                                                  ;
    case 213                                                               :
      PageUp       ( )                                                     ;
    break                                                                  ;
    case 214                                                               :
      PageDown     ( )                                                     ;
    break                                                                  ;
    case 301                                                               :
      if (NotNull(item))                                                   {
        SUID picture = item->data(Qt::UserRole+1).toULongLong()            ;
        if (picture>0)                                                     {
          if (Group>0)                                                     {
            emit ViewPicture (windowTitle(),Group,SourceType,picture)      ;
          } else                                                           {
            emit ViewPicture (picture)                                     ;
          }                                                                ;
        }                                                                  ;
      } else                                                               {
        Alert ( Error )                                                    ;
      }                                                                    ;
    break                                                                  ;
    case 302                                                               :
      JoinBadFaces ( )                                                     ;
    break                                                                  ;
    case 303                                                               :
      DeleteFaces  ( )                                                     ;
    break                                                                  ;
    case 304                                                               :
      AwayBadFaces ( )                                                     ;
    break                                                                  ;
    case 311                                                               :
      emit Features(Group,Uuids)                                           ;
    break                                                                  ;
    case 312                                                               :
      emit Model   (Group,Uuids)                                           ;
    break                                                                  ;
    case 410                                                               :
      Relocate     (           )                                           ;
    break                                                                  ;
    case 1000001                                                           :
      emit attachMdi (this,0)                                              ;
    break                                                                  ;
    case 1000002                                                           :
      emit attachDock                                                      (
        this                                                               ,
        windowTitle()                                                      ,
        Qt::RightDockWidgetArea                                            ,
        Qt::TopDockWidgetArea                                              |
        Qt::BottomDockWidgetArea                                           |
        Qt::LeftDockWidgetArea                                             |
        Qt::RightDockWidgetArea                                          ) ;
    break                                                                  ;
    default                                                                :
      RunAdjustment ( mm , aa )                                            ;
    break                                                                  ;
  }                                                                        ;
  return true                                                              ;
}
