#include <qthuman.h>

N::CrowdView:: CrowdView (QWidget * parent,Plan * p)
             : GroupView (          parent,       p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::CrowdView::~CrowdView (void)
{
}

void N::CrowdView::Configure(void)
{
  type    = Types::People                                      ;
  display = ThumbIcon                                          ;
  //////////////////////////////////////////////////////////////
  setWindowTitle               ( tr("Crowds")          )       ;
  setDragDropMode              ( DragDrop              )       ;
  setViewMode                  ( IconMode              )       ;
  setIconSize                  ( QSize(128,128)        )       ;
  setGridSize                  ( QSize(144,180)        )       ;
  setMovement                  ( Snap                  )       ;
  setResizeMode                ( Adjust                )       ;
  setSelectionMode             ( ExtendedSelection     )       ;
  setWordWrap                  ( true                  )       ;
  setMouseTracking             ( true                  )       ;
  setWrapping                  ( true                  )       ;
  setTextElideMode             ( Qt::ElideNone         )       ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded )       ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded )       ;
  setMinimumSize               ( QSize(172,192)        )       ;
  //////////////////////////////////////////////////////////////
  setDropFlag                  ( DropText     , false  )       ;
  setDropFlag                  ( DropUrls     , false  )       ;
  setDropFlag                  ( DropImage    , true   )       ;
  setDropFlag                  ( DropHtml     , false  )       ;
  setDropFlag                  ( DropColor    , false  )       ;
  setDropFlag                  ( DropTag      , false  )       ;
  setDropFlag                  ( DropPicture  , true   )       ;
  setDropFlag                  ( DropPeople   , true   )       ;
  setDropFlag                  ( DropVideo    , false  )       ;
  setDropFlag                  ( DropAlbum    , false  )       ;
  setDropFlag                  ( DropGender   , false  )       ;
  setDropFlag                  ( DropDivision , true   )       ;
  //////////////////////////////////////////////////////////////
  plan->setFont                ( this                  )       ;
  //////////////////////////////////////////////////////////////
  LocalIcons [ "Update"    ] = QIcon(":/images/update.png"   ) ;
  LocalIcons [ "AddPeople" ] = QIcon(":/images/addpeople.png") ;
}

UUIDs N::CrowdView::GroupUuids(SqlConnection & connection)
{
  return GroupItems :: Groups         (
           connection                 ,
           (Types::ObjectTypes)type   ,
           "order by position desc" ) ;
}

QString N::CrowdView::ToolTipTotoal(int total)
{
  return tr("%1 people in this crowd").arg(total) ;
}

int N::CrowdView::Counting(SqlConnection & Connection,SUID u)
{
  GroupItems GI ( plan )           ;
  GI . AutoMap    = true           ;
  GI . GroupTable = GI . LookTable (
         Types  :: Division        ,
         type                      ,
         Groups :: Subordination ) ;
  return GI . Count                (
           Connection              ,
           u                       ,
           Types::Division         ,
           type                    ,
           Groups::Subordination ) ;
}

QString N::CrowdView::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType            (
           mime                           ,
           "people/uuid;people/uuids;"
           "picture/uuid;picture/uuids;"
           "division/uuid;division/uuids"
         )                                ;
}

UUIDs N::CrowdView::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                                           ;
  QByteArray data = mime->data(mimetype)                     ;
  if (data.size()<=0) return Uuids                           ;
  if (mimetype=="people/uuid"   ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="people/uuids"  ) Uuids  = GetUuids ( data ) ; else
  if (mimetype=="picture/uuid"  ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="picture/uuids" ) Uuids  = GetUuids ( data ) ; else
  if (mimetype=="division/uuid" ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="division/uuids") Uuids  = GetUuids ( data ) ;
  return Uuids                                               ;
}

void N::CrowdView::GroupPositions(RMAPs & Position,UUIDs & Uuids)
{
  int Id = Uuids . count() ;
  SUID uuid                ;
  foreach (uuid,Uuids)     {
    Position [ uuid ] = Id ;
    Id --                  ;
  }                        ;
}

bool N::CrowdView::JoinImage(SUID uuid,const QImage & image)
{
  bool result = setGroupIcon(uuid,image) ;
  QListWidgetItem * item = uuidAt(uuid)  ;
  Refresh ( item )                       ;
  return result                          ;
}

bool N::CrowdView::JoinPictures(SUID uuid,const UUIDs & Uuids)
{
  if (Uuids.count()<=0) return true            ;
  bool result = setGroupIcon( uuid , Uuids[0]) ;
  QListWidgetItem * item = uuidAt(uuid)        ;
  Refresh ( item )                             ;
  return result                                ;
}

void N::CrowdView::reportItems(void)
{
  QString m                                  ;
  m = tr ( "%1 crowds" ) . arg ( count ( ) ) ;
  emit assignToolTip           ( m         ) ;
  plan -> Talk                 ( m         ) ;
}

bool N::CrowdView::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                   ;
  QAction         * aa                            ;
  QListWidgetItem * it                            ;
  QMenu           * me                            ;
  /////////////////////////////////////////////////
  it = itemAt(pos)                                ;
  mm . add ( 101                                  ,
             LocalIcons [ "AddPeople" ]           ,
             tr("New crowd")                    ) ;
  if (NotNull(it)) mm.add(102,tr("Rename"))       ;
  if (NotNull(it)) mm.add(103,tr("Delete"))       ;
  mm . addSeparator ( )                           ;
  mm . add ( 201                                  ,
             LocalIcons [ "Update" ]              ,
             tr("Reflush")                      ) ;
  if (NotNull(it)) mm.add(202,tr("Reflush item")) ;
  mm . addSeparator()                             ;
  mm . add ( 301 , tr("Copy to clipboard") )      ;
  mm . add ( 302 , tr("Clear selection"  ) )      ;
  mm . add ( 303 , tr("Select all"       ) )      ;
  me = mm . addMenu ( tr("Edit") )                ;
  mm.add(me,211,tr("Multilingual translations"))  ;
  AdjustMenu ( mm , me )                          ;
  /////////////////////////////////////////////////
  mm . setFont ( plan                )            ;
  aa = mm.exec (                     )            ;
  nKickOut     ( IsNull(aa) , true   )            ;
  /////////////////////////////////////////////////
  UUIDs Tuids                                     ;
  switch (mm[aa])                                 {
    case 101                                      :
      New             ( )                         ;
    break                                         ;
    case 102                                      :
      Rename          ( )                         ;
    break                                         ;
    case 103                                      :
      Delete          ( )                         ;
    break                                         ;
    case 201                                      :
      startup         ( )                         ;
    break                                         ;
    case 202                                      :
      Refresh         ( it )                      ;
    break                                         ;
    case 211                                      :
      Tuids = ItemUuids()                         ;
      emit Translations(windowTitle(),Tuids)      ;
    break                                         ;
    case 301                                      :
      CopyToClipboard ( )                         ;
    break                                         ;
    case 302                                      :
      SelectNone      ( )                         ;
    break                                         ;
    case 303                                      :
      SelectAll       ( )                         ;
    break                                         ;
    default                                       :
      RunAdjustment ( mm , aa )                   ;
    break                                         ;
  }                                               ;
  return true                                     ;
}
