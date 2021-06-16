#include <qthuman.h>

N::PeopleListing:: PeopleListing ( QWidget * parent , Plan * p )
                 : RowTable      (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleListing::~PeopleListing (void)
{
}

QSize N::PeopleListing::sizeHint (void) const
{
  return QSize ( 800 , 600 ) ;
}

void N::PeopleListing::Configure (void)
{
  t2 = N::Types::People                                          ;
  setTable       ( PlanTable(PeopleUuid) )                       ;
  setColumnCount ( 16                    )                       ;
  setFunction    ( 2001 , true           )                       ;
  setFunction    ( 2002 , true           )                       ;
  ////////////////////////////////////////////////////////////////
  FullyFunctions   [  1 ] = tr("Sexuality"        )              ;
  FullyFunctions   [  2 ] = tr("Age"              )              ;
  FullyFunctions   [  3 ] = tr("Birthday"         )              ;
  FullyFunctions   [  4 ] = tr("Height (cm)"      )              ;
  FullyFunctions   [  5 ] = tr("Weight (kg)"      )              ;
  FullyFunctions   [  6 ] = tr("Bust (cm)"        )              ;
  FullyFunctions   [  7 ] = tr("Waist (cm)"       )              ;
  FullyFunctions   [  8 ] = tr("Hip (cm)"         )              ;
  FullyFunctions   [  9 ] = tr("Blood type"       )              ;
  FullyFunctions   [ 10 ] = tr("Videos"           )              ;
  FullyFunctions   [ 11 ] = tr("Pictures"         )              ;
  FullyFunctions   [ 12 ] = tr("Faces"            )              ;
  FullyFunctions   [ 13 ] = tr("Possible pictures")              ;
  FullyFunctions   [ 14 ] = tr("Documents"        )              ;
  FullyFunctions   [ 15 ] = tr("Organizations"    )              ;
  FullyFunctions   [ 16 ] = tr("Bookmarks"        )              ;
  FullyFunctions   [ 17 ] = tr("Nation"           )              ;
  FullyFunctions   [ 18 ] = tr("Race"             )              ;
  ////////////////////////////////////////////////////////////////
  ColumnNames      [  0 ] = tr("People"           )              ;
  for (int i=1;i<=15;i++)                                        {
    ColumnNames [ i ] = FullyFunctions [ i ]                     ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  ColumnAlignments [  0 ] = Qt::AlignLeft  | Qt::AlignVCenter    ;
  ColumnAlignments [  1 ] = Qt::AlignLeft  | Qt::AlignVCenter    ;
  ColumnAlignments [  2 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [  3 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [  4 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [  5 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [  6 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [  7 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [  8 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [  9 ] = Qt::AlignCenter                      ;
  ColumnAlignments [ 10 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [ 11 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [ 12 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [ 13 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [ 14 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ColumnAlignments [ 15 ] = Qt::AlignRight | Qt::AlignVCenter    ;
  ////////////////////////////////////////////////////////////////
  for (int i=1;i<=15;i++) InstallRetrievers ( i , i )            ;
  ////////////////////////////////////////////////////////////////
  setHeader ( )                                                  ;
}

int N::PeopleListing::InstallRetrievers(int column,int RetrieverId)
{
  RetrieveSexuality * RS                                         ;
  RetrievePeriod    * RP                                         ;
  RetrieveCalendar  * RC                                         ;
  RetrieveVariable  * RV                                         ;
  RetrieveMembers   * RM                                         ;
  switch ( RetrieverId )                                         {
    case  1                                                      :
      RS  = new RetrieveSexuality ( this,plan )                  ;
      Retrievers [ column ] = RS                                 ;
      RS -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case  2                                                      :
      RP  = new RetrievePeriod    ( this,plan )                  ;
      Retrievers [ column ] = RP                                 ;
      RP -> column    = column                                   ;
      ////////////////////////////////////////////////////////////
      RP -> StartType = History::PeopleBirth                     ;
      RP -> EndType   = History::PeopleDie                       ;
      RP -> Keywords [ 11 ] = tr ( "%1 years %2 days" )          ;
      RP -> Keywords [ 12 ] = tr ( "%1 days"          )          ;
      RP -> Keywords [ 13 ] = tr ( "After %1 days"    )          ;
      RP -> setFirst  ( 1                                        ,
                        Types  :: People                         ,
                        Types  :: History                        ,
                        Groups :: Subordination                ) ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case  3                                                      :
      RC  = new RetrieveCalendar  ( this,plan )                  ;
      Retrievers [ column ] = RC                                 ;
      RC -> column    = column                                   ;
      ////////////////////////////////////////////////////////////
      RC -> StartType = History::PeopleBirth                     ;
      RC -> Keywords [ 11 ] = tr ( "C.E."     )                  ;
      RC -> Keywords [ 12 ] = tr ( "B.C."     )                  ;
      RC -> Keywords [ 13 ] = tr ( "%1/%2/%3" )                  ;
      RC -> Keywords [ 14 ] = tr ( "%1:%2:%3" )                  ;
      RC -> setFirst  ( 1                                        ,
                        Types  :: People                         ,
                        Types  :: History                        ,
                        Groups :: Subordination                ) ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case  4                                                      :
      RV  = new RetrieveVariable  ( this,plan )                  ;
      Retrievers [ column ] = RV                                 ;
      RV -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RV -> Keywords [  0 ] = "Height"                           ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case  5                                                      :
      RV  = new RetrieveVariable  ( this,plan )                  ;
      Retrievers [ column ] = RV                                 ;
      RV -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RV -> Keywords [  0 ] = "Weight"                           ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case  6                                                      :
      RV  = new RetrieveVariable  ( this,plan )                  ;
      Retrievers [ column ] = RV                                 ;
      RV -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RV -> Keywords [  0 ] = "Bust"                             ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case  7                                                      :
      RV  = new RetrieveVariable  ( this,plan )                  ;
      Retrievers [ column ] = RV                                 ;
      RV -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RV -> Keywords [  0 ] = "Waist"                            ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case  8                                                      :
      RV  = new RetrieveVariable  ( this,plan )                  ;
      Retrievers [ column ] = RV                                 ;
      RV -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RV -> Keywords [  0 ] = "Hip"                              ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case  9                                                      :
      RV  = new RetrieveVariable  ( this,plan )                  ;
      Retrievers [ column ] = RV                                 ;
      RV -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RV -> Keywords [  0 ] = "Blood"                            ;
      RV -> Keywords [  1 ] = tr("%1")                           ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case 10                                                      :
      RM  = new RetrieveMembers   ( this,plan )                  ;
      Retrievers [ column ] = RM                                 ;
      RM -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RM -> setSecond ( 1                                        ,
                        Types  :: Album                          ,
                        Types  :: People                         ,
                        Groups :: Subordination                ) ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case 11                                                      :
      RM  = new RetrieveMembers   ( this,plan )                  ;
      Retrievers [ column ] = RM                                 ;
      RM -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RM -> setFirst  ( 1                                        ,
                        Types  :: People                         ,
                        Types  :: Picture                        ,
                        Groups :: Subordination                ) ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case 12                                                      :
      RM  = new RetrieveMembers   ( this,plan )                  ;
      Retrievers [ column ] = RM                                 ;
      RM -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RM -> setFirst  ( 1                                        ,
                        Types  :: People                         ,
                        Types  :: Face                           ,
                        Groups :: Subordination                ) ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case 13                                                      :
      RM  = new RetrieveMembers   ( this,plan )                  ;
      Retrievers [ column ] = RM                                 ;
      RM -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RM -> setFirst  ( 1                                        ,
                        Types  :: People                         ,
                        Types  :: Picture                        ,
                        Groups :: Possible                     ) ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case 14                                                      :
      RM  = new RetrieveMembers   ( this,plan )                  ;
      Retrievers [ column ] = RM                                 ;
      RM -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RM -> setFirst  ( 1                                        ,
                        Types  :: People                         ,
                        Types  :: Document                       ,
                        Groups :: Subordination                ) ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
    case 15                                                      :
      RM  = new RetrieveMembers   ( this,plan )                  ;
      Retrievers [ column ] = RM                                 ;
      RM -> column = column                                      ;
      ////////////////////////////////////////////////////////////
      RM -> setSecond ( 1                                        ,
                        Types  :: Organization                   ,
                        Types  :: People                         ,
                        Groups :: Subordination                ) ;
      ////////////////////////////////////////////////////////////
      ColumnRetrievers [ column      ] = RetrieverId             ;
      RetrieverColumns [ RetrieverId ] = column                  ;
      ActiveRetrievers                << RetrieverId             ;
    break                                                        ;
  }                                                              ;
  return Retrievers . count ( )                                  ;
}

bool N::PeopleListing::LoadCell(SqlConnection & SC,int row,int column,SUID uuid)
{
  switch ( column )                                         {
    case 0                                                  :
    return LoadNames ( SC , row , column , uuid )           ;
    default                                                 :
      if (Retrievers.contains(column))                      {
        NewTableWidgetItem ( IT )                           ;
        Retrievers [ column ] -> Obtain ( SC , uuid , IT  ) ;
        IT -> setTextAlignment ( ColumnAlignments[column] ) ;
        emit putItem ( row , column , IT )                  ;
      }                                                     ;
    return true                                             ;
  }                                                         ;
  return true                                               ;
}

void N::PeopleListing::setHeader (void)
{
  for (int i=0;i<columnCount();i++)             {
    QTableWidgetItem * HIT                      ;
    HIT = horizontalHeaderItem ( i )            ;
    if (IsNull(HIT))                            {
      HIT = new QTableWidgetItem ( )            ;
    }                                           ;
    HIT -> setText          ( ColumnNames[i]  ) ;
    HIT -> setTextAlignment ( Qt::AlignCenter ) ;
    setHorizontalHeaderItem ( i , HIT         ) ;
  }                                             ;
}

bool N::PeopleListing::startup(void)
{
  clear     (       ) ;
  setHeader (       ) ;
  start     ( 10001 ) ;
  return true         ;
}

int N::PeopleListing::Counting(SqlConnection & SC)
{
  int total = 0                                     ;
  QString Q                                         ;
  GroupItems GI ( plan )                            ;
  GI . GroupTable  = AlterTables [ Tables::Groups ] ;
  if (isFirst())                                    {
    Q = SC . sql . SelectFrom                       (
          "count(*)"                                ,
          GI . GroupTable                           ,
          GI . FirstItem (first ,t1,t2,relation)  ) ;
  } else
  if (isSecond())                                   {
    Q = SC . sql . SelectFrom                       (
          "count(*)"                                ,
          GI . GroupTable                           ,
          GI . SecondItem(second,t1,t2,relation)  ) ;
  } else                                            {
    Q = SC . sql . SelectFrom                       (
          "count(*)"                                ,
          MasterTable                             ) ;
  }                                                 ;
  if (SC.Fetch(Q)) total = SC.Int(0)                ;
  return total                                      ;
}

void N::PeopleListing::List(void)
{
  emit OnBusy  ( )                           ;
  ////////////////////////////////////////////
  CUIDs IDs = Retrievers.keys()              ;
  int   id                                   ;
  foreach ( id , IDs )                       {
    Retrievers[id] -> column   = id          ;
    Retrievers[id] -> language = vLanguageId ;
  }                                          ;
  ////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                ;
    UUIDs U                                  ;
    LimitValues [ 2 ] = Counting ( SC )      ;
    if (LoadUuids(SC,U))                     {
      setRowCount ( U.count() )              ;
      LoadItems   ( SC , U    )              ;
    }                                        ;
  LeaveSQL ( SC , plan->sql )                ;
  ////////////////////////////////////////////
  emit GoRelax (      )                      ;
  emit AutoFit (      )                      ;
  Alert        ( Done )                      ;
}

void N::PeopleListing::ActiveRetriever(int Id,bool active)
{
}

bool N::PeopleListing::Menu(QPoint pos)
{
  MenuManager mm ( this )                                       ;
  QMenu     * me                                                ;
  QMenu     * mi                                                ;
  QMenu     * ms                                                ;
  QMenu     * ma                                                ;
  QMenu     * mc                                                ;
  QAction   * aa                                                ;
  int         StartId  = 0                                      ;
  int         PageSize = 0                                      ;
  ///////////////////////////////////////////////////////////////
  PageMenu                                                      (
    mm                                                          ,
    tr("Total %1 people")                                       ,
    StartId                                                     ,
    PageSize                                                  ) ;
  ///////////////////////////////////////////////////////////////
  me = mm . addMenu ( tr("Edit"       ) )                       ;
  mi = mm . addMenu ( tr("Items"      ) )                       ;
  ms = mm . addMenu ( tr("Sorting"    ) )                       ;
  ma = mm . addMenu ( tr("Adjustments") )                       ;
  mc = mm . addMenu ( tr("Selections" ) )                       ;
  ///////////////////////////////////////////////////////////////
  SortingMenu    ( mm , ms )                                    ;
  AdjustMenu     ( mm , ma )                                    ;
  SelectionsMenu ( mm , mc )                                    ;
  ///////////////////////////////////////////////////////////////
  UUIDs FID = FullyFunctions . keys ( )                         ;
  SUID  fid                                                     ;
  foreach (fid,FID)                                             {
    mm . add                                                    (
      mi                                                        ,
      (int)( fid + 1000 )                                       ,
      FullyFunctions [ fid ]                                    ,
      true                                                      ,
      ActiveRetrievers.contains(fid)                          ) ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  mm . add ( me , 2002 , tr("Append data immediately")          ,
             true , isFunction(2002)                          ) ;
  ///////////////////////////////////////////////////////////////
  mm . setFont ( plan )                                         ;
  aa = mm.exec()                                                ;
  if (IsNull(aa))                                               {
    PageChanged(StartId,PageSize)                               ;
    return false                                                ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  if ( RunSorting    ( mm , aa ) ) return true                  ;
  if ( RunAdjustment ( mm , aa ) ) return true                  ;
  if ( RunSelections ( mm , aa ) ) return true                  ;
  ///////////////////////////////////////////////////////////////
  if ( ( mm[aa] > 1000 ) && ( mm[aa] < 1100 ) )                 {
    ActiveRetriever ( mm[aa] - 1000 , aa->isChecked() )         ;
    return true                                                 ;
  } else                                                        {
    switch (mm[aa])                                             {
      case 2002                                                 :
        setFunction ( 2002 , aa->isChecked() )                  ;
      return true                                               ;
    }                                                           ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  RunPageMenu ( mm , aa , StartId , PageSize )                  ;
  PageChanged ( StartId , PageSize           )                  ;
  ///////////////////////////////////////////////////////////////
  return true                                                   ;
}
