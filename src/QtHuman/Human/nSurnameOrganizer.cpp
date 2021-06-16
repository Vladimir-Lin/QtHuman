#include <qthuman.h>

N::SurnameOrganizer:: SurnameOrganizer (QWidget * parent,Plan * p)
                    : TreeWidget       (          parent,       p)
                    , SearchText       (NULL                     )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SurnameOrganizer::~SurnameOrganizer(void)
{
}

QSize N::SurnameOrganizer::sizeHint(void) const
{
  if (plan->Booleans["Phone"]) return QSize( 320 , 240 ) ;
  return QSize ( 640 , 240 )                             ;
}

void N::SurnameOrganizer::Configure(void)
{
  StackSize   = 0                                     ;
  Reservation = false                                 ;
  /////////////////////////////////////////////////////
  setFocusPolicy          ( Qt::WheelFocus          ) ;
  setDragDropMode         ( NoDragDrop              ) ;
  setAlternatingRowColors ( true                    ) ;
  setRootIsDecorated      ( false                   ) ;
  setSelectionMode        ( ExtendedSelection       ) ;
  setColumnCount          ( 3                       ) ;
  /////////////////////////////////////////////////////
  setWindowTitle          ( tr("Surname organizer") ) ;
  NewTreeWidgetItem       ( head                    ) ;
  head -> setText         ( 0,tr("People name")     ) ;
  head -> setText         ( 1,tr("Language"   )     ) ;
  head -> setText         ( 2,tr("Surname"    )     ) ;
  setAllAlignments        ( head , Qt::AlignCenter  ) ;
  setFont                 ( head , Fonts::ListView  ) ;
  setHeaderItem           ( head                    ) ;
  /////////////////////////////////////////////////////
  plan   -> setFont       ( this                    ) ;
  MountClicked            ( 1                       ) ;
  MountClicked            ( 2                       ) ;
}

void N::SurnameOrganizer::run(void)
{
  AbstractGui::Mutex.lock  ()                         ;
  clear       ()                                      ;
  setEnabled (false)                                  ;
  TreeWidgetItems Items                               ;
  SqlConnection SC ( plan->sql )                      ;
  PeopleManager PM ( plan      )                      ;
  emit OnBusy      (           )                      ;
  if (SC.open("nSurnameOrganizer","List"))            {
    UUIDs   People = PM . Lists ( SC )                ;
    UUIDs   NameUuids                                 ;
    UUIDs   Names                                     ;
    SUID    uu                                        ;
    foreach (uu,People)                               {
      Names = PM.NameList ( SC , uu )                 ;
      NameUuids += Names                              ;
    }                                                 ;
    foreach (uu,NameUuids)                            {
      if (PM.NameLanguage(SC,uu))                     {
        NewTreeWidgetItem (IT)                        ;
        int     language  = SC . Int     ( 0       )  ;
        QString name      = SC . String  ( 1       )  ;
        SUID    suid      = PM . Surname ( SC , uu )  ;
        QString sname     = ""                        ;
        if (suid>0) sname = PM .Name(SC,suid)         ;
        IT -> setData   (0,Qt::UserRole,uu      )     ;
        IT -> setData   (1,Qt::UserRole,language)     ;
        IT -> setData   (2,Qt::UserRole,suid    )     ;
        IT -> setText   (0,name    )                  ;
        IT -> setText   (1,plan->languages[language]) ;
        IT -> setText   (2,sname   )                  ;
        Items << IT                                   ;
      }                                               ;
    }                                                 ;
    SC.close()                                        ;
  }                                                   ;
  emit GoRelax              (       )                 ;
  SC.remove                 (       )                 ;
  addTopLevelItems          ( Items )                 ;
  reportItems               (       )                 ;
  setEnabled                ( true  )                 ;
  AbstractGui::Mutex.unlock (       )                 ;
  emit AutoFit              (       )                 ;
  Alert                     ( Done  )                 ;
}

bool N::SurnameOrganizer::FocusIn(void)
{
  AssignAction ( Label   , windowTitle() ) ;
  LinkAction   ( Refresh , List       () ) ;
  return true                              ;
}

bool N::SurnameOrganizer::List(void)
{
  start ( )   ;
  return true ;
}

void N::SurnameOrganizer::selectLanguage(QTreeWidgetItem * item,int column)
{
  int language = item->data(1,Qt::UserRole).toInt() ;
  QComboBox * combo                                 ;
  combo = new QComboBox (this)                      ;
    combo  << plan->languages                       ;
  (*combo) <= language                              ;
  ItemEditing = item                                ;
  ItemColumn  = column                              ;
  ItemWidget  = combo                               ;
  setItemWidget ( item , column , combo )           ;
  connect(combo,SIGNAL(activated      (int))        ,
          this ,SLOT  (languageChanged(int))      ) ;
  combo -> showPopup ( )                            ;
}

void N::SurnameOrganizer::selectSurname(QTreeWidgetItem * item,int column)
{
  QComboBox * combo                                      ;
  combo = new QComboBox (this)                           ;
  combo->setEditable    (true)                           ;
  AbstractGui::Mutex.lock  ()                            ;
  SqlConnection SC ( plan->sql )                         ;
  PeopleManager PM ( plan      )                         ;
  if (SC.open("nSurnameOrganizer","doubleClicked"))      {
    int language = item->data(1,Qt::UserRole).toInt()    ;
    SUID suid    = nTreeUuid (item,2)                    ;
    QString name = item->text(0)                         ;
    if (suid==0)                                         {
      PM . SurnameCombo ( combo , SC , language , name ) ;
    } else                                               {
      PM . SurnameCombo ( combo , SC , language , ""   ) ;
      (*combo) <= suid                                   ;
    }                                                    ;
    SC . close()                                         ;
  }                                                      ;
  SC.remove()                                            ;
  ItemEditing = item                                     ;
  ItemColumn  = column                                   ;
  ItemWidget  = combo                                    ;
  setItemWidget ( item , column , combo )                ;
  connect(combo->lineEdit(),SIGNAL(returnPressed  ())    ,
          this             ,SLOT  (editingFinished()))   ;
  connect(combo,SIGNAL(activated     (int))              ,
          this ,SLOT  (surnameChanged(int))            ) ;
  AbstractGui::Mutex.unlock()                            ;
}

void N::SurnameOrganizer::singleClicked(QTreeWidgetItem * item,int column)
{
  SUID suid = nTreeUuid(item,2)                       ;
  Alert ( Click )                                     ;
  plan->disableAction(Menus::Delete)                  ;
  if (column==2 && suid>0)                            {
    plan->connectAction                               (
      Menus::Delete                                   ,
      this                                            ,
      SLOT(DeleteSurname())                           ,
      true                                          ) ;
  }                                                   ;
  if (ItemEditing==item && ItemColumn==column) return ;
  removeOldItem ( )                                   ;
}

void N::SurnameOrganizer::doubleClicked(QTreeWidgetItem * item,int column)
{
  removeOldItem ( )                              ;
  switch (column)                                {
    case 1: selectLanguage (item,column) ; break ;
    case 2: selectSurname  (item,column) ; break ;
  }                                              ;
}

void N::SurnameOrganizer::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return           ;
  if (IsNull(ItemWidget )) return           ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  ItemWidget  = NULL                        ;
  ItemEditing = NULL                        ;
  ItemColumn  = -1                          ;
}

void N::SurnameOrganizer::editingFinished (void)
{
  QComboBox * combo = Casting(QComboBox,ItemWidget)              ;
  if (NotNull(combo))                                            {
    QString name     = combo->lineEdit()->text()                 ;
    int     index    = combo->findText(name)                     ;
    int     language = ItemEditing->data(1,Qt::UserRole).toInt() ;
    SUID    uu       = nTreeUuid(ItemEditing,0)                  ;
    SUID    suid     = 0                                         ;
    if (index>=0)                                                {
      suid = combo->itemData(index,Qt::UserRole).toULongLong()   ;
    }                                                            ;
    SqlConnection SC ( plan->sql  )                              ;
    PeopleManager PM ( plan       )                              ;
    if (SC.open("nSurnameOrganizer","editingFinished"))          {
      if (suid<=0) suid = PM.addSurname ( SC , language , name ) ;
      if (suid> 0) PM   . PeopleSurname ( SC , suid     , uu   ) ;
      SC . close()                                               ;
    }                                                            ;
    SC.remove()                                                  ;
    if (suid>0)                                                  {
      ItemEditing->setText(2,name)                               ;
      ItemEditing->setData(2,Qt::UserRole,suid)                  ;
    } else                                                       {
    }                                                            ;
  }                                                              ;
  removeOldItem ()                                               ;
}

void N::SurnameOrganizer::surnameChanged(int index)
{
  QComboBox * combo = Casting(QComboBox,ItemWidget)            ;
  if (NotNull(combo))                                          {
    int  language = ItemEditing->data(1,Qt::UserRole).toInt()  ;
    QString name  = combo->lineEdit()->text()                  ;
    SUID uu   = nTreeUuid(ItemEditing,0)                       ;
    SUID suid = 0                                              ;
    if (index>=0)                                              {
      suid = combo->itemData(index,Qt::UserRole).toULongLong() ;
    } else                                                     {
    }                                                          ;
    SqlConnection SC ( plan->sql )                             ;
    PeopleManager PM ( plan      )                             ;
    if (SC.open("nSurnameOrganizer","surnameChanged"))         {
      if (suid<=0) suid = PM.addSurname (SC,language,name)     ;
      if (suid> 0) PM   . PeopleSurname (SC,suid    ,uu  )     ;
      SC . close()                                             ;
    }                                                          ;
    SC.remove()                                                ;
    if (suid>0)                                                {
      ItemEditing->setText(2,combo->currentText())             ;
      ItemEditing->setData(2,Qt::UserRole,suid   )             ;
    } else                                                     {
    }                                                          ;
  }                                                            ;
  removeOldItem ()                                             ;
}

void N::SurnameOrganizer::languageChanged(int index)
{
  QComboBox * combo = qobject_cast<QComboBox *>(ItemWidget)    ;
  if (NotNull(combo))                                          {
    int language = combo->itemData(index,Qt::UserRole).toInt() ;
    SUID uu      = nTreeUuid ( ItemEditing ,  0 )              ;
    SqlConnection SC ( plan->sql  )                            ;
    if (SC.open("nSurnameOrganizer","languageChanged"))        {
      QString Q                                                ;
      Q = SC . sql . Update                                    (
            plan->Tables[Tables::Names]                        ,
            QString("where uuid = %1") . arg ( uu )            ,
            1                                                  ,
            "language"                                       ) ;
      SC . Prepare ( Q )                                       ;
      SC . Bind    ("language",language)                       ;
      SC . Exec    (   )                                       ;
      SC . close()                                             ;
    }                                                          ;
    SC.remove()                                                ;
    ItemEditing->setData(1,Qt::UserRole,language)              ;
    ItemEditing->setText(1,plan->languages[language] )         ;
  }                                                            ;
  removeOldItem ()                                             ;
}

void N::SurnameOrganizer::DeleteSurname(void)
{
  QTreeWidgetItem * it = currentItem ()             ;
  if (IsNull(it)) return                            ;
  SUID uu   = nTreeUuid(it,0)                       ;
  SUID suid = nTreeUuid(it,2)                       ;
  it->setData(2,Qt::UserRole,0)                     ;
  it->setText(2,"")                                 ;
  if (uu  <=0) return                               ;
  if (suid<=0) return                               ;
  SqlConnection SC ( plan->sql  )                   ;
  if (SC.open("nSurnameOrganizer","DeleteSurname")) {
    QString Q                                       ;
    Q = SC.sql.DeleteFrom                           (
          plan->Tables[Tables::Groups]              ,
            QString("where first = %1 and second = %2 and t1 = %3 and t2 = %4 and relation = %5")
            .arg(suid                               )
            .arg(uu                                 )
            .arg(Types::Surname                     )
            .arg(Types::Name                        )
            .arg(Groups::Subordination              )
        )                                           ;
    SC . Query ( Q )                                ;
    SC . close (   )                                ;
  }                                                 ;
  SC.remove()                                       ;
}

void N::SurnameOrganizer::Guess(void)
{
  TreeWidgetItems items = selectedItems()         ;
  if (items.count()<=0) return                    ;
  emit OnBusy ( )                                 ;
  PeopleManager PM ( plan )                       ;
  EnterSQL ( SC , plan->sql )                     ;
    QString Q                                     ;
    UUIDs   surnames = PM . Surnames ( SC )       ;
    NAMEs   names                                 ;
    RMAPs   languages                             ;
    SUID    u                                     ;
    bool    ok                                    ;
    ///////////////////////////////////////////////
    foreach (u,surnames)                          {
      Q = SC.sql.SelectFrom                       (
            "language,name"                       ,
            PlanTable(Names)                      ,
            SC.WhereUuid(u)                     ) ;
      if (SC.Fetch(Q))                            {
        int     l = SC . Int    ( 0 )             ;
        QString n = SC . String ( 1 )             ;
        names     [ u ] = n                       ;
        languages [ u ] = l                       ;
      }                                           ;
    }                                             ;
    ///////////////////////////////////////////////
    int scount = surnames.count() - 1             ;
    for (int i=0;i<items.count();i++)             {
      QTreeWidgetItem * it = items[i]             ;
      SUID    u = nTreeUuid ( it , 0 )            ;
      int     l = nTreeInt  ( it , 1 )            ;
      QString n = it->text(0)                     ;
      QString c = it->text(2)                     ;
      SUID    s = 0                               ;
      if (c.length()>0) continue                  ;
      ok        = false                           ;
      for (int j=0;!ok && j<surnames.count();j++) {
        SUID x  = surnames[scount-j]              ;
        if (languages[x]==l)                      {
          QString nn = names[x]                   ;
          int     nl = nn.length ( )              ;
          QString nx                              ;
          switch (l)                              {
            case 1551                             : // German
              nx = n.right (nl )                  ;
            break                                 ;
            case 1819                             :
              nx = n.right (nl )                  ;
            break                                 ;
            case 1932                             : // French
              nx = n.right (nl )                  ;
            break                                 ;
            case 5903                             : // Spanish
              nx = n.right (nl )                  ;
            break                                 ;
            case 5568                             : // Russian
              nx = n.right (nl )                  ;
            break                                 ;
            default                               : // Asia
              nx = n.left  (nl )                  ;
            break                                 ;
          }                                       ;
          if (nx==nn)                             {
            s  = x                                ;
            ok = true                             ;
          }                                       ;
        }                                         ;
      }                                           ;
      if (ok)                                     {
        PM  . PeopleSurname ( SC , s , u   )      ;
        it -> setText       ( 2 , names[s] )      ;
      }                                           ;
      DoProcessEvents                             ;
    }                                             ;
    ///////////////////////////////////////////////
  ErrorSQL ( SC , plan->sql )                     ;
  LeaveSQL ( SC , plan->sql )                     ;
  emit GoRelax (      )                           ;
  Alert        ( Done )                           ;
}

void N::SurnameOrganizer::Search(void)
{
  if (NotNull(SearchText)) return                      ;
  //////////////////////////////////////////////////////
  SearchText  = new QComboBox(plan->status)            ;
  SearchText -> setInsertPolicy(QComboBox::NoInsert)   ;
  SearchText -> setEditable( true )                    ;
  //////////////////////////////////////////////////////
  QFont f    = plan->fonts[N::Fonts::Status]           ;
  QSize s    = plan->status->size()                    ;
  QSize m(s.width()/6,s.height()-4)                    ;
  if (f.pixelSize()>0)                                 {
    f   .setPixelSize (f.pixelSize()-2)                ;
    SearchText ->setFont (f           )                ;
  }                                                    ;
  SearchText -> setMinimumSize ( m )                   ;
  SearchText -> setMaximumSize ( m )                   ;
  //////////////////////////////////////////////////////
  QStringList words                                    ;
  for (int i=0;i<topLevelItemCount();i++)              {
    QTreeWidgetItem * it = topLevelItem(i)             ;
    QString t = it->text(0)                            ;
    SearchText->addItem(t)                             ;
    words << t                                         ;
  }                                                    ;
  QCompleter * comp = new QCompleter(words,SearchText) ;
  QLineEdit  * le   = SearchText->lineEdit()           ;
  le->setFont     (f   )                               ;
  le->setCompleter(comp)                               ;
  le->setText     (""  )                               ;
  le->setPlaceholderText(tr("search surname"))         ;
  nConnect ( le   , SIGNAL(returnPressed(   ))         ,
             this , SLOT  (SearchReady  (   ))       ) ;
  //////////////////////////////////////////////////////
  plan -> status -> addPermanentWidget ( SearchText )  ;
  SearchText     -> show               (            )  ;
  le -> setFocus (Qt::TabFocusReason)                  ;
}

void N::SurnameOrganizer::Search(QString text)
{
  QTreeWidgetItem * it = currentItem()    ;
  int s = 0                               ;
  nIfSafe(it) s = indexOfTopLevelItem(it) ;
  for (int i=s;i<topLevelItemCount();i++) {
    it = topLevelItem(i)                  ;
    if (it->text(0)==text)                {
      scrollToItem   ( it   )             ;
      setCurrentItem ( it   )             ;
      Alert          ( Done )             ;
      return                              ;
    }                                     ;
  }                                       ;
  Alert ( Error )                         ;
}

void N::SurnameOrganizer::SearchReady(void)
{
  if (IsNull(SearchText)) return                ;
  QString text = SearchText->lineEdit()->text() ;
  SearchText  -> deleteLater()                  ;
  SearchText   = NULL                           ;
  Search ( text )                               ;
}

bool N::SurnameOrganizer::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                      ;
  QAction * aa                                                   ;
  QMenu   * ms                                                   ;
  QMenu   * ma                                                   ;
  QMenu   * mp                                                   ;
  QTreeWidgetItem * item = currentItem ( )                       ;
  mm . add          ( 101 , tr("Refresh")         )              ;
  mm . addSeparator (                             )              ;
  mm . add          ( 301 , tr("Search"    )      )              ;
  mm . add          ( 302 , tr("Guess"     )      )              ;
  mm . addSeparator (                             )              ;
  ma = mm . addMenu ( tr("Adjustments")           )              ;
  mp = mm . addMenu ( tr("Selection"  )           )              ;
  ms = mm . addMenu ( tr("Sorting"    )           )              ;
  AdjustMenu        ( mm , ma                     )              ;
  SelectionsMenu    ( mm , mp                     )              ;
  SortingMenu       ( mm , ms                     )              ;
  mm . setFont      ( plan                        )              ;
  if (plan->Booleans["Desktop"])                                 {
    pos = QCursor::pos()                                         ;
  } else                                                         {
    pos = mapToGlobal ( pos )                                    ;
  }                                                              ;
  aa = mm.exec      ( pos                                      ) ;
  if (!plan->Booleans["Desktop"])                                {
    allowGesture = true                                          ;
  }                                                              ;
  nKickOut          ( IsNull(aa) , true                        ) ;
  if ( RunSorting    ( mm , aa ) ) return true                   ;
  if ( RunAdjustment ( mm , aa ) ) return true                   ;
  if ( RunSelections ( mm , aa ) ) return true                   ;
  switch (mm[aa])                                                {
    nFastCast  ( 101 , List           ( ) )                      ;
    nFastCast  ( 301 , Search         ( ) )                      ;
    nFastCast  ( 302 , Guess          ( ) )                      ;
  }                                                              ;
  return true                                                    ;
}
