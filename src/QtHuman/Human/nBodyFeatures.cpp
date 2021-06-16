#include <qthuman.h>

N::BodyFeatures:: BodyFeatures  ( QWidget * parent , Plan * p )
                : TreeWidget    (           parent ,        p )
                , Object        ( 0 , Types::None             )
                , PeopleManager (                           p )
                , GroupItems    (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::BodyFeatures::~BodyFeatures(void)
{
}

QSize N::BodyFeatures::sizeHint(void) const
{
  if (plan->Booleans["Phone"]) {
    return QSize(320,240)      ;
  }                            ;
  return QSize(640,240)        ;
}

void N::BodyFeatures::Configure(void)
{
  NewTreeWidgetItem  ( head                    ) ;
  head -> setText    ( 0 , tr("People")        ) ;
  head -> setText    ( 1 , tr("Blood" )        ) ;
  head -> setText    ( 2 , tr("Weight")        ) ;
  head -> setText    ( 3 , tr("Height")        ) ;
  head -> setText    ( 4 , tr("Bust"  )        ) ;
  head -> setText    ( 5 , tr("Waist" )        ) ;
  head -> setText    ( 6 , tr("Hip"   )        ) ;
  head -> setText    ( 7 , ""                  ) ;
  setColumnCount     ( 8                       ) ;
  setRootIsDecorated ( false                   ) ;
  assignHeaderItems  ( head                    ) ;
  setWindowTitle     ( tr("Body measurements") ) ;
  plan -> setFont    ( this                    ) ;
  MountClicked       ( 2                       ) ;
}

bool N::BodyFeatures::FocusIn(void)
{
  AssignAction ( Label   , windowTitle ( ) ) ;
  LinkAction   ( Refresh , startup     ( ) ) ;
  return true                                ;
}

void N::BodyFeatures::run(int type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch   ( type ) {
    case 10001      :
      List ( )      ;
    break           ;
  }                 ;
}

bool N::BodyFeatures::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::BodyFeatures::List(void)
{
  TreeWidgetItems items                           ;
  clear            (           )                  ;
  setEnabled       ( false     )                  ;
  GroupItems    GI ( plan      )                  ;
  SqlConnection SC ( plan->sql )                  ;
  if (SC.open("BodyFeatures","List"))             {
    UUIDs       Uuids                             ;
    SUID        u                                 ;
    if (ObjectUuid()<=0)                          {
      Uuids = Lists ( SC , SC.OrderByDesc("id") ) ;
    } else                                        {
      Uuids = GI.Subordination                    (
                SC                                ,
                ObjectUuid()                      ,
                ObjectType()                      ,
                Types::People                     ,
                Groups::Subordination             ,
                SC.OrderByAsc("position") )       ;
    }                                             ;
    foreach (u,Uuids)                             {
      QString name                                ;
      QString V                                   ;
      name = PreferName ( SC , u )                ;
      NewTreeWidgetItem ( item                  ) ;
      item -> setData   ( 0 , Qt::UserRole , u  ) ;
      item -> setText   ( 0 , name              ) ;
      if (Variable(SC,u,"Blood" ,V))              {
        item -> setText ( 1 , V )                 ;
      }                                           ;
      if (Variable(SC,u,"Weight",V))              {
        item -> setText ( 2 , V )                 ;
      }                                           ;
      if (Variable(SC,u,"Height",V))              {
        item -> setText ( 3 , V )                 ;
      }                                           ;
      if (Variable(SC,u,"Bust"  ,V))              {
        item -> setText ( 4 , V )                 ;
      }                                           ;
      if (Variable(SC,u,"Waist" ,V))              {
        item -> setText ( 5 , V )                 ;
      }                                           ;
      if (Variable(SC,u,"Hip"   ,V))              {
        item -> setText ( 6 , V )                 ;
      }                                           ;
      items << item                               ;
    }                                             ;
    SC.close()                                    ;
  }                                               ;
  SC.remove()                                     ;
  addTopLevelItems ( items )                      ;
  setEnabled       ( true  )                      ;
  emit AutoFit     (       )                      ;
  Alert            ( Done  )                      ;
}

void N::BodyFeatures::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( column < 1 )             ;
  QLineEdit * le                      ;
  le = setLineEdit                    (
         item                         ,
         column                       ,
         SIGNAL(returnPressed   ())   ,
         SLOT  (variableFinished()) ) ;
  le -> setFocus(Qt::TabFocusReason)  ;
}

void N::BodyFeatures::variableFinished (void)
{
  SUID u = nTreeUuid(ItemEditing,ItemColumn)             ;
  QLineEdit * name = Casting(QLineEdit,ItemWidget)       ;
  if (NotNull(name))                                     {
    QString value = name->text()                         ;
    SqlConnection SC(plan->sql)                          ;
    if (SC.open("BodyFeatures","variableFinished"))      {
      QString measure = ""                               ;
      switch (ItemColumn)                                {
        case 1                                           :
          measure = "Blood"                              ;
        break                                            ;
        case 2                                           :
          measure = "Weight"                             ;
        break                                            ;
        case 3                                           :
          measure = "Height"                             ;
        break                                            ;
        case 4                                           :
          measure = "Bust"                               ;
        break                                            ;
        case 5                                           :
          measure = "Waist"                              ;
        break                                            ;
        case 6                                           :
          measure = "Hip"                                ;
        break                                            ;
      }                                                  ;
      if (measure.length()>0)                            {
        assureVariable(SC,u,Types::People,measure,value) ;
      }                                                  ;
      SC.close()                                         ;
    }                                                    ;
    SC.remove()                                          ;
  }                                                      ;
  removeOldItem ( )                                      ;
}

bool N::BodyFeatures::Menu(QPoint pos)
{
  MenuManager       mm (this)              ;
  QMenu         *   me                     ;
  QMenu         *   ma                     ;
  QMenu         *   ms                     ;
  QMenu         *   mc                     ;
  QAction       *   a                      ;
  mm . add ( 101 , tr("Refresh") )         ;
  mm . addSeparator (                   )  ;
  me = mm . addMenu ( tr("Edit"       ) )  ;
  ma = mm . addMenu ( tr("Adjustments") )  ;
  ms = mm . addMenu ( tr("Sorting"    ) )  ;
  mc = mm . addMenu ( tr("Selections" ) )  ;
  AdjustMenu     ( mm , ma )               ;
  SortingMenu    ( mm , ms )               ;
  SelectionsMenu ( mm , mc )               ;
  mm . setFont(plan)                       ;
  if (plan->Booleans["Desktop"])           {
    pos = QCursor::pos()                   ;
  } else                                   {
    pos = mapToGlobal ( pos )              ;
  }                                        ;
  a = mm.exec(pos)                         ;
  if (!plan->Booleans["Desktop"])          {
    allowGesture = true                    ;
  }                                        ;
  if (IsNull(a)) return true               ;
  switch (mm[a])                           {
    nFastCast ( 101 , List   ( ) )         ;
    default                                :
      if (RunAdjustment(mm,a)) return true ;
      if (RunSorting   (mm,a)) return true ;
      if (RunSelections(mm,a)) return true ;
    return true                            ;
  }                                        ;
  return true                              ;
}
