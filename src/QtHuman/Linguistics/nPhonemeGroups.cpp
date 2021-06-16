#include <qthuman.h>

N::PhonemeGroups:: PhonemeGroups ( QWidget * parent , Plan * p )
                 : TreeDock      (           parent ,        p )
                 , GroupItems    (                           p )
                 , dropAction    ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PhonemeGroups::~PhonemeGroups (void)
{
}

QSize N::PhonemeGroups::sizeHint(void) const
{
  return QSize ( 480 , 480 ) ;
}

bool N::PhonemeGroups::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::PhonemeGroups::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::PhonemeGroups::FocusIn(void)
{
  LinkAction ( Insert     , New             ( ) ) ;
  LinkAction ( Refresh    , startup         ( ) ) ;
  LinkAction ( Copy       , CopyToClipboard ( ) ) ;
  LinkAction ( SelectNone , SelectNone      ( ) ) ;
  LinkAction ( SelectAll  , SelectAll       ( ) ) ;
  return true                                     ;
}

void N::PhonemeGroups::Configure(void)
{
  setWindowTitle               ( tr("Phoneme groups")             ) ;
  setWindowIcon                ( QIcon(":/images/audiofiles.png") ) ;
  NewTreeWidgetItem            ( head                             ) ;
  head -> setText              ( 0 , tr("Name"      )             ) ;
  head -> setText              ( 1 , tr("Identifier")             ) ;
  head -> setText              ( 2 , tr("Language"  )             ) ;
  setDragDropMode              ( DropOnly                         ) ;
  setRootIsDecorated           ( false                            ) ;
  setAlternatingRowColors      ( true                             ) ;
  setSelectionMode             ( ExtendedSelection                ) ;
  setColumnCount               ( 3                                ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded            ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded            ) ;
  assignHeaderItems            ( head                             ) ;
  MountClicked                 ( 2                                ) ;
  plan -> setFont              ( this                             ) ;
}

void N::PhonemeGroups::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch   ( Type ) {
    case 10001      :
      List (      ) ;
    break           ;
  }                 ;
}

void N::PhonemeGroups::List(void)
{
  blockSignals      ( true        )                      ;
  setEnabled        ( false       )                      ;
  FinanceManager FM ( plan        )                      ;
  SqlConnection  SC ( plan -> sql )                      ;
  if ( SC . open ( "PhonemeGroups" , "List" ) )          {
    QString N                                            ;
    QString I                                            ;
    UUIDs   U                                            ;
    SUID    u                                            ;
    int     L                                            ;
    U = GroupItems :: Groups                             (
          SC                                             ,
          Types::Phoneme                                 ,
          SC.OrderByAsc("id") )                          ;
    foreach ( u , U )                                    {
      N = SC . getName                                   (
            PlanTable(Names)                             ,
            "uuid"                                       ,
            vLanguageId                                  ,
            u                                          ) ;
      I = FM . FetchIdentifier ( SC , u                ) ;
      L = usingLanguage ( SC , u , Types::Division , 0 ) ;
      NewTreeWidgetItem ( it                           ) ;
      it -> setData     ( 0 , Qt::UserRole , u         ) ;
      it -> setData     ( 2 , Qt::UserRole , L         ) ;
      it -> setText     ( 0 , N                        ) ;
      it -> setText     ( 1 , I                        ) ;
      if (L>0)                                           {
        it -> setText   ( 2 , (plan->languages)[L]     ) ;
      }                                                  ;
      addTopLevelItem   ( it                           ) ;
    }                                                    ;
    SC . close          (                              ) ;
  }                                                      ;
  SC . remove           (                              ) ;
  setEnabled            ( true                         ) ;
  blockSignals          ( false                        ) ;
  reportItems           (                              ) ;
  plan -> StopBusy      (                              ) ;
  emit AutoFit          (                              ) ;
  Alert                 ( Done                         ) ;
}

bool N::PhonemeGroups::startup(void)
{
  clear             (       ) ;
  plan -> StartBusy (       ) ;
  start             ( 10001 ) ;
  return true                 ;
}

void N::PhonemeGroups::New(void)
{
  NewTreeWidgetItem ( item                 ) ;
  item -> setData   ( 0 , Qt::UserRole , 0 ) ;
  item -> setData   ( 2 , Qt::UserRole , 0 ) ;
  addTopLevelItem   ( item                 ) ;
  scrollToItem      ( item                 ) ;
  doubleClicked     ( item , 0             ) ;
}

void N::PhonemeGroups::doubleClicked (QTreeWidgetItem * item,int column)
{
  SUID u = nTreeUuid ( item , 0 )          ;
  int  l = nTreeInt  ( item , 2 )          ;
  QLineEdit * line                         ;
  ComboBox  * cc                           ;
  switch (column)                          {
    case 0                                 :
      line = setLineEdit                   (
        item                               ,
        column                             ,
        SIGNAL(returnPressed())            ,
        SLOT  (nameChanged  ()) )          ;
      line->setFocus(Qt::TabFocusReason)   ;
    break                                  ;
    case 1                                 :
      if ( u <= 0 ) return                 ;
      line = setLineEdit                   (
        item                               ,
        column                             ,
        SIGNAL(returnPressed())            ,
        SLOT  (identChanged ()) )          ;
      line->setFocus(Qt::TabFocusReason)   ;
    break                                  ;
    case 2                                 :
      if ( u <= 0 ) return                 ;
      cc = (ComboBox *)setComboBox         (
        item                               ,
        column                             ,
        SIGNAL(activated  (int))           ,
        SLOT  (langChanged(int))         ) ;
      ((QComboBox *)cc) << plan->languages ;
      if (l>0) (*(QComboBox *)cc) <= l     ;
      cc -> setEditable ( true )           ;
      cc -> showPopup   (      )           ;
    break                                  ;
  }                                        ;
}

void N::PhonemeGroups::nameChanged(void)
{
  if (IsNull(ItemEditing)) return                       ;
  if (IsNull(ItemWidget )) return                       ;
  QLineEdit * line    = Casting(QLineEdit,ItemWidget)   ;
  SUID        u       = nTreeUuid(ItemEditing,0)        ;
  QString     n       = ""                              ;
  bool        success = false                           ;
  if (NotNull(line)) n = line->text()                   ;
  if ( n . length ( ) > 0 )                             {
    EnterSQL ( SC , plan->sql )                         ;
      if ( u <= 0 )                                     {
        u = NewDivision ( SC , n , Types::Phoneme     ) ;
        ItemEditing -> setData ( 0 , Qt::UserRole , u ) ;
      }                                                 ;
      SC . assureName                                   (
        PlanTable(Names)                                ,
        u                                               ,
        vLanguageId                                     ,
        n                                             ) ;
      ItemEditing -> setText ( 0 , n )                  ;
      success = true                                    ;
    LeaveSQL ( SC , plan->sql )                         ;
  }                                                     ;
  removeOldItem ( true , 0 )                            ;
  if (success)                                          {
    Alert ( Done  )                                     ;
  } else                                                {
    Alert ( Error )                                     ;
  }                                                     ;
}

void N::PhonemeGroups::identChanged(void)
{
  if (IsNull(ItemEditing)) return                  ;
  if (IsNull(ItemWidget )) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  SUID        u    = nTreeUuid(ItemEditing,0)      ;
  QString     n    = ""                            ;
  if (NotNull(line)) n = line->text()              ;
  if (u>0 && n.length()>0)                         {
    FinanceManager FM       ( plan           )     ;
    EnterSQL                ( SC , plan->sql )     ;
      FM . assureIdentifier ( SC , u , n     )     ;
    LeaveSQL                ( SC , plan->sql )     ;
    Alert                   ( Done           )     ;
  }                                                ;
  removeOldItem             ( false , 1      )     ;
}

void N::PhonemeGroups::langChanged(int)
{
  if (IsNull(ItemEditing)) return                         ;
  if (IsNull(ItemWidget )) return                         ;
  QComboBox * c = Casting(QComboBox,ItemWidget)           ;
  SUID        u = nTreeUuid(ItemEditing,0)                ;
  int         L = N :: GetUuid ( c )                      ;
  if (u>0 && L>0)                                         {
    EnterSQL ( SC , plan->sql )                           ;
      GroupItems :: assignLanguage                        (
        SC                                                ,
        u                                                 ,
        Types::Division                                   ,
        L                                               ) ;
      ItemEditing -> setData ( 2 , Qt::UserRole , L     ) ;
      ItemEditing -> setText ( 2 , (plan->languages)[L] ) ;
    LeaveSQL ( SC , plan->sql )                           ;
    Alert    ( Done           )                           ;
  }                                                       ;
  removeOldItem ( false , 2 )                             ;
}

bool N::PhonemeGroups::dropPhonemes(QWidget * widget,QPointF pos,const UUIDs & Uuids)
{
  return true ;
}

bool N::PhonemeGroups::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                       ;
  QAction         * aa                            ;
  QTreeWidgetItem * it = itemAt ( pos )           ;
  mm . add ( 101 , tr("New"         ) )           ;
  if (NotNull(it))                                {
    mm . add ( 102 , tr("Edit"      ) )           ;
  }                                               ;
  mm . add ( 103 , tr("Refresh"     ) )           ;
  mm . addSeparator  (                )           ;
  mm . add ( 201 , tr("Copy"        ) )           ;
  mm . add ( 202 , tr("Select all"  ) )           ;
  mm . add ( 203 , tr("Select none" ) )           ;
  mm . addSeparator (                 )           ;
  mm . add ( 901 , tr("Translations") )           ;
  DockingMenu ( mm )                              ;
  mm.setFont(plan)                                ;
  aa = mm.exec()                                  ;
  if (IsNull(aa)) return true                     ;
  if (RunDocking(mm,aa)) return true              ;
  UUIDs U                                         ;
  switch (mm[aa])                                 {
    case 101                                      :
      New               (   )                     ;
    break                                         ;
    case 102                                      :
      emit Edit ( it->text(0) , nTreeUuid(it,0) ) ;
    break                                         ;
    case 103                                      :
      startup           (   )                     ;
    break                                         ;
    case 201                                      :
      CopyToClipboard   (   )                     ;
    break                                         ;
    case 202                                      :
      SelectAll         (   )                     ;
    break                                         ;
    case 203                                      :
      SelectNone        (   )                     ;
    break                                         ;
    case 901                                      :
      U = itemUuids     ( 0                 )     ;
      emit Translations ( windowTitle() , U )     ;
    break                                         ;
  }                                               ;
  return true                                     ;
}
