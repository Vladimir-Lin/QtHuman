#include <qthuman.h>

N::PeopleReporter:: PeopleReporter ( QWidget * parent , Plan * p )
                  : TextEdit       (           parent ,        p )
                  , Object         ( 0 , Types::People           )
                  , GroupItems     (                           p )
                  , PeopleManager  (                           p )
                  , PictureManager (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PeopleReporter::~PeopleReporter (void)
{
}

QSize N::PeopleReporter::sizeHint(void) const
{
  return QSize ( 640 , 480 ) ;
}

void N::PeopleReporter::Configure (void)
{
  setReadOnly ( true ) ;
}

bool N::PeopleReporter::Variable (
       SqlConnection & SC      ,
       SUID            u       ,
       QString         name    ,
       QString       & result  )
{
  QString Q                                    ;
  Q = SC.sql.SelectFrom                        (
        "value"                                ,
        PlanTable(Variables)                   ,
        SC.sql.Where(3,"uuid","type","name") ) ;
  SC . Prepare ( Q                      )      ;
  SC . Bind    ( "uuid" , u             )      ;
  SC . Bind    ( "type" , Types::People )      ;
  SC . Bind    ( "name" , name.toUtf8() )      ;
  IfSqlExec ( SC )                             {
    result = SC . String ( 0 )                 ;
    return true                                ;
  }                                            ;
  return false                                 ;
}

void N::PeopleReporter::Append(QString html)
{
  append        ( html          ) ;
  HTML . append ( html.toUtf8() ) ;
}

void N::PeopleReporter::startup(SUID people)
{
  setObjectUuid ( people ) ;
//  start         ( 100001 ) ;
  Report        (        ) ;
}

void N::PeopleReporter::run(int type,ThreadData * data)
{ Q_UNUSED   ( data ) ;
  switch     ( type ) {
    case 100001       :
      Report (      ) ;
    break             ;
  }                   ;
}

void N::PeopleReporter::Report(void)
{
  nDropOut         ( ObjectUuid() <=0 )                  ;
  SqlConnection SC ( plan->sql        )                  ;
  if (SC.open("PeopleReporter","Report"))                {
    QStringList names                                    ;
    names = PeopleManager :: Names ( SC , ObjectUuid() ) ;
    if (names.count()>0) setWindowTitle ( names [0] )    ;
    Append ( Introduction    ( SC , names ) )            ;
    DoProcessEvents                                      ;
    Append ( DisplayNames    ( SC         ) )            ;
    DoProcessEvents                                      ;
    Append ( DisplayFeatures ( SC         ) )            ;
    DoProcessEvents                                      ;
    Append ( DisplayBookmark ( SC         ) )            ;
    DoProcessEvents                                      ;
    Append ( DisplayVideos   ( SC         ) )            ;
    DoProcessEvents                                      ;
    Append ( DisplayPictures ( SC         ) )            ;
    DoProcessEvents                                      ;
    Append ( DisplayFaces    ( SC         ) )            ;
    DoProcessEvents                                      ;
    SC . close   (            )                          ;
  }                                                      ;
  SC   . remove  (            )                          ;
}

QString N::PeopleReporter::Introduction(SqlConnection & SC,QStringList & names)
{
  QString M = ""                                                                 ;
  QString T = tr("<tr><td bgcolor=\"#ffccff\" width=\"30%\">%1</td><td bgcolor=\"#ffccff\" >%2</td></tr>") ;
  M += "<table cellpadding=\"1\" cellspacing=\"1\" border=\"1\" width=\"100%\">" ;
  M += "<tbody>"                                                                 ;
  M += "<tr>"                                                                    ;
  M += "<td align=\"center\" height=\"128\" valign=\"middle\" width=\"128\">"    ;
  M += DisplayIcon ( SC )                                                        ;
  M += "</td>"                                                                   ;
  M += "<td>"                                                                    ;
  M += "<table cellpadding=\"2\" cellspacing=\"2\" border=\"0\" width=\"100%\">" ;
  M += "<tbody>"                                                                 ;
  M += DisplayName  ( T , names )                                                ;
  M += setSexuality ( SC , T    )                                                ;
  M += DisplayBirth ( SC , T    )                                                ;
  // append items here
  M += "</tbody>"                                                                ;
  M += "</table>"                                                                ;
  M += "</td>"                                                                   ;
  M += "</tr>"                                                                   ;
  M += "</tbody>"                                                                ;
  M += "</table>"                                                                ;
  return M                                                                       ;
}

QString N::PeopleReporter::DisplayName(QString TD,QStringList & names)
{
  nKickOut ( names.count() <= 0 , "" )               ;
  QString M                                          ;
  M  = QString(TD).arg(tr("Name")).arg(names [0]   ) ;
  M += QString(TD).arg(tr("UUID")).arg(ObjectUuid()) ;
  return M                                           ;
}

QString N::PeopleReporter::DisplayNames(SqlConnection & SC)
{
  UUIDs       Uuids                         ;
  Uuids = SC.Uuids                          (
            PlanTable(NameMaps)             ,
            "name"                          ,
            QString ( "%1 %2"               )
            .arg(SC.WhereUuid(ObjectUuid()) )
            .arg(SC.OrderByAsc("prefer")) ) ;
  if (Uuids.count()<=1) return ""           ;
  Uuids . takeAt ( 0 )                      ;
  ///////////////////////////////////////////
  SUID    u                                 ;
  QString M = ""                            ;
  M += "<table cellpadding=\"1\" cellspacing=\"1\" border=\"1\" width=\"100%\">" ;
  M += "<tbody>"                            ;
  M += QString("<tr><td colspan=\"2\" align=\"center\">%1</td></tr>").arg(tr("Other names")) ;
  foreach (u,Uuids)                         {
    QString Q = SC.sql.SelectFrom           (
                  "language,name"           ,
                  PlanTable(Names)          ,
                  SC.WhereUuid(u) )         ;
    if (SC.Fetch(Q))                        {
      int     language = SC . Int    ( 0 )  ;
      QString name     = SC . String ( 1 )  ;
      M += "<tr><td width=\"20%\">"         ;
      M += plan->languages[language]        ;
      M += "</td><td>"                      ;
      M += name                             ;
      M += "</td></tr>"                     ;
    }                                       ;
  }                                         ;
  M += "</tbody>"                           ;
  M += "</table>"                           ;
  return M                                  ;
}

QString N::PeopleReporter::DisplayIcon(SqlConnection & SC)
{
  SUID     puid    = FindSecond                            (
                       SC                                  ,
                       ObjectUuid()                        ,
                       Types::People                       ,
                       Types::Picture                      ,
                       Groups::Icon                        ,
                       QString("%1 %2"                     )
                       .arg( SC.OrderByAsc("position")     )
                       .arg( SC.sql.Limit(0,1))          ) ;
  //////////////////////////////////////////////////////////
  QImage * image   = Thumb ( SC , puid )                   ;
  if (IsNull(image))                                       {
    image  = new QImage()                                  ;
    *image = QImage(":/images/face.png")                   ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  QUrl Uri(QString("file://%1.png").arg(ObjectUuid()))     ;
  QTextDocument * textDocument = document()                ;
  textDocument -> addResource                              (
                    QTextDocument::ImageResource           ,
                    Uri                                    ,
                    QVariant ( *image )                  ) ;
  return QString("<img src=\"%1\">").arg(Uri.toString())   ;
}

QString N::PeopleReporter::DisplayBirth(SqlConnection & SC,QString TD)
{
  UUIDs TUIDs = Subordination                                        (
                SC                                                   ,
                ObjectUuid()                                         ,
                Types::People                                        ,
                Types::History                                       ,
                Groups::Subordination                                ,
                SC.OrderByAsc("position") )                          ;
  if (TUIDs.count()<=0) return ""                                    ;
  HistoryManager HM ( plan )                                         ;
  SUID xuid = 0                                                      ;
  xuid = HM . GetHistoryType                                         (
           SC                                                        ,
           TUIDs                                                     ,
           History::PeopleBirth                                    ) ;
  if (xuid<=0) return ""                                             ;
  TUID tuid = 0                                                      ;
  tuid = HM.GetTime(SC,xuid)                                         ;
  if (tuid<=0) return ""                                             ;
  QDateTime D                                                        ;
  D = HM.toDateTime(tuid)                                            ;
  QString M                                                          ;
  M += QString(TD).arg(tr("Birthday")).arg(D.toString("yyyy/MM/dd")) ;
  M += setAge(TD,D)                                                  ;
  return M                                                           ;
}

QString N::PeopleReporter::setAge(QString TD,QDateTime dt)
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
  ////////////////////////////////////////////////////////////////
  if (error) return ""                                           ;
  QString M                                                      ;
  QString A = tr("%1 years %2 days").arg(Years).arg(Days)        ;
  M = QString(TD).arg(tr("Age")).arg(A)                          ;
  return M                                                       ;
}

QString N::PeopleReporter::setSexuality(SqlConnection & SC,QString TD)
{
  NAMEs Sexualities                                           ;
  UUIDs Uuids                                                 ;
  SUID  uuid                                                  ;
  /////////////////////////////////////////////////////////////
  Uuids = HumanSexualities(SC)                                ;
  foreach (uuid,Uuids)                                        {
    QString name = Name(SC,uuid,vLanguageId)                  ;
    Sexualities[uuid] = name                                  ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  SUID xuid = 0                                               ;
  xuid = FindSecond                                           (
           SC                                                 ,
           ObjectUuid()                                       ,
           Types::People                                      ,
           Types::Gender                                      ,
           Groups::Sexuality                                  ,
           SC . OrderByAsc ("position")                     ) ;
  if (xuid<=0) return ""                                      ;
  /////////////////////////////////////////////////////////////
  QString M                                                   ;
  M = QString(TD).arg(tr("Sexuality")).arg(Sexualities[xuid]) ;
  return M                                                    ;
}

QString N::PeopleReporter::DisplayFeatures(SqlConnection & SC)
{
  QString M = ""                                    ;
  M += "<table cellpadding=\"1\" cellspacing=\"1\" border=\"1\" width=\"100%\">" ;
  M += "<tbody>"                                    ;
  M += QString("<tr><td colspan=\"2\" align=\"center\">%1</td></tr>").arg(tr("Characteristics")) ;
  ///////////////////////////////////////////////////
  QString V                                         ;
  NAMEs   names                                     ;
  SUID    u                                         ;
  UUIDs   Uuids                                     ;
  Uuids = SC.Uuids                                  (
            PlanTable(Hairs)                        ,
            "uuid"                                  ,
            SC.OrderByAsc("id")                   ) ;
  foreach (u,Uuids)                                 {
    names [ u ] = Name(SC,u,vLanguageId)            ;
  }                                                 ;
  ///////////////////////////////////////////////////
  u =  FindFirst                                    (
         SC                                         ,
         ObjectUuid()                               ,
         Types::Hairs                               ,
         Types::People                              ,
         Groups::Subordination                      ,
         SC.OrderByAsc("position")                ) ;
  if (u>0)                                          {
    M += "<tr><td width=\"20%\">"                   ;
    M += tr("Hairs")                                ;
    M += "</td><td>"                                ;
    M += names[u]                                   ;
    M += "</td></tr>"                               ;
  }                                                 ;
  ///////////////////////////////////////////////////
  Uuids . clear ( )                                 ;
  names . clear ( )                                 ;
  Uuids = SC.Uuids                                  (
            PlanTable(Eyes)                         ,
            "uuid"                                  ,
            SC.OrderByAsc("id")                   ) ;
  foreach (u,Uuids)                                 {
    names [ u ] = Name(SC,u,vLanguageId)            ;
  }                                                 ;
  ///////////////////////////////////////////////////
  u =  FindFirst                                    (
         SC                                         ,
         ObjectUuid()                               ,
         Types::Eyes                                ,
         Types::People                              ,
         Groups::Subordination                      ,
         SC.OrderByAsc("position")                ) ;
  if (u>0)                                          {
    M += "<tr><td width=\"20%\">"                   ;
    M += tr("Eyes")                                 ;
    M += "</td><td>"                                ;
    M += names[u]                                   ;
    M += "</td></tr>"                               ;
  }                                                 ;
  ///////////////////////////////////////////////////
  if (Variable(SC,ObjectUuid(),"Blood" ,V))         {
    M += "<tr><td width=\"20%\">"                   ;
    M += tr("Blood")                                ;
    M += "</td><td>"                                ;
    M += V                                          ;
    M += "</td></tr>"                               ;
  }                                                 ;
  if (Variable(SC,ObjectUuid(),"Weight",V))         {
    M += "<tr><td width=\"20%\">"                   ;
    M += tr("Weight")                               ;
    M += "</td><td>"                                ;
    M += V                                          ;
    M += "</td></tr>"                               ;
  }                                                 ;
  if (Variable(SC,ObjectUuid(),"Height",V))         {
    M += "<tr><td width=\"20%\">"                   ;
    M += tr("Height")                               ;
    M += "</td><td>"                                ;
    M += V                                          ;
    M += "</td></tr>"                               ;
  }                                                 ;
  if (Variable(SC,ObjectUuid(),"Bust"  ,V))         {
    M += "<tr><td width=\"20%\">"                   ;
    M += tr("Bust")                                 ;
    M += "</td><td>"                                ;
    M += V                                          ;
    M += "</td></tr>"                               ;
  }                                                 ;
  if (Variable(SC,ObjectUuid(),"Waist" ,V))         {
    M += "<tr><td width=\"20%\">"                   ;
    M += tr("Waist")                                ;
    M += "</td><td>"                                ;
    M += V                                          ;
    M += "</td></tr>"                               ;
  }                                                 ;
  if (Variable(SC,ObjectUuid(),"Hip"   ,V))         {
    M += "<tr><td width=\"20%\">"                   ;
    M += tr("Hip")                                  ;
    M += "</td><td>"                                ;
    M += V                                          ;
    M += "</td></tr>"                               ;
  }                                                 ;
  ///////////////////////////////////////////////////
  M += "</tbody>"                                   ;
  M += "</table>"                                   ;
  return M                                          ;
}

QString N::PeopleReporter::DisplayBookmark(SqlConnection & SC)
{
  return "" ;
}

QString N::PeopleReporter::DisplayVideos(SqlConnection & SC)
{
  return "" ;
}

QString N::PeopleReporter::DisplayPictures(SqlConnection & SC)
{
  return "" ;
}

QString N::PeopleReporter::DisplayFaces(SqlConnection & SC)
{
  return "" ;
}

bool N::PeopleReporter::Menu(QPoint pos)
{
  return true ;
}
