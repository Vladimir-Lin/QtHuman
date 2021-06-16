#include <qthuman.h>

N::SurnameEditor:: SurnameEditor ( QWidget * parent , Plan * p )
                 : TreeDock      (           parent ,        p )
                 , Method        ( 0                           )
                 , OnlyLanguage  ( 0                           )
                 , dropAction    ( false                       )
                 , SearchText    ( NULL                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SurnameEditor::~SurnameEditor(void)
{
}

QSize N::SurnameEditor::sizeHint(void) const
{
  return QSize ( 240 , 240 ) ;
}

void N::SurnameEditor::Configure(void)
{
  StackSize    = 0                                                  ;
  Reservation  = false                                              ;
  OnlyLanguage = plan->LanguageId                                   ;
  ///////////////////////////////////////////////////////////////////
  setFocusPolicy          ( Qt::WheelFocus                        ) ;
  setDragDropMode         ( DragDrop                              ) ;
  setAlternatingRowColors ( true                                  ) ;
  setRootIsDecorated      ( false                                 ) ;
  setSelectionMode        ( ExtendedSelection                     ) ;
  setColumnCount          ( 3                                     ) ;
  ///////////////////////////////////////////////////////////////////
  setWindowTitle          ( tr("Surname editor")                  ) ;
  NewTreeWidgetItem       ( head                                  ) ;
  head -> setText         ( 0    , tr("Surname"   )               ) ;
  head -> setText         ( 1    , tr("Language"  )               ) ;
  head -> setText         ( 2    , tr("Statistics")               ) ;
  assignHeaderItems       ( head                                  ) ;
  setColumnHidden         ( 2 , true                              ) ;
  setAlignment            ( 2 , Qt::AlignVCenter | Qt::AlignRight ) ;
  MountClicked            ( 2                                     ) ;
  plan->setFont           ( this                                  ) ;
}

bool N::SurnameEditor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true                ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Insert       , New             ( ) ) ;
  LinkAction        ( Paste        , Paste           ( ) ) ;
  LinkAction        ( Refresh      , List            ( ) ) ;
  LinkAction        ( Rename       , Rename          ( ) ) ;
  LinkAction        ( Search       , Search          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  return true                                              ;
}

QMimeData * N::SurnameEditor::dragMime(void)
{
  return standardMime("surname")         ;
}

bool N::SurnameEditor::hasItem(void)
{
  UUIDs Uuids = selectedUuids(0)                               ;
  plan->showMessage(tr("Drag %1 surnames").arg(Uuids.count())) ;
  return ( Uuids.count() > 0 )                                 ;
}

bool N::SurnameEditor::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::SurnameEditor::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  if (pos.manhattanLength()<40) return false                   ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::SurnameEditor::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::SurnameEditor::finishDrag(QMouseEvent * event)
{
  return true ;
}

UUIDs N::SurnameEditor::LoadUuids(SqlConnection & Connection)
{
  PeopleManager PM   ( plan       ) ;
  return PM.Surnames ( Connection ) ;
}

void N::SurnameEditor::LoadNames(SqlConnection & Connection,UUIDs & Uuids)
{
  SUID uu    = 0                                    ;
  TreeWidgetItems items                             ;
  foreach (uu,Uuids)                                {
    QString Q                                       ;
    QString name                                    ;
    int     language                                ;
    Q = Connection.sql.SelectFrom                   (
          "language,name"                           ,
          PlanTable(Names)                          ,
          Connection.WhereUuid(uu)                ) ;
    if (Connection.Fetch(Q))                        {
      bool exact = true                             ;
      language = Connection.Value(0).toInt()        ;
      name     = Connection.String(1)               ;
      if (OnlyLanguage>0 && OnlyLanguage!=language) {
        exact = false                               ;
      }                                             ;
      if (exact && name.length()>0)                 {
        NewTreeWidgetItem ( IT                    ) ;
        IT -> setText (0,name                     ) ;
        IT -> setData (0,Qt::UserRole,uu          ) ;
        IT -> setData (1,Qt::UserRole,language    ) ;
        IT -> setText (1,plan->languages[language]) ;
        items << IT                                 ;
      }                                             ;
    }                                               ;
  }                                                 ;
  addTopLevelItems ( items )                        ;
}

QTreeWidgetItem * N::SurnameEditor::AppendItem (void)
{
  NewTreeWidgetItem (IT)        ;
  IT->setData(0,Qt::UserRole,0) ;
  addTopLevelItem (IT)          ;
  return IT                     ;
}

void N::SurnameEditor::run(void)
{
  switch (Method)             {
    case 1: Load  ( ) ; break ;
    case 2: Items ( ) ; break ;
  }                           ;
}

void N::SurnameEditor::Load(void)
{
  AbstractGui::Mutex.lock  ()           ;
  setEnabled (false)                    ;
  emit OnBusy ( )                       ;
  SqlConnection SC(plan->sql)           ;
  if (SC.open("nSurnameEditor","Load")) {
    UUIDs Uuids = LoadUuids(SC)         ;
    LoadNames(SC,Uuids)                 ;
    SC.close()                          ;
  }                                     ;
  SC.remove()                           ;
  emit GoRelax (      )                 ;
  setEnabled   ( true )                 ;
  reportItems  (      )                 ;
  AbstractGui::Mutex.unlock()           ;
  Alert ( Done )                        ;
}

void N::SurnameEditor::Items(void)
{
  AbstractGui::Mutex.lock  ()                    ;
  setEnabled (false)                             ;
  emit OnBusy ( )                                ;
  SqlConnection SC(plan->sql)                    ;
  if (SC.open("nSurnameEditor","Items"))         {
    QString Q                                    ;
    int     t1 = Types  :: Surname               ;
    int     t2 = Types  :: Name                  ;
    int     rl = Groups :: Subordination         ;
    for (int i=0;i<topLevelItemCount();i++)      {
      QTreeWidgetItem * item                     ;
      SUID              uuid                     ;
      item = topLevelItem(i)                     ;
      uuid = nTreeUuid(item,0)                   ;
      Q = SC.sql.SelectFrom                      (
            "count(*)"                           ,
            PlanTable(Groups)                    ,
            QString("where first = %1 "
                    "and t1 = %2 "
                    "and t2 = %3 "
                    "and relation = %4"          )
            .arg(uuid).arg(t1).arg(t2).arg(rl) ) ;
      if (SC.Fetch(Q))                           {
        int total = SC . Int ( 0 )               ;
        item->setText(2,QString::number(total))  ;
        setAlignments(item                    )  ;
      }                                          ;
    }                                            ;
    SC.close()                                   ;
  }                                              ;
  emit GoRelax (      )                          ;
  SC.remove    (      )                          ;
  reportItems  (      )                          ;
  setEnabled   ( true )                          ;
  AbstractGui::Mutex.unlock()                    ;
  Alert ( Done )                                 ;
}

bool N::SurnameEditor::List(void)
{
  Method = 1  ;
  clear ( )   ;
  start ( )   ;
  return true ;
}

void N::SurnameEditor::editName(QTreeWidgetItem * item,int column)
{
  removeOldItem ( )                       ;
  dbClicked = true                        ;
  for (int i=0;i<10;i++)                  {
    DoProcessEvents                       ;
  }                                       ;
  /////////////////////////////////////////
  QLineEdit * name = new QLineEdit(this)  ;
  ItemEditing = item                      ;
  ItemColumn  = 0                         ;
  ItemWidget  = (QWidget *)name           ;
  name -> setText (item->text(0))         ;
  setItemWidget(item,0,name)              ;
  connect(name,SIGNAL(returnPressed  ())  ,
          this,SLOT  (editingFinished())) ;
  Alert ( Click )                         ;
  name->setFocus(Qt::TabFocusReason)      ;
}

void N::SurnameEditor::selectLanguage(QTreeWidgetItem * item,int column)
{
  removeOldItem ( )                                 ;
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

void N::SurnameEditor::doubleClicked(QTreeWidgetItem * item,int column)
{
  switch (column)                                    {
    case 0: editName       ( item , column ) ; break ;
    case 1: selectLanguage ( item , column ) ; break ;
  }                                                  ;
}

void N::SurnameEditor::New(void)
{
  removeOldItem     (  )                  ;
  QTreeWidgetItem * IT = AppendItem ()    ;
  scrollToItem(IT)                        ;
  QLineEdit * name = new QLineEdit(this)  ;
  ItemEditing = IT                        ;
  ItemColumn  = 0                         ;
  ItemWidget  = (QWidget *)name           ;
  IT->setData(0,Qt::UserRole  ,0        ) ;
  setItemWidget(IT,0,name)                ;
  connect(name,SIGNAL(editingFinished())  ,
          this,SLOT  (editingFinished())) ;
  name->setFocus(Qt::TabFocusReason)      ;
  if (!plan->Booleans["Desktop"])         {
    allowGesture = false                  ;
  }                                       ;
}

void N::SurnameEditor::Rename(void)
{
  QTreeWidgetItem * item = currentItem() ;
  nDropOut      ( IsNull(item) )         ;
  doubleClicked ( item , 0     )         ;
}

void N::SurnameEditor::AssignLanguage(void)
{
  bool changed = false                                         ;
  LanguageSelections * NLS = new LanguageSelections(this,plan) ;
  NLS->List(OnlyLanguage)                                      ;
  if (NLS->exec()==QDialog::Accepted)                          {
    OnlyLanguage = NLS->Language()                             ;
    changed      = true                                        ;
  }                                                            ;
  NLS -> deleteLater ( )                                       ;
  if (changed) List  ( )                                       ;
}

void N::SurnameEditor::removeOldItem(void)
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

void N::SurnameEditor::editingFinished(void)
{
  SUID uuid = nTreeUuid(ItemEditing,ItemColumn)                    ;
  QLineEdit * name = Casting(QLineEdit,ItemWidget)                 ;
  if (NotNull(name))                                               {
    QString task = name->text()                                    ;
    if (task.length()>0)                                           {
      AbstractGui::Mutex.lock  ()                                  ;
      SqlConnection SC ( plan->sql )                               ;
      PeopleManager PM ( plan      )                               ;
      if (SC.open("nSurnameEditor","editingFinished"))             {
        if (uuid<=0)                                               {
          int language = vLanguageId                               ;
          uuid = PM.addSurname(SC,language,task)                   ;
          ItemEditing->setText(1,plan->languages[language])        ;
          ItemEditing->setData(1,Qt::UserRole,language)            ;
        } else                                                     {
          int language = ItemEditing->data(1,Qt::UserRole).toInt() ;
          QString Q                                                ;
          Q = SC.sql.Update(plan->Tables[Tables::Names]            ,
                "where uuid = :UUID and language = :LANGUAGE"      ,
                1,"name"                                         ) ;
          SC . insertName(Q,uuid,language,task)                    ;
        }                                                          ;
        Alert ( Done )                                             ;
        SC.close()                                                 ;
      }                                                            ;
      SC.remove()                                                  ;
      AbstractGui::Mutex.unlock()                                  ;
      ItemEditing->setText(0,task)                                 ;
      ItemEditing->setData(0,Qt::UserRole,uuid)                    ;
    }                                                              ;
  }                                                                ;
  removeOldItem ( )                                                ;
}

void N::SurnameEditor::languageChanged(int index)
{
  SUID uuid = nTreeUuid(ItemEditing,0)                              ;
  QComboBox * comb = qobject_cast<QComboBox *>(ItemWidget)          ;
  if (NotNull(comb))                                                {
    int language = comb->itemData(index,Qt::UserRole).toULongLong() ;
    AbstractGui::Mutex.lock  ()                                     ;
    SqlConnection SC(plan->sql)                                     ;
    if (SC.open("nSurnameEditor","languageChanged"))                {
      QString Q                                                     ;
      if (uuid>0)                                                   {
        Q = SC.sql.Update(plan->Tables[Tables::Names]               ,
                QString("where uuid = %1").arg(uuid)                ,
                1,"language"                                      ) ;
        SC . Prepare ( Q )                                          ;
        SC . Bind    ("language",language)                          ;
        SC . Exec    (   )                                          ;
        ItemEditing->setText(1,plan->languages[language])           ;
        ItemEditing->setData(1,Qt::UserRole,language)               ;
      }                                                             ;
      SC.close()                                                    ;
    }                                                               ;
    SC.remove()                                                     ;
    AbstractGui::Mutex.unlock()                                     ;
  }                                                                 ;
  removeOldItem ( )                                                 ;
}

void N::SurnameEditor::Statistics(void)
{
  setColumnHidden ( 2 , false ) ;
  Method = 2                    ;
  start ( )                     ;
}

void N::SurnameEditor::CopyToClipboard(void)
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

void N::SurnameEditor::Search(void)
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

void N::SurnameEditor::Search(QString text)
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

void N::SurnameEditor::SearchReady(void)
{
  if (IsNull(SearchText)) return                ;
  QString text = SearchText->lineEdit()->text() ;
  SearchText  -> deleteLater()                  ;
  SearchText   = NULL                           ;
  Search ( text )                               ;
}

void N::SurnameEditor::Paste(void)
{
  QString text = nClipboardText                                ;
  if (text.length()<=0)                                        {
    Alert ( Error )                                            ;
    return                                                     ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  bool changed     = false                                     ;
  int  uselanguage = OnlyLanguage                              ;
  LanguageSelections * NLS = new LanguageSelections(this,plan) ;
  NLS->List(OnlyLanguage)                                      ;
  if (NLS->exec()==QDialog::Accepted)                          {
    uselanguage = NLS->Language()                              ;
    changed     = true                                         ;
  }                                                            ;
  NLS -> deleteLater ( )                                       ;
  if (!changed      ) return                                   ;
  if (uselanguage<=0) return                                   ;
  //////////////////////////////////////////////////////////////
  emit OnBusy ( )                                              ;
  //////////////////////////////////////////////////////////////
  QStringList L = text.split("\n")                             ;
  QString     S                                                ;
  L = File::PurifyLines(L)                                     ;
  if (L.count()<=0)                                            {
    Alert ( Error )                                            ;
    return                                                     ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  PeopleManager PM ( plan )                                    ;
  EnterSQL ( SC , plan->sql )                                  ;
    QStringList O                                              ;
    QString     Q                                              ;
    UUIDs       U = PM.Surnames ( SC )                         ;
    SUID        u                                              ;
    ////////////////////////////////////////////////////////////
    foreach (u,U)                                              {
      QString name                                             ;
      int     language                                         ;
      Q = SC.sql.SelectFrom                                    (
            "language,name"                                    ,
            PlanTable(Names)                                   ,
            SC.WhereUuid(u) )                                  ;
      if (SC.Fetch(Q))                                         {
        bool exact = true                                      ;
        language = SC.Value(0).toInt()                         ;
        name     = SC.String(1)                                ;
        if (language!=uselanguage) exact = false               ;
        if (exact) O << name                                   ;
      }                                                        ;
    }                                                          ;
    ////////////////////////////////////////////////////////////
    foreach (S,L)                                              {
      QString C = S                                            ;
      C = C.simplified()                                       ;
      if (C.length()>0 && !O.contains(C))                      {
        SUID uu = PM.addSurname(SC,uselanguage,C)              ;
        if (uu>0)                                              {
          QTreeWidgetItem * IT                                 ;
          IT       = addItem(C,uu,0)                           ;
          IT -> setData (1,Qt::UserRole,uselanguage)           ;
          IT -> setText (1,plan->languages[uselanguage])       ;
          scrollToItem ( IT )                                  ;
          DoProcessEvents                                      ;
          O << C                                               ;
        }                                                      ;
      }                                                        ;
    }                                                          ;
  ErrorSQL ( SC , plan->sql )                                  ;
  LeaveSQL ( SC , plan->sql )                                  ;
  //////////////////////////////////////////////////////////////
  emit GoRelax (      )                                        ;
  Alert        ( Done )                                        ;
}

void N::SurnameEditor::Location(void)
{
  UUIDs Uuids = itemUuids(0)                                 ;
  ScopedProgress P(plan,tr("%v/%m"),100)                     ;
  P.setRange (0,Uuids.count()*2)                             ;
  P.show     (                 )                             ;
  setEnabled (false            )                             ;
  EnterSQL ( SC , plan->sql )                                ;
    QString Q                                                ;
    int     last = 0                                         ;
    UUIDs   A    = LoadUuids(SC)                             ;
    for (int i=0;i<Uuids.count();i++)                        {
      SUID u = Uuids[i]                                      ;
      if (A.contains(u))                                     {
        int index = A.indexOf(u)                             ;
        A.takeAt(index)                                      ;
      }                                                      ;
    }                                                        ;
    A << Uuids                                               ;
    Uuids = A                                                ;
    Q = SC.sql.SelectFrom                                    (
          "id"                                               ,
          PlanTable(Surnames)                                ,
          QString("%1 %2"                                    )
          .arg(SC.OrderByDesc("id")                          )
          .arg(SC.sql.Limit(0,1)   )                       ) ;
    if (SC.Fetch(Q)) last = SC.Int(0)                        ;
    last++                                                   ;
    P.Start (0)                                              ;
    for (int i=0;i<Uuids.count();i++)                        {
      ++P                                                    ;
      Q = SC.sql.Update                                      (
            PlanTable(Surnames)                              ,
            SC.WhereUuid(Uuids[i])                           ,
            1                                                ,
            "id"                                           ) ;
      SC . Prepare ( Q               )                       ;
      SC . Bind    ( "id" , i + last )                       ;
      SC . Exec    (                 )                       ;
    }                                                        ;
    for (int i=0;i<Uuids.count();i++)                        {
      ++P                                                    ;
      Q = SC.sql.Update                                      (
            PlanTable(Surnames)                              ,
            SC.WhereUuid(Uuids[i])                           ,
            1                                                ,
            "id"                                           ) ;
      SC . Prepare ( Q            )                          ;
      SC . Bind    ( "id" , i + 1 )                          ;
      SC . Exec    (              )                          ;
    }                                                        ;
    P.Finish( )                                              ;
  ErrorSQL   ( SC , plan->sql )                              ;
  LeaveSQL   ( SC , plan->sql )                              ;
  setEnabled ( true           )                              ;
  Alert      ( Done           )                              ;
}

void N::SurnameEditor::ImportMappings(void)
{
  QString filename                               ;
  filename = QFileDialog::getOpenFileName        (
               this                              ,
               tr("Import surname mapping file") ,
               plan->Temporary("")               ,
               "*.txt"                         ) ;
  if (filename.length()<=0)                      {
    Alert ( Error )                              ;
    return                                       ;
  }                                              ;
  QStringList Lines                              ;
  Lines = File::StringList(filename,"\n")        ;
  if (Lines.count()<=0)                          {
    Alert ( Error )                              ;
    return                                       ;
  }                                              ;
  Lines = File::PurifyLines(Lines)               ;
  ////////////////////////////////////////////////
  int slanguage = 0                              ;
  int tlanguage = 0                              ;
  ////////////////////////////////////////////////
  LanguageSelections * NLS                       ;
  NLS = new LanguageSelections(this,plan)        ;
  NLS->setWindowTitle(tr("Major language"))      ;
  NLS->List(OnlyLanguage)                        ;
  if (NLS->exec()==QDialog::Accepted)            {
    slanguage = NLS->Language()                  ;
  } else                                         {
    NLS -> deleteLater ( )                       ;
    Alert ( Error )                              ;
    return                                       ;
  }                                              ;
  NLS->setWindowTitle(tr("Mapping language"))    ;
  NLS->List(OnlyLanguage)                        ;
  if (NLS->exec()==QDialog::Accepted)            {
    tlanguage = NLS->Language()                  ;
  } else                                         {
    NLS -> deleteLater ( )                       ;
    Alert ( Error )                              ;
    return                                       ;
  }                                              ;
  NLS -> deleteLater ( )                         ;
  ////////////////////////////////////////////////
  Alert ( Click )                                ;
  emit OnBusy ( )                                ;
  PeopleManager PM ( plan )                      ;
  ScopedProgress P ( plan , tr("%v/%m") , 10 )   ;
  P . setRange ( 0 , Lines.count() )             ;
  P . show     (                   )             ;
  EnterSQL ( SC , plan->sql )                    ;
    QString Q                                    ;
    QString S                                    ;
    UUIDs   surnames = PM . Surnames ( SC )      ;
    SUID    u                                    ;
    ZMAPs   muids                                ;
    ZMAPs   tuids                                ;
    P . setRange ( 0 , surnames.count() )        ;
    P . Start    ( 0                    )        ;
    foreach (u,surnames)                         {
      ++P                                        ;
      Q = SC.sql.SelectFrom                      (
            "language,name"                      ,
            PlanTable(Names)                     ,
            SC.WhereUuid(u)                    ) ;
      if (SC.Fetch(Q))                           {
        int     l = SC . Int    ( 0 )            ;
        QString n = SC . String ( 1 )            ;
        if (l==slanguage) muids [ n ] = u        ;
        if (l==tlanguage) tuids [ n ] = u        ;
      }                                          ;
    }                                            ;
    P . Finish   (                   )           ;
    P . setRange ( 0 , Lines.count() )           ;
    P . Start    ( 0                 )           ;
    foreach (S,Lines)                            {
      ++P                                        ;
      QStringList T = S.split(' ')               ;
      if (T.count()>1)                           {
        QString z = T[0]                         ;
        if (z.length()>0 && muids.contains(z))   {
          SUID U = muids[z]                      ;
          for (int i=1;i<T.count();i++)          {
            QString w = T[i]                     ;
            if (w.length()>0)                    {
              if (tuids.contains(w))             {
                SUID u = tuids[w]                ;
                PM.addEqualSurname(SC,U,u)       ;
              }                                  ;
            }                                    ;
          }                                      ;
        }                                        ;
      }                                          ;
    }                                            ;
    P . Finish ( )                               ;
    Alert  ( Done           )                    ;
  ErrorSQL ( SC , plan->sql )                    ;
    Alert  ( Error          )                    ;
  LeaveSQL ( SC , plan->sql )                    ;
  emit GoRelax (      )                          ;
}

bool N::SurnameEditor::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                      ;
  QAction * aa                                                   ;
  QMenu   * ms                                                   ;
  QMenu   * ma                                                   ;
  QMenu   * mp                                                   ;
  QTreeWidgetItem * item = currentItem ( )                       ;
  mm . add          ( 101 , tr("Refresh")         )              ;
  mm . add          ( 102 , tr("Insert" )         )              ;
  mm . add          ( 103 , tr("Paste"  )         )              ;
  mm . add          ( 104 , tr("Assign language") )              ;
  mm . add          ( 105 , tr("All languages"  ) )              ;
  nIfSafe(item)                                                  {
    mm . add        ( 106 , tr("Rename" )         )              ;
    mm . addSeparator (                           )              ;
    mm . add        ( 201 , tr("Equivalences"  )  )              ;
    mm . add        ( 202 , tr("Documents"     )  )              ;
    mm . add        ( 203 , tr("Related people")  )              ;
  }                                                              ;
  mm . addSeparator (                             )              ;
  mm . add          ( 301 , tr("Search"    )      )              ;
  mm . add          ( 302 , tr("Update sequences to database") ) ;
  mm . add          ( 303 , tr("Import surname mappings"     ) ) ;
  mm . add          ( 901 , tr("Statistics")      )              ;
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
    nFastCast  ( 102 , New            ( ) )                      ;
    nFastCast  ( 103 , Paste          ( ) )                      ;
    nFastCast  ( 104 , AssignLanguage ( ) )                      ;
    nFastCast  ( 106 , Rename         ( ) )                      ;
    nFastCast  ( 301 , Search         ( ) )                      ;
    nFastCast  ( 302 , Location       ( ) )                      ;
    nFastCast  ( 303 , ImportMappings ( ) )                      ;
    nFastCast  ( 901 , Statistics     ( ) )                      ;
    case 105                                                     :
      OnlyLanguage = 0                                           ;
      List ( )                                                   ;
    break                                                        ;
    case 201                                                     :
      nIfSafe(item)                                              {
        QString name    = item->text(0)                          ;
        SUID    surname = nTreeUuid(item,0)                      ;
        emit Translations ( name , surname )                     ;
      }                                                          ;
    break                                                        ;
    case 202                                                     :
      nIfSafe(item)                                              {
        QString name    = item->text(0)                          ;
        SUID    surname = nTreeUuid(item,0)                      ;
        emit Documents ( name , surname )                        ;
      }                                                          ;
    break                                                        ;
    case 203                                                     :
      nIfSafe(item)                                              {
        QString name    = item->text(0)                          ;
        SUID    surname = nTreeUuid(item,0)                      ;
        UUIDs   people                                           ;
        PeopleManager PM ( plan )                                ;
        EnterSQL ( SC , plan->sql )                              ;
          PM . PersonHasSurname ( SC , surname , people )        ;
        ErrorSQL ( SC , plan->sql )                              ;
          plan->showMessage(tr("Database can not be accessed.")) ;
        LeaveSQL ( SC , plan->sql )                              ;
        if (people.count()>0)                                    {
          emit People ( name , surname ,people )                 ;
        } else                                                   {
          Alert ( Error )                                        ;
        }                                                        ;
      }                                                          ;
    break                                                        ;
  }                                                              ;
  return true                                                    ;
}
