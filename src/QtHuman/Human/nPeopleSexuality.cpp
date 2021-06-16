#include <qthuman.h>

#define TABLES(ID) PeoplePlan->Tables[Tables::ID]

N::PeopleSexuality:: PeopleSexuality ( QWidget * parent , Plan * p )
                   : TreeDock        (           parent ,        p )
                   , PeopleManager   (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleSexuality::~PeopleSexuality(void)
{
}

QSize N::PeopleSexuality::sizeHint(void) const
{
  return QSize(240,240) ;
}

void N::PeopleSexuality::Configure(void)
{
  StackSize   = 0                           ;
  Reservation = false                       ;
  ///////////////////////////////////////////
  NewTreeWidgetItem ( head                ) ;
  head -> setText   ( 0 , tr("People"   ) ) ;
  head -> setText   ( 1 , tr("Sexuality") ) ;
  setColumnCount    ( 2                   ) ;
  assignHeaderItems ( head                ) ;
  setWindowTitle    ( tr("Sexuality")     ) ;
  plan -> setFont   ( this                ) ;
  MountClicked      ( 2                   ) ;
}

bool N::PeopleSexuality::FocusIn(void)
{
  AssignAction ( Label   , windowTitle     ( ) ) ;
  LinkAction   ( Refresh , List            ( ) ) ;
  LinkAction   ( Copy    , CopyToClipboard ( ) ) ;
  return true                                    ;
}

void N::PeopleSexuality::run(void)
{
  TreeWidgetItems Items                               ;
  GroupItems GI    ( plan                    )        ;
  setEnabled       ( false                   )        ;
  EnterSQL         ( SC , plan->sql          )        ;
    QStringList names                                 ;
    QString     Q                                     ;
    UUIDs       Uuids                                 ;
    SUID        uuid                                  ;
    ///////////////////////////////////////////////////
    Uuids = HumanSexualities(SC)                      ;
    foreach (uuid,Uuids)                              {
      QString name = Name(SC,uuid,vLanguageId)        ;
      Sexualities[uuid] = name                        ;
    }                                                 ;
    ///////////////////////////////////////////////////
    Uuids = Lists  ( SC , "order by id desc" )        ;
    foreach (uuid,Uuids)                              {
      names = Names ( SC , uuid )                     ;
      if (names.count()>0)                            {
        SUID xuid = 0                                 ;
        xuid = GI.FindSecond                          (
                 SC                                   ,
                 uuid                                 ,
                 Types::People                        ,
                 Types::Gender                        ,
                 Groups::Sexuality                    ,
                 "order by position asc"            ) ;
        NewTreeWidgetItem ( item                    ) ;
        item -> setData   ( 0 , Qt::UserRole , uuid ) ;
        item -> setData   ( 1 , Qt::UserRole , xuid ) ;
        item -> setText   ( 0 , names[0]            ) ;
        if (xuid>0)                                   {
          item -> setText ( 1 , Sexualities[xuid]   ) ;
        }                                             ;
        Items << item                                 ;
      }                                               ;
    }                                                 ;
  LeaveSQL         ( SC , plan->sql          )        ;
  setEnabled       ( true                    )        ;
  if (Items.count()>0) addTopLevelItems(Items)        ;
  emit AutoFit     (                         )        ;
  Alert            ( Done                    )        ;
}

bool N::PeopleSexuality::List(void)
{
  clear ( )   ;
  start ( )   ;
  return true ;
}

void N::PeopleSexuality::doubleClicked(QTreeWidgetItem * item,int column)
{
  if (column!=1) return                 ;
  SUID        xuid = nTreeUuid(item,1)  ;
  QComboBox * sex  = NULL               ;
  sex = setComboBox                     (
          item                          ,
          1                             ,
          SIGNAL(activated(int))        ,
          SLOT(sexualityChanged(int)) ) ;
    sex  << Sexualities                 ;
  (*sex) <= xuid                        ;
    sex  -> showPopup ( )               ;
}

void N::PeopleSexuality::sexualityChanged(int)
{
  if (IsNull(ItemEditing)) return              ;
  if (IsNull(ItemWidget )) return              ;
  QComboBox * sex = NULL                       ;
  sex = Casting(QComboBox,ItemWidget)          ;
  if (IsNull(sex)) return                      ;
  SUID      uuid = nTreeUuid(ItemEditing,0)    ;
  SUID      xuid = N::GetUuid(sex)             ;
  ItemEditing->setText(1,sex->currentText())   ;
  ItemEditing->setData(1,Qt::UserRole,xuid )   ;
  EnterSQL         ( SC , plan->sql          ) ;
    AssignSexuality( SC , uuid , xuid        ) ;
  LeaveSQL         ( SC , plan->sql          ) ;
  removeOldItem    (                         ) ;
  Alert            ( Done                    ) ;
}
