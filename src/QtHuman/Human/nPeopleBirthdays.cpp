#include <qthuman.h>

#define TABLES(ID) PeoplePlan->Tables[Tables::ID]

N::PeopleBirthdays:: PeopleBirthdays ( QWidget * parent , Plan * p )
                   : TreeDock        (           parent ,        p )
                   , PeopleManager   (                           p )
                   , HistoryManager  (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleBirthdays::~PeopleBirthdays(void)
{
}

QSize N::PeopleBirthdays::sizeHint(void) const
{
  if (plan->Booleans["Phone"]) {
    return QSize(320,240)      ;
  }                            ;
  return QSize(480,240)        ;
}

void N::PeopleBirthdays::Configure(void)
{
  NewTreeWidgetItem ( head               ) ;
  head -> setText   ( 0 , tr("People"  ) ) ;
  head -> setText   ( 1 , tr("Birthday") ) ;
  head -> setText   ( 2 , tr("Age"     ) ) ;
  setColumnCount    ( 3                  ) ;
  assignHeaderItems ( head               ) ;
  setWindowTitle    ( tr("Birthdays")    ) ;
  plan -> setFont   ( this               ) ;
  MountClicked      ( 2                  ) ;
}

bool N::PeopleBirthdays::FocusIn(void)
{
  AssignAction ( Label , windowTitle ( ) ) ;
  return true                              ;
}

void N::PeopleBirthdays::run(int type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch   ( type ) {
    case 10001      :
      List (      ) ;
    break           ;
  }                 ;
}

bool N::PeopleBirthdays::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

bool N::PeopleBirthdays::List(void)
{
  TreeWidgetItems items                               ;
  clear            (                         )        ;
  setEnabled       ( false                   )        ;
  GroupItems GI    ( plan                    )        ;
  EnterSQL         ( SC , plan->sql          )        ;
    QStringList names                                 ;
    QString     Q                                     ;
    UUIDs       Uuids                                 ;
    SUID        uuid                                  ;
    Uuids = Lists  ( SC , "order by id desc" )        ;
    foreach (uuid,Uuids)                              {
      names = Names ( SC , uuid )                     ;
      if (names.count()>0)                            {
        SUID  xuid  = 0                               ;
        TUID  tuid  = 0                               ;
        UUIDs TUIDs = GI.Subordination                (
                      SC                              ,
                      uuid                            ,
                      Types::People                   ,
                      Types::History                  ,
                      Groups::Subordination           ,
                      "order by position asc"       ) ;
        if (TUIDs.count()>0)                          {
          xuid = GetHistoryType                       (
                   SC                                 ,
                   TUIDs                              ,
                   History::PeopleBirth             ) ;
          if (xuid>0) tuid = GetTime(SC,xuid)         ;
        }                                             ;
        QDateTime D                                   ;
        if (tuid>0) D = toDateTime(tuid)              ;
        NewTreeWidgetItem ( item                    ) ;
        item -> setData   ( 0 , Qt::UserRole , uuid ) ;
        item -> setData   ( 1 , Qt::UserRole , xuid ) ;
        item -> setText   ( 0 , names[0]            ) ;
        if (tuid>0)                                   {
          setDateTime     ( item , D                ) ;
        }                                             ;
        items << item                                 ;
      }                                               ;
    }                                                 ;
  LeaveSQL         ( SC , plan->sql          )        ;
  addTopLevelItems ( items                   )        ;
  setEnabled       ( true                    )        ;
  emit AutoFit     (                         )        ;
  Alert            ( Done                    )        ;
  return true                                         ;
}

void N::PeopleBirthdays::doubleClicked(QTreeWidgetItem * item,int column)
{
  if (column!=1) return                                ;
  QString   T = item->text(1)                          ;
  QDateTime E                                          ;
  QTime     Z ( 0 , 0 , 0 , 0 )                        ;
  if (T.length()>0)                                    {
    E = QDateTime::fromString(T,"yyyy/MM/dd hh:mm:ss") ;
  } else                                               {
    E = QDateTime::currentDateTime (   )               ;
    E . setTime                    ( Z )               ;
  }                                                    ;
  QDateTimeEdit * dte = new QDateTimeEdit(this)        ;
  dte -> setDisplayFormat ( "yyyy/MM/dd hh:mm:ss" )    ;
  dte -> setDateTime      ( E                     )    ;
  setItemWidget           ( item , column , dte   )    ;
  ItemEditing = item                                   ;
  ItemColumn  = column                                 ;
  ItemWidget  = dte                                    ;
  connect(dte ,SIGNAL(editingFinished())               ,
          this,SLOT  (timeChanged    ())             ) ;
}

void N::PeopleBirthdays::timeChanged(void)
{
  if (IsNull(ItemEditing)) return                 ;
  if (IsNull(ItemWidget )) return                 ;
  QDateTimeEdit * dte = NULL                      ;
  dte = Casting(QDateTimeEdit,ItemWidget)         ;
  if (IsNull(dte)) return                         ;
  QDateTime d    = dte->dateTime()                ;
  SUID      uuid = nTreeUuid(ItemEditing,0)       ;
  SUID      tuid = nTreeUuid(ItemEditing,1)       ;
  QDateTime Now  = QDateTime::currentDateTime ( ) ;
  StarDate SD                                     ;
  SD = d                                          ;
  setDateTime      ( ItemEditing , d         )    ;
  GroupItems GI    ( plan                    )    ;
  EnterSQL         ( SC , plan->sql          )    ;
    SUID ouid = tuid                              ;
    tuid = assureTime                             (
             SC                                   ,
             tuid                                 ,
             History::PeopleBirth                 ,
             SD.Stardate                        ) ;
    if (ouid<=0 && tuid>0)                        {
      UUIDs TUIDs                                 ;
      TUIDs << tuid                               ;
      GI.Join                                     (
        SC                                        ,
        uuid                                      ,
        Types::People                             ,
        Types::History                            ,
        Groups::Subordination                     ,
        0                                         ,
        TUIDs                                   ) ;
    }                                             ;
  LeaveSQL         ( SC , plan->sql          )    ;
  removeOldItem    (                         )    ;
  Alert ( Done )                                  ;
}

void N::PeopleBirthdays::setDateTime(QTreeWidgetItem * item,QDateTime dt)
{
  QDateTime Now   = QDateTime::currentDateTime ( )               ;
  QDate     ND    = Now.date()                                   ;
  QDate     DD    = dt.date()                                    ;
  QDate     XD    = dt.date()                                    ;
  QDate     SD    = dt.date()                                    ;
  int       Secs  = dt.secsTo(Now)                               ;
  int       Days  = 0                                            ;
  int       Years = 0                                            ;
  bool      done  = false                                        ;
  bool      error = false                                        ;
  if (!dt.isValid() || !DD.isValid() || !XD.isValid())           {
    error = true                                                 ;
    done  = true                                                 ;
  }                                                              ;
  if (Secs<0) done = true                                        ;
  Years = ND.year() - DD.year() - 3                              ;
  if (Years<0)                                                   {
    Years = 0                                                    ;
    error = true                                                 ;
    done  = true                                                 ;
  }                                                              ;
  if (Years>0)                                                   {
    if (SD.month()==2 && SD.day()==29)                           {
      int  dxs  = DD.year() + Years                              ;
      bool skip = false                                          ;
      if ((dxs%400)==0) skip = false                        ; else
      if ((dxs%100)==0) skip = true                         ; else
      if ((dxs%  4)==0) skip = false                        ; else
                        skip = true                              ;
      if (skip)                                                  {
        XD.setDate(dxs,3,1)                                      ;
      } else                                                     {
        XD.setDate(dxs,2,29)                                     ;
      }                                                          ;
    } else                                                       {
      XD.setDate(DD.year()+Years,DD.month(),DD.day())            ;
    }                                                            ;
    DD = XD                                                      ;
  }                                                              ;
  while (!done)                                                  {
    if (Years>5000)                                              {
      error = true                                               ;
      done  = true                                               ;
    }                                                            ;
    if (SD.month()==2 && SD.day()==29)                           {
      int  dxs  = DD.year() + 1                                  ;
      bool skip = false                                          ;
      if ((dxs%400)==0) skip = false                        ; else
      if ((dxs%100)==0) skip = true                         ; else
      if ((dxs%  4)==0) skip = false                        ; else
                        skip = true                              ;
      if (skip)                                                  {
        XD.setDate(dxs,3,1)                                      ;
      } else                                                     {
        XD.setDate(dxs,2,29)                                     ;
      }                                                          ;
    } else                                                       {
      XD.setDate(DD.year()+1,DD.month(),DD.day())                ;
    }                                                            ;
    if (XD>ND)                                                   {
      Days = DD.daysTo(ND)                                       ;
      done = true                                                ;
    } else                                                       {
      Years ++                                                   ;
      DD = XD                                                    ;
    }                                                            ;
  }                                                              ;
  item->setText(1,dt.toString("yyyy/MM/dd hh:mm:ss"))            ;
  if (error)                                                     {
    item->setText(2,tr("Wrong timestamp")                      ) ;
  } else                                                         {
    item->setText(2,tr("%1 years %2 days").arg(Years).arg(Days)) ;
  }                                                              ;
}

bool N::PeopleBirthdays::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                                      ;
  QMdiSubWindow   * mdi    = Casting(QMdiSubWindow,parent())         ;
  QDockWidget     * dock   = Casting(QDockWidget  ,parent())         ;
  QTreeWidgetItem * item   = itemAt(pos)                             ;
  QPoint            global = mapToGlobal(pos)                        ;
  QAction       * a                                                  ;
  nIfSafe(dock) mm.add(1000001,tr("Move to window area"))            ;
  nIfSafe(mdi ) mm.add(1000002,tr("Move to dock area"  ))            ;
  mm . setFont ( plan )                                              ;
  a = mm.exec(global)                                                ;
  if (IsNull(a)) return true                                         ;
  switch (mm[a])                                                     {
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
