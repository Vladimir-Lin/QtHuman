#include <qthuman.h>

N::SurnameTranslations:: SurnameTranslations (QWidget * parent,Plan * p)
                       : TreeWidget          (          parent,       p)
                       , Object              (0,Types::Surname         )
                       , Relation            (Groups::Equivalent,0     )
                       , PeopleManager       (                        p)
                       , dropAction          ( false                   )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SurnameTranslations::~SurnameTranslations (void)
{
}

QSize N::SurnameTranslations::sizeHint(void) const
{
  return QSize ( 240 , 240 ) ;
}

void N::SurnameTranslations::Configure(void)
{
  ///////////////////////////////////////////////////////////////////
  setFocusPolicy          ( Qt::WheelFocus                        ) ;
  setDragDropMode         ( DragDrop                              ) ;
  setAlternatingRowColors ( true                                  ) ;
  setRootIsDecorated      ( false                                 ) ;
  setSelectionMode        ( ExtendedSelection                     ) ;
  setColumnCount          ( 1                                     ) ;
  ///////////////////////////////////////////////////////////////////
  setWindowTitle          ( tr("Surname equivalences")            ) ;
  NewTreeWidgetItem       ( head                                  ) ;
  head -> setText         ( 0    , tr("Surname"   )               ) ;
  setAllAlignments        ( head , Qt::AlignCenter                ) ;
  setFont                 ( head , Fonts::ListView                ) ;
  setHeaderItem           ( head                                  ) ;
  MountClicked            ( 1                                     ) ;
  MountClicked            ( 2                                     ) ;
  plan->setFont           ( this                                  ) ;
}

bool N::SurnameTranslations::FocusIn(void)
{
  AssignAction ( Label  , windowTitle ( ) ) ;
  LinkAction   ( Insert , New         ( ) ) ;
  LinkAction   ( Delete , Detach      ( ) ) ;
  return true                               ;
}

QMimeData * N::SurnameTranslations::dragMime(void)
{
  return standardMime("surname")         ;
}

bool N::SurnameTranslations::hasItem(void)
{
  UUIDs Uuids = selectedUuids(0)                               ;
  plan->showMessage(tr("Drag %1 surnames").arg(Uuids.count())) ;
  return ( Uuids.count() > 0 )                                 ;
}

bool N::SurnameTranslations::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::SurnameTranslations::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::SurnameTranslations::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::SurnameTranslations::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::SurnameTranslations::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

bool N::SurnameTranslations::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{ Q_UNUSED ( pos )               ;
  if (dropAction  ) return false ;
  if (source==this) return false ;
  return acceptPrivate (mime)    ;
}

bool N::SurnameTranslations::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction  ) return false ;
  if (source==this) return false ;
  return acceptPrivate (mime)    ;
}

bool N::SurnameTranslations::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false             ;
  return dropItems ( source , mime , pos ) ;
}

bool N::SurnameTranslations::acceptPrivate(const QMimeData * mime)
{
  if (mime->hasFormat("surname/uuid" )) return true ;
  if (mime->hasFormat("surname/uuids")) return true ;
  return false                                      ;
}

bool N::SurnameTranslations::dropPrivate(nDeclWidget,const QMimeData * mime,QPointF pos)
{
  UUIDs Uuids                                          ;
  if (mime->hasFormat("surname/uuid" ))                {
    QByteArray data   = mime->data ( "surname/uuid"  ) ;
    SUID       uuid   = GetUuid    ( data            ) ;
    Uuids << uuid                                      ;
  } else
  if (mime->hasFormat("surname/uuids"))                {
    QByteArray data   = mime->data ( "surname/uuids" ) ;
    Uuids             = GetUuids   ( data            ) ;
  }                                                    ;
  if (Uuids.count()>0)                                 {
    dropAction = true                                  ;
    append ( Uuids , pos )                             ;
    dropAction = false                                 ;
    return true                                        ;
  }                                                    ;
  return false                                         ;
}

bool N::SurnameTranslations::startup(void)
{
  LoadSurnames (      ) ;
  LoadEquals   (      ) ;
  Alert        ( Done ) ;
  return true           ;
}

void N::SurnameTranslations::LoadSurnames(void)
{
  emit OnBusy    (                          ) ;
  EnterSQL       ( SC , plan->sql           ) ;
    UUIDs U                                   ;
    SUID  u                                   ;
    U = Surnames ( SC , SC.OrderByAsc("id") ) ;
    suruuids = U                              ;
    foreach (u,U)                             {
      QString Q                               ;
      Q = SC.sql.SelectFrom                   (
            "language,name"                   ,
            PlanTable(Names)                  ,
            SC.WhereUuid(u)                 ) ;
      if (SC.Fetch(Q))                        {
        int     language = SC . Int    ( 0  ) ;
        QString name     = SC . String ( 1  ) ;
        surnames     [ u ] = name             ;
        surlanguages [ u ] = language         ;
      }                                       ;
    }                                         ;
  ErrorSQL       ( SC , plan->sql           ) ;
    Alert        ( Error                    ) ;
  LeaveSQL       ( SC , plan->sql           ) ;
  emit GoRelax   (                          ) ;
}

void N::SurnameTranslations::LoadEquals(void)
{
  clear        (                          )   ;
  emit OnBusy  (                          )   ;
  GroupItems GI( plan                     )   ;
  EnterSQL     ( SC , plan->sql           )   ;
    UUIDs U                                   ;
    if (ObjectUuid()>0)                       {
      U = GI.Subordination                    (
            SC                                ,
            ObjectUuid ( )                    ,
            ObjectType ( )                    ,
            Types  :: Surname                 ,
            connexion  ( )                    ,
            SC.OrderByAsc("position")     )   ;
      if (U.count()>0)                        {
        SUID u                                ;
        foreach (u,U)                         {
          QString t = ""                      ;
          QString n = ""                      ;
          int     l = 0                       ;
          NewTreeWidgetItem ( I )             ;
          if (surnames.contains(u))           {
            n = surnames[u]                   ;
          }                                   ;
          if (surlanguages.contains(u))       {
            QString L                         ;
            l = surlanguages[u]               ;
            if (l>0)                          {
              L = (plan->languages)[l]        ;
              t = tr("%1 (%2)").arg(n).arg(L) ;
            }                                 ;
          }                                   ;
          if (t.length()>0)                   {
            I -> setData ( 0,Qt::UserRole,u ) ;
            I -> setText ( 0,t              ) ;
            addTopLevelItem   ( I )           ;
          }                                   ;
        }                                     ;
      }                                       ;
    }                                         ;
  ErrorSQL     ( SC , plan->sql           )   ;
  Alert        ( Error                    )   ;
  LeaveSQL     ( SC , plan->sql           )   ;
  emit GoRelax (                          )   ;
}

void N::SurnameTranslations::append(UUIDs & Uuids)
{
  UUIDs C = itemUuids ( 0 )                        ;
  UUIDs U                                          ;
  SUID  u                                          ;
  foreach (u,Uuids)                                {
    if (!C.contains(u)) U << u                     ;
  }                                                ;
  if (U.count()<=0) return                         ;
  //////////////////////////////////////////////////
  EnterSQL     ( SC , plan->sql           )        ;
    foreach (u,U)                                  {
      QString t = ""                               ;
      QString n = ""                               ;
      int     l = 0                                ;
      NewTreeWidgetItem ( I )                      ;
      if (surnames.contains(u))                    {
        n = surnames[u]                            ;
      }                                            ;
      if (surlanguages.contains(u))                {
        QString L                                  ;
        l = surlanguages[u]                        ;
        if (l>0)                                   {
          L = (plan->languages)[l]                 ;
          t = tr("%1 (%2)").arg(n).arg(L)          ;
          if (ObjectType()==Types  :: Surname     &&
              connexion ()==Groups :: Equivalent ) {
            addEqualSurname(SC,ObjectUuid(),u)     ;
          } else                                   {
            GroupItems GI(plan)                    ;
            UUIDs J                                ;
            J << u                                 ;
            GI.Join(SC                             ,
                    ObjectUuid()                   ,
                    ObjectType()                   ,
                    Types::Surname                 ,
                    connexion()                    ,
                    0                              ,
                    J                            ) ;
          }                                        ;
        }                                          ;
      }                                            ;
      if (t.length()>0)                            {
        I -> setData ( 0,Qt::UserRole,u )          ;
        I -> setText ( 0,t              )          ;
        addTopLevelItem   ( I )                    ;
      }                                            ;
    }                                              ;
    Alert      ( Done                     )        ;
  ErrorSQL     ( SC , plan->sql           )        ;
  Alert        ( Error                    )        ;
  LeaveSQL     ( SC , plan->sql           )        ;
}

void N::SurnameTranslations::append(UUIDs & Uuids,QPointF pos)
{ Q_UNUSED ( pos   ) ;
  append   ( Uuids ) ;
}

void N::SurnameTranslations::CopyToClipboard(void)
{
  TreeWidgetItems items = selectedItems()   ;
  if (items.count()==1)                     {
    QTreeWidgetItem * item = currentItem () ;
    nDropOut ( IsNull(item) )               ;
    int cc = currentColumn()                ;
    QString text = item -> text (cc)        ;
    nSetClipboard (text)                    ;
  } else                                    {
    QString     text                        ;
    QStringList st                          ;
    for (int i=0;i<items.count();i++)       {
      QStringList lt                        ;
      for (int j=0;j<columnCount();j++)     {
        lt << items[i]->text(j)             ;
      }                                     ;
      st << lt.join("\t")                   ;
    }                                       ;
    text = st.join("\n")                    ;
    nSetClipboard (text)                    ;
  }                                         ;
  QString m                                 ;
  m = tr("duplicate text to clipboard")     ;
  plan->Talk(m)                             ;
}

void N::SurnameTranslations::New(void)
{
  Alert             ( Click                 ) ;
  removeOldItem     (                       ) ;
  NewTreeWidgetItem ( IT                    ) ;
  addTopLevelItem   ( IT                    ) ;
  scrollToItem      ( IT                    ) ;
  IT->setData       ( 0  , Qt::UserRole , 0 ) ;
  doubleClicked     ( IT , 0                ) ;
}

void N::SurnameTranslations::Detach(void)
{
  TreeWidgetItems items = selectedItems()                 ;
  nDropOut ( items.count() <= 0 )                         ;
  EnterSQL ( SC , plan->sql )                             ;
    bool  exact = true                                    ;
    UUIDs U                                               ;
    if (ObjectType()!=Types  :: Surname   ) exact = false ;
    if (connexion ()!=Groups :: Equivalent) exact = false ;
    for (int i=0;i<items.count();i++)                     {
      SUID u     = nTreeUuid(items[i],0)                  ;
      int  index = indexOfTopLevelItem(items[i])          ;
      if (exact)                                          {
        removeEqualSurname(SC,ObjectUuid(),u)             ;
      } else U << u                                       ;
      if (index>=0) takeTopLevelItem(index)               ;
    }                                                     ;
    if (U.count()>0)                                      {
      GroupItems GI(plan)                                 ;
      GI.Detach ( SC                                      ,
                  ObjectUuid()                            ,
                  ObjectType()                            ,
                  Types::Surname                          ,
                  connexion()                             ,
                  U                                     ) ;
      Alert ( Done )                                      ;
    }                                                     ;
  ErrorSQL ( SC , plan->sql )                             ;
  Alert    ( Error          )                             ;
  LeaveSQL ( SC , plan->sql )                             ;
}

void N::SurnameTranslations::DetachOneSide(void)
{
  TreeWidgetItems items = selectedItems()                 ;
  nDropOut ( items.count() <= 0 )                         ;
  EnterSQL ( SC , plan->sql )                             ;
    bool  exact = true                                    ;
    UUIDs U                                               ;
    if (ObjectType()!=Types  :: Surname   ) exact = false ;
    if (connexion ()!=Groups :: Equivalent) exact = false ;
    for (int i=0;i<items.count();i++)                     {
      SUID u     = nTreeUuid(items[i],0)                  ;
      int  index = indexOfTopLevelItem(items[i])          ;
      if (exact)                                          {
        detachEqualSurname(SC,ObjectUuid(),u)             ;
      } else U << u                                       ;
      if (index>=0) takeTopLevelItem(index)               ;
    }                                                     ;
    if (U.count()>0)                                      {
      GroupItems GI(plan)                                 ;
      GI.Detach ( SC                                      ,
                  ObjectUuid()                            ,
                  ObjectType()                            ,
                  Types::Surname                          ,
                  connexion()                             ,
                  U                                     ) ;
      Alert ( Done )                                      ;
    }                                                     ;
  ErrorSQL ( SC , plan->sql )                             ;
  Alert    ( Error          )                             ;
  LeaveSQL ( SC , plan->sql )                             ;
}

bool N::SurnameTranslations::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                    ;
  QAction * aa                                 ;
  QMenu   * ms                                 ;
  QMenu   * ma                                 ;
  QMenu   * mp                                 ;
  QTreeWidgetItem * item = currentItem ( )     ;
  mm . add          ( 101 , tr("Refresh")    ) ;
  mm . add          ( 102 , tr("Insert" )    ) ;
  nIfSafe(item)                                {
    mm . add        ( 103 , tr("Detach" )    ) ;
    mm . add        ( 104 , tr("Unilateral detach") ) ;
  }                                            ;
  mm . addSeparator (                        ) ;
  ma = mm . addMenu ( tr("Adjustments")      ) ;
  mp = mm . addMenu ( tr("Selection"  )      ) ;
  ms = mm . addMenu ( tr("Sorting"    )      ) ;
  AdjustMenu        ( mm , ma                ) ;
  SelectionsMenu    ( mm , mp                ) ;
  SortingMenu       ( mm , ms                ) ;
  mm . setFont      ( plan                   ) ;
  if (plan->Booleans["Desktop"])               {
    pos = QCursor::pos()                       ;
  } else                                       {
    pos = mapToGlobal ( pos )                  ;
  }                                            ;
  aa = mm.exec      ( pos                    ) ;
  if (!plan->Booleans["Desktop"])              {
    allowGesture = true                        ;
  }                                            ;
  nKickOut          ( IsNull(aa) , true      ) ;
  if ( RunSorting    ( mm , aa ) ) return true ;
  if ( RunAdjustment ( mm , aa ) ) return true ;
  if ( RunSelections ( mm , aa ) ) return true ;
  switch (mm[aa])                              {
    nFastCast  ( 101 , startup       ( ) )     ;
    nFastCast  ( 102 , New           ( ) )     ;
    nFastCast  ( 103 , Detach        ( ) )     ;
    nFastCast  ( 104 , DetachOneSide ( ) )     ;
  }                                            ;
  return true                                  ;
}

void N::SurnameTranslations::singleClicked(QTreeWidgetItem * item,int column)
{ Q_UNUSED ( item   ) ;
  Q_UNUSED ( column ) ;
  Alert    ( Click ) ;
}

void N::SurnameTranslations::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( NotEqual ( column , 0 ) )             ;
  QStringList words                                ;
  SUID  u = nTreeUuid(item,0)                      ;
  SUID  i                                          ;
  UUIDs U = suruuids                               ;
  int   p = -1                                     ;
  QComboBox * cb = new QComboBox(this)             ;
  cb->setEditable(true)                            ;
  QLineEdit * le = cb->lineEdit()                  ;
  setItemWidget ( item , column , cb )             ;
  ItemEditing = item                               ;
  ItemColumn  = column                             ;
  ItemWidget  = cb                                 ;
  foreach (i,U)                                    {
    QString n = surnames    [i]                    ;
    int     l = surlanguages[i]                    ;
    if (l>0)                                       {
      QString L = (plan->languages)[l]             ;
      QString t = tr("%1 (%2)").arg(n).arg(L)      ;
      if (t.length()>0)                            {
        if (nEqual(i,u)) p = cb->count()           ;
        cb->addItem(t,i)                           ;
        if (!words.contains(t)) words << t         ;
      }                                            ;
    }                                              ;
  }                                                ;
  if (p>=0) cb->setCurrentIndex(p)                 ;
  QCompleter * comp = new QCompleter(words,cb)     ;
  cb->lineEdit()->setCompleter(comp)               ;
  nConnect ( cb   , SIGNAL(activated      (int))   ,
             this , SLOT  (languageChanged(int)) ) ;
  nConnect ( le   , SIGNAL(returnPressed  (   ))   ,
             this , SLOT  (editingFinished(   )) ) ;
  cb -> showPopup ( )                              ;
}

void N::SurnameTranslations::editingFinished(void)
{
  QComboBox * comb = Casting(QComboBox,ItemWidget) ;
  nIfSafe(comb)                                    {
    QLineEdit * le = comb->lineEdit()              ;
    nIfSafe(le)                                    {
      QString n     = le->text()                   ;
      int     index = comb->findText(n)            ;
      if (index>=0)                                {
        languageChanged(index)                     ;
        return                                     ;
      }                                            ;
    }                                              ;
  }                                                ;
  removeOldItem ( )                                ;
}

void N::SurnameTranslations::languageChanged(int index)
{
  QComboBox * comb = Casting(QComboBox,ItemWidget) ;
  if (NotNull(comb) && index>=0)                   {
    SUID  u = comb->itemData(index).toULongLong()  ;
    UUIDs E = itemUuids(0)                         ;
    if (nEqual(ObjectUuid(),u)                    ||
        E.contains(u)                             ||
        !surnames.contains(u)                    ) {
      Alert ( Error )                              ;
    } else                                         {
      QString n = surnames    [u]                  ;
      int     l = surlanguages[u]                  ;
      QString L = (plan->languages)[l]             ;
      QString t = tr("%1 (%2)").arg(n).arg(L)      ;
      if (t.length()>0 && ObjectUuid()>0)          {
        EnterSQL ( SC , plan->sql )                ;
          if (ObjectType()==Types  :: Surname     &&
              connexion ()==Groups :: Equivalent ) {
            addEqualSurname(SC,ObjectUuid(),u)     ;
          } else                                   {
            GroupItems GI(plan)                    ;
            UUIDs J                                ;
            J << u                                 ;
            GI.Join(SC                             ,
                    ObjectUuid()                   ,
                    ObjectType()                   ,
                    Types::Surname                 ,
                    connexion()                    ,
                    0                              ,
                    J                            ) ;
          }                                        ;
        ErrorSQL ( SC , plan->sql )                ;
        LeaveSQL ( SC , plan->sql )                ;
        NewTreeWidgetItem ( I )                    ;
        I -> setData ( 0,Qt::UserRole,u )          ;
        I -> setText ( 0,t              )          ;
        addTopLevelItem   ( I )                    ;
        Alert ( Done )                             ;
     }                                             ;
    }                                              ;
  }                                                ;
  removeOldItem ( )                                ;
}

void N::SurnameTranslations::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return                ;
  SUID uuid = nTreeUuid(ItemEditing,ItemColumn)  ;
  removeItemWidget (ItemEditing,ItemColumn)      ;
  if (uuid<=0)                                   {
    int index = indexOfTopLevelItem(ItemEditing) ;
    takeTopLevelItem(index)                      ;
  }                                              ;
  ItemEditing = NULL                             ;
  ItemWidget  = NULL                             ;
  ItemColumn  = -1                               ;
  if (!plan->Booleans["Desktop"])                {
    allowGesture = true                          ;
  }                                              ;
}
