#include <qthuman.h>

#define TABLES(ID) PicturePlan->Tables[Tables::ID]

N::FaceScanner:: FaceScanner       ( QObject * parent , Plan * p )
               : QThread           (           parent            )
               , PictureManager    (                           p )
               , PictureRecognizer (                           p )
               , Chunk             ( 1000                        )
               , ProgressID        ( -1                          )
               , ProgressValue     ( 0                           )
               , ProgressGo        ( false                       )
{
}

N::FaceScanner::~FaceScanner (void)
{
}

bool N::FaceScanner::InsertFace(SqlConnection & Connection,SUID uuid,FaceParaments & face)
{
  if (!face.isPartial()) return false     ;
  QString Q                               ;
  SUID fuid                               ;
  fuid = InsertRect(Connection,face.Face) ;
  InsertGroup                             (
    Connection                            ,
    uuid                                  ,
    fuid                                  ,
    Types::Picture              ,
    Types::Rectangle            ,
    Groups::Subordination     ) ;
  return true                             ;
}

bool N::FaceScanner::FaceParament (
       SqlConnection & SC      ,
       SUID             uuid    ,
       QString          name    ,
       QList<QRect>     Rects   )
{
  nKickOut ( Rects.count() <= 0 , false ) ;
  for (int i=0;i<Rects.count();i++)       {
    QRect  R         = Rects [ i ]        ;
    QPoint P         = R . center ()      ;
    SUID   rectangle = InsertRect  (SC,R) ;
    SUID   point     = InsertPoint (SC,P) ;
    InsertFaceParament                    (
      SC                                  ,
      uuid                                ,
      name                                ,
      rectangle                           ,
      point                             ) ;
  }                                       ;
  return true                             ;
}

bool N::FaceScanner::FaceExists (
       SqlConnection & SC    ,
       SUID             uuid  ,
       QRect            Rect  )
{
  QString Q                           ;
  UUIDs   Ruids                       ;
  SUID    ruid                        ;
  Q = SC.sql.SelectFrom               (
        "rectangle"                   ,
        TABLES(Faces)                 ,
        QString( "where picture = %1" )
        .arg(uuid)                  ) ;
  SqlLoopNow ( SC , Q )               ;
    ruid = SC.Uuid(0)                 ;
    if (!Ruids.contains(ruid))        {
      Ruids << ruid                   ;
    }                                 ;
  SqlLoopErr ( SC , Q )               ;
  SqlLoopEnd ( SC , Q )               ;
  if (Ruids.count()<=0) return false  ;
  QRectF RF                           ;
  QRect  ZF                           ;
  foreach (ruid,Ruids)                {
    if (Rectangle(SC,ruid,RF))        {
      ZF = RF.toRect()                ;
      if (ZF==Rect) return true       ;
    }                                 ;
  }                                   ;
  return false                        ;
}

SUID N::FaceScanner::AnalyzeFace (
       SqlConnection & SC        ,
       UUIDs         & People    ,
       SUID            uuid      ,
       QSize           iSize     ,
       FaceParaments & face      )
{
  SUID   fuid       = 0                             ;
  double membership = 1.0                           ;
  if (face.isIntact()) { ; }                     else
  if (face.isPartial())                             {
    membership = face.Possibility()                 ;
  } else membership = -1                            ;
  nKickOut ( membership < 0           , 0 )         ;
  ///////////////////////////////////////////////////
  QRect  Enhance   = FlatRatio  ( face.Face,iSize ) ;
  nKickOut ( FaceExists(SC,uuid,Enhance) , 0 )      ;
  SUID   rectangle = InsertRect ( SC,Enhance      ) ;
  SUID   puid                                       ;
  fuid = PictureManager::InsertFace                 (
           SC                                       ,
           uuid                                     ,
           rectangle                                ,
           membership                             ) ;
  nKickOut ( fuid <= 0 , 0 )                        ;
  ///////////////////////////////////////////////////
  if (People.count()>0)                             {
    foreach (puid,People)                           {
      PictureManager::InsertGroup                   (
        SC                                          ,
        puid                                        ,
        fuid                                        ,
        Types::People                               ,
        Types::Face                                 ,
        Groups::Subordination,0                   ) ;
    }                                               ;
  }                                                 ;
  ///////////////////////////////////////////////////
  FaceParament ( SC,fuid,"LeftEye" ,face.LeftEye  ) ;
  FaceParament ( SC,fuid,"RightEye",face.RightEye ) ;
  FaceParament ( SC,fuid,"Mouth"   ,face.Mouth    ) ;
  FaceParament ( SC,fuid,"Nose"    ,face.Noses    ) ;
  ///////////////////////////////////////////////////
  return fuid                                       ;
}

void N::FaceScanner::DetectFace(SqlConnection & SC,SUID uuid,IplImage * image)
{
  Phiz faces = Faces ( image )      ;
  nDropOut ( faces.count() <= 0 )   ;
  for (int j=0;j<faces.count();j++) {
    InsertFace(SC,uuid,faces[j])    ;
  }                                 ;
}

void N::FaceScanner::DetectDetail (
       SqlConnection & SC      ,
       SUID             uuid    ,
       QSize            iSize   ,
       IplImage       * image   )
{
  Phiz faces = Faces ( image )       ;
  nDropOut ( faces.count() <= 0 )    ;
  UUIDs People                       ;
  People = FindOwners                (
    SC                               ,
    uuid                             ,
    Types::People          ,
    Types::Picture         ,
    Groups::Subordination) ;
  for (int j=0;j<faces.count();j++)  {
    AnalyzeFace                      (
      SC                             ,
      People                         ,
      uuid                           ,
      iSize                          ,
      faces[j]                     ) ;
  }                                  ;
}

void N::FaceScanner::DetectFaces(void)
{
  SUID    uuid   = 0                                         ;
  QString LockOn = ""                                        ;
  LockOn = "where scope = 'Faces' and keyword = 'UUID'"      ;
  SqlConnection SC ( FilterPlan->sql )                       ;
  if (SC.open("nFaceScanner","DetectFaces"))                 {
    bool    done  = false                                    ;
    int     id    = 0                                        ;
    int     total = 0                                        ;
    QString Limit = QString(" limit 0,%1").arg(Chunk)        ;
    UUIDs   Uuids                                            ;
    QMap<SUID,int> IDs                                       ;
    QString Q                                                ;
    QString Options                                          ;
    //////////////////////////////////////////////////////////
    Q = SC.sql.SelectFrom( "value",TABLES(Settings),LockOn ) ;
    if (SC.Fetch(Q))                                         {
      QString V = SC.String(0)                               ;
      uuid = V.toULongLong()                                 ;
    }                                                        ;
    //////////////////////////////////////////////////////////
    Q = SC.sql.SelectFrom                                    (
          "id",TABLES(PictureUuid)                           ,
          "order by id desc limit 0,1")                      ;
    if (SC.Fetch(Q)) total = SC . Int ( 0 )                  ;
    PicturePlan->setRange(ProgressID,0,total)                ;
    Limit = " order by id asc" + Limit                       ;
    if (uuid>0)                                              {
      Q = SC.sql.SelectFrom                                  (
            "id",TABLES(PictureUuid)                         ,
            QString("where uuid = %1").arg(uuid))            ;
      if (SC.Fetch(Q)) id = SC.Int(0)                        ;
    }                                                        ;
    ProgressValue = id                                       ;
    ProgressGo    = true                                     ;
    PicturePlan -> Start ( ProgressID,&ProgressValue,&ProgressGo  ) ;
    //////////////////////////////////////////////////////////
    while (!done)                                            {
      ////////////////////////////////////////////////////////
      Uuids . clear ( )                                      ;
      IDs   . clear ( )                                      ;
      Options = QString("where id > %1").arg(id)             ;
      Q = SC.sql.SelectFrom                                  (
            "id,uuid",TABLES(PictureUuid),Options + Limit  ) ;
      SqlLoopNow ( SC , Q )                                  ;
        int  ID   = SC . Int  ( 0 )                          ;
        SUID suid = SC . Uuid ( 1 )                          ;
        Uuids        << suid                                 ;
        IDs [ suid ]  = ID                                   ;
      SqlLoopErr ( SC , Q )                                  ;
      SqlLoopEnd ( SC , Q )                                  ;
      ////////////////////////////////////////////////////////
      if (Uuids.count()==0) done = true; else                {
        for (int i=0;ProgressGo && i<Uuids.count();i++)      {
          uuid = Uuids [ i    ]                              ;
          id   = IDs   [ uuid ]                              ;
          ProgressValue = id                                 ;
          QImage * II = Picture(SC,uuid)                     ;
          if (NotNull(II))                                   {
            if (!II->isNull()                               &&
                 II->width ()>=40                           &&
                 II->height()>=40                          ) {
              IplImage * ipl = toIplImage(*II)               ;
              if (NotNull(ipl))                              {
                DetectFace(SC,uuid,ipl)                      ;
                ipl = NULL                                   ;
              }                                              ;
            }                                                ;
            delete II                                        ;
            II = NULL                                        ;
          }                                                  ;
        }                                                    ;
      }                                                      ;
      if (!ProgressGo) done = true                           ;
    }                                                        ;
    //////////////////////////////////////////////////////////
    {
      QString V = QString::number(uuid)                      ;
      Q = SC.sql.Update( TABLES(Settings),LockOn,1,"value" ) ;
      SC.Prepare ( Q                  )                      ;
      SC.Bind    ( "value",V.toUtf8() )                      ;
      SC.Exec    (                    )                      ;
    }                                                        ;
    //////////////////////////////////////////////////////////
    SC . close ( )                                           ;
  }                                                          ;
  SC.remove()                                                ;
  PicturePlan -> Finish ( ProgressID )                       ;
  emit Finished ( )                                          ;
}

void N::FaceScanner::DetectDetails(void)
{
  SUID    uuid         = 0                                             ;
  QString LockOn       = ""                                            ;
  int     MinWidth     = 40                                            ;
  int     MinHeight    = 40                                            ;
  int     MaxWidth     = 3000                                          ;
  int     MaxHeight    = 8000                                          ;
  int     FaceWidth    = 512                                           ;
  int     FaceHeight   = 512                                           ;
  LockOn = "where scope = 'Faces' and keyword = 'Detail'"              ;
  //////////////////////////////////////////////////////////////////////
  SqlConnection SC ( FilterPlan->sql )                                 ;
  if (SC.open("nFaceScanner","DetectFaces"))                           {
    bool    done  = false                                              ;
    int     id    = 0                                                  ;
    int     total = 0                                                  ;
    QString Limit = QString(" limit 0,%1").arg(Chunk)                  ;
    QString V                                                          ;
    UUIDs   Uuids                                                      ;
    QMap<SUID,int> IDs                                                 ;
    QString Q                                                          ;
    QString Options                                                    ;
    ParamentQuery PQ ( Dynamic::Functionality , 1 , "Facial"      )    ;
    ParamentQuery PF ( Dynamic::Functionality , 1 , "Classifiers" )    ;
    ////////////////////////////////////////////////////////////////////
    MinWidth    = PQ.Value(SC,TABLES(Paraments),"MinWidth"           ) ;
    MinHeight   = PQ.Value(SC,TABLES(Paraments),"MinHeight"          ) ;
    MaxWidth    = PQ.Value(SC,TABLES(Paraments),"MaxWidth"           ) ;
    MaxHeight   = PQ.Value(SC,TABLES(Paraments),"MaxHeight"          ) ;
    FaceWidth   = PQ.Value(SC,TABLES(Paraments),"FaceWidth"          ) ;
    FaceHeight  = PQ.Value(SC,TABLES(Paraments),"FaceHeight"         ) ;
    ////////////////////////////////////////////////////////////////////
    int flip    = PF.Value(SC,TABLES(Paraments),"Flip"               ) ;
    int fdw     = PF.Value(SC,TABLES(Paraments),"FaceDetectWidth"    ) ;
    int fdh     = PF.Value(SC,TABLES(Paraments),"FaceDetectHeight"   ) ;
    int ffw     = PF.Value(SC,TABLES(Paraments),"FeatureDetectWidth" ) ;
    int ffh     = PF.Value(SC,TABLES(Paraments),"FeatureDetectHeight") ;
    int fdn     = PF.Value(SC,TABLES(Paraments),"FaceNeighbors"      ) ;
    int ffn     = PF.Value(SC,TABLES(Paraments),"FeatureNeighbors"   ) ;
    PF.Variety  = 2                                                    ;
    double fds  = PF.Floating(SC,TABLES(Paraments),"FaceScale"       ) ;
    double ffs  = PF.Floating(SC,TABLES(Paraments),"FeatureScale"    ) ;
    PF.Variety  = 3                                                    ;
    QString fdf = PF.String(SC,TABLES(Paraments),"FaceFlags"         ) ;
    QString fff = PF.String(SC,TABLES(Paraments),"FeatureFlags"      ) ;
    Flip             = ( flip == 1 )                                   ;
    FaceDetect       = QSize(fdw,fdh)                                  ;
    FeatureDetect    = QSize(ffw,ffh)                                  ;
    MinWindowSize    = QSize(FaceWidth,FaceHeight)                     ;
    FaceNeighbors    = fdn                                             ;
    FeatureNeighbors = ffn                                             ;
    FaceScale        = fds                                             ;
    FeatureScale     = ffs                                             ;
    if (fdf=="CV_HAAR_DO_CANNY_PRUNING")                               {
      FaceFlags    = CV_HAAR_DO_CANNY_PRUNING                          ;
    }                                                                  ;
    if (fff=="CV_HAAR_DO_CANNY_PRUNING")                               {
      FeatureFlags = CV_HAAR_DO_CANNY_PRUNING                          ;
    }                                                                  ;
    ////////////////////////////////////////////////////////////////////
    Q = SC.sql.SelectFrom( "value",TABLES(Settings),LockOn )           ;
    if (SC.Fetch(Q))                                                   {
      V    = SC.String(0)                                              ;
      uuid = V.toULongLong()                                           ;
    }                                                                  ;
    ////////////////////////////////////////////////////////////////////
    Q = SC.sql.SelectFrom                                              (
          "id"                                                         ,
          TABLES(PictureUuid)                                          ,
          QString ( "%1 %2"                                            )
          .arg    ( SC.OrderByDesc("id")                               )
          .arg    ( SC.sql.Limit(0,1)                                  )
        )                                                              ;
    if (SC.Fetch(Q)) total = SC . Int ( 0 )                            ;
    PicturePlan->setRange(ProgressID,0,total)                          ;
    Limit = " order by id asc" + Limit                                 ;
    if (uuid>0)                                                        {
      Q = SC.sql.SelectFrom                                            (
            "id",TABLES(PictureUuid)                                   ,
            QString("where uuid = %1").arg(uuid))                      ;
      if (SC.Fetch(Q)) id = SC.Int(0)                                  ;
    }                                                                  ;
    ProgressValue = id                                                 ;
    ProgressGo    = true                                               ;
    PicturePlan -> Start ( ProgressID,&ProgressValue,&ProgressGo )     ;
    ////////////////////////////////////////////////////////////////////
    while (!done)                                                      {
      //////////////////////////////////////////////////////////////////
      Q = SC.sql.SelectFrom                                            (
            "id",TABLES(PictureUuid)                                   ,
            "order by id desc limit 0,1")                              ;
      if (SC.Fetch(Q)) total = SC . Int ( 0 )                          ;
      PicturePlan->setRange(ProgressID,0,total)                        ;
      //////////////////////////////////////////////////////////////////
      Uuids . clear ( )                                                ;
      IDs   . clear ( )                                                ;
      Options = QString("where id > %1").arg(id)                       ;
      Q = SC.sql.SelectFrom                                            (
            "id,uuid",TABLES(PictureUuid),Options + Limit            ) ;
      SqlLoopNow ( SC , Q )                                            ;
        int  ID   = SC . Int  ( 0 )                                    ;
        SUID suid = SC . Uuid ( 1 )                                    ;
        Uuids        << suid                                           ;
        IDs [ suid ]  = ID                                             ;
      SqlLoopErr ( SC , Q )                                            ;
      SqlLoopEnd ( SC , Q )                                            ;
      //////////////////////////////////////////////////////////////////
      if (Uuids.count()==0) done = true; else                          {
        for (int i=0;ProgressGo && i<Uuids.count();i++)                {
          uuid = Uuids [ i    ]                                        ;
          id   = IDs   [ uuid ]                                        ;
          ProgressValue = id                                           ;
          QImage * II = Picture(SC,uuid)                               ;
          if (NotNull(II))                                             {
            PicturePlan -> ProgressText                                       (
              ProgressID                                               ,
              QString("%1:[%2 x %3]"                                   )
              .arg(uuid                                                )
              .arg(II->width ()                                        )
              .arg(II->height())                                     ) ;
            if (!II->isNull()                                         &&
                 II->width () >= MinWidth                             &&
                 II->height() >= MinHeight                            &&
                 II->width () <= MaxWidth                             &&
                 II->height() <= MaxHeight                           ) {
              IplImage * ipl = toIplImage(*II)                         ;
              if (NotNull(ipl))                                        {
                QSize is = II->size()                                  ;
                DetectDetail(SC,uuid,is,ipl)                           ;
                ipl = NULL                                             ;
              }                                                        ;
            }                                                          ;
            delete II                                                  ;
            II = NULL                                                  ;
          }                                                            ;
        }                                                              ;
      }                                                                ;
      if (!ProgressGo) done = true                                     ;
      //////////////////////////////////////////////////////////////////
      V = QString::number(uuid)                                        ;
      Q = SC.sql.Update( TABLES(Settings),LockOn,1,"value" )           ;
      SC.Prepare ( Q                  )                                ;
      SC.Bind    ( "value",V.toUtf8() )                                ;
      SC.Exec    (                    )                                ;
      //////////////////////////////////////////////////////////////////
    }                                                                  ;
    ////////////////////////////////////////////////////////////////////
    // Table acceleration query
    Q = SC.sql.SelectFrom("count(*)",TABLES(Faces))                 ;
    SC . Query (Q)                                                     ;
    Q = SC.sql.SelectFrom                                              (
                        "count(*)",TABLES(FaceParaments))           ;
    SC . Query (Q)                                                     ;
    ////////////////////////////////////////////////////////////////////
    SC . close ( )                                                     ;
  }                                                                    ;
  SC.remove()                                                          ;
  PicturePlan -> Finish ( ProgressID                    )                     ;
  PicturePlan -> Talk   ( tr("Face detection finished") )                     ;
  emit Finished  (                               )                     ;
}

void N::FaceScanner::run(void)
{
  switch (Mode)                  {
    nFastCast(0,DetectFaces  ()) ;
    nFastCast(1,DetectDetails()) ;
  }                              ;
  deleteLater ( )                ;
}

void N::FaceScanner::Launch(int mode)
{
  Mode = mode                            ;
  switch (Mode)                          {
    case 0                               :
      ProgressID = PicturePlan->Progress (
        tr("Face scanner"  )             ,
        tr("%v/%m pictures")           ) ;
    break                                ;
    case 1                               :
      ProgressID = PicturePlan->Progress (
        tr("Face details scanner")       ,
        tr("%v/%m pictures"      )     ) ;
    break                                ;
  }                                      ;
  ProgressValue = 0                      ;
  ProgressGo    = false                  ;
  start ( )                              ;
}
