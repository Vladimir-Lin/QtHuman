#include <qthuman.h>

N::FaceShapes:: FaceShapes      ( QWidget * parent , Plan * p )
              : ListDock        (           parent ,        p )
              , PeopleManager   (                           p )
              , CommandSequence ( new QTimer ( this )         )
              , Commanding      ( false                       )
              , dropAction      ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::FaceShapes::~FaceShapes(void)
{
}

QSize N::FaceShapes::sizeHint(void) const
{
  return QSize ( 172 , 192 ) ;
}

void N::FaceShapes::Configure(void)
{
  setWindowTitle               ( tr("Face shapes")     )             ;
  setViewMode                  ( IconMode              )             ;
  setIconSize                  ( QSize(128,128)        )             ;
  setGridSize                  ( QSize(144,192)        )             ;
  setMovement                  ( Snap                  )             ;
  setResizeMode                ( Adjust                )             ;
  setSelectionMode             ( SingleSelection       )             ;
  setWordWrap                  ( true                  )             ;
  setMouseTracking             ( true                  )             ;
  setWrapping                  ( true                  )             ;
  setTextElideMode             ( Qt::ElideNone         )             ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded )             ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded )             ;
  setMinimumSize               ( QSize(172,144)        )             ;
  setDragDropMode              ( DragDrop              )             ;
  setDropFlag                  ( DropPicture  , true   )             ;
  setDropFlag                  ( DropPeople   , true   )             ;
  plan -> setFont              ( this                  )             ;
  ////////////////////////////////////////////////////////////////////
  LimitValues [    49 ] =   1                                        ;
  LimitValues [    60 ] =  10                                        ;
  LimitValues [    61 ] =  10                                        ;
  LimitValues [ 37714 ] = 300                                        ;
  ////////////////////////////////////////////////////////////////////
  if (plan->Booleans["Phone"]) setViewMode ( QListView::ListMode )   ;
  CommandSequence -> setParent ( this )                              ;
  nConnect ( this            , SIGNAL ( TriggerCommand ( ) )         ,
             this            , SLOT   ( StartCommand   ( ) )       ) ;
  nConnect ( CommandSequence , SIGNAL ( timeout        ( ) )         ,
             this            , SLOT   ( CommandHandler ( ) )       ) ;
  nConnect ( this , SIGNAL (setIcon    (QImage*,QListWidgetItem*))   ,
             this , SLOT   (setItemIcon(QImage*,QListWidgetItem*)) ) ;
  nConnect ( this , SIGNAL (listItems  (ListWidgetItems&))           ,
             this , SLOT   (appendItems(ListWidgetItems&))         ) ;
}

bool N::FaceShapes::FocusIn(void)
{
  AssignAction ( Label   , windowTitle ( ) ) ;
  LinkAction   ( Refresh , startup     ( ) ) ;
  LinkAction   ( Insert  , New         ( ) ) ;
  LinkAction   ( Copy    , Copy        ( ) ) ;
  return true                                ;
}

QMimeData * N::FaceShapes::dragMime(void)
{
  QListWidgetItem * IT = currentItem()            ;
  if (IsNull(IT)) return NULL                     ;
  SUID  uuid  = nListUuid(IT)                     ;
  QMimeData * mime = new QMimeData()              ;
  setMime ( mime , "faceshape/uuid"  , uuid )     ;
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

bool N::FaceShapes::hasItem(void)
{
  QListWidgetItem * item = currentItem() ;
  return NotNull ( item )                ;
}

bool N::FaceShapes::startDrag(QMouseEvent * event)
{
  QListWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return false                                               ;
}

bool N::FaceShapes::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::FaceShapes::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::FaceShapes::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::FaceShapes::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

QString N::FaceShapes::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType         (
           mime                        ,
           "picture/uuid;picture/uuids;"
           "people/uuid;people/uuids"
         )                             ;
}

UUIDs N::FaceShapes::MimeUuids(const QMimeData * mime,QString mimetype)
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
  if (mimetype=="people/uuid")           {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="people/uuids")          {
    Uuids  = GetUuids ( data )           ;
  }                                      ;
  return Uuids                           ;
}

bool N::FaceShapes::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
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
  if (mtype=="people/uuid"         ||
      mtype=="people/uuids"         )
    plan->showMessage               (
    tr("Copy %1 people from %2"     )
      .arg(Uuids.count()            )
      .arg(source->windowTitle()) ) ;
  return true                       ;
}

bool N::FaceShapes::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false                                ;
  if (source==this)                                           {
    QListWidgetItem * atItem = itemAt(pos)                    ;
    if (IsNull (atItem)) return false                         ;
    if (NotNull(atItem) && atItem->isSelected()) return false ;
  }                                                           ;
  return true                                                 ;
}

bool N::FaceShapes::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false             ;
  return dropItems ( source , mime , pos ) ;
}

void N::FaceShapes::run(int Type,ThreadData * data)
{ Q_UNUSED ( data )  ;
  switch   ( Type )  {
    case 10001       :
      FetchUUIDs ( ) ;
    break            ;
    case 10003       :
      FetchIcons ( ) ;
    break            ;
  }                  ;
}

void N::FaceShapes::StartCommand(void)
{
  nDropOut ( Commanding )                            ;
  CommandSequence -> start ( LimitValues [ 37714 ] ) ;
}

void N::FaceShapes::CommandHandler(void)
{
  nDropOut ( Commanding )         ;
  Commanding = true               ;
  while ( Sequences.count() > 0 ) {
    RunCommand ( Sequences[0] )   ;
    Sequences . takeAt ( 0 )      ;
  }                               ;
  CommandSequence -> stop ( )     ;
  Commanding = false              ;
}

bool N::FaceShapes::RunCommand(VarArgs & arguments)
{
  if (arguments.count()<1) return false ;
  VarArgs V = arguments                 ;
  UUIDs   U                             ;
  int     c = V [ 0 ] . toInt ( )       ;
  switch         ( c     )              {
    case 10001                          :
      start ( 10001 )                   ;
    break                               ;
    case 10002                          :
      ArgsToUuids ( 1 , V , U )         ;
      plan -> processEvents ( )         ;
      ListIcons ( U )                   ;
    break                               ;
    default                             :
    return false                        ;
  }                                     ;
  return true                           ;
}

bool N::FaceShapes::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::FaceShapes::setItemIcon  (
       QImage          * image ,
       QListWidgetItem * item  )
{
  QSize          IS = iconSize (  ) ;
  PictureManager PM ( plan        ) ;
  QIcon icon = PM . Icon (image,IS) ;
  item -> setIcon ( icon )          ;
  delete image                      ;
}

void N::FaceShapes::FetchUUIDs(void)
{
  UUIDs Uuids                        ;
  EnterSQL ( SC , plan->sql )        ;
    Uuids = SC . Uuids               (
              PlanTable(FaceShapes)  ,
              "uuid"                 ,
              SC.OrderByAsc("id") )  ;
  LeaveSQL ( SC , plan->sql )        ;
  if (Uuids.count()>0)               {
    N::VarArgs V                     ;
    V << 10002                       ;
    V << Uuids . count (           ) ;
    toVariants         ( Uuids , V ) ;
    addSequence        ( V         ) ;
    //////////////////////////////////
    emit TriggerCommand (     )      ;
  }                                  ;
}

void N::FaceShapes::ListIcons(UUIDs & Uuids)
{
  QIcon icon = plan -> Icon                    (
                 Types :: FaceShape            ,
                 1                             ,
                 0                             ,
                 QIcon(":/images/faces.png") ) ;
  SUID  uuid                                   ;
  foreach (uuid,Uuids)                         {
    NewListWidgetItem ( TWI )                  ;
    TWI -> setText ( "       "           )     ;
    TWI -> setIcon ( icon                )     ;
    TWI -> setData ( Qt::UserRole , uuid )     ;
    QListWidget::addItem ( TWI )               ;
  }                                            ;
  start ( 10003 )                              ;
}

void N::FaceShapes::FetchIcons(void)
{
  GroupItems     GI ( plan )                 ;
  PictureManager PM ( plan )                 ;
  GI . AutoMap    = true                     ;
  GI . GroupTable = GI . LookTable           (
                      Types  :: FaceShape    ,
                      Types  :: Picture      ,
                      Groups :: Icon       ) ;
  EnterSQL ( SC , plan->sql )                ;
    QListWidgetItem * it                     ;
    QString           name                   ;
    SUID              uuid                   ;
    SUID              puid                   ;
    for (int i=0;i<count();i++)              {
      it   = item ( i )                      ;
      uuid = nListUuid ( it )                ;
      name = Name(SC,uuid,vLanguageId)       ;
      it -> setText    ( name              ) ;
      it -> setToolTip ( name              ) ;
      puid = GI.FindSecond                   (
        SC                                   ,
        uuid                                 ,
        Types  :: FaceShape                  ,
        Types  :: Picture                    ,
        Groups :: Icon                       ,
        "order by position asc limit 0,1"  ) ;
      QImage * image   = PM.Thumb  (SC,puid) ;
      if (NotNull(image))                    {
        emit setIcon ( image , it )          ;
      }                                      ;
    }                                        ;
  LeaveSQL ( SC , plan->sql )                ;
  Alert ( Done )                             ;
}

void N::FaceShapes::List(void)
{
  ListWidgetItems Items                                                     ;
  GroupItems     GI ( plan )                                                ;
  PictureManager PM ( plan )                                                ;
  EnterSQL ( SC , plan->sql )                                               ;
    QString name                                                            ;
    SUID    uuid                                                            ;
    UUIDs   Uuids = SC.Uuids                                                (
                      PlanTable(FaceShapes)                                 ,
                      "uuid"                                                ,
                      SC.OrderByAsc("id")                                 ) ;
    foreach (uuid,Uuids)                                                    {
      NewListWidgetItem ( TWI )                                             ;
//      QIcon icon                                                            ;
      name = Name(SC,uuid,vLanguageId)                                      ;
      TWI -> setText    ( name                )                             ;
      TWI -> setToolTip ( name                )                             ;
      TWI -> setData    ( Qt::UserRole , uuid )                             ;
      SUID puid = GI.FindSecond                                             (
        SC                                                                  ,
        uuid                                                                ,
        Types  :: FaceShape                                                 ,
        Types  :: Picture                                                   ,
        Groups :: Icon                                                      ,
        "order by position asc limit 0,1"                                 ) ;
      QImage * image   = PM.Thumb  (SC,puid)                                ;
      if (NotNull(image))                                                   {
        emit setIcon ( image , TWI )                                        ;
//      } else                                                                {
//        icon = plan->Icon(Types::FaceShape,1,0,QIcon(":/images/faces.png")) ;
      }                                                                     ;
      Items << TWI                                                          ;
    }                                                                       ;
  LeaveSQL ( SC , plan->sql )                                               ;
  emit listItems ( Items )                                                  ;
  Alert ( Done )                                                            ;
}

void N::FaceShapes::Copy(void)
{
  QMimeData * mime = dragMime    (      ) ;
  if (IsNull(mime)) return                ;
  qApp->clipboard()->setMimeData ( mime ) ;
}

bool N::FaceShapes::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                                      ;
  QMdiSubWindow   * mdi    = Casting(QMdiSubWindow,parent())         ;
  QDockWidget     * dock   = Casting(QDockWidget  ,parent())         ;
  QListWidgetItem * item   = itemAt(pos)                             ;
  QPoint            global = mapToGlobal(pos)                        ;
  QAction       * a                                                  ;
  mm.add(202,tr("Refresh"       ))                                   ;
  mm.add(201,tr("New face shape"))                                   ;
  if (NotNull(item))                                                 {
    mm . addSeparator ( )                                            ;
    mm.add(101,tr("People" ))                                        ;
    mm.add(102,tr("Gallery"))                                        ;
    mm.add(103,tr("Shape equation"))                                 ;
  }                                                                  ;
  mm . addSeparator ( )                                              ;
  if (!plan->Booleans["Phone"])                                      {
    if (viewMode()==QListView::IconMode) mm.add(301,tr("List view")) ;
    if (viewMode()==QListView::ListMode) mm.add(302,tr("Icon view")) ;
  }                                                                  ;
  mm . add ( 401 , tr("Translations") )                              ;
  mm . addSeparator ( )                                              ;
  nIfSafe(dock) mm.add(1000001,tr("Move to window area"))            ;
  nIfSafe(mdi ) mm.add(1000002,tr("Move to dock area"  ))            ;
  mm . setFont ( plan )                                              ;
  a = mm.exec(global)                                                ;
  if (IsNull(a)) return true                                         ;
  switch (mm[a])                                                     {
    case 101                                                         :
      emit People  ( nListUuid(item),Types::FaceShape,item->text() ) ;
    break                                                            ;
    case 102                                                         :
      emit Gallery ( item->text(),nListUuid(item),Types::FaceShape ) ;
    break                                                            ;
    case 103                                                         :
      emit ShapeEquation(nListUuid(item),Types::FaceShape,item->text()) ;
    break                                                            ;
    case 201                                                         :
      New ( )                                                        ;
    break                                                            ;
    case 202                                                         :
      startup ( )                                                    ;
    break                                                            ;
    case 301                                                         :
      setViewMode  ( QListView::ListMode )                           ;
    break                                                            ;
    case 302                                                         :
      setViewMode  ( QListView::IconMode )                           ;
    break                                                            ;
    case 401                                                         :
      if (count()>0)                                                 {
        UUIDs u = ItemUuids()                                        ;
        emit Translations ( windowTitle() , u )                      ;
      }                                                              ;
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

bool N::FaceShapes::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return true          ;
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (IsNull(atItem)) return true        ;
  if (Uuids.count()<=0) return true      ;
  SUID face  = nListUuid(atItem)         ;
  dropAction = true                      ;
  GroupItems GI(plan)                    ;
  EnterSQL(SC,plan->sql)                 ;
    UUIDs u = Uuids                      ;
    GI . Join                            (
      SC                                 ,
      face                               ,
      Types::FaceShape                   ,
      Types::Picture                     ,
      Groups::Subordination              ,
      0                                  ,
      u                                ) ;
  LeaveSQL(SC,plan->sql)                 ;
  Alert ( Done )                         ;
  dropAction = false                     ;
  startup()                              ;
  return true                            ;
}

bool N::FaceShapes::dropPeople(QWidget * source,QPointF psf,const UUIDs & Uuids)
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
      Types::FaceShape                   ,
      Types::People                      ,
      Groups::Subordination              ,
      0 , AUIDs                        ) ;
  LeaveSQL(SC,plan->sql)                 ;
  Alert ( Done )                         ;
  dropAction = false                     ;
  return true                            ;
}

void N::FaceShapes::New(void)
{
  NewListWidgetItem ( LWI              )                              ;
  QIcon icon                                                          ;
  icon = plan->Icon(Types::FaceShape,1,0,QIcon(":/images/faces.png")) ;
  LWI->setIcon      ( icon             )                              ;
  LWI->setData      ( Qt::UserRole , 0 )                              ;
  QListWidget :: addItem         ( LWI )                              ;
  scrollToItem      ( LWI              )                              ;
  setCurrentItem    ( LWI              )                              ;
  ItemEditing   = LWI                                                 ;
  QRect       R = visualItemRect(LWI)                                 ;
  QLineEdit * l = new QLineEdit(this)                                 ;
  QFont       f = font()                                              ;
  QRect       L                                                       ;
  L . setTop    ( R.bottom    ()     )                                ;
  L . setLeft   ( R.left      ()     )                                ;
  L . setWidth  ( R.width     ()     )                                ;
  L . setHeight ( f.pixelSize () + 2 )                                ;
  setItemWidget ( ItemEditing , l )                                   ;
  l -> setGeometry ( L )                                              ;
  l -> setFont     ( f )                                              ;
  ItemWidget = l                                                      ;
  connect(l   ,SIGNAL(editingFinished())                              ,
          this,SLOT  (editingFinished()) )                            ;
  l->setFocus(Qt::TabFocusReason)                                     ;
}

void N::FaceShapes::editingFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget)              ;
  if (IsNull(l)) return                                      ;
  ////////////////////////////////////////////////////////////
  QString name = l->text()                                   ;
  removeItemWidget(ItemEditing)                              ;
  if (name.length()<=0)                                      {
    takeItem(row(ItemEditing))                               ;
    ItemEditing = NULL                                       ;
    ItemWidget  = NULL                                       ;
    return                                                   ;
  }                                                          ;
  ItemEditing->setText(name)                                 ;
  ////////////////////////////////////////////////////////////
  Bustle ( )                                                 ;
  SqlConnection SC(plan->sql)                                ;
  if (SC.open("FaceShapes","editingFinished"))               {
    SUID u = 0                                               ;
    u = SC.Unique ( PlanTable(MajorUuid),"uuid",3576 )       ;
    if (u>0)                                                 {
      SC.assureUuid(PlanTable(MajorUuid),u,Types::FaceShape) ;
      SC.insertUuid(PlanTable(FaceShapes),u,"uuid")          ;
      SC.assureName(PlanTable(Names),u,vLanguageId,name)     ;
    }                                                        ;
    SC . close           (           )                       ;
    if (u>0) ItemEditing->setData ( Qt::UserRole,u )         ;
  } else                                                     {
    takeItem(row(ItemEditing))                               ;
  }                                                          ;
  SC . remove ( )                                            ;
  Vacancy     ( )                                            ;
  ////////////////////////////////////////////////////////////
  ItemEditing = NULL                                         ;
  ItemWidget  = NULL                                         ;
  Alert ( Done )                                             ;
}
