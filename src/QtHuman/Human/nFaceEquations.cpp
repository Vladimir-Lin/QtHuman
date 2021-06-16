#include <qthuman.h>

N::FaceEquations:: FaceEquations       (void)
                 : QList<FaceEquation> (    )
{
}

N::FaceEquations::~FaceEquations(void)
{
}

bool N::FaceEquations::load(PurePlan * plan,SqlConnection & SC)
{
  UUIDs U                        ;
  SUID  u                        ;
  clear ( )                      ;
  U = SC . Uuids                 (
        PlanTable(FaceShapes)    ,
        "uuid"                   ,
        SC . OrderByAsc ("id") ) ;
  foreach (u,U)                  {
    FaceEquation E ( u )         ;
    E . load ( plan , SC )       ;
    append ( E )                 ;
  }                              ;
  return ( count() > 0 )         ;
}

bool N::FaceEquations::save(PurePlan * plan,SqlConnection & SC)
{
  for (int i=0;i<count();i++)     {
    ME [ i ] . save ( plan , SC ) ;
  }                               ;
  return ( count() > 0 )          ;
}

bool N::FaceEquations::load(PurePlan * plan)
{
  SqlConnection SC(plan->sql)          ;
  if (SC.open("FaceEquations","load")) {
    load(plan,SC)                      ;
    SC.close()                         ;
  }                                    ;
  SC.remove()                          ;
  return true                          ;
}

bool N::FaceEquations::save(PurePlan * plan)
{
  SqlConnection SC(plan->sql)          ;
  if (SC.open("FaceEquations","load")) {
    save(plan,SC)                      ;
    SC.close()                         ;
  }                                    ;
  SC.remove()                          ;
  return true                          ;
}
