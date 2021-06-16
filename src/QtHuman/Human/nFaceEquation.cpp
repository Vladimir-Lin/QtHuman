#include <qthuman.h>

typedef struct  {
  int points    ;
  int sequences ;
  int controls  ;
  int mark      ;
} feHeader      ;

typedef struct  {
  int    id     ;
  double x      ;
  double y      ;
} fePoint       ;

typedef struct {
  int  id      ;
  SUID uuid    ;
  int  first   ;
  int  second  ;
} feControl    ;

N::FaceEquation:: FaceEquation (void              )
                : Object       (0,Types::FaceShape)
{
}

N::FaceEquation:: FaceEquation (SUID u            )
                : Object       (u,Types::FaceShape)
{
}

N::FaceEquation::~FaceEquation(void)
{
}

void N::FaceEquation::StandardFace(void)
{
  Sequences     . clear ( )                      ;
  Points        . clear ( )                      ;
  ControlFirst  . clear ( )                      ;
  ControlSecond . clear ( )                      ;
  ControlUuids  . clear ( )                      ;
  ControlNames  . clear ( )                      ;
  Points [ 0 ]  = QPointF ( 0.0 , 2.0 )          ;
  double L = 12.5                                ;
  double W =  9.0                                ;
  double x                                       ;
  double y                                       ;
  for (int i=0;i<32;i++)                         {
    double angle                                 ;
    angle = ( 450 - ( ( i * 360 ) / 32 ) ) % 360 ;
    double Cos = Math::fastCosine(angle)         ;
    double Sin = Math::fastSine  (angle)         ;
    x = Cos * W                                  ;
    y = Sin * L                                  ;
    Points [ i + 1 ] = QPointF ( x , y )         ;
    Sequences << i + 1                           ;
  }                                              ;
}

void N::FaceEquation::Reposition(void)
{
  IMAPs             NewIDs              ;
  IMAPs             RevIDs              ;
  CUIDs             CleanIDs            ;
  CUIDs             sequences           ;
  QMap<int,QPointF> points              ;
  IMAPs             first               ;
  IMAPs             second              ;
  CMAPs             controlUuids        ;
  EMAPs             controlNames        ;
  int               index = 0           ;
  ///////////////////////////////////////
  CleanIDs = Points.keys()              ;
  for (int i=0;i<Sequences.count();i++) {
    int sid = Sequences[i]              ;
    if (CleanIDs.contains(sid))         {
      int idx = CleanIDs.indexOf(sid)   ;
      if (idx>=0) CleanIDs.takeAt(idx)  ;
    }                                   ;
  }                                     ;
  ///////////////////////////////////////
  for (int i=0;i<CleanIDs.count();i++)  {
    int v = CleanIDs[i]                 ;
    NewIDs[index] = v                   ;
    RevIDs[v    ] = index               ;
    index++                             ;
  }                                     ;
  ///////////////////////////////////////
  for (int i=0;i<Sequences.count();i++) {
    int v = Sequences[i]                ;
    NewIDs[index] = v                   ;
    RevIDs[v    ] = index               ;
    sequences << index                  ;
    index++                             ;
  }                                     ;
  ///////////////////////////////////////
  CleanIDs = Points.keys()              ;
  for (int i=0;i<CleanIDs.count();i++)  {
    int v = CleanIDs[i]                 ;
    int n = RevIDs  [v]                 ;
    points[n] = Points[v]               ;
  }                                     ;
  ///////////////////////////////////////
  CleanIDs = ControlFirst.keys()        ;
  for (int i=0;i<CleanIDs.count();i++)  {
    int     v = CleanIDs        [ i ]   ;
    int     f = ControlFirst    [ v ]   ;
    int     s = ControlSecond   [ v ]   ;
    SUID    u = ControlUuids    [ v ]   ;
    QString n = ControlNames    [ v ]   ;
    first        [ v ] = RevIDs [ f ]   ;
    second       [ v ] = RevIDs [ s ]   ;
    controlUuids [ v ] = u              ;
    controlNames [ v ] = n              ;
  }                                     ;
  ///////////////////////////////////////
  Sequences     = sequences             ;
  Points        = points                ;
  ControlFirst  = first                 ;
  ControlSecond = second                ;
  ControlUuids  = controlUuids          ;
  ControlNames  = controlNames          ;
}

bool N::FaceEquation::Generate(QByteArray & body)
{
  int headerSize   = sizeof(feHeader )                          ;
  int sequenceSize = sizeof(int      ) * Sequences    . count() ;
  int pointsSize   = sizeof(fePoint  ) * Points       . count() ;
  int controlSize  = sizeof(feControl) * ControlFirst . count() ;
  int   p          = 0                                          ;
  int   total                                                   ;
  CUIDs K                                                       ;
  ///////////////////////////////////////////////////////////////
  total  = headerSize                                           ;
  total += sequenceSize                                         ;
  total += pointsSize                                           ;
  total += controlSize                                          ;
  body . resize ( total )                                       ;
  ///////////////////////////////////////////////////////////////
  feHeader * feh = (feHeader *)body.data()                      ;
  feh->points    = Points       . count()                       ;
  feh->sequences = Sequences    . count()                       ;
  feh->controls  = ControlFirst . count()                       ;
  feh->mark      = 0                                            ;
  p              = headerSize                                   ;
  ///////////////////////////////////////////////////////////////
  int * ses = (int *)(body.data()+p)                            ;
  for (int i=0;i<Sequences.count();i++) ses[i] = Sequences[i]   ;
  p             += sequenceSize                                 ;
  ///////////////////////////////////////////////////////////////
  fePoint * fep = (fePoint *)(body.data()+p)                    ;
  K = Points . keys ( )                                         ;
  for (int i=0;i<K.count();i++)                                 {
    int id = K[i]                                               ;
    fep[i].id = id                                              ;
    fep[i].x  = Points [ id ] . x ( )                           ;
    fep[i].y  = Points [ id ] . y ( )                           ;
  }                                                             ;
  p             += pointsSize                                   ;
  ///////////////////////////////////////////////////////////////
  feControl * fec = (feControl *)(body.data()+p)                ;
  K = ControlFirst . keys ( )                                   ;
  for (int i=0;i<K.count();i++)                                 {
    int  id       = K[i]                                        ;
    SUID u        = 0                                           ;
    if (ControlUuids.contains(id)) u = ControlUuids[id]         ;
    fec[i].id     = id                                          ;
    fec[i].uuid   = u                                           ;
    fec[i].first  = ControlFirst  [ id ]                        ;
    fec[i].second = ControlSecond [ id ]                        ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  return true                                                   ;
}

bool N::FaceEquation::Obtain(QByteArray & body)
{
  Sequences     . clear ( )                             ;
  Points        . clear ( )                             ;
  ControlFirst  . clear ( )                             ;
  ControlSecond . clear ( )                             ;
  ControlUuids  . clear ( )                             ;
  ControlNames  . clear ( )                             ;
  ///////////////////////////////////////////////////////
  feHeader * feh = (feHeader *)body.data()              ;
  int headerSize   = sizeof(feHeader )                  ;
  int sequenceSize = sizeof(int      ) * feh->sequences ;
  int pointsSize   = sizeof(fePoint  ) * feh->points    ;
  int controlSize  = sizeof(feControl) * feh->controls  ;
  int   p          = headerSize                         ;
  ///////////////////////////////////////////////////////
  int * ses = (int *)(body.data()+p)                    ;
  for (int i=0;i<feh->sequences;i++)                    {
    Sequences << ses[i]                                 ;
  }                                                     ;
  p += sequenceSize                                     ;
  ///////////////////////////////////////////////////////
  fePoint * fep = (fePoint *)(body.data()+p)            ;
  for (int i=0;i<feh->points;i++)                       {
    int    id  = fep[i].id                              ;
    double x   = fep[i].x                               ;
    double y   = fep[i].y                               ;
    Points[id] = QPointF(x,y)                           ;
  }                                                     ;
  p += pointsSize                                       ;
  ///////////////////////////////////////////////////////
  if (feh->controls<=0) return true                     ;
  feControl * fec = (feControl *)(body.data()+p)        ;
  for (int i=0;i<feh->controls;i++)                     {
    int  id              = fec[i].id                    ;
    SUID u               = fec[i].uuid                  ;
    int  f               = fec[i].first                 ;
    int  s               = fec[i].second                ;
    ControlFirst  [ id ] = f                            ;
    ControlSecond [ id ] = s                            ;
    if (u>0) ControlUuids  [ id ] = u                   ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  return true                                           ;
}

bool N::FaceEquation::load(PurePlan * plan,SqlConnection & SC)
{
  nKickOut ( ObjectUuid()<=0 , false )   ;
  QString Q                              ;
  Name = SC . getName                    (
           PlanTable(Names)              ,
           "uuid"                        ,
           plan->LanguageId              ,
           ObjectUuid()                ) ;
  Q = SC.sql.SelectFrom                  (
        "paraments"                      ,
        PlanTable(FaceShapes)            ,
        SC.WhereUuid(ObjectUuid())     ) ;
  if (SC.Fetch(Q))                       {
    QByteArray Body                      ;
    Body = SC.ByteArray(0)               ;
    if (Body.size()<=0)                  {
      StandardFace ( )                   ;
    } else                               {
      Obtain (Body)                      ;
      if (ControlUuids.count()>0)        {
        CUIDs IDs = ControlUuids.keys()  ;
        int   id                         ;
        foreach (id,IDs)                 {
          QString n                      ;
          SUID    u = ControlUuids[id]   ;
          if (u>0)                       {
            n = SC.getName               (
                  PlanTable(Names)       ,
                  "uuid"                 ,
                  plan->LanguageId       ,
                  u                    ) ;
            ControlNames [ u ] = n       ;
          }                              ;
        }                                ;
      }                                  ;
    }                                    ;
  } else                                 {
    StandardFace ( )                     ;
  }                                      ;
  return true                            ;
}

bool N::FaceEquation::save(PurePlan * plan,SqlConnection & SC)
{
  nKickOut ( ObjectUuid()<=0 , false )        ;
  QString Q                                   ;
  QByteArray Body                             ;
  Generate (Body)                             ;
  Q = SC.sql.Update                           (
        PlanTable(FaceShapes)                 ,
        SC.sql.Where(1,"uuid")                ,
        1                                     ,
        "paraments"                         ) ;
  SC . Prepare ( Q                          ) ;
  SC . Bind    ( "uuid"      , ObjectUuid() ) ;
  SC . Bind    ( "paraments" , Body         ) ;
  return SC . Exec ( )                        ;
}

bool N::FaceEquation::load(PurePlan * plan)
{
  SqlConnection SC(plan->sql)         ;
  if (SC.open("FaceEquation","load")) {
    load(plan,SC)                     ;
    SC.close()                        ;
  }                                   ;
  SC.remove()                         ;
  return true                         ;
}

bool N::FaceEquation::save(PurePlan * plan)
{
  SqlConnection SC(plan->sql)         ;
  if (SC.open("FaceEquation","load")) {
    save(plan,SC)                     ;
    SC.close()                        ;
  }                                   ;
  SC.remove()                         ;
  return true                         ;
}

QPointF N::FaceEquation::Quadratic(double t,QPointF & P1,QPointF & P2,QPointF & P3)
{
  double  mt = 1 - t       ;
  double  tt = t           ;
  double  A  = mt * mt     ;
  double  B  = mt * tt * 2 ;
  double  C  = tt * tt     ;
  QPointF P                ;
  P   = P1 * A             ;
  P  += P2 * B             ;
  P  += P3 * C             ;
  return  P                ;
}

bool N::FaceEquation::Shape(QPolygonF & face)
{
  QMap<int,QPointF> pts             ;
  int               ps              ;
  QPolygonF         C               ;
  QPolygonF         R               ;
  QPointF           P0              ;
  QPointF           P1              ;
  QPointF           P2              ;
  QPointF           P3              ;
  int               total           ;
  face  . clear             ( )     ;
  total = Sequences . count ( )     ;
  for (int i=0;i<total;i++)         {
    ps        = Sequences [ i  ]    ;
    pts [ i ] = Points    [ ps ]    ;
  }                                 ;
  for (int i=0;i<total;i++)         {
    int a = i     ; a %= total      ;
    int b = i + 1 ; b %= total      ;
    int c = i + 2 ; c %= total      ;
    P1    = pts[a]                  ;
    P2    = pts[b]                  ;
    P3    = pts[c]                  ;
    P0    = P1 + P2                 ;
    P0   /= 2                       ;
    C    << P0                      ;
    P0    = Quadratic(0.5,P1,P2,P3) ;
    R    << P0                      ;
  }                                 ;
  face << R[0]                      ;
  for (int i=1;i<total;i++)         {
    face << C [ i ]                 ;
    face << R [ i ]                 ;
  }                                 ;
  face << C [ 0 ]                   ;
  face << R [ 0 ]                   ;
  return ( face . count ( ) > 0 )   ;
}

QPointF N::FaceEquation::EyesCenter(void)
{
  int     L  = ControlFirst  [ 1 ] ;
  int     R  = ControlSecond [ 1 ] ;
  QPointF P1 = Points        [ L ] ;
  QPointF P2 = Points        [ R ] ;
  QPointF P  = P1 + P2             ;
  P         /= 2                   ;
  return  P                        ;
}

double N::FaceEquation::EyeLength(void)
{
  int     L  = ControlFirst  [ 1 ] ;
  int     R  = ControlSecond [ 1 ] ;
  QPointF P1 = Points        [ L ] ;
  QPointF P2 = Points        [ R ] ;
  QLineF  LL ( P1 , P2 )           ;
  return LL . length ( )           ;
}

double N::FaceEquation::CenterLength(void)
{
  int     M  = ControlSecond [ 0 ] ;
  QPointF P1 = Points        [ M ] ;
  QPointF P2 = EyesCenter    (   ) ;
  QLineF  LL ( P1 , P2 )           ;
  return LL . length ( )           ;
}

double N::FaceEquation::EyeFactor(QPointF Left,QPointF Right)
{
  double EL = EyeLength (  ) ;
  QLineF LL ( Left , Right ) ;
  double LN = LL . length () ;
  return LN / EL             ;
}

double N::FaceEquation::NoseFactor(QPointF Left,QPointF Right,QPointF Mouth)
{
  QPointF CE = ( Left + Right ) / 2 ;
  QLineF  LL ( CE , Mouth )         ;
  double  CL = CenterLength ( )     ;
  double  IL = LL . length  ( )     ;
  return  IL / CL                   ;
}

void N::FaceEquation::CleanIsolations (void)
{
  CUIDs FP = ControlFirst  . keys ( ) ;
  CUIDs EP = ControlSecond . keys ( ) ;
  CUIDs PA = Points        . keys ( ) ;
  CUIDs PP                            ;
  int   id                            ;
  PP << Sequences                     ;
  foreach (id,FP)                     {
    int p = ControlFirst  [ id ]      ;
    if (!PP.contains(p)) PP << p      ;
  }                                   ;
  foreach (id,EP)                     {
    int p = ControlSecond [ id ]      ;
    if (!PP.contains(p)) PP << p      ;
  }                                   ;
  foreach (id,PA)                     {
    if (!PP.contains(id))             {
      Points.take(id)                 ;
    }                                 ;
  }                                   ;
}

void N::FaceEquation::DoubleUp (void)
{
  if (Sequences.count()<=2) return ;
  CUIDs Z                          ;
  CUIDs P = Points    . keys  ( )  ;
  int   T = Sequences . count ( )  ;
  int   I = P         . last  ( )  ;
  LoopInt LF ( 0 , T - 1 )         ;
  LoopInt LE ( 0 , T - 1 )         ;
  LF.V = 0                         ;
  LE.V = 1                         ;
  for (int i=0;i<T;i++)            {
    int     L  = Sequences [LF.V]  ;
    int     R  = Sequences [LE.V]  ;
    QPointF P0 = Points    [L   ]  ;
    QPointF P1 = Points    [R   ]  ;
    QPointF PA = P0 + P1           ;
    I++                            ;
    PA /= 2                        ;
    Points [ I ] = PA              ;
    ++LF                           ;
    ++LE                           ;
    Z << Sequences [ i ]           ;
    Z << I                         ;
  }                                ;
  Sequences = Z                    ;
}

void N::FaceEquation::Dismantle(int point)
{
  CUIDs R                       ;
  CUIDs I = ControlFirst.keys() ;
  int   i                       ;
  foreach (i,I)                 {
    int t1                      ;
    int t2                      ;
    t1 = ControlFirst  [ i ]    ;
    t2 = ControlSecond [ i ]    ;
    if ( point == t1 )          {
      if (!R.contains(i))       {
        R << i                  ;
      }                         ;
    }                           ;
    if ( point == t2 )          {
      if (!R.contains(i))       {
        R << i                  ;
      }                         ;
    }                           ;
  }                             ;
  if (R.count()<=0) return      ;
  foreach (i,R)                 {
    ControlFirst  . take ( i )  ;
    ControlSecond . take ( i )  ;
  }                             ;
}

void N::FaceEquation::Delete(int point)
{
  if (!Points.contains(point)) return ;
  Dismantle ( point )                 ;
  int idx = Sequences.indexOf(point)  ;
  if (idx>=0) Sequences.takeAt(idx)   ;
  Points.remove(point)                ;
}
