#include <qthuman.h>

#define EDITPOINTS 33011

N::FaceShape:: FaceShape   (QWidget * parent,Plan * p)
             : Widget      (          parent,       p)
             , Object      (0,Types::FaceShape       )
             , image       (NULL                     )
             , ImageX      (0.0                      )
             , ImageY      (0.0                      )
             , ImageFactor (1.0                      )
             , Factor      (1.0                      )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::FaceShape::~FaceShape(void)
{
  if (NotNull(image)) {
    delete image      ;
    image = NULL      ;
  }                   ;
}

QSize N::FaceShape::sizeHint(void) const
{
  if (plan->Booleans["Desktop"]) {
    return QSize ( 640 , 640 )   ;
  }                              ;
  return   QSize ( 320 , 320 )   ;
}

void N::FaceShape::Configure (void)
{
  Properties [ 0 ] = true           ;
  Properties [ 1 ] = true           ;
  Properties [ 2 ] = true           ;
  Properties [ 3 ] = true           ;
  Properties [ 4 ] = true           ;
  Properties [ 5 ] = true           ;
  Properties [ 6 ] = false          ;
  setFocusPolicy ( Qt::WheelFocus ) ;
}

bool N::FaceShape::FocusIn(void)
{
  AssignAction ( Label  , windowTitle() ) ;
  LinkAction   ( Save   , Save       () ) ;
  LinkAction   ( Export , Export     () ) ;
  LinkAction   ( Import , background () ) ;
  return true                             ;
}

void N::FaceShape::focusInEvent(QFocusEvent * event)
{
  if (!focusIn (event)) QWidget::focusInEvent (event) ;
}

void N::FaceShape::focusOutEvent(QFocusEvent * event)
{
  if (!focusOut(event)) QWidget::focusOutEvent(event) ;
}

void N::FaceShape::closeEvent(QCloseEvent * event)
{
  emit Leave ( this )        ;
  QWidget::closeEvent(event) ;
}

void N::FaceShape::contextMenuEvent(QContextMenuEvent * event)
{
  if (Menu(event->pos())) event->accept(); else
  QWidget::contextMenuEvent(event)            ;
}

QPointF N::FaceShape::Quadratic(double t,QPointF & P1,QPointF & P2,QPointF & P3)
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

void N::FaceShape::PaintFace(QPainter & p)
{
  CUIDs PS                                      ;
  int   ps                                      ;
  if (equation.Sequences.count()<=0) return     ;
  if ( Properties [ 2 ] )                       {
    QMap<int,QPointF> pts                       ;
    QPen         pen   ( QColor(  0,  0,  0) )  ;
    QBrush       brush                          ;
    QPainterPath path                           ;
    QPointF      p1                             ;
    QPointF      p2                             ;
    QPolygonF    C                              ;
    QPolygonF    R                              ;
    QPointF      P0                             ;
    QPointF      P1                             ;
    QPointF      P2                             ;
    QPointF      P3                             ;
    int          total                          ;
    pen . setWidthF ( 2.4   )                   ;
    PS    = equation . Sequences                ;
    total = PS       . count ( )                ;
    for (int i=0;i<total;i++)                   {
      ps = PS[i]                                ;
      double x  = equation.Points[ps].x()       ;
      double y  = equation.Points[ps].y()       ;
      double xs                                 ;
      double ys                                 ;
      xs = plan->screen.widthPixels  ( x * 10 ) ;
      ys = plan->screen.heightPixels ( y * 10 ) ;
      xs *= Factor                              ;
      ys *= Factor                              ;
      xs += ( width  ( ) / 2 )                  ;
      ys  = ((double)(height()/2)) - ys         ;
      p1 . setX ( xs )                          ;
      p1 . setY ( ys )                          ;
      pts [ i ] = p1                            ;
    }                                           ;
    for (int i=0;i<total;i++)                   {
      int a = i     ; a %= total                ;
      int b = i + 1 ; b %= total                ;
      int c = i + 2 ; c %= total                ;
      P1 = pts[a]                               ;
      P2 = pts[b]                               ;
      P3 = pts[c]                               ;
      P0 = P1 + P2 ; P0 /= 2                    ;
      C << P0                                   ;
      P0 = Quadratic(0.5,P1,P2,P3)              ;
      R << P0                                   ;
    }                                           ;
    path . moveTo ( R[0] )                      ;
    for (int i=1;i<total;i++)                   {
      path . quadTo ( C[ i ] , R[ i ] )         ;
    }                                           ;
    path . quadTo ( C[ 0 ] , R[ 0 ] )           ;
    if ( Properties [ 6 ] )                     {
      QBrush brh   ( QColor(224,224,224) )      ;
      p . fillPath ( path , brh          )      ;
    }                                           ;
    p . setPen   ( pen   )                      ;
    p . setBrush ( brush )                      ;
    p . drawPath ( path  )                      ;
  }                                             ;
  if ( Properties [ 3 ] )                       {
    QMap<int,QPointF> sp                        ;
    QPen  pen ( QColor (255,192,192) )          ;
    pen . setWidthF ( 1.1   )                   ;
    PS = equation.Points.keys()                 ;
    foreach (ps,PS)                             {
      double x  = equation.Points[ps].x()       ;
      double y  = equation.Points[ps].y()       ;
      double xs                                 ;
      double ys                                 ;
      double hs                                 ;
      double zs = 7.0                           ;
      xs = plan->screen.widthPixels  ( x * 10 ) ;
      ys = plan->screen.heightPixels ( y * 10 ) ;
      xs *= Factor                              ;
      ys *= Factor                              ;
      zs *= Factor                              ;
      hs  = zs / 2                              ;
      xs += ( width  ( ) / 2 )                  ;
      ys  = ((double)(height()/2)) - ys         ;
      sp[ps] = QPointF(xs,ys)                   ;
      QRectF R(xs-hs,ys-hs,zs,zs)               ;
      QRadialGradient grad                      ;
      grad = QRadialGradient(xs,ys,hs)          ;
      grad.setColorAt(0,QColor(192,192, 64))    ;
      grad.setColorAt(1,QColor(255,255,255))    ;
      QBrush brush    ( grad  )                 ;
      p . setPen      ( pen   )                 ;
      p . setBrush    ( brush )                 ;
      p . drawEllipse ( R     )                 ;
      if ( Properties [ 4 ]   )                 { //
      }                                         ;
    }                                           ;
    if ( Properties [ 5 ] && equation.ControlFirst.count()>0 ) {
      QBrush brush (QColor(224,224,255))        ;
      PS = equation.ControlFirst.keys()         ;
      pen . setColor  ( QColor(0,0,255) )       ;
      pen . setWidthF ( 1.5             )       ;
      foreach (ps,PS)                           {
        int f = equation . ControlFirst  [ ps ] ;
        int s = equation . ControlSecond [ ps ] ;
        p . setPen      ( pen           )       ;
        p . setBrush    ( brush         )       ;
        p . drawLine    ( sp[f] , sp[s] )       ;
      }                                         ;
    }                                           ;
  }                                             ;
}

void N::FaceShape::PaintAll(QPainter & p)
{
  p . setBrush ( QBrush ( QColor ( 255,255,255 ) ) ) ;
  p . setPen   ( QPen   ( QColor ( 255,255,255 ) ) ) ;
  p . drawRect ( QRect  ( QPoint (0,0) , size()  ) ) ;
  if (NotNull(image))                                {
    QRectF R                                         ;
    double cx = width  ( ) / 2                       ;
    double cy = height ( ) / 2                       ;
    double iw = image->width  ( )                    ;
    double ih = image->height ( )                    ;
    iw *= ImageFactor                                ;
    ih *= ImageFactor                                ;
    cx  = cx - ( iw / 2 )                            ;
    cy  = cy - ( ih / 2 )                            ;
    cx += ImageX                                     ;
    cy += ImageY                                     ;
    R . setLeft   ( cx         )                     ;
    R . setTop    ( cy         )                     ;
    R . setWidth  ( iw         )                     ;
    R . setHeight ( ih         )                     ;
    p . drawImage ( R , *image )                     ;
  }                                                  ;
  if ( Properties [ 0 ] )                            {
    QPointF p1                                       ;
    QPointF p2                                       ;
    p  . setBrush ( QBrush ( QColor ( 0,255,0 )  ) ) ;
    p  . setPen   ( QPen   ( QColor ( 0,255,0 )  ) ) ;
    p1 . setX ( 0            )                       ;
    p2 . setX ( width ()     )                       ;
    p1 . setY ( height() / 2 )                       ;
    p2 . setY ( height() / 2 )                       ;
    p  . drawLine ( p1 , p2                        ) ;
    p1 . setX ( width () / 2 )                       ;
    p2 . setX ( width () / 2 )                       ;
    p1 . setY ( 0            )                       ;
    p2 . setY ( height()     )                       ;
    p  . drawLine ( p1 , p2                        ) ;
  }                                                  ;
  if ( Properties [ 1 ] )                            {
    QPen   pen   ( QColor (224,224,224) )            ;
    int wp = plan->screen.widthPixels (10)           ;
    int hp = plan->screen.heightPixels(10)           ;
    wp  = (int)(((double)wp) * Factor )              ;
    hp  = (int)(((double)hp) * Factor )              ;
    int xp = (int)( ((double)width ()) / wp )        ;
    int yp = (int)( ((double)height()) / hp )        ;
    pen . setWidthF ( 1.1   )                        ;
    p  . setPen     ( pen   )                        ;
    xp /= 2                                          ;
    yp /= 2                                          ;
    for (int y=-yp;y<=yp;y++)                        {
      for (int x=-xp;x<=xp;x++)                      {
        double xs                                    ;
        double ys                                    ;
        double hs                                    ;
        double ps = 3.2                              ;
        xs = plan->screen.widthPixels  ( x * 10 )    ;
        ys = plan->screen.heightPixels ( y * 10 )    ;
        xs *= Factor                                 ;
        ys *= Factor                                 ;
        ps *= Factor                                 ;
        hs  = ps / 2                                 ;
        xs += ( width  ( ) / 2 )                     ;
        ys += ( height ( ) / 2 )                     ;
        QRectF R(xs-hs,ys-hs,ps,ps)                  ;
        QRadialGradient grad                         ;
        grad = QRadialGradient(xs,ys,hs)             ;
        grad.setColorAt(0,QColor(224,240,240))       ;
        grad.setColorAt(1,QColor(255,255,255))       ;
        QBrush brush    ( grad  )                    ;
        p . setBrush    ( brush )                    ;
        p . drawEllipse ( R     )                    ;
      }                                              ;
    }                                                ;
  }                                                  ;
  PaintFace ( p )                                    ;
}

void N::FaceShape::paintEvent(QPaintEvent * event)
{ Q_UNUSED   ( event ) ;
  QPainter p ( this  ) ;
  PaintAll   ( p     ) ;
}

void N::FaceShape::resizeEvent(QResizeEvent * event)
{
  QWidget :: resizeEvent ( event ) ;
  relocation             (       ) ;
}

void N::FaceShape::mouseDoubleClickEvent(QMouseEvent * event)
{
  QWidget :: mouseDoubleClickEvent ( event ) ;
}

void N::FaceShape::mouseMoveEvent(QMouseEvent * event)
{
  QWidget :: mouseMoveEvent ( event ) ;
}

void N::FaceShape::mousePressEvent(QMouseEvent * event)
{
  QWidget :: mousePressEvent ( event ) ;
}

void N::FaceShape::mouseReleaseEvent(QMouseEvent * event)
{
  QWidget :: mouseReleaseEvent ( event ) ;
}

void N::FaceShape::relocation(void)
{
}

void N::FaceShape::Changed(int id,QWidget * widget)
{
  update ( ) ;
}

void N::FaceShape::Closed(int id,QWidget * widget)
{
  Widgets . take ( id ) ;
}

bool N::FaceShape::startup(void)
{
  nKickOut ( ObjectUuid() <=0 , false     ) ;
  equation . setObjectUuid ( ObjectUuid() ) ;
  equation . load          ( plan         ) ;
  repaint  (                              ) ;
  return true                               ;
}

void N::FaceShape::background(void)
{
  QString filename = QFileDialog::getOpenFileName                            (
                       this                                                  ,
                       tr("Import images")                                   ,
                       plan->Dirs[N::Directory::Images].absoluteFilePath("") ,
                       N::Images::ImageFilters()                           ) ;
  nDropOut ( filename.length() <= 0 )                                        ;
  QImage * img = new QImage()                                                ;
  if (!img->load(filename))                                                  {
    delete img                                                               ;
    return                                                                   ;
  }                                                                          ;
  QImage * bkg = image                                                       ;
  image = img                                                                ;
  if (NotNull(bkg)) delete bkg                                               ;
  update ( )                                                                 ;
  Alert       ( Done     )                                                   ;
}

int N::FaceShape::Nearby(QPoint pos)
{
  double x = pos.x()               ;
  double y = pos.y()               ;
  x -=  width () / 2               ;
  y  = (height() / 2) - y          ;
  x /= Factor                      ;
  y /= Factor                      ;
  x *= plan->screen.WidthLength    ;
  x /= plan->screen.WidthPixels    ;
  x /= 10.0                        ;
  y *= plan->screen.HeightLength   ;
  y /= plan->screen.HeightPixels   ;
  y /= 10.0                        ;
  QPointF P(x,y)                   ;
  CUIDs K = equation.Points.keys() ;
  int   k                          ;
  foreach (k,K)                    {
    QPointF Z = equation.Points[k] ;
    QPointF D = Z - P              ;
    if (D.manhattanLength()<0.1)   {
      return k                     ;
    }                              ;
  }                                ;
  return -1                        ;
}

void N::FaceShape::Save(void)
{
  equation . save ( plan ) ;
  update          (      ) ;
  Alert           ( Done ) ;
}

void N::FaceShape::Export(void)
{
  QString filename = QFileDialog::getSaveFileName                            (
                       this                                                  ,
                       tr("Import images")                                   ,
                       plan->Dirs[N::Directory::Images].absoluteFilePath("") ,
                       N::Images::ImageFilters()                           ) ;
  nDropOut ( filename.length() <= 0 )                                        ;
  QPainter p                                                                 ;
  QImage img  ( size() , QImage::Format_ARGB32 )                             ;
  p   . begin ( &img     )                                                   ;
  PaintAll    ( p        )                                                   ;
  p   . end   (          )                                                   ;
  img . save  ( filename )                                                   ;
  Alert       ( Done     )                                                   ;
}

void N::FaceShape::CleanIsolations(void)
{
}

void N::FaceShape::EditPoints(void)
{
  QString              M                               ;
  CtrlPadPointsIndex * CPPI                            ;
  M = tr("Points of `%1`").arg(windowTitle())          ;
  CPPI  = new N::CtrlPadPointsIndex(NULL,plan)         ;
  CPPI -> setPoints  ( equation.Points       )         ;
  plan -> addControl ( M , CPPI , this       )         ;
  CPPI -> startup    (                       )         ;
  //////////////////////////////////////////////////////
  Widgets [ EDITPOINTS ] = CPPI                        ;
  nConnect ( CPPI , SIGNAL ( Changed(int,QWidget*) )   ,
             this , SLOT   ( Changed(int,QWidget*) ) ) ;
  nConnect ( CPPI , SIGNAL ( Closed (int,QWidget*) )   ,
             this , SLOT   ( Closed (int,QWidget*) ) ) ;
}

bool N::FaceShape::Menu(QPoint pos)
{
  QAction       * aa      = NULL       ;
  DoubleSpinBox * ImageV  = NULL       ;
  DoubleSpinBox * imageX  = NULL       ;
  DoubleSpinBox * imageY  = NULL       ;
  DoubleSpinBox * pointX  = NULL       ;
  DoubleSpinBox * pointY  = NULL       ;
  DoubleSpinBox * FactorV = NULL       ;
  ComboBox      * pointID = NULL       ;
  int             PID = -1             ;
  nScopedMenu    ( mm , this         ) ;
  QMenu * mi                           ;
  QMenu * md                           ;
  QMenu * mp                           ;
  pointX = NULL                        ;
  pointY = NULL                        ;
  mm . add ( 501 , tr("Save") )        ;
  mi = mm . addMenu ( tr("Image"  ) )  ;
  md = mm . addMenu ( tr("Display") )  ;
  ImageV  = new DoubleSpinBox(this,plan)    ;
  ImageV -> setValue (ImageFactor)     ;
  ImageV -> setSingleStep ( 0.01 )     ;
  ImageV -> External = &ImageFactor    ;
  imageX  = new DoubleSpinBox(this,plan)    ;
  imageX -> setRange(-10000,10000)     ;
  imageX -> setValue (ImageX)          ;
  imageX -> setSingleStep ( 0.50 )     ;
  imageX -> External = &ImageX         ;
  imageY  = new DoubleSpinBox(this,plan)    ;
  imageY -> setRange(-10000,10000)     ;
  imageY -> setValue (ImageY)          ;
  imageY -> setSingleStep ( 0.50 )     ;
  imageY -> External = &ImageY         ;
  FactorV = new DoubleSpinBox(this,plan)    ;
  FactorV-> setValue (Factor)          ;
  FactorV-> setSingleStep ( 0.01 )     ;
  FactorV-> External = &Factor         ;
  mm . add ( mi  , 101 , tr("Image") ) ;
  mm . add ( mi  , 102 , tr("Clear") ) ;
  mm . add ( mi  , 502 , tr("Export")) ;
  mm . add ( mi  , 103 , ImageV      ) ;
  mm . add ( mi  , 104 , imageX      ) ;
  mm . add ( mi  , 105 , imageY      ) ;
  mm . add ( md  , 201 , tr("Axis"      ) , true , Properties[0] ) ;
  mm . add ( md  , 202 , tr("Grid"      ) , true , Properties[1] ) ;
  mm . add ( md  , 203 , tr("Contour"   ) , true , Properties[2] ) ;
  mm . add ( md  , 204 , tr("Points"    ) , true , Properties[3] ) ;
  mm . add ( md  , 205 , tr("Index"     ) , true , Properties[4] ) ;
  mm . add ( md  , 206 , tr("Connection") , true , Properties[5] ) ;
  mm . add ( md  , 207 , tr("Fill"      ) , true , Properties[6] ) ;
  mm . add ( md  , 211 , FactorV     ) ;
  PID = Nearby(pos)                    ;
  mp = mm.addMenu ( tr("Points" )  )   ;
  mm . add ( mp , 401 , tr("Reposition") ) ;
  if ( ! Widgets.contains(EDITPOINTS) ) {
    mm . add ( mp , 411 , tr("Edit") ) ;
  }                                    ;
  if (PID>=0)                          {
    CUIDs K                            ;
    int idx = equation.Sequences.indexOf(PID) ;
    mm . add ( mp , 301 , tr("Delete %1").arg(PID) ) ;
    if (idx>=0)                        {
      idx++                            ;
      idx %= equation.Sequences.count() ;
      idx  = equation.Sequences[idx]   ;
      mm . add ( mp , 302 , tr("Insert point between %1 and %2").arg(PID).arg(idx) ) ;
    }                                  ;
    mm . add ( mp , 304 , tr("Connect %1 as a pair").arg(PID) ) ;
    mm . add ( mp , 307 , tr("Disconnect all links connected to %1").arg(PID) ) ;
    pointX  = new DoubleSpinBox(this,plan)    ;
    pointX -> setRange(-10000,10000)     ;
    pointX -> setValue (equation.Points[PID].x()) ;
    pointX -> setSingleStep ( 0.10 )   ;
    pointY  = new DoubleSpinBox(this,plan)  ;
    pointY -> setRange(-10000,10000)   ;
    pointY -> setValue (equation.Points[PID].y()) ;
    pointY -> setSingleStep ( 0.10 )   ;
    pointID = new ComboBox(this,plan)  ;
    pointID->setEditable(true)         ;
    K   = equation.Points.keys()       ;
    idx = K.indexOf(PID)               ;
    if (idx>=0) K.takeAt(idx)          ;
    foreach (idx,K) pointID->addItem(QString::number(idx) ) ;
    mm . add ( mp  , 311 , pointX    ) ;
    mm . add ( mp  , 312 , pointY    ) ;
    mm . add ( mp  , 313 , pointID   ) ;
  } else                               {
    mm . add ( mp , 303 , tr("Add point") ) ;
    mm . add ( mp , 305 , tr("Clean isolated points") ) ;
    mm . add ( mp , 306 , tr("Double up points") ) ;
  }                                    ;
  mm . setFont   ( plan              ) ;
  aa = mm . exec (                   ) ;
  if (NotNull(pointX))                 {
    if (PID>=0)                        {
      equation.Points[PID].setX(pointX->value()) ;
    }                                  ;
    pointX->deleteLater()              ;
  }                                    ;
  if (NotNull(pointY))                 {
    if (PID>=0)                        {
      equation.Points[PID].setY(pointY->value()) ;
    }                                  ;
    pointY->deleteLater()              ;
  }                                    ;
  if (IsNull(aa))                      {
    update ( )                         ;
    return true                        ;
  }                                    ;
  switch ( mm[aa] )                    {
    case 101                           :
      background ( )                   ;
    break                              ;
    case 102                           :
      if (NotNull(image))              {
        QImage * bkg = image           ;
        image = NULL                   ;
        delete bkg                     ;
        update ( )                     ;
      }                                ;
    break                              ;
    case 201                           :
      Properties[0] = aa->isChecked()  ;
      update ( )                       ;
    break                              ;
    case 202                           :
      Properties[1] = aa->isChecked()  ;
      update ( )                       ;
    break                              ;
    case 203                           :
      Properties[2] = aa->isChecked()  ;
      update ( )                       ;
    break                              ;
    case 204                           :
      Properties[3] = aa->isChecked()  ;
      update ( )                       ;
    break                              ;
    case 205                           :
      Properties[4] = aa->isChecked()  ;
      update ( )                       ;
    break                              ;
    case 206                           :
      Properties[5] = aa->isChecked()  ;
      update ( )                       ;
    break                              ;
    case 207                           :
      Properties[6] = aa->isChecked()  ;
      update ( )                       ;
    break                              ;
    case 301                           :
      if (PID>=0)                      {
        equation . Delete ( PID )      ;
        update ( )                     ;
      }                                ;
    break                              ;
    case 302                           :
      if (PID>=0)                      {
        int idx = equation.Sequences.indexOf(PID) ;
        int ndx = idx + 1              ;
        ndx %= equation.Sequences.count() ;
        ndx  = equation.Sequences[ndx]    ;
        QPointF p1 = equation.Points[PID] ;
        QPointF p2 = equation.Points[ndx] ;
        QPointF m1 = p1 + p2           ;
        m1 /= 2                        ;
        CUIDs K = equation.Points.keys() ;
        ndx = -1                       ;
        for (int i=0;i<K.count();i++)  {
          if (ndx<K[i]) ndx = K[i]     ;
        }                              ;
        ndx++                          ;
        equation.Points[ndx] = m1      ;
        equation.Sequences.insert(idx+1,ndx) ;
        update ( )                     ;
      }                                ;
    break                              ;
    case 303                         : {
      double x = pos.x()               ;
      double y = pos.y()               ;
      x -=  width () / 2               ;
      y  = (height() / 2) - y          ;
      x /= Factor                      ;
      y /= Factor                      ;
      x *= plan->screen.WidthLength    ;
      x /= plan->screen.WidthPixels    ;
      x /= 10.0                        ;
      y *= plan->screen.HeightLength   ;
      y /= plan->screen.HeightPixels   ;
      y /= 10.0                        ;
      QPointF P(x,y)                   ;
      CUIDs K = equation.Points.keys() ;
      int ndx = -1                     ;
      for (int i=0;i<K.count();i++)    {
        if (ndx<K[i]) ndx = K[i]       ;
      }                                ;
      ndx++                            ;
      equation.Points[ndx] = P         ;
      update ( )                       ;
    }                                  ;
    break                              ;
    case 304                           :
      if (NotNull(pointID))            {
        CUIDs KID = equation.ControlFirst.keys() ;
        int sid = pointID->currentText().toInt() ;
        int nid = -1                   ;
        for (int i=0;i<KID.count();i++) {
          if (KID[i]>nid) nid = KID[i] ;
        }                              ;
        nid++                          ;
        equation . ControlFirst  [ nid ] = PID ;
        equation . ControlSecond [ nid ] = sid ;
        update   ( )                   ;
      }                                ;
    break                              ;
    case 305                           :
      equation . CleanIsolations (   ) ;
      update     ( )                   ;
    break                              ;
    case 306                           :
      equation . DoubleUp        (   ) ;
      update     ( )                   ;
    break                              ;
    case 307                           :
      equation . Dismantle ( PID )     ;
      update     ( )                   ;
    break                              ;
    case 401                           :
      equation . Reposition      (   ) ;
      update     ( )                   ;
    break                              ;
    case 411                           :
      EditPoints ( )                   ;
    break                              ;
    case 501                           :
      Save       ( )                   ;
    break                              ;
    case 502                           :
      Export     ( )                   ;
    break                              ;
    default                            :
      update     ( )                   ;
    break                              ;
  }                                    ;
  ImageV  -> deleteLater ( )           ;
  imageX  -> deleteLater ( )           ;
  imageY  -> deleteLater ( )           ;
  FactorV -> deleteLater ( )           ;
  return true                          ;
}
