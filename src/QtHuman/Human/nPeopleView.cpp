#include <qthuman.h>

#define hasMenu isFunction(1001)
#define ALERT(ID) AnotherALERT(N::Command::ID)

N::PeopleView:: PeopleView   ( QWidget * parent , Plan * p                 )
              : SnapView     (           parent ,        p                 )
              , Group        ( 0 , Types::Division , Groups::Subordination )
              , Search       ( NULL                                        )
              , dropAction   ( false                                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleView::~PeopleView (void)
{
}

void N::PeopleView::Configure(void)
{
  StackSize          = 0                                 ;
  Reservation        = false                             ;
  t2                 = Types::People                     ;
  ////////////////////////////////////////////////////////
  LimitValues [  0 ] =   0                               ;
  LimitValues [  1 ] =  60                               ;
  LimitValues [  2 ] =   0                               ;
  LimitValues [ 11 ] = (int)Qt::AscendingOrder           ;
  LimitValues [ 75 ] =   0                               ;
  LimitValues [ 76 ] = 100                               ;
  LimitValues [ 77 ] = 500                               ;
  LimitValues [ 81 ] =   1                               ;
  ////////////////////////////////////////////////////////
  dockingOrientation = 0                                 ;
  dockingPlace       = Qt::BottomDockWidgetArea          ;
  ////////////////////////////////////////////////////////
  setWindowTitle               ( tr("People")          ) ;
  setViewMode                  ( IconMode              ) ;
  setIconSize                  ( QSize(128,128)        ) ;
  setGridSize                  ( QSize(140,168)        ) ;
  setMovement                  ( Snap                  ) ;
  setResizeMode                ( Adjust                ) ;
  setSelectionMode             ( ExtendedSelection     ) ;
  setWordWrap                  ( true                  ) ;
  setMouseTracking             ( true                  ) ;
  setWrapping                  ( true                  ) ;
  setTextElideMode             ( Qt::ElideNone         ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  setMinimumSize               ( QSize(144,180)        ) ;
  setDragDropMode              ( DragDrop              ) ;
  setAccessibleName            ( "N::PeopleView"       ) ;
  setObjectName                ( "N::PeopleView"       ) ;
  ////////////////////////////////////////////////////////
  setDropFlag                  ( DropFont     , true   ) ;
  setDropFlag                  ( DropPen      , true   ) ;
  setDropFlag                  ( DropBrush    , true   ) ;
  setDropFlag                  ( DropText     , true   ) ;
  setDropFlag                  ( DropUrls     , true   ) ;
  setDropFlag                  ( DropImage    , true   ) ;
  setDropFlag                  ( DropHtml     , true   ) ;
  setDropFlag                  ( DropColor    , false  ) ;
  setDropFlag                  ( DropTag      , true   ) ;
  setDropFlag                  ( DropPicture  , true   ) ;
  setDropFlag                  ( DropPeople   , true   ) ;
  setDropFlag                  ( DropVideo    , false  ) ;
  setDropFlag                  ( DropAlbum    , true   ) ;
  setDropFlag                  ( DropGender   , true   ) ;
  setDropFlag                  ( DropDivision , false  ) ;
  ////////////////////////////////////////////////////////
  setFunction                  ( 1001         , true   ) ;
  setFunction                  ( 1002         , true   ) ;
  setFunction                  ( 1101         , true   ) ;
  setFunction                  ( 3101         , true   ) ;
  ////////////////////////////////////////////////////////
  plan->setFont                ( this                  ) ;
  MountCommands                (                       ) ;
}

bool N::PeopleView::canDelete(void)
{
  return ( isOwner ( ) ) ;
}

bool N::PeopleView::contains(SUID uuid)
{
  for (int i=0;i<count();i++)      {
    QListWidgetItem * it = item(i) ;
    SUID suid = ItemUuid ( it )    ;
    if (suid==uuid) return true    ;
  }                                ;
  return false                     ;
}

void N::PeopleView::closeEvent(QCloseEvent * event)
{
  StopIcons                (       ) ;
  ListWidget :: closeEvent ( event ) ;
}

QMimeData * N::PeopleView::dragMime(void)
{
  PickedUuids . clear ( )                         ;
  if (Items.count()<=0) return NULL               ;
  QMimeData       * mime    = new QMimeData ( )   ;
  QListWidgetItem * current = currentItem   ( )   ;
  if (Items.count()==1)                           {
    SUID        type = nListUuid(Items[0])        ;
    QByteArray  data = CreateByteArray(type)      ;
    mime -> setData ("people/uuid",data)          ;
  } else                                          {
    UUIDs       Uuids = Selected        (       ) ;
    QByteArray  data  = CreateByteArray ( Uuids ) ;
    mime -> setData ("people/uuids",data)         ;
    PickedUuids = Uuids                           ;
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
  tooltip = tr("%1 people selected"               )
            .arg(Items.count()                  ) ;
  QToolTip :: showText ( QCursor::pos(),tooltip ) ;
  return mime                                     ;
}

QString N::PeopleView::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType(
           mime,
           "album/uuid;album/uuids;"
           "picture/uuid;picture/uuids;"
           "tag/uuid;tag/uuids;"
           "people/uuid;people/uuids;"
           "gender/uuid"
         );
}

UUIDs N::PeopleView::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="album/uuid")            {
    Uuids << GetUuid  ( data )           ;
  } else
  if (mimetype=="album/uuids")           {
    Uuids  = GetUuids ( data )           ;
  } else
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
  } else
  if (mimetype=="gender/uuid")           {
    Uuids << GetUuid(data)               ;
  }                                      ;
  return Uuids                           ;
}

bool N::PeopleView::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

bool N::PeopleView::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  QString    mtype                             ;
  UUIDs      Uuids                             ;
  if (source==this)                            {
    mtype = MimeType  (mime      )             ;
    Uuids = MimeUuids (mime,mtype)             ;
    plan->showMessage                          (
      tr("Start moving %1 people"              )
                .arg(Uuids.count()))           ;
    DoProcessEvents                            ;
  } else                                       {
    if (mime->hasImage())                      {
      QListWidgetItem * atItem = itemAt(pos)   ;
      if (NotNull(atItem))                     {
        SUID uuid = nListUuid(atItem)          ;
        if (uuid>0)                            {
          PictureManager PM(plan)              ;
          QImage * I = PM.MimeImage(mime)      ;
          if (NotNull(I))                      {
            plan->showMessage                  (
              tr("Picture [%1 x %2] join %3"   )
              .arg(I->width ()                 )
              .arg(I->height()                 )
              .arg(atItem->text())           ) ;
          }                                    ;
          delete I                             ;
          I = NULL                             ;
        }                                      ;
      }                                        ;
      return true                              ;
    } else
    if (mime->hasUrls())                       {
      return true                              ;
    } else                                     {
      mtype = MimeType  (mime      )           ;
      Uuids = MimeUuids (mime,mtype)           ;
      if (mtype=="album/uuid"                 ||
          mtype=="album/uuids"                 )
      plan->showMessage                        (
        tr("Copy %1 album from %2"             )
        .arg(Uuids.count()                     )
        .arg(source->windowTitle())          ) ;
      if (mtype=="picture/uuid"               ||
          mtype=="picture/uuids"               )
      plan->showMessage                        (
        tr("Copy %1 pictures from %2"          )
        .arg(Uuids.count()                     )
        .arg(source->windowTitle())          ) ;
      if (mtype=="tag/uuid"                   ||
          mtype=="tag/uuids"                   )
      plan->showMessage                        (
        tr("Assign %1 tags to %2"              )
        .arg(Uuids.count()                     )
        .arg(source->windowTitle())          ) ;
      if (mtype=="people/uuid"                ||
          mtype=="people/uuids"                )
      plan->showMessage                        (
        tr("Copy %1 people from %2"            )
        .arg(Uuids.count()                     )
        .arg(source->windowTitle())          ) ;
      if (mtype=="gender/uuid"                 )
      plan->showMessage                        (
        tr("Assign sexual gender to people") ) ;
      DoProcessEvents                          ;
    }                                          ;
  }                                            ;
  return true                                  ;
}

bool N::PeopleView::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false                                ;
  if (source==this)                                           {
    QListWidgetItem * atItem = itemAt(pos)                    ;
    if (IsNull (atItem)) return false                         ;
    if (NotNull(atItem) && atItem->isSelected()) return false ;
  }                                                           ;
  return true                                                 ;
}

bool N::PeopleView::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if ( dropAction ) return false           ;
  return dropItems ( source , mime , pos ) ;
}

bool N::PeopleView::setUuid         (
       SqlConnection   & Connection ,
       SUID              uuid       ,
       QListWidgetItem * item       )
{
  PictureManager PM ( plan )                        ;
  PeopleManager  PP ( plan )                        ;
  GroupItems     GI ( plan )                        ;
  GI.AutoMap    = true                              ;
  GI.GroupTable = GI . LookTable                    (
                    Types  :: People                ,
                    Types  :: Picture               ,
                    Groups :: Icon                ) ;
  QSize is (128,128)                                ;
  QString  name    = PP.PreferName(Connection,uuid) ;
  SUID     puid    = GI . FindSecond                (
                  Connection                        ,
                  uuid                              ,
                  Types  :: People                  ,
                  Types  :: Picture                 ,
                  Groups :: Icon                    ,
                  Connection.OrderByAsc("position") ,
                  Connection.sql.Limit(0,1)       ) ;
  QImage * image   = PM.Thumb     (Connection,puid) ;
  QString  tooltip = name                           ;
  bool     exists  = false                          ;
  if (NotNull(image))                               {
    QIcon icon = PM . Icon (image,is)               ;
    delete image                                    ;
    if (IsNull(item))                               {
      addItem(uuid,icon,name,tooltip)               ;
    } else                                          {
      item -> setText    ( name    )                ;
      item -> setIcon    ( icon    )                ;
      item -> setToolTip ( tooltip )                ;
    }                                               ;
    exists = true                                   ;
  } else                                            {
    QIcon icon = QIcon(":/images/face.png")         ;
    if (IsNull(item))                               {
      addItem(uuid,icon,name,tooltip)               ;
    } else                                          {
      item -> setText    ( name    )                ;
      item -> setIcon    ( icon    )                ;
      item -> setToolTip ( tooltip )                ;
    }                                               ;
  }                                                 ;
  return exists                                     ;
}

bool N::PeopleView::addUuid(SqlConnection & Connection,SUID uuid)
{
  return setUuid ( Connection , uuid , NULL ) ;
}

QListWidgetItem * N::PeopleView::newUuid(SqlConnection & Connection,SUID uuid)
{
  PeopleManager  PP ( plan )                        ;
  QString  name    = PP.PreferName(Connection,uuid) ;
  QString  tooltip = name                           ;
  QIcon    icon    = QIcon(":/images/face.png")     ;
  return addItem(uuid,icon,name,tooltip)            ;
}

void N::PeopleView::FolderLanguage(void)
{
  if ( ! isFirst ( ) ) return            ;
  int OL = vLanguageId                   ;
  AbstractGui :: assignLanguage ( this ) ;
  if ( OL == vLanguageId ) return        ;
  EnterSQL ( SC , plan->sql )            ;
    GroupItems :: assignLanguage         (
      SC                                 ,
      first                              ,
      t1                                 ,
      vLanguageId                      ) ;
  ErrorSQL ( SC , plan->sql )            ;
  LeaveSQL ( SC , plan->sql )            ;
}

void N::PeopleView::ResumeLanguage(void)
{
  if ( isFirst() && ( LimitValues [ 81 ] > 0 ) ) {
    EnterSQL ( SC , plan->sql )                  ;
      int L                                      ;
      L = usingLanguage(SC,first,t1,vLanguageId) ;
      if (L>0) vLanguageId = L                   ;
      LimitValues [ 81 ] --                      ;
    ErrorSQL ( SC , plan->sql )                  ;
    LeaveSQL ( SC , plan->sql )                  ;
  }                                              ;
}

bool N::PeopleView::startup(void)
{
  if ( !isStopped ( ) ) return false ;
  ResumeLanguage      (       )      ;
  clear               (       )      ;
  addSequence         ( 10002 )      ;
  emit TriggerCommand (       )      ;
  return true                        ;
}

/////////////////////////////////////////////////////////////////////////////

bool N::PeopleView::isOwner(void)
{
  if ( isFirst  ( ) ) return true ;
  if ( isSecond ( ) ) return true ;
  return false                    ;
}

void N::PeopleView::StopIcons(void)
{
  stopForcely ( )                         ;
  while ( detection ( ) ) Time::skip(100) ;
  emit Leave  ( this )                    ;
}

void N::PeopleView::FetchUUIDs(void)
{
  UUIDs         Uuids                                                ;
  int           startId =                 LimitValues [  0 ]         ;
  int           page    =                 LimitValues [  1 ]         ;
  int           total   =                 LimitValues [  2 ]         ;
  Qt::SortOrder order   = (Qt::SortOrder) LimitValues [ 11 ]         ;
  if (page<=0) page = total                                          ;
  UuidMaps . take ( 10001 )                                          ;
  GroupItems     GI ( plan )                                         ;
  GI.AutoMap    = true                                               ;
  GI.GroupTable = GI . LookTable ( t1 , t2 , relation              ) ;
  EnterSQL ( SC , plan->sql )                                        ;
    if (isFirst())                                                   {
      Uuids = GI . Subordination                                     (
                SC                                                   ,
                first                                                ,
                t1                                                   ,
                t2                                                   ,
                relation                                             ,
                SC . OrderBy   ( "position" , order                ) ,
                SC . sql.Limit ( startId    , page               ) ) ;
    } else
    if (isSecond())                                                  {
      Uuids = GI . GetOwners                                         (
                SC                                                   ,
                second                                               ,
                t1                                                   ,
                t2                                                   ,
                relation                                             ,
                SC . OrderBy   ( "reversal" , order                ) ,
                SC . sql.Limit ( startId    , page               ) ) ;
    } else                                                           {
      Uuids = SC.Uuids                                               (
                PlanTable(PeopleUuid)                                ,
                "uuid"                                               ,
                QString ( "%1 %2"                                    )
                . arg   ( SC.OrderBy   ( "id"    , order )           )
                . arg   ( SC.sql.Limit ( startId , page  )       ) ) ;
    }                                                                ;
  LeaveSQL ( SC , plan->sql )                                        ;
  UuidMaps [ 10001 ] = Uuids                                         ;
}

void N::PeopleView::FetchTOTALs(void)
{
  int total = 0                                 ;
  GroupItems GI ( plan )                        ;
  GI.AutoMap    = true                          ;
  GI.GroupTable = GI . LookTable                (
                    t1                          ,
                    t2                          ,
                    relation                  ) ;
  EnterSQL ( SC , plan->sql )                   ;
    QString Q                                   ;
    if (isFirst())                              {
      Q = SC . sql . SelectFrom                 (
            "count(*)"                          ,
            GI.GroupTable                       ,
            FirstItem (first ,t1,t2,relation) ) ;
    } else
    if (isSecond())                             {
      Q = SC . sql . SelectFrom                 (
            "count(*)"                          ,
            GI.GroupTable                       ,
            SecondItem(second,t1,t2,relation) ) ;
    } else                                      {
      Q = SC . sql . SelectFrom                 (
            "count(*)"                          ,
            PlanTable(PeopleUuid)             ) ;
    }                                           ;
    if (SC.Fetch(Q))                            {
      total = SC.Int(0)                         ;
    }                                           ;
  LeaveSQL ( SC , plan->sql )                   ;
  LimitValues [ 2 ] = total                     ;
}

void N::PeopleView::ActorUUIDs(void)
{
  UUIDs Uuids                                                              ;
  UuidMaps . take ( 10001 )                                                ;
  GroupItems GI ( plan )                                                   ;
  GI.AutoMap    = true                                                     ;
  GI.GroupTable = GI . LookTable ( t1 , t2 , relation                    ) ;
  EnterSQL ( SC , plan->sql )                                              ;
    QString Q                                                              ;
    Q = SC . sql . SelectFrom                                              (
          "second"                                                         ,
          GI.GroupTable                                                    ,
          QString ( "where `t1` = %1 and `t2` = %2 and `relation` = %3 %4" )
          . arg   ( t1                                                     )
          . arg   ( t2                                                     )
          . arg   ( relation                                               )
          . arg   ( SC.OrderByAsc("id"))                                 ) ;
    SqlLoopNow ( SC , Q )                                                  ;
      SUID u = SC . Uuid ( 0 )                                             ;
      if (!Uuids.contains(u)) Uuids << u                                   ;
    SqlLoopErr ( SC , Q )                                                  ;
    SqlLoopEnd ( SC , Q )                                                  ;
  LeaveSQL ( SC , plan->sql )                                              ;
  UuidMaps [ 10001 ] = Uuids                                               ;
}

void N::PeopleView::ActorTOTALs(void)
{
  int total = 0                                                            ;
  UUIDs Uuids                                                              ;
  GroupItems GI ( plan )                                                   ;
  GI.AutoMap    = true                                                     ;
  GI.GroupTable = GI . LookTable ( t1 , t2 , relation                    ) ;
  EnterSQL ( SC , plan->sql )                                              ;
    QString Q                                                              ;
    Q = SC . sql . SelectFrom                                              (
          "second"                                                         ,
          GI.GroupTable                                                    ,
          QString ( "where `t1` = %1 and `t2` = %2 and `relation` = %3 %4" )
          . arg   ( t1                                                     )
          . arg   ( t2                                                     )
          . arg   ( relation                                               )
          . arg   ( SC.OrderByAsc("id"))                                 ) ;
    SqlLoopNow ( SC , Q )                                                  ;
      SUID u = SC . Uuid ( 0 )                                             ;
      if (!Uuids.contains(u)) Uuids << u                                   ;
    SqlLoopErr ( SC , Q )                                                  ;
    SqlLoopEnd ( SC , Q )                                                  ;
    total = Uuids.count()                                                  ;
  LeaveSQL ( SC , plan->sql )                                              ;
  LimitValues [ 2 ] = total                                                ;
}

bool N::PeopleView::PeoplePictures(SqlConnection & SC,QListWidgetItem * item)
{
  if ( IsNull(item) ) return false                      ;
  SUID uuid = nListUuid ( item )                        ;
  if ( uuid  <= 0   ) return false                      ;
  GroupItems GI ( plan )                                ;
  GI.AutoMap    = true                                  ;
  GI.GroupTable = GI . LookTable                        (
                    Types  :: People                    ,
                    Types  :: Picture                   ,
                    Groups :: Subordination           ) ;
  ///////////////////////////////////////////////////////
  int total = GI . Count                                (
                SC                                      ,
                uuid                                    ,
                Types  :: People                        ,
                Types  :: Picture                       ,
                Groups :: Subordination               ) ;
  if ( total <= 0   ) return true                       ;
  ///////////////////////////////////////////////////////
  QString name = item->text()                           ;
  QString m                                             ;
  m     = tr("%1 has %2 pictures").arg(name).arg(total) ;
  item -> setToolTip ( m )                              ;
  ///////////////////////////////////////////////////////
  return true                                           ;
}

bool N::PeopleView::PossiblePictures(SqlConnection & SC,QListWidgetItem * item)
{
  if ( IsNull(item) ) return false                           ;
  SUID uuid = nListUuid ( item )                             ;
  if ( uuid  <= 0   ) return false                           ;
  GroupItems GI ( plan )                                     ;
  GI.AutoMap    = true                                       ;
  GI.GroupTable = GI . LookTable                             (
                    Types  :: People                         ,
                    Types  :: Picture                        ,
                    Groups :: Possible                     ) ;
  ////////////////////////////////////////////////////////////
  int total = GI . Count                                     (
                SC                                           ,
                uuid                                         ,
                Types  :: People                             ,
                Types  :: Picture                            ,
                Groups :: Possible                         ) ;
  if ( total <= 0   ) return true                            ;
  ////////////////////////////////////////////////////////////
  QString name = item->text()                                ;
  QString m                                                  ;
  m = tr("%1 has %2 possible pictures").arg(name).arg(total) ;
  item -> setToolTip ( m )                                   ;
  ////////////////////////////////////////////////////////////
  return true                                                ;
}

bool N::PeopleView::ReportDetails(SqlConnection & Connection,QListWidgetItem * item,int flags)
{
  switch ( flags )                                {
    case 4                                        :
    return PeoplePictures   ( Connection , item ) ;
    case 5                                        :
    return PossiblePictures ( Connection , item ) ;
  }                                               ;
  return false                                    ;
}

/////////////////////////////////////////////////////////////////////////////

void N::PeopleView::View(UUIDs & Uuids)
{
  emit clearItems       (                ) ;
  emit assignEnabling   ( false          ) ;
  emit assignWaitCursor (                ) ;
  plan -> processEvents (                ) ;
  LockGui               (                ) ;
  blockSignals          ( true           ) ;
  //////////////////////////////////////////
  int             PPL = LimitValues [ 76 ] ;
  int             PRC = LimitValues [ 77 ] ;
  LoopInt         LIP ( 0 , 0 , PPL )      ;
  QIcon           ICON                     ;
  ListWidgetItems ITEMs                    ;
  bool            GUI                      ;
  //////////////////////////////////////////
  ICON  = plan->Icon(Types::FaceShape,1,0) ;
  ITEMs = SnapView :: New ( Uuids )        ;
  GUI   = ( ITEMs.count() > PRC  )         ;
  for (int i=0;i<ITEMs.count();i++)        {
    ITEMs [ i ] -> setIcon ( ICON        ) ;
    ITEMs [ i ] -> setText ( " "         ) ;
    QListWidget :: addItem ( ITEMs [ i ] ) ;
    if ( GUI )                             {
      ++LIP                                ;
      if ( 0 == LIP.V )                    {
        plan -> processEvents ( )          ;
      }                                    ;
    }                                      ;
  }                                        ;
  //////////////////////////////////////////
  blockSignals           ( false         ) ;
  UnlockGui              (               ) ;
  emit assignArrowCursor (               ) ;
  emit assignEnabling    ( true          ) ;
  SnapView::Relocation   (               ) ;
  //////////////////////////////////////////
  QString TT                               ;
  QString MM                               ;
  LimitValues [ 75 ] ++                    ;
  TT = tr("Total %1 people, displayed %2 start at %3") ;
  MM = QString ( TT                        )
      . arg    ( LimitValues[2]            )
      . arg    ( ITEMs.count( )            )
      . arg    ( LimitValues[0]          ) ;
  plan -> processEvents (                ) ;
  emit assignToolTip    ( MM             ) ;
  //////////////////////////////////////////
  Group   G                                ;
  VarArgs V                                ;
  //////////////////////////////////////////
  G.first      = 0                         ;
  G.second     = 0                         ;
  G.t1         = Types  :: People          ;
  G.t2         = Types  :: Picture         ;
  G.relation   = Groups :: Icon            ;
  G.position   = 0                         ;
  G.membership = 1.0                       ;
  //////////////////////////////////////////
  addSequence           ( 20002          ) ;
  //////////////////////////////////////////
  V  . clear            (                ) ;
  V << 30002                               ;
  V << 3                                   ;
  addSequence           ( V              ) ;
  //////////////////////////////////////////
  V  . clear            (                ) ;
  V << 40001                               ;
  G . addParaments      ( V              ) ;
  addSequence           ( V              ) ;
  //////////////////////////////////////////
  emit TriggerCommand   (                ) ;
  plan -> processEvents (                ) ;
}

/////////////////////////////////////////////////////////////////////////////

void N::PeopleView::SearchIndex(int index)
{
  SUID uuid = 0;
  if (index>=0) uuid = Search->itemData(index,Qt::UserRole).toULongLong();
  Search->hide();
  Search->deleteLater() ;
  Search = NULL ;
  if (uuid<=0) return ;
  QListWidgetItem * LWI = NULL ;
  for (int i=0;IsNull(LWI) && i<count();i++) {
    QListWidgetItem * it = item(i) ;
    SUID nuid = it->data(Qt::UserRole).toULongLong() ;
    if (nuid==uuid) LWI = it ;
  };
  if (IsNull(LWI)) return ;
  scrollToItem(LWI);
  setCurrentItem(LWI);
  DoProcessEvents ;
  QRect vw = visualItemRect(LWI);
  QPoint vc = vw.center() ;
  QCursor::setPos(mapToGlobal(vc));
}

void N::PeopleView::Find(void)
{
  if (NotNull(Search)) Search->deleteLater()        ;
  Search = (ComboBox *)plan->ComboBox(plan->status) ;
  QFont font = plan->status->font()                 ;
  QSize s    = plan->status->size()                 ;
  QSize m(s.width()/8,s.height()-2)                 ;
  Search->setEditable(true)                         ;
  plan->status->addPermanentWidget(Search)          ;
  if (font.pixelSize()>0)                           {
    font.setPixelSize(font.pixelSize())             ;
    Search->setFont(font)                           ;
    Search->lineEdit()->setFont(font)               ;
  }                                                 ;
  Search -> setMinimumSize ( m )                    ;
  Search -> setMaximumSize ( m )                    ;
  QLineEdit * le = Search->lineEdit ()              ;
  QSize ml(m.width()-30,m.height()-2)               ;
  le     -> setMinimumSize ( ml )                   ;
  le     -> setMaximumSize ( ml )                   ;
  AbstractGui::Mutex.lock()                         ;
  SqlConnection SC ( plan->sql )                    ;
  PeopleManager PM ( plan      )                    ;
  if ( SC . open ( FunctionString ) )               {
    PM . PeopleComboBox ( SC , Search )             ;
    SC . close          (             )             ;
  }                                                 ;
  SC   . remove         (             )             ;
  UnlockGui             (             )             ;
  nConnect ( Search , SIGNAL(activated  (int))      ,
             this   , SLOT  (SearchIndex(int))    ) ;
  Search->lineEdit()->setFocus(Qt::TabFocusReason)  ;
}

void N::PeopleView::Refresh(QListWidgetItem * item)
{
  LockGui           (               ) ;
  SqlConnection  SC ( plan->sql     ) ;
  if ( SC . open ( FunctionString ) ) {
    SUID uuid = nListUuid ( item    ) ;
    setUuid    ( SC , uuid , item   ) ;
    SC . close (                    ) ;
  }                                   ;
  SC . remove  (                    ) ;
  UnlockGui    (                    ) ;
}

void N::PeopleView::Add(SUID uuid)
{
  QListWidgetItem * it = NULL               ;
  for (int i=0;IsNull(it) && i<count();i++) {
    QListWidgetItem * lwi = item(i)         ;
    SUID puid = nListUuid(lwi)              ;
    if ( puid == uuid ) it = lwi            ;
  }                                         ;
  if (NotNull(it))                          {
    Refresh(it)                             ;
    return                                  ;
  }                                         ;
  EnterSQL  ( SC , plan->sql )              ;
    addUuid ( SC , uuid      )              ;
  LeaveSQL  ( SC , plan->sql )              ;
}

void N::PeopleView::Add(UUIDs & Uuids,int sourceType)
{
  UUIDs Euids = ItemUuids (  )   ;
  UUIDs Puids                    ;
  SUID  uuid                     ;
  t1 = sourceType                ;
  foreach (uuid,Uuids)           {
    if (!Euids.contains(uuid))   {
      if (!Puids.contains(uuid)) {
        Puids << uuid            ;
      }                          ;
    }                            ;
  }                              ;
  EnterSQL  ( SC , plan->sql )   ;
  foreach   ( uuid , Puids   )   {
    addUuid ( SC , uuid      )   ;
    plan -> processEvents (  )   ;
  }                              ;
  LeaveSQL  ( SC , plan->sql )   ;
  pendingStartup ( )             ;
}

void N::PeopleView::JoinActor(SqlConnection & Connection,SUID uuid)
{
  if ( ! isOwner ( ) ) return ;
  UUIDs U                     ;
  U << uuid                   ;
  if ( isFirst () )           {
    GroupItems :: Join        (
      Connection              ,
      first                   ,
      t1                      ,
      t2                      ,
      relation                ,
      0                       ,
      U                       ,
      false                 ) ;
  } else
  if ( isSecond() )           {
    U << second               ;
    GroupItems :: JoinOwner   (
      Connection              ,
      second                  ,
      t1                      ,
      t2                      ,
      relation                ,
      U                       ,
      false                 ) ;
  }                           ;
}

void N::PeopleView::JoinActor(SqlConnection & Connection,SUID uuid,QListWidgetItem * Item)
{
  if ( ! isOwner ( ) ) return             ;
  Item -> setData ( Qt::UserRole , uuid ) ;
  JoinActor  ( Connection , uuid        ) ;
  setUuid    ( Connection , uuid , Item ) ;
}

void N::PeopleView::DetachActors(SqlConnection & Connection,UUIDs & Uuids)
{
  if ( ! isOwner ( ) ) return    ;
  GroupItems GI ( plan )         ;
  GI.AutoMap    = true           ;
  GI.GroupTable = GI . LookTable (
                    t1           ,
                    t2           ,
                    relation   ) ;
  GI . Detach                    (
    Connection                   ,
    first                        ,
    t1                           ,
    t2                           ,
    relation                     ,
    Uuids                      ) ;
}

void N::PeopleView::NewPeople(void)
{
  RemoveOldItem ( )                                           ;
  QComboBox * NameEditor = new QComboBox(this)                ;
  QIcon icon   = QIcon ( ":/images/face.png" )                ;
  ItemWidget   = NameEditor                                   ;
  ItemEditing  = new QListWidgetItem (                      ) ;
  ItemEditing -> setData ( Qt::UserRole , 0                 ) ;
  ItemEditing -> setIcon ( icon                             ) ;
  QListWidget::blockSignals ( true                          ) ;
  QListWidget::addItem      ( ItemEditing                   ) ;
  QListWidget::blockSignals ( false                         ) ;
  DoProcessEvents                                             ;
  /////////////////////////////////////////////////////////////
  QRect IR = visualItemRect ( ItemEditing                   ) ;
  QFont ff = font           (                               ) ;
  QRect ER(IR.left(),IR.bottom(),IR.width(),ff.pixelSize()+4) ;
  setItemWidget             ( ItemEditing , NameEditor      ) ;
  NameEditor -> setFont     ( ff                            ) ;
  NameEditor -> setGeometry ( ER                            ) ;
  NameEditor -> setEditable ( true                          ) ;
  emit assignEnabling       ( false                         ) ;
  LockGui                   (                               ) ;
  emit OnBusy               (                               ) ;
  SqlConnection SC          ( plan -> sql                   ) ;
  PeopleManager PM          ( plan                          ) ;
  if                        ( SC . open ( FunctionString )  ) {
    PM . PeopleComboBox     ( SC , NameEditor               ) ;
    SC . close              (                               ) ;
  }                                                           ;
  SC . remove               (                               ) ;
  emit GoRelax              (                               ) ;
  UnlockGui                 (                               ) ;
  emit assignEnabling       ( true                          ) ;
  NameEditor -> show        (                               ) ;
  scrollToItem              ( ItemEditing                   ) ;
  ff . setPixelSize         ( ff . pixelSize ( )            ) ;
  NameEditor->lineEdit()->setFont ( ff )                      ;
  nConnect                  ( NameEditor -> lineEdit ( )      ,
                              SIGNAL ( returnPressed ( ) )    ,
                              this                            ,
                              SLOT   ( NameFinished  ( ) )  ) ;
  NameEditor->lineEdit()->setFocus ( Qt::TabFocusReason     ) ;
}

void N::PeopleView::LocatePeople(void)
{
}

void N::PeopleView::NameFinished(void)
{
  QComboBox * comb = qobject_cast<QComboBox *>(ItemWidget) ;
  if (IsNull(comb))                                        {
    RemoveOldItem ( )                                      ;
    return                                                 ;
  }                                                        ;
  QString name = comb->lineEdit()->text()                  ;
  if (name.length()<=0)                                    {
    RemoveOldItem ( )                                      ;
    return                                                 ;
  }                                                        ;
  int index = comb -> findText ( name )                    ;
  SUID uuid = 0                                            ;
  if (index>=0) uuid = comb->itemData(index).toULongLong() ;
  LockGui ( )                                              ;
  SqlConnection SC ( plan->sql )                           ;
  PeopleManager PM ( plan      )                           ;
  if ( SC . open ( FunctionString ) )                      {
    if (uuid==0)                                           {
      uuid = PM . addPeople  ( SC , vLanguageId , name   ) ;
      JoinActor              ( SC , uuid                 ) ;
      ItemEditing -> setData ( Qt::UserRole , uuid       ) ;
      ItemEditing -> setText ( name                      ) ;
    } else                                                 {
      UUIDs Uuids = ItemUuids ()                           ;
      if (Uuids.contains(uuid))                            {
        QListWidgetItem * it = uuidAt(uuid)                ;
        if (NotNull(it))                                   {
          scrollToItem   ( it )                            ;
          setCurrentItem ( it )                            ;
          DoProcessEvents                                  ;
          QRect  vw = visualItemRect (it)                  ;
          QPoint vc = vw.center      (  )                  ;
          QCursor::setPos(mapToGlobal(vc))                 ;
        }                                                  ;
      } else JoinActor ( SC , uuid , ItemEditing )         ;
    }                                                      ;
    SC.close()                                             ;
  }                                                        ;
  SC.remove()                                              ;
  UnlockGui ( )                                            ;
  RemoveOldItem ( )                                        ;
}

void N::PeopleView::singleClicked(QListWidgetItem * item)
{
  ListWidget :: singleClicked(item)   ;
  ALERT ( Click )                     ;
  plan->disableAction(Menus::Delete)  ;
  if (canDelete())                    {
    plan->connectAction               (
      Menus::Delete                   ,
      this,SLOT(RemovePeople()),true) ;
  }                                   ;
  if (item==ItemEditing) return       ;
  RemoveOldItem ( )                   ;
}

void N::PeopleView::RemoveOldItem(void)
{
  if (IsNull(ItemEditing)) return         ;
  if (IsNull(ItemWidget )) return         ;
  SUID uuid = ItemUuid(ItemEditing      ) ;
  removeItemWidget (ItemEditing)          ;
  if (uuid<=0) takeItem(row(ItemEditing)) ;
  ItemWidget  = NULL                      ;
  ItemEditing = NULL                      ;
}

void N::PeopleView::RemovePeople(void)
{
  if ( !isOwner ( ) ) return                ;
  UUIDs Uuids                               ;
  for (int i=0;i<count();i++)               {
    QListWidgetItem * it = item ( i )       ;
    SUID uuid = ItemUuid ( it )             ;
    if (it->isSelected()) Uuids << uuid     ;
  }                                         ;
  if (Uuids.count()<=0) return              ;
  LockGui          (            )           ;
  SqlConnection SC ( plan->sql  )           ;
  if (SC.open("PeopleView","RemovePeople")) {
    DetachActors   ( SC , Uuids )           ;
    SC . close     (            )           ;
  }                                         ;
  SC . remove      (            )           ;
  UnlockGui        (            )           ;
  pendingStartup   (            )           ;
}

bool N::PeopleView::FocusIn(void)
{
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Insert       , NewPeople       ( ) ) ;
  LinkAction        ( Search       , Find            ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Delete       , RemovePeople    ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( Paste        , Paste           ( ) ) ;
  LinkAction        ( GoHome       , PageHome        ( ) ) ;
  LinkAction        ( GoEnd        , PageEnd         ( ) ) ;
  LinkAction        ( PreviousPage , PageUp          ( ) ) ;
  LinkAction        ( NextPage     , PageDown        ( ) ) ;
  LinkAction        ( Font         , setFont         ( ) ) ;
  ALERT             ( Click                              ) ;
  return true                                              ;
}

void N::PeopleView::MovePeople(SUID atUuid,const UUIDs & Uuids)
{
  SUID            uuid                                   ;
  UUIDs           ITEMs = ItemUuids ( )                  ;
  ListWidgetItems mItems                                 ;
  foreach (uuid,Uuids)                                   {
    QListWidgetItem * it = uuidAt ( uuid )               ;
    mItems << it                                         ;
    takeItem ( row ( it ) )                              ;
    ITEMs . takeAt ( ITEMs . indexOf (uuid) )            ;
  }                                                      ;
  QListWidgetItem * rit   = uuidAt ( atUuid )            ;
  int               rid   = row    ( rit    )            ;
  for (int i=0;i<mItems.count();i++,rid++)               {
    insertItem ( rid , mItems [ i ] )                    ;
  }                                                      ;
  rid = ITEMs . indexOf ( atUuid )                       ;
  if (rid<0) rid = 0                                     ;
  foreach (uuid,Uuids)                                   {
    ITEMs . insert ( rid , uuid )                        ;
    rid++                                                ;
  }                                                      ;
  RMAPs Position                                         ;
  int xId = 1                                            ;
  foreach (uuid,ITEMs)                                   {
    Position [ uuid ] = xId                              ;
    xId ++                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  Bustle ( )                                             ;
  SqlConnection SC ( plan->sql )                         ;
  GroupItems GI ( plan )                                 ;
  GI.AutoMap    = true                                   ;
  GI.GroupTable = GI . LookTable ( t1,t2,relation      ) ;
  if (SC.open("PeopleView","MovePeople"))                {
    QString Q                                            ;
    if ( isFirst ( ) )                                   {
      foreach (uuid,ITEMs)                               {
        int Id = Position [ uuid ]                       ;
        Q = SC.sql.Update                                (
              GI.GroupTable                              ,
              ExactItem(first,uuid,t1,t2,relation)       ,
              1                                          ,
              "position"                               ) ;
        SC . Prepare ( Q               )                 ;
        SC . Bind    ( "position" , Id )                 ;
        SC . Exec    (                 )                 ;
      }                                                  ;
    } else
    if ( isSecond ( ))                                   {
      foreach (uuid,ITEMs)                               {
        int Id = Position [ uuid ]                       ;
        Q = SC.sql.Update                                (
              GI.GroupTable                              ,
              ExactItem(first,uuid,t1,t2,relation)       ,
              1                                          ,
              "reversal"                               ) ;
        SC . Prepare ( Q               )                 ;
        SC . Bind    ( "reversal" , Id )                 ;
        SC . Exec    (                 )                 ;
      }                                                  ;
    } else                                               {
      foreach (uuid,ITEMs)                               {
        int Id = Position [ uuid ]                       ;
        Id += 1000000000                                 ;
        Q = SC.sql.Update                                (
              PlanTable(PeopleUuid)                      ,
              SC.WhereUuid(uuid)                         ,
              1                                          ,
              "id"                                     ) ;
        SC . Prepare ( Q         )                       ;
        SC . Bind    ( "id" , Id )                       ;
        SC . Exec    (           )                       ;
      }                                                  ;
      foreach (uuid,ITEMs)                               {
        int Id = Position [ uuid ]                       ;
        Q = SC.sql.Update                                (
              PlanTable(PeopleUuid)                      ,
              SC.WhereUuid(uuid)                         ,
              1                                          ,
              "id"                                     ) ;
        SC . Prepare ( Q         )                       ;
        SC . Bind    ( "id" , Id )                       ;
        SC . Exec    (           )                       ;
      }                                                  ;
      Q = QString("alter table %1 auto_increment = %2 ;" )
          .arg(PlanTable(PeopleUuid)                     )
          .arg(ITEMs.count()+1                         ) ;
      SC . Query ( Q )                                   ;
    }                                                    ;
    SC.close()                                           ;
  }                                                      ;
  SC.remove()                                            ;
  ////////////////////////////////////////////////////////
  Vacancy  ( )                                           ;
  ALERT ( Done )                                         ;
}

void N::PeopleView::JoinCrowds(SUID before,const UUIDs & Uuids)
{
  if ( ! isOwner     ( )      ) return ;
  if ( Uuids . count ( ) <= 0 ) return ;
  dropAction = true                    ;
  //////////////////////////////////////
  UUIDs      U    = Uuids              ;
  N::VarArgs V                         ;
  N::Group   G                         ;
  G  . first      = first              ;
  G  . second     = 0                  ;
  G  . t1         = t1                 ;
  G  . t2         = t2                 ;
  G  . relation   = relation           ;
  G  . position   = before             ;
  G  . membership = 1.0                ;
  V << 10012                           ;
  G  . addParaments ( V     )          ;
  V << U . count    (       )          ;
  toVariants        ( U , V )          ;
  addSequence       ( V     )          ;
  //////////////////////////////////////
  dropAction = false                   ;
  emit TriggerCommand ( )              ;
}

void N::PeopleView::JoinActors (UUIDs & Uuids)
{
  JoinCrowd ( Uuids ) ;
}

void N::PeopleView::JoinCrowd(UUIDs & Uuids)
{
  if ( ! isOwner     ( )      ) return ;
  if ( Uuids . count ( ) <= 0 ) return ;
  dropAction = true                    ;
  //////////////////////////////////////
  UUIDs      U    = Uuids              ;
  N::VarArgs V                         ;
  N::Group   G                         ;
  G  . first      = first              ;
  G  . second     = 0                  ;
  G  . t1         = t1                 ;
  G  . t2         = t2                 ;
  G  . relation   = relation           ;
  G  . position   = 0                  ;
  G  . membership = 1.0                ;
  V << 10012                           ;
  G  . addParaments ( V     )          ;
  V << U . count    (       )          ;
  toVariants        ( U , V )          ;
  addSequence       ( V     )          ;
  //////////////////////////////////////
  dropAction = false                   ;
  emit TriggerCommand ( )              ;
}

void N::PeopleView::JoinImage(SUID people,const QMimeData * mime)
{
  PictureManager  PM             ( plan ) ;
  QImage * I    = PM . MimeImage ( mime ) ;
  if (IsNull(I)) return                   ;
  SUID puid    = 0                        ;
  bool success = false                    ;
  success = PM . Import ( *I , puid )     ;
  delete I                                ;
  I = NULL                                ;
  if ( puid <= 0 ) return                 ;
  SqlConnection SC ( plan->sql )          ;
  if ( SC . open ( FunctionString ) )     {
    UUIDs Uuids                           ;
    Uuids << puid                         ;
    GroupItems::Join                      (
      SC                                  ,
      people                              ,
      Types::People                       ,
      Types::Picture                      ,
      Groups::Subordination               ,
      0                                   ,
      Uuids                             ) ;
    SC.close()                            ;
  }                                       ;
  SC.remove()                             ;
  ALERT ( Done )                          ;
}

bool N::PeopleView::dropText(QWidget * source,QPointF psf,const QString & text)
{
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  return true                            ;
}

bool N::PeopleView::dropUrls(QWidget * source,QPointF psf,const QList<QUrl> & urls)
{
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  return true                            ;
}

bool N::PeopleView::dropImage(QWidget * source,QPointF psf,const QImage & image)
{
  if ( source == this ) return true      ;
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (IsNull(atItem)) return true        ;
  SUID people = nListUuid(atItem)        ;
  PictureManager PM ( plan )             ;
  SUID puid    = 0                       ;
  bool success = false                   ;
  QImage I = image                       ;
  success = PM . Import ( I , puid )     ;
  if ( puid <= 0 ) return true           ;
  dropAction = true                      ;
  UUIDs Uuids                            ;
  Uuids << puid                          ;
  SqlConnection SC ( plan -> sql )       ;
  GroupItems    GI ( plan        )       ;
  if ( SC . open ( FunctionString ) )    {
    GI . AutoMap    = true               ;
    GI . GroupTable = GI . LookTable     (
      Types  :: People                   ,
      Types  :: Picture                  ,
      Groups :: Subordination          ) ;
    GI . Join                            (
      SC                                 ,
      people                             ,
      Types::People                      ,
      Types::Picture                     ,
      Groups::Subordination              ,
      0                                  ,
      Uuids                            ) ;
    SC.close()                           ;
  }                                      ;
  SC.remove()                            ;
  ALERT ( Done )                         ;
  dropAction = false                     ;
  return true                            ;
}

bool N::PeopleView::dropHtml(QWidget * source,QPointF psf,const QString & html)
{
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  return true                            ;
}

bool N::PeopleView::dropTags(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if ( source == this      ) return true ;
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if ( IsNull ( atItem )   ) return true ;
  SUID people = nListUuid(atItem)        ;
  if ( people <= 0         ) return true ;
  dropAction = true                      ;
  VarArgs args                           ;
  args << people                         ;
  toVariants ( Uuids , args )            ;
  dropAction = false                     ;
  start ( 80003 , args )                 ;
  return true                            ;
}

bool N::PeopleView::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if ( source == this ) return true         ;
  QPoint pos = psf.toPoint()                ;
  QListWidgetItem * atItem = itemAt(pos)    ;
  if (IsNull(atItem)) return true           ;
  ///////////////////////////////////////////
  SUID people = nListUuid(atItem)           ;
  dropAction = true                         ;
  ///////////////////////////////////////////
  UUIDs      U    = Uuids                   ;
  N::VarArgs V                              ;
  N::Group   G                              ;
  G  . first      = people                  ;
  G  . second     = 0                       ;
  G  . t1         = Types  :: People        ;
  G  . t2         = Types  :: Picture       ;
  G  . relation   = Groups :: Subordination ;
  G  . position   = 0                       ;
  G  . membership = 1.0                     ;
  V << 10011                                ;
  G  . addParaments ( V     )               ;
  V << U.count()                            ;
  toVariants        ( U , V )               ;
  addSequence       ( V     )               ;
  ///////////////////////////////////////////
  dropAction = false                        ;
  emit TriggerCommand ( )                   ;
  return true                               ;
}

bool N::PeopleView::dropPeople(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  QListWidgetItem * atItem = itemAt(psf.toPoint()) ;
  if ( source == this )                            {
    if (IsNull(atItem)) return true                ;
    UUIDs   Puid = Uuids                           ;
    SUID    auid = nListUuid(atItem)               ;
    VarArgs V                                      ;
    dropAction = true                              ;
    V << 11001                                     ;
    V << auid                                      ;
    V << Puid.count()                              ;
    toVariants ( Puid , V )                        ;
    dropAction = false                             ;
    addSequence         ( V )                      ;
    emit TriggerCommand (   )                      ;
  } else                                           {
    if ( ! isOwner ( ) ) return true               ;
    SUID  auid = 0                                 ;
    UUIDs ITEMs = ItemUuids()                      ;
    UUIDs AUIDs                                    ;
    SUID  uuid                                     ;
    if (NotNull(atItem)) auid = nListUuid(atItem)  ;
    foreach (uuid,Uuids)                           {
      if (!ITEMs.contains(uuid)) AUIDs << uuid     ;
    }                                              ;
    if (AUIDs.count()<=0) return true              ;
    JoinCrowds ( auid , AUIDs )                    ;
  }                                                ;
  return true                                      ;
}

bool N::PeopleView::dropAlbums(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if ( source == this       ) return true ;
  QPoint pos = psf.toPoint()              ;
  QListWidgetItem * atItem = itemAt(pos)  ;
  if ( IsNull ( atItem )    ) return true ;
  /////////////////////////////////////////
  SUID people = nListUuid(atItem)         ;
  if ( people <= 0          ) return true ;
  dropAction = true                       ;
  VarArgs args                            ;
  args << people                          ;
  toVariants ( Uuids , args )             ;
  dropAction = false                      ;
  start ( 80002 , args )                  ;
  return true                             ;
}

bool N::PeopleView::dropGender(QWidget * source,QPointF psf,const SUID gender)
{
  if ( source == this       ) return true ;
  QPoint pos = psf.toPoint()              ;
  QListWidgetItem * atItem = itemAt(pos)  ;
  if ( IsNull ( atItem )    ) return true ;
  /////////////////////////////////////////
  SUID people = nListUuid(atItem)         ;
  if ( people <= 0          ) return true ;
  dropAction = true                       ;
  VarArgs args                            ;
  args << people                          ;
  args << gender                          ;
  dropAction = false                      ;
  start ( 80001 , args )                  ;
  return true                             ;
}

void N::PeopleView::JoinTags(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 2 ) return          ;
  SUID  people = d -> Arguments [ 0 ] . toULongLong ( ) ;
  UUIDs U                                               ;
  FromVariants ( 1 , d -> Arguments , U )               ;
  ///////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                      ;
  GroupItems    GI ( plan        )                      ;
  ///////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                   {
    UUIDs Puids                                         ;
    SUID  uuid                                          ;
    Puids << people                                     ;
    GI . AutoMap    = true                              ;
    GI . GroupTable = GI . LookTable                    (
                        Types  :: Tag                   ,
                        Types  :: People                ,
                        Groups :: Subordination       ) ;
    foreach ( uuid , U )                                {
      GI . Join                                         (
        SC                                              ,
        uuid                                            ,
        Types::Tag                                      ,
        Types::People                                   ,
        Groups::Subordination                           ,
        0                                               ,
        Puids                                         ) ;
    }                                                   ;
    SC . close  (      )                                ;
  }                                                     ;
  SC   . remove (      )                                ;
  ///////////////////////////////////////////////////////
  ALERT         ( Done )                                ;
}

void N::PeopleView::JoinAlbums(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 2 ) return          ;
  SUID  people = d -> Arguments [ 0 ] . toULongLong ( ) ;
  UUIDs U                                               ;
  FromVariants ( 1 , d -> Arguments , U )               ;
  ///////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                      ;
  GroupItems    GI ( plan        )                      ;
  ///////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                   {
    UUIDs Puids                                         ;
    SUID  album                                         ;
    Puids << people                                     ;
    GI . AutoMap    = true                              ;
    GI . GroupTable = GI . LookTable                    (
                        Types  :: Album                 ,
                        Types  :: People                ,
                        Groups :: Subordination       ) ;
    foreach ( album , U )                               {
      GI . Join                                         (
        SC                                              ,
        album                                           ,
        Types::Album                                    ,
        Types::People                                   ,
        Groups::Subordination                           ,
        0                                               ,
        Puids                                         ) ;
    }                                                   ;
    SC . close  (      )                                ;
  }                                                     ;
  SC   . remove (      )                                ;
  ///////////////////////////////////////////////////////
  ALERT         ( Done )                                ;
}

void N::PeopleView::JoinGender(ThreadData * d)
{
  if ( d -> Arguments . count ( ) < 2 ) return         ;
  SUID gender = d -> Arguments [ 0 ] . toULongLong ( ) ;
  SUID people = d -> Arguments [ 1 ] . toULongLong ( ) ;
  //////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                     ;
  GroupItems    GI ( plan        )                     ;
  //////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                  {
    UUIDs Puids                                        ;
    Puids << gender                                    ;
    GI . AutoMap    = true                             ;
    GI . GroupTable = GI . LookTable                   (
                        Types  :: People               ,
                        Types  :: Gender               ,
                        Groups :: Sexuality          ) ;
    GI . FullDetach                                    (
      SC                                               ,
      people                                           ,
      Types  :: People                                 ,
      Types  :: Gender                                 ,
      Groups :: Sexuality                            ) ;
    GI . Join                                          (
      SC                                               ,
      people                                           ,
      Types  :: People                                 ,
      Types  :: Gender                                 ,
      Groups :: Sexuality                              ,
      0                                                ,
      Puids                                          ) ;
    SC . close  (      )                               ;
  }                                                    ;
  SC   . remove (      )                               ;
  //////////////////////////////////////////////////////
  ALERT         ( Done )                               ;
}

void N::PeopleView::Update(QWidget * widget,SUID uuid)
{
  Add ( uuid ) ;
}

void N::PeopleView::RelatedItems(SUID uuid,QString title)
{
  UUIDs   ResultUuids                     ;
  RMAPs   ResultTypes                     ;
  NAMEs   ResultNames                     ;
  int     ptype = Types::People           ;
  emit assignEnabling ( false           ) ;
  GroupItems GI ( plan )                  ;
  EnterSQL   ( SC , plan->sql )           ;
    QString Q                             ;
    SUID    puid                          ;
    Q = SC.sql.SelectFrom                 (
          "name"                          ,
          PlanTable(NameMaps)             ,
          SC.WhereUuid(uuid)            ) ;
    SqlLoopNow ( SC , Q )                 ;
      SUID kuid = SC.Uuid(0)              ;
      if (!ResultUuids.contains(kuid))    {
        ResultUuids << kuid               ;
      }                                   ;
    SqlLoopErr ( SC , Q )                 ;
    SqlLoopEnd ( SC , Q )                 ;
    Q = SC.sql.SelectFrom                 (
          "second"                        ,
          PlanTable(Groups)               ,
    QString("where first = %1 and t1 = %2")
          .arg(uuid).arg(ptype)         ) ;
    SqlLoopNow ( SC , Q )                 ;
      SUID kuid = SC.Uuid(0)              ;
      if (!ResultUuids.contains(kuid))    {
        ResultUuids << kuid               ;
      }                                   ;
    SqlLoopErr ( SC , Q )                 ;
    SqlLoopEnd ( SC , Q )                 ;
    Q = SC.sql.SelectFrom                 (
          "first"                         ,
          PlanTable(Groups)               ,
   QString("where second = %1 and t2 = %2")
          .arg(uuid).arg(ptype)         ) ;
    SqlLoopNow ( SC , Q )                 ;
      SUID kuid = SC.Uuid(0)              ;
      if (!ResultUuids.contains(kuid))    {
        ResultUuids << kuid               ;
      }                                   ;
    SqlLoopErr ( SC , Q )                 ;
    SqlLoopEnd ( SC , Q )                 ;
    foreach (puid,ResultUuids)            {
      int t = 0                           ;
      Q = SC.sql.SelectFrom               (
            "type"                        ,
            PlanTable(MajorUuid)          ,
            SC.WhereUuid(puid)          ) ;
      if (SC.Fetch(Q)) t = SC.Int(0)      ;
      ResultTypes[puid] = t               ;
      if (t==Types::Name)       {
        Q = SC.sql.SelectFrom             (
              "name"                      ,
              PlanTable(Names)            ,
              SC.WhereUuid(puid)        ) ;
        if (SC.Fetch(Q))                  {
          ResultNames[puid]=SC.String(0)  ;
        }                                 ;
      } else                              {
        Q = SC.sql.SelectFrom             (
              "id"                        ,
              PlanTable(Names)            ,
              QString("where uuid = %1 and language = %2")
              .arg(puid).arg(vLanguageId)
            )                             ;
        if (!SC.Fetch(Q))                 {
          QStringList Ns                  ;
          Ns = GI.Names(SC,puid)          ;
          if (Ns.count()>0)               {
            ResultNames[puid] = Ns[0]     ;
          }                               ;
        } else                            {
          ResultNames[puid] = SC.getName  (
            PlanTable(Names)              ,
            "uuid"                        ,
            vLanguageId                   ,
            puid                        ) ;
        }                                 ;
      }                                   ;
    }                                     ;
  LeaveSQL   ( SC , plan->sql )           ;
  emit assignEnabling ( true            ) ;
  if (ResultUuids.count()<=0)             {
    ALERT ( Error )                       ;
  } else                                  {
    ALERT ( Done  )                       ;
    emit UuidItems                        (
           ResultUuids                    ,
           ResultTypes                    ,
           ResultNames                    ,
           title                        ) ;
  }                                       ;
}

void N::PeopleView::doTranslations(void)
{
  UUIDs   Uuids = ItemUuids   ( ) ;
  QString title = windowTitle ( ) ;
  emit Translations(title,Uuids)  ;
}

void N::PeopleView::setPositions(void)
{
  UUIDs Uuids = ItemUuids (    )                         ;
  nDropOut ( Uuids.count() <= 0)                         ;
  int    ID                                              ;
  qint64 Value = 0                                       ;
  bool Go = true                                         ;
  ID = plan->Progress(windowTitle(),tr("%v/%m people"))  ;
  Time :: skip ( 500 )                                   ;
  plan -> Start    ( ID , &Value , &Go           )       ;
  plan -> setRange ( ID , 0      , Uuids.count() )       ;
  GroupItems GI ( plan )                                 ;
  GI.AutoMap    = true                                   ;
  GI.GroupTable = GI . LookTable ( t1,t2,relation      ) ;
  EnterSQL ( SC , plan->sql )                            ;
    QString Q                                            ;
    SUID    uuid                                         ;
    int     position = 0                                 ;
    if ( isFirst ( ) )                                   {
      foreach (uuid,Uuids)                               {
        ++Value                                          ;
        ++position                                       ;
        Q = SC.sql.Update                                (
              GI.GroupTable                              ,
              ExactItem(first,uuid,t1,t2,relation)       ,
              1                                          ,
              "position"                               ) ;
        SC . Prepare ( Q                               ) ;
        SC . Bind    ( "position" , position           ) ;
        SC . Exec    (                                 ) ;
      }                                                  ;
    } else
    if ( isSecond ( ) )                                  {
      foreach (uuid,Uuids)                               {
        ++Value                                          ;
        ++position                                       ;
        Q = SC.sql.Update                                (
              GI.GroupTable                              ,
              ExactItem(first,uuid,t1,t2,relation)       ,
              1                                          ,
              "reversal"                               ) ;
        SC . Prepare ( Q                               ) ;
        SC . Bind    ( "reversal" , position           ) ;
        SC . Exec    (                                 ) ;
      }                                                  ;
    } else                                               {
      foreach (uuid,Uuids)                               {
        ++Value                                          ;
        ++position                                       ;
        Q = SC.sql.Update                                (
              PlanTable(PeopleUuid)                      ,
              SC.sql.Where                               (
                1                                        ,
                "uuid"                                 ) ,
                1                                        ,
                "id"                                   ) ;
        SC . Prepare ( Q                               ) ;
        SC . Bind    ( "id"   , position               ) ;
        SC . Bind    ( "uuid" , uuid                   ) ;
        SC . Exec    (                                 ) ;
      }                                                  ;
    }                                                    ;
  LeaveSQL ( SC , plan->sql )                            ;
  plan -> Finish ( ID )                                  ;
  ALERT    ( Done           )                            ;
}

void N::PeopleView::CountPictures(void)
{
  UUIDs Uuids = ItemUuids (    )                       ;
  nDropOut ( Uuids.count() <= 0)                       ;
  ScopedProgress Progress ( plan,tr("%v/%m people"),5) ;
  EnterSQL ( SC , plan->sql )                          ;
    SUID    uuid                                       ;
    Progress . setRange ( 0 , Uuids.count() )          ;
    Progress . show     (                   )          ;
    Progress . Start    (                   )          ;
    foreach (uuid,Uuids)                               {
      ++Progress                                       ;
      QListWidgetItem * it = uuidAt(uuid)              ;
      if ( NotNull(it) ) PeoplePictures ( SC , it )    ;
    }                                                  ;
    Progress . Finish ( )                              ;
  LeaveSQL ( SC , plan->sql )                          ;
  ALERT    ( Done           )                          ;
}

void N::PeopleView::CountPossible(void)
{
  UUIDs Uuids = ItemUuids (    )                       ;
  nDropOut ( Uuids.count() <= 0)                       ;
  ScopedProgress Progress ( plan,tr("%v/%m people"),5) ;
  EnterSQL ( SC , plan->sql )                          ;
    QString Q                                          ;
    SUID    uuid                                       ;
    Progress . setRange ( 0 , Uuids.count() )          ;
    Progress . show     (                   )          ;
    Progress . Start    (                   )          ;
    foreach (uuid,Uuids)                               {
      ++Progress                                       ;
      QListWidgetItem * it = uuidAt(uuid)              ;
      if ( NotNull(it) ) PossiblePictures ( SC , it )  ;
    }                                                  ;
    Progress . Finish ( )                              ;
  LeaveSQL ( SC , plan->sql )                          ;
  ALERT    ( Done           )                          ;
}

void N::PeopleView::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::PeopleView::Paste(QString text)
{
  QStringList S = text.split("\n")                                       ;
  QString     T                                                          ;
  S = File :: PurifyLines ( S )                                          ;
  if (S.count()<=0) return                                               ;
  PeopleManager PM ( plan )                                              ;
  EnterSQL ( SC , plan->sql )                                            ;
    foreach ( T , S )                                                    {
      bool appended = false                                              ;
      if (isFunction(1101))                                              {
        SUID u = PM . FindPeople    ( SC , T                           ) ;
        if (u>0)                                                         {
          JoinActor                 ( SC , u                           ) ;
          NewListWidgetItem         ( it                               ) ;
          it -> setData             ( Qt::UserRole , u                 ) ;
          it -> setText             ( T                                ) ;
          it -> setIcon             ( plan->Icon(Types::FaceShape,1,0) ) ;
          if ( Qt::DescendingOrder == LimitValues [ 11 ]               ) {
            QListWidget::insertItem ( 0 , it                           ) ;
          } else                                                         {
            QListWidget::addItem    ( it                               ) ;
          }                                                              ;
          plan -> processEvents     (                                  ) ;
          appended = true                                                ;
        }                                                                ;
      }                                                                  ;
      if (!appended) addPeople ( SC , T )                                ;
    }                                                                    ;
  LeaveSQL ( SC , plan->sql )                                            ;
  ALERT    ( Done           )                                            ;
}

void N::PeopleView::addPeople(SqlConnection & SC,QString name)
{
  PeopleManager PM          ( plan                             ) ;
  SUID u = PM . addPeople   ( SC , vLanguageId , name          ) ;
  JoinActor                 ( SC , u                           ) ;
  NewListWidgetItem         ( it                               ) ;
  it -> setData             ( Qt::UserRole , u                 ) ;
  it -> setText             ( name                             ) ;
  it -> setIcon             ( plan->Icon(Types::FaceShape,1,0) ) ;
  if ( Qt::DescendingOrder == LimitValues [ 11 ]               ) {
    QListWidget::insertItem ( 0 , it                           ) ;
  } else                                                         {
    QListWidget::addItem    ( it                               ) ;
  }                                                              ;
  plan -> processEvents     (                                  ) ;
}

void N::PeopleView::ExportNames(void)
{
  QString filename = QFileDialog::getSaveFileName          (
                       this                                ,
                       tr("Export people names into file") ,
                       plan->Temporary("")                 ,
                       tr("Plain text file(*.txt)")      ) ;
  nDropOut ( filename.length() <= 0 )                      ;
  //////////////////////////////////////////////////////////
  QStringList L                                            ;
  for (int i=0;i<count();i++)                              {
    QListWidgetItem * it = item(i)                         ;
    L << it->text()                                        ;
  }                                                        ;
  nDropOut ( L.count() <= 0 )                              ;
  //////////////////////////////////////////////////////////
  QString    M = L.join("\r\n")                            ;
  QByteArray B = M.toUtf8()                                ;
  QFile      F(filename)                                   ;
  if (F.open(QIODevice::WriteOnly))                        {
    F . write ( B     )                                    ;
    F . close (       )                                    ;
    ALERT     ( Done  )                                    ;
  } else                                                   {
    ALERT     ( Error )                                    ;
  }                                                        ;
}

void N::PeopleView::ExportAllNames(void)
{
  QString filename = QFileDialog::getSaveFileName          (
                       this                                ,
                       tr("Export people names into file") ,
                       plan->Temporary("")                 ,
                       tr("Plain text file(*.txt)")      ) ;
  nDropOut ( filename.length() <= 0 )                      ;
  //////////////////////////////////////////////////////////
  QStringList L                                            ;
  EnterSQL(SC,plan->sql)                                   ;
    QString Q                                              ;
    UUIDs   Uuids = ItemUuids()                            ;
    UUIDs   Nuids                                          ;
    SUID    uuid                                           ;
    SUID    nuid                                           ;
    foreach (uuid,Uuids)                                   {
      Nuids . clear ( )                                    ;
      Q = SC.sql.SelectFrom                                (
            "name"                                         ,
            PlanTable(NameMaps)                            ,
            QString( "where uuid = %1 order by prefer asc" )
            .arg(uuid)                                   ) ;
      SqlLoopNow ( SC , Q )                                ;
        Nuids << SC.Uuid(0)                                ;
      SqlLoopErr ( SC , Q )                                ;
      SqlLoopEnd ( SC , Q )                                ;
      if (Nuids.count()>0)                                 {
        foreach (nuid,Nuids)                               {
          Q = SC.sql.SelectFrom                            (
                "name,language"                            ,
                PlanTable(Names)                           ,
                SC.WhereUuid(nuid)                       ) ;
          if (SC.Fetch(Q))                                 {
            QString name = SC . String ( 0 )               ;
            int     lang = SC . Int    ( 1 )               ;
            QString Lann = plan->languages[lang]           ;
            QString M                                      ;
            M = QString("%1\t=>\t%2").arg(name).arg(Lann)  ;
            L << M                                         ;
          }                                                ;
        }                                                  ;
      }                                                    ;
    }                                                      ;
  LeaveSQL(SC,plan->sql)                                   ;
  nDropOut ( L.count() <= 0 )                              ;
  //////////////////////////////////////////////////////////
  QString    M = L.join("\r\n")                            ;
  QByteArray B = M.toUtf8()                                ;
  QFile      F(filename)                                   ;
  if (F.open(QIODevice::WriteOnly))                        {
    F . write ( B     )                                    ;
    F . close (       )                                    ;
    ALERT     ( Done  )                                    ;
  } else                                                   {
    ALERT     ( Error )                                    ;
  }                                                        ;
}

void N::PeopleView::run(int Type,ThreadData * data)
{
  if ( ! IsContinue ( data ) ) return            ;
  VarArgs  V      = data -> Arguments            ;
  bool     loaded = false                        ;
  QString  TT                                    ;
  N::Group group                                 ;
  UUIDs    U                                     ;
  SUID     u                                     ;
  int      t                                     ;
  startLoading              (                  ) ;
  switch                    ( Type             ) {
    case 10002                                   :
      TT = tr("Counting people...")              ;
      emit assignToolTip    ( TT               ) ;
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      FetchTOTALs           (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
      addSequence           ( 10003            ) ;
      emit TriggerCommand   (                  ) ;
    break                                        ;
    case 10003                                   :
      TT = tr("Loading people...")               ;
      emit assignToolTip    ( TT               ) ;
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      FetchUUIDs            (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
      if   ( LimitValues[1] > 0                ) {
        if ( UuidMaps . contains ( 10001 )     ) {
          UUIDs U = UuidMaps [ 10001 ]           ;
          if (U.count()>0)                       {
            V  . clear          (              ) ;
            V << 10004                           ;
            V << U.count        (              ) ;
            toVariants          ( U , V        ) ;
            addSequence         ( V            ) ;
            emit TriggerCommand (              ) ;
            loaded = true                        ;
          }                                      ;
        }                                        ;
      }                                          ;
      ////////////////////////////////////////////
      if ( ! loaded )                            {
        TT = tr("No people found")               ;
        emit assignToolTip  ( TT               ) ;
      }                                          ;
    break                                        ;
    case 50002                                   :
      TT = tr("Counting people...")              ;
      emit assignToolTip    ( TT               ) ;
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      ActorTOTALs           (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
      addSequence           ( 50003            ) ;
      emit TriggerCommand   (                  ) ;
    break                                        ;
    case 50003                                   :
      TT = tr("Loading people...")               ;
      emit assignToolTip    ( TT               ) ;
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      ActorUUIDs            (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
      if   ( LimitValues[1] > 0                ) {
        if ( UuidMaps . contains ( 10001 )     ) {
          UUIDs U = UuidMaps [ 10001 ]           ;
          if (U.count()>0)                       {
            V  . clear          (              ) ;
            V << 10004                           ;
            V << U.count        (              ) ;
            toVariants          ( U , V        ) ;
            addSequence         ( V            ) ;
            emit TriggerCommand (              ) ;
            loaded = true                        ;
          }                                      ;
        }                                        ;
      }                                          ;
      ////////////////////////////////////////////
      if ( ! loaded )                            {
        TT = tr("No people found")               ;
        emit assignToolTip  ( TT               ) ;
      }                                          ;
    break                                        ;
    case 10011                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if ( group.first > 0 )                     {
        JoinGroupThread     ( group , U        ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 10012                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if ( group.first > 0 )                     {
        JoinGroupThread     ( group , U        ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
      ////////////////////////////////////////////
      pendingStartup        (                  ) ;
    break                                        ;
    case 10013                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if ( group.first > 0 )                     {
        JoinOwnerThread     ( group , U        ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 10014                                   :
      emit OnBusy           (                  ) ;
      ArgsToGroup           ( 0 , V , group    ) ;
      ArgsToUuids           ( 7 , V , U        ) ;
      if ( group.first > 0 )                     {
        JoinOwnerThread     ( group , U        ) ;
        IsCompleted         (                  ) ;
      }                                          ;
      emit GoRelax          (                  ) ;
      ////////////////////////////////////////////
      pendingStartup        (                  ) ;
    break                                        ;
    case 11001                                   :
      u = V [ 0 ] . toULongLong (              ) ;
      ArgsToUuids           ( 1 , V , U        ) ;
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      MovePeople            ( u , U            ) ;
      stopLoading           (                  ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
      addSequence           ( 12001            ) ;
      emit TriggerCommand   (                  ) ;
    break                                        ;
    case 20002                                   :
      emit OnBusy           (                  ) ;
      NamesThreads          ( 20101            ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 20003                                   :
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      setPositions          (                  ) ;
      stopLoading           (                  ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 20004                                   :
      emit OnBusy           (                  ) ;
      startLoading          (                  ) ;
      FetchTOTALs           (                  ) ;
      stopLoading           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 20101                                   :
      emit OnBusy           (                  ) ;
      NamesThread           ( V                ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 30001                                   :
      ArgsToGroup           ( 0 , V , group    ) ;
      emit OnBusy           (                  ) ;
      SnapView::IconsThread ( group            ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 30002                                   :
      if (V.count()>0)                           {
        t = V [ 0 ] . toInt (                  ) ;
        emit OnBusy         (                  ) ;
        DetailsThread       ( t                ) ;
        IsCompleted         (                  ) ;
        emit GoRelax        (                  ) ;
      }                                          ;
    break                                        ;
    case 30003                                   :
      emit OnBusy           (                  ) ;
      SnapView::IconsThread ( V                ) ;
      IsCompleted           (                  ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 80001                                   :
      emit OnBusy           (                  ) ;
      JoinGender            ( data             ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 80002                                   :
      emit OnBusy           (                  ) ;
      JoinAlbums            ( data             ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
    case 80003                                   :
      emit OnBusy           (                  ) ;
      JoinTags              ( data             ) ;
      emit GoRelax          (                  ) ;
    break                                        ;
  }                                              ;
  stopLoading               (                  ) ;
}

bool N::PeopleView::RunCommand(VarArgs & arguments)
{
  if (arguments.count()<1) return false  ;
  VarArgs  V = arguments                 ;
  N::Group group                         ;
  UUIDs    U                             ;
  int      c = V [ 0 ] . toInt ( )       ;
  switch         ( c     )               {
    case 10001                           :
      startup    (       )               ;
    break                                ;
    case 10003                           :
    case 50003                           :
      start      ( c     )               ;
    break                                ;
    case 10004                           :
      ArgsToUuids ( 1 , V , U )          ;
      plan -> processEvents ( )          ;
      if (U.count()>0)                   {
        View     ( U                   ) ;
        plan -> processEvents (        ) ;
        setFocus ( Qt::TabFocusReason  ) ;
      }                                  ;
    break                                ;
    case 10002                           :
    case 10011                           :
    case 10012                           :
    case 10013                           :
    case 10014                           :
    case 11001                           :
    case 20002                           :
    case 20003                           :
    case 20004                           :
    case 20101                           :
    case 30001                           :
    case 30002                           :
    case 30003                           :
    case 50002                           :
      V . takeAt ( 0     )               ;
      start      ( c , V )               ;
    break                                ;
    case 40001                           :
      ArgsToGroup    ( 1 , V , group  )  ;
      GoIconsThreads ( 30003 , group  )  ;
    break                                ;
    case 12001                           :
      ArgsToUuids ( 1 , V , U )          ;
      plan -> processEvents ( )          ;
      setSelections ( U )                ;
    break                                ;
    default                              :
    return false                         ;
  }                                      ;
  return true                            ;
}

bool N::PeopleView::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                                               ;
  UUIDs             Uuids                                                     ;
  SUID              uuid     = 0                                              ;
  QString           name     = ""                                             ;
  QAction         * aa       = NULL                                           ;
  QListWidgetItem * it       = itemAt(pos)                                    ;
  QMenu           * pm       = NULL                                           ;
  QMenu           * im       = NULL                                           ;
  QMenu           * fm       = NULL                                           ;
  QMenu           * sm       = NULL                                           ;
  QMenu           * ms       = NULL                                           ;
  QMenu           * mp       = NULL                                           ;
  QMenu           * mx       = NULL                                           ;
  int               StartId  = 0                                              ;
  int               PageSize = 0                                              ;
  Uuids                      = Selected()                                     ;
  /////////////////////////////////////////////////////////////////////////////
  if (NotNull(it))                                                            {
    uuid = nListUuid ( it )                                                   ;
    name = it->text  (    )                                                   ;
  }                                                                           ;
  /////////////////////////////////////////////////////////////////////////////
  if (NotNull(it))                                                            {
    aa  = mm . add     ( 101 , it->text() )                                   ;
    aa -> setEnabled   ( false            )                                   ;
    mm  . addSeparator (                  )                                   ;
  }                                                                           ;
  /////////////////////////////////////////////////////////////////////////////
  if (isFunction(3101))                                                       {
    mp = PageMenu ( mm,tr("Total %1 people"),StartId,PageSize )               ;
  }                                                                           ;
  /////////////////////////////////////////////////////////////////////////////
  mm . add ( 401 , tr("New person") )                                         ;
  if (isLoading())                                                            {
    mm.add(603,tr("Stop loading icons" ) )                                    ;
  } else                                                                      {
    if (isStopped() && isFunction(3101) ) mm.add(602,tr("Reload" )  )         ;
    if (Uuids.count()>1) mm.add(621,tr("Merge selected people"))              ;
  }                                                                           ;
  mm . addSeparator ( )                                                       ;
  mm . add(211,tr("Check existance before append people"),true,isFunction(1101)) ;
  if (isFirst()) mm . add(221,tr("People listings editor"))                   ;
  mm . addSeparator ( )                                                       ;
  if (NotNull(it))                                                            {
    if (canDelete()) mm . add(402,tr("Remove this person"))                   ;
    mm   . addSeparator ( )                                                   ;
    mm   . add(201,tr("Edit"      ))                                          ;
    mm   . add(202,tr("Edit names"))                                          ;
    mm   . addSeparator ( )                                                   ;
    pm   = mm . addMenu(tr("Profile" ))                                       ;
    fm   = mm . addMenu(tr("Features"))                                       ;
    im   = mm . addMenu(tr("Internet"))                                       ;
    mm   . add(pm,701,tr("Personal documents"))                               ;
    mm   . add(pm,702,tr("Personal gallery"  ))                               ;
    mm   . add(pm,703,tr("Video clips"       ))                               ;
    mm   . addSeparator (pm                   )                               ;
    mm   . add(pm,704,tr("Potential pictures"))                               ;
    mm   . add(pm,705,tr("Subgroups"         ))                               ;
    mm   . addSeparator (pm                   )                               ;
    mm   . add(pm,711,tr("All related items" ))                               ;
    mm   . add(im,801,tr("Search name via Google"       ))                    ;
    mm   . add(im,802,tr("Image search via Google image"))                    ;
    mm   . addSeparator (im                              )                    ;
    mm   . add(im,803,tr("Search name by customized engine" ))                ;
    mm   . add(im,804,tr("Search image by customized engine"))                ;
    mm   . addSeparator (im                                  )                ;
    mm   . add(im,805,QIcon(":/images/bookmarks.png")                         ,
                      tr("Personal bookmarks")                              ) ;
    mm   . add(fm,811,tr("Personality"))                                      ;
    mm   . addSeparator (fm                                  )                ;
    mm   . add(fm,821,tr("3D Models"))                                        ;
    mm   . addSeparator (fm                                  )                ;
    mm   . add(fm,812,tr("Faces"))                                            ;
    mm   . add(fm,813,tr("Measurements"))                                     ;
    mm   . add(fm,814,tr("Body" ))                                            ;
    mm   . add(fm,815,tr("Tatto" ))                                           ;
    mm   . addSeparator (fm                                  )                ;
    mm   . add(fm,816,tr("Possible faces"))                                   ;
    mm   . addSeparator ( )                                                   ;
    mm   . add(301,tr("Refresh people"))                                      ;
    mm   . addSeparator()                                                     ;
  }                                                                           ;
  /////////////////////////////////////////////////////////////////////////////
  sm = mm . addMenu ( tr("Statistics" )                     )                 ;
  mm . add          ( sm , 611 , tr("Pictures"         )    )                 ;
  mm . add          ( sm , 612 , tr("Possible pictures")    )                 ;
  mm . addSeparator ( sm                                    )                 ;
  mm . add          ( sm , 613 , tr("Export people names to file") )          ;
  mm . add          ( sm , 614 , tr("Export people names in all languages") ) ;
  /////////////////////////////////////////////////////////////////////////////
  ms = mm . addMenu ( tr("Adjustments")                     )                 ;
  mx = mm . addMenu ( tr("Selections" )                     )                 ;
  mm . add     ( ms , 501 , tr("Ascending Sort"           ) )                 ;
  mm . add     ( ms , 502 , tr("Descending Sort"          ) )                 ;
  mm . addSeparator ( ms )                                                    ;
  mm . add     ( ms , 604 , tr("Multilingual translations") )                 ;
  mm . add     ( ms , 605 , tr("Set people positions"     ) )                 ;
  if ( isFirst ( ) )                                                          {
    mm . addSeparator ( ms )                                                  ;
    mm . add   ( ms , 606 , tr("Assign folder language"   ) )                 ;
  }                                                                           ;
  /////////////////////////////////////////////////////////////////////////////
  AdjustMenu     ( mm , ms )                                                  ;
  SelectionsMenu ( mm , mx )                                                  ;
  DockingMenu    ( mm      )                                                  ;
  /////////////////////////////////////////////////////////////////////////////
  mm . setFont   ( plan )                                                     ;
  aa = mm . exec (      )                                                     ;
  /////////////////////////////////////////////////////////////////////////////
  if (IsNull(aa))                                                             {
    if (isFunction(3101))                                                     {
      PageChanged(StartId,PageSize)                                           ;
    }                                                                         ;
    return true                                                               ;
  }                                                                           ;
  /////////////////////////////////////////////////////////////////////////////
  if ( RunDocking     ( mm , aa ) ) return true                               ;
  if ( RunAdjustment  ( mm , aa ) ) return true                               ;
  if ( RunSelections  ( mm , aa ) ) return true                               ;
  /////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                        {
    case 201                                                                  :
      if ( uuid > 0 ) emit Command     ( uuid )                               ;
    return true                                                               ;
    case 202                                                                  :
      if ( uuid > 0 ) emit PeopleNames ( this , uuid , name )                 ;
    return true                                                               ;
    case 211                                                                  :
      setFunction ( 1101 , aa -> isChecked ( ) )                              ;
    return true                                                               ;
    case 221                                                                  :
      emit PeopleListings ( windowTitle ( ) , first , t1 , relation )         ;
    return true                                                               ;
    case 301                                                                  :
      if ( NotNull ( it ) ) Refresh ( it )                                    ;
    return true                                                               ;
    case 401                                                                  :
      NewPeople    (                     )                                    ;
    return true                                                               ;
    case 402                                                                  :
      RemovePeople (                     )                                    ;
    return true                                                               ;
    case 501                                                                  :
      sortItems    ( Qt::AscendingOrder  )                                    ;
    return true                                                               ;
    case 502                                                                  :
      sortItems    ( Qt::DescendingOrder )                                    ;
    return true                                                               ;
    case 602                                                                  :
      startup        ( )                                                      ;
    return true                                                               ;
    case 603                                                                  :
      StopIcons      ( )                                                      ;
    return true                                                               ;
    case 604                                                                  :
      doTranslations ( )                                                      ;
    return true                                                               ;
    case 605                                                                  :
      start          ( 20003 )                                                ;
    return true                                                               ;
    case 606                                                                  :
      FolderLanguage ( )                                                      ;
    return true                                                               ;
    case 611                                                                  :
      CountPictures  ( )                                                      ;
    return true                                                               ;
    case 612                                                                  :
      CountPossible  ( )                                                      ;
    return true                                                               ;
    case 613                                                                  :
      ExportNames    ( )                                                      ;
    return true                                                               ;
    case 614                                                                  :
      ExportAllNames ( )                                                      ;
    return true                                                               ;
    case 621                                                                  :
      emit MergePeople ( Uuids )                                              ;
    return true                                                               ;
    case 701                                                                  :
      if ( uuid > 0 ) emit PeopleDocuments   ( this , uuid , name           ) ;
    return true                                                               ;
    case 702                                                                  :
      if ( uuid > 0 ) emit Gallery           ( this , uuid , name           ) ;
    return true                                                               ;
    case 703                                                                  :
      if ( uuid > 0 ) emit VideoAlbum        ( this , uuid , name           ) ;
    return true                                                               ;
    case 704                                                                  :
      if ( uuid > 0 ) emit PotentialPictures ( this , uuid , name           ) ;
    return true                                                               ;
    case 705                                                                  :
      if ( uuid > 0 ) emit ListSubgroups     ( name , uuid                  ) ;
    return true                                                               ;
    case 711                                                                  :
      if ( uuid > 0 ) RelatedItems           ( uuid , it -> text ( )        ) ;
    return true                                                               ;
    case 801                                                                  :
    case 802                                                                  :
    case 803                                                                  :
    case 804                                                                  :
    case 805                                                                  :
      if ( uuid > 0 ) emit Internet          ( this , uuid , name , mm [aa] ) ;
    return true                                                               ;
    case 811                                                                  :
    case 812                                                                  :
    case 813                                                                  :
    case 814                                                                  :
    case 815                                                                  :
    case 816                                                                  :
    case 821                                                                  :
      if ( uuid > 0 ) emit Feature           ( mm[aa],uuid            )           ;
    return true                                                               ;
    default                                                                   :
    break                                                                     ;
  }                                                                           ;
  /////////////////////////////////////////////////////////////////////////////
  if ( isFunction ( 3101 ) )                                                  {
    RunPageMenu ( mm , aa , StartId , PageSize )                              ;
    PageChanged ( StartId , PageSize           )                              ;
  }                                                                           ;
  /////////////////////////////////////////////////////////////////////////////
  return true                                                                 ;
}
