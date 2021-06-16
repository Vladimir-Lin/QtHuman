#include <qthuman.h>

N::Consanguinities:: Consanguinities ( QWidget * parent , Plan * p )
                   : TreeDock        (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::Consanguinities::~Consanguinities(void)
{
}

void N::Consanguinities::Configure(void)
{
  NewTreeWidgetItem            ( head                     ) ;
  head -> setText              ( 0 , tr("Relation")       ) ;
  head -> setText              ( 1 , tr("Blood"   )       ) ;
  ///////////////////////////////////////////////////////////
  setWindowTitle               ( tr("Consanguinities")    ) ;
  setDragDropMode              ( NoDragDrop               ) ;
  setRootIsDecorated           ( false                    ) ;
  setAlternatingRowColors      ( true                     ) ;
  setSelectionMode             ( SingleSelection          ) ;
  setColumnCount               ( 2                        ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded    ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded    ) ;
  assignHeaderItems            ( head                     ) ;
  plan -> setFont              ( this                     ) ;
  MountClicked                 ( 2                        ) ;
  ///////////////////////////////////////////////////////////
  BloodRelations[Blood::Unknown      ]= tr("Unknown"      ) ;
  BloodRelations[Blood::Affinity     ]= tr("Affinity"     ) ;
  BloodRelations[Blood::Consanguinity]= tr("Consanguinity") ;
  BloodRelations[Blood::Foster       ]= tr("Foster"       ) ;
  BloodRelations[Blood::Society      ]= tr("Society"      ) ;
  BloodRelations[Blood::Artificial   ]= tr("Artificial"   ) ;
  BloodRelations[Blood::Irrelevant   ]= tr("Irrelevant"   ) ;
}

bool N::Consanguinities::FocusIn(void)
{
  AssignAction ( Label   , windowTitle     ( ) ) ;
  LinkAction   ( Refresh , startup         ( ) ) ;
  LinkAction   ( Copy    , CopyToClipboard ( ) ) ;
  LinkAction   ( Export  , Export          ( ) ) ;
  return true                                    ;
}

bool N::Consanguinities::startup(void)
{
  clear              (                )         ;
  DocumentManager DM ( plan           )         ;
  EnterSQL           ( SC , plan->sql )         ;
    QString Q                                   ;
    UUIDs Uuids = SC . Uuids                    (
                     PlanTable(Consanguinities) ,
                     "uuid","order by id asc" ) ;
    SUID  uu                                    ;
    foreach (uu,Uuids)                          {
      NewTreeWidgetItem(item)                   ;
      QString name = DM.Name(SC,uu,vLanguageId) ;
      int     blood = 0                         ;
      Q = SC.sql.SelectFrom                     (
            "blood"                             ,
            PlanTable(Consanguinities)          ,
            SC.WhereUuid(uu)                  ) ;
      if (SC.Fetch(Q)) blood = SC.Int(0)        ;
      item->setData(0,Qt::UserRole,uu         ) ;
      item->setText(0,name                    ) ;
      item->setText(1,BloodRelations[blood]   ) ;
      addTopLevelItem(item)                     ;
    }                                           ;
  LeaveSQL         ( SC , plan->sql           ) ;
  SuitableColumns  (                          ) ;
  reportItems      (                          ) ;
  Alert            ( Done                     ) ;
  return true                                   ;
}

void N::Consanguinities::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Save consanguinities") ,
                       plan->Temporary("")        ,
                       tr("Text file (*.txt)")  ) ;
  if (filename.length()<=0) return                ;
  QFile F(filename)                               ;
  if (!F.open(QIODevice::WriteOnly)) return       ;
  for (int i=0;i<topLevelItemCount();i++)         {
    QTreeWidgetItem * item  = topLevelItem(i)     ;
    QString name   = item->text(0)                ;
    QString symbol = item->text(1)                ;
    QString report                                ;
    report = tr("%1 %2\n"                         )
            .arg(name                             )
            .arg(symbol                         ) ;
    F.write(report.toUtf8())                      ;
  }                                               ;
  F.close()                                       ;
  Alert ( Done )                                  ;
}

bool N::Consanguinities::Menu(QPoint pos)
{
  nScopedMenu   ( mm , this)                                    ;
  QMdiSubWindow * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget   * dock   = Casting ( QDockWidget   , parent() ) ;
  QAction    * aa                                               ;
  nIfSafe(dock) mm.add(1000001,tr("Move to window area"))       ;
  nIfSafe(mdi ) mm.add(1000002,tr("Move to dock area"  ))       ;
  mm.setFont ( SystemPlan    )                                  ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 1000001                                                :
      emit attachMdi ( this , 0 )                               ;
    break                                                       ;
    case 1000002                                                :
      emit attachDock                                           (
        this , windowTitle()                                    ,
        Qt::RightDockWidgetArea                                 ,
        Qt::LeftDockWidgetArea  | Qt::RightDockWidgetArea       |
        Qt::TopDockWidgetArea   | Qt::BottomDockWidgetArea    ) ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}

void N::Consanguinities::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                         ;
  SUID        uuid = nTreeUuid(item,0)     ;
  switch (column)                          {
    case 0                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (nameFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
  }                                        ;
}

void N::Consanguinities::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  SUID    uuid = nTreeUuid(ItemEditing,0)          ;
  QString name = line->text()                      ;
  DocumentManager DM ( plan )                      ;
  EnterSQL         ( SC , plan->sql            )   ;
    DM.assureName  ( SC ,uuid,vLanguageId,name )   ;
    ItemEditing->setText ( 0 , name            )   ;
  LeaveSQL         ( SC , plan->sql            )   ;
  removeOldItem    ( true , 0                  )   ;
  Alert            ( Done                      )   ;
}
