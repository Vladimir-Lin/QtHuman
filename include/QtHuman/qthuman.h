/****************************************************************************
 *                                                                          *
 * Copyright (C) 2015 Neutrino International Inc.                           *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_HUMAN_H
#define QT_HUMAN_H

#include <QtPhotography>
#include <DataWidgets>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#  if defined(QT_BUILD_QTHUMAN_LIB)
#    define Q_HUMAN_EXPORT Q_DECL_EXPORT
#  else
#    define Q_HUMAN_EXPORT Q_DECL_IMPORT
#  endif
#else
#    define Q_HUMAN_EXPORT
#endif

namespace N
{

class Q_HUMAN_EXPORT CrowdView               ;
class Q_HUMAN_EXPORT PeopleView              ;
class Q_HUMAN_EXPORT ActorView               ;
class Q_HUMAN_EXPORT PeopleList              ;
class Q_HUMAN_EXPORT BodyFeatures            ;
class Q_HUMAN_EXPORT PeopleListing           ;
class Q_HUMAN_EXPORT PeopleMerge             ;
class Q_HUMAN_EXPORT PeopleTags              ;
class Q_HUMAN_EXPORT PeopleReporter          ;
class Q_HUMAN_EXPORT PeopleDispatcher        ;
class Q_HUMAN_EXPORT PeopleProcessor         ;
class Q_HUMAN_EXPORT SexualityTree           ;
class Q_HUMAN_EXPORT SexualityList           ;
class Q_HUMAN_EXPORT FaceScanner             ;
class Q_HUMAN_EXPORT FaceView                ;
class Q_HUMAN_EXPORT PeopleBirthdays         ;
class Q_HUMAN_EXPORT PeopleSexuality         ;
class Q_HUMAN_EXPORT EyesView                ;
class Q_HUMAN_EXPORT EyesList                ;
class Q_HUMAN_EXPORT HairsView               ;
class Q_HUMAN_EXPORT HairsList               ;
class Q_HUMAN_EXPORT OrganLists              ;
class Q_HUMAN_EXPORT AcupunctureLists        ;
class Q_HUMAN_EXPORT BodyParts               ;
class Q_HUMAN_EXPORT BodyLists               ;
class Q_HUMAN_EXPORT Consanguinities         ;
class Q_HUMAN_EXPORT RaceLists               ;
class Q_HUMAN_EXPORT EyesShapes              ;
class Q_HUMAN_EXPORT FaceEquation            ;
class Q_HUMAN_EXPORT FaceEquations           ;
class Q_HUMAN_EXPORT FaceShape               ;
class Q_HUMAN_EXPORT FaceShapes              ;
class Q_HUMAN_EXPORT EmotionLists            ;
class Q_HUMAN_EXPORT PhonemeItems            ;
class Q_HUMAN_EXPORT PhonemeGroups           ;
class Q_HUMAN_EXPORT CrowdsShelf             ;
class Q_HUMAN_EXPORT PeopleShelf             ;

/*****************************************************************************
 *                                                                           *
 *                               People widgets                              *
 *                                                                           *
 *****************************************************************************/

class Q_HUMAN_EXPORT CrowdView : public GroupView
{
  Q_OBJECT
  public:

    explicit CrowdView             (StandardConstructor) ;
    virtual ~CrowdView             (void);

  protected:

    virtual void    Configure      (void) ;

    virtual QString MimeType       (const QMimeData * mime);
    virtual UUIDs   MimeUuids      (const QMimeData * mime,QString mimetype) ;

    virtual UUIDs   GroupUuids     (SqlConnection & Connection) ;
    virtual QString ToolTipTotoal  (int total) ;
    virtual int     Counting       (SqlConnection & Connection,SUID uuid) ;

    virtual void    reportItems    (void) ;

  private:

  public slots:

    virtual void    GroupPositions (RMAPs & Position,UUIDs & Uuids) ;
    virtual bool    JoinImage      (SUID uuid,const QImage & image) ;
    virtual bool    JoinPictures   (SUID uuid,const UUIDs & Uuids) ;

  protected slots:

    virtual bool    Menu           (QPoint pos) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT PeopleView : public SnapView
                                , public Group
{
  Q_OBJECT
  public:

    explicit PeopleView                  (StandardConstructor) ;
    virtual ~PeopleView                  (void);

    bool contains                        (SUID uuid) ;

  protected:

    ComboBox * Search     ;
    bool       dropAction ;

    virtual bool        FocusIn          (void) ;

    virtual void        closeEvent       (QCloseEvent * event) ;

    virtual void        Configure        (void) ;

    virtual QMimeData * dragMime         (void) ;

    virtual bool        acceptDrop       (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew          (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving       (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend       (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual QString     MimeType         (const QMimeData * mime);
    virtual UUIDs       MimeUuids        (const QMimeData * mime,QString mimetype) ;

    virtual bool        dropText         (nDeclDrops,const QString     & text  ) ;
    virtual bool        dropUrls         (nDeclDrops,const QList<QUrl> & urls  ) ;
    virtual bool        dropImage        (nDeclDrops,const QImage      & image ) ;
    virtual bool        dropHtml         (nDeclDrops,const QString     & html  ) ;
    virtual bool        dropTags         (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPictures     (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPeople       (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropAlbums       (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropGender       (nDeclDrops,const SUID          gender) ;

    virtual bool        setUuid          (SqlConnection & Connection,SUID uuid,QListWidgetItem * item) ;
    virtual bool        addUuid          (SqlConnection & Connection,SUID uuid) ;
    virtual void        JoinActor        (SqlConnection & Connection,SUID uuid) ;
    virtual void        JoinActor        (SqlConnection & Connection,SUID uuid,QListWidgetItem * Item) ;
    virtual void        DetachActors     (SqlConnection & Connection,UUIDs & Uuids) ;

    QListWidgetItem *   newUuid          (SqlConnection & Connection,SUID uuid) ;

    virtual void        StopIcons        (void) ;
    virtual void        run              (int Type,ThreadData * data) ;

    virtual void        JoinTags         (ThreadData * data) ;
    virtual void        JoinAlbums       (ThreadData * data) ;
    virtual void        JoinGender       (ThreadData * data) ;

    virtual bool        canDelete        (void) ;

    virtual bool        isOwner          (void) ;

    virtual bool        PeoplePictures   (SqlConnection & Connection,QListWidgetItem * item) ;
    virtual bool        PossiblePictures (SqlConnection & Connection,QListWidgetItem * item) ;
    virtual bool        ReportDetails    (SqlConnection & Connection,QListWidgetItem * item,int flags) ;
    virtual void        FetchUUIDs       (void) ;
    virtual void        FetchTOTALs      (void) ;
    virtual void        ActorUUIDs       (void) ;
    virtual void        ActorTOTALs      (void) ;

  private:

  public slots:

    virtual bool        startup          (void) ;
    virtual void        FolderLanguage   (void) ;
    virtual void        ResumeLanguage   (void) ;
    virtual void        View             (UUIDs & Uuids) ;
    virtual void        Find             (void) ;
    virtual void        Refresh          (QListWidgetItem * item) ;
    virtual void        Add              (SUID uuid) ;
    virtual void        Add              (UUIDs & Uuids,int sourceType = 0) ;
    virtual void        Paste            (void) ;
    virtual void        Paste            (QString text) ;
    virtual void        addPeople        (SqlConnection & Connection,QString name) ;
    virtual void        NewPeople        (void) ;
    virtual void        LocatePeople     (void) ;
    virtual void        RemovePeople     (void) ;
    virtual void        JoinActors       (UUIDs & Uuids);
    virtual void        JoinCrowd        (UUIDs & Uuids) ;
    virtual void        JoinImage        (SUID people,const QMimeData * mime) ;
    virtual void        MovePeople       (SUID before,const UUIDs & Uuids) ;
    virtual void        JoinCrowds       (SUID before,const UUIDs & Uuids) ;

    virtual void        Update           (QWidget * widget,SUID uuid) ;

    virtual void        RelatedItems     (SUID uuid,QString title) ;

    virtual void        ExportNames      (void) ;
    virtual void        ExportAllNames   (void) ;

    virtual bool        RunCommand       (VarArgs & arguments) ;

  protected slots:

    virtual void        singleClicked    (QListWidgetItem * item) ;

    virtual void        SearchIndex      (int index) ;
    virtual void        NameFinished     (void) ;
    virtual void        RemoveOldItem    (void) ;

    virtual bool        Menu             (QPoint pos) ;

    virtual void        doTranslations   (void) ;
    virtual void        setPositions     (void) ;
    virtual void        CountPictures    (void) ;
    virtual void        CountPossible    (void) ;

  private slots:

  signals:

    void NewPerson         (void);
    void Gallery           (QWidget * widget,SUID people,QString name) ;
    void PotentialPictures (QWidget * widget,SUID people,QString name) ;
    void VideoAlbum        (QWidget * widget,SUID people,QString name) ;
    void PeopleNames       (QWidget * widget,SUID people,QString name) ;
    void PeopleDocuments   (QWidget * widget,SUID people,QString name) ;
    void Internet          (QWidget * widget,SUID people,QString name,int Id);
    void UuidItems         (UUIDs & Uuids,RMAPs & Types,NAMEs & Names,QString title) ;
    void Translations      (QString title,UUIDs & Uuids) ;
    void Feature           (int Catalog,SUID uuid) ;
    void MergePeople       (UUIDs & Uuids) ;
    void ListSubgroups     (QString name,SUID uuid) ;
    void PeopleListings    (QString name,SUID uuid,int type,int relation) ;

};

class Q_HUMAN_EXPORT ActorView : public PeopleView
{
  Q_OBJECT
  public:

    explicit     ActorView (StandardConstructor) ;
    virtual     ~ActorView (void) ;

  protected:

  private:

  public slots:

    virtual bool startup   (void) ;

  protected slots:

  private slots:

  signals:

};

class Q_HUMAN_EXPORT PeopleList : public ComboBox
                                , public PeopleManager
{
  Q_OBJECT
  public:

    explicit PeopleList    (StandardConstructor) ;
    virtual ~PeopleList    (void) ;

  protected:

    virtual void Configure (void) ;

    virtual void run       (int type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup   (void) ;
    virtual void List      (void) ;
    virtual void Search    (void) ;

  protected slots:

    void setCompleter      (QStringList words) ;

  private slots:

  signals:

    void assignCompleter   (QStringList words) ;
    void People            (QString name,SUID uuid) ;

};

class Q_HUMAN_EXPORT BodyFeatures : public TreeWidget
                                  , public Object
                                  , public PeopleManager
                                  , public GroupItems
{
  Q_OBJECT
  public:

    explicit BodyFeatures         (StandardConstructor) ;
    virtual ~BodyFeatures         (void) ;

    virtual QSize sizeHint        (void) const ;

  protected:

    virtual void Configure        (void) ;
    virtual bool FocusIn          (void) ;

    virtual void run              (int type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup          (void) ;
    virtual void List             (void) ;

  protected slots:

    virtual bool Menu             (QPoint pos) ;
    virtual void doubleClicked    (QTreeWidgetItem * item,int column) ;
    virtual void variableFinished (void) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT PeopleListing : public RowTable
{
  Q_OBJECT
  public:

    explicit PeopleListing         (StandardConstructor) ;
    virtual ~PeopleListing         (void) ;

    virtual QSize sizeHint         (void) const ;

  protected:

    NAMEs ColumnNames      ;
    NAMEs FullyFunctions   ;
    IMAPs ColumnAlignments ;
    IMAPs ColumnRetrievers ;
    IMAPs RetrieverColumns ;
    CUIDs ActiveRetrievers ;

    virtual void Configure         (void) ;

    virtual bool LoadCell          (SqlConnection & Connection,int row,int column,SUID uuid) ;

    virtual void setHeader         (void) ;

    virtual int  Counting          (SqlConnection & Connection) ;

    virtual int  InstallRetrievers (int column,int RetrieverId) ;

  private:

  public slots:

    virtual bool startup           (void) ;

    virtual void List              (void) ;

    virtual void ActiveRetriever   (int Id,bool active) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT PeopleMerge : public TreeDock
                                 , public GroupItems
{
  Q_OBJECT
  public:

    explicit PeopleMerge         (StandardConstructor) ;
    virtual ~PeopleMerge         (void) ;

    virtual QSize sizeHint       (void) const ;

  protected:

    bool Deletion ;
    bool Execute  ;

    virtual bool FocusIn         (void) ;
    virtual void Configure       (void) ;

    virtual bool acceptDrop      (QWidget * source,const QMimeData * mime);
    virtual bool dropNew         (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropMoving      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropAppend      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropPeople      (nDeclDrops,const UUIDs & Uuids) ;

    virtual bool NamesMerger     (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool GroupsRemove    (SqlConnection & SC,QList<Group> & groups) ;
    virtual bool GroupsJoin      (SqlConnection & SC,QList<Group> & groups) ;
    virtual bool GroupsMerger    (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool OwnersMerger    (SqlConnection & SC,SUID master,SUID deletion) ;

    virtual void run             (int Type,ThreadData * data) ;

  private:

    virtual bool MergeTriples    (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool MergeFlags      (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool MergeVariables  (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool MergeIdentifier (SqlConnection & SC,SUID master,SUID deletion) ;
    virtual bool DeleteMerged    (SqlConnection & SC,SUID deletion) ;

  public slots:

    virtual void New             (void) ;
    virtual void Delete          (void) ;
    virtual void Merge           (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void peopleFinished  (int) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT PeopleTags : public TagsEditor
{
  Q_OBJECT
  public:

    explicit     PeopleTags       (StandardConstructor) ;
    virtual     ~PeopleTags       (void) ;

  protected:

    virtual void Configure        (void) ;

    virtual bool addMenuItems     (MenuManager & menu,QPoint pos) ;

    virtual void TagsUnion        (ThreadData * data) ;
    virtual void TagsIntersection (ThreadData * data) ;

  private:

  public slots:

    virtual bool RunMenuResult    (QAction * action,MenuManager & menu,QPoint pos) ;

  protected slots:

    virtual void SendPending      (void) ;

  private slots:

  signals:

    void TagSelected              (QString name,SUID    uuid ) ;
    void ListPeople               (QString name,UUIDs & Uuids) ;

} ;

class Q_HUMAN_EXPORT PeopleReporter : public TextEdit
                                    , public Object
                                    , public GroupItems
                                    , public PeopleManager
                                    , public PictureManager
{
  Q_OBJECT
  public:

    explicit PeopleReporter         (StandardConstructor) ;
    virtual ~PeopleReporter         (void) ;

    virtual QSize sizeHint          (void) const ;

  protected:

    QByteArray HTML ;

    virtual void Configure          (void) ;

    virtual void run                (int type,ThreadData * data) ;

    bool            Variable        (SqlConnection & SC,SUID uuid,QString name,QString & result) ;
    virtual QString Introduction    (SqlConnection & SC,QStringList & names) ;
    virtual QString DisplayName     (QString TD,QStringList & names) ;
    virtual QString DisplayNames    (SqlConnection & SC) ;
    virtual QString DisplayIcon     (SqlConnection & SC) ;
    virtual QString DisplayBirth    (SqlConnection & SC,QString TD) ;
    virtual QString setAge          (QString TD,QDateTime dt) ;
    virtual QString setSexuality    (SqlConnection & SC,QString TD) ;
    virtual QString DisplayFeatures (SqlConnection & SC) ;
    virtual QString DisplayBookmark (SqlConnection & SC) ;
    virtual QString DisplayVideos   (SqlConnection & SC) ;
    virtual QString DisplayPictures (SqlConnection & SC) ;
    virtual QString DisplayFaces    (SqlConnection & SC) ;

    virtual void Append             (QString html) ;

  private:

  public slots:

    virtual void startup            (SUID people) ;
    virtual void Report             (void) ;

  protected slots:

    virtual bool Menu               (QPoint pos) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT PeopleDispatcher : public QObject
                                      , public SyntaxDispatcher
                                      , public UuidSyntax
{
  Q_OBJECT
  public:

    explicit PeopleDispatcher        (QObject * parent) ;
    virtual ~PeopleDispatcher        (void) ;

    virtual int         type         (void) const ; // CiosMenuId ( 78 , 0 , 1 )
    virtual int         matching     (QString command) ;
    virtual bool        execute      (int id) ;
    virtual void        setExtras    (void * data) ;
    virtual void        Help         (QStringList & help) ;

  protected:

    Plan              * plan       ;
    WMAPs               Variables  ;
    QMap<QString,UUIDs> ListQueues ;
    QStringList         Paddings   ;
    QMap<QString,UUIDs> Found      ;

    virtual bool        FilterType   (SqlConnection & SC,int type,UUIDs & U,UUIDs & P) ;
    virtual bool        NamesOwners  (SqlConnection & SC,UUIDs & U,UUIDs & P) ;
    virtual bool        FetchPeople  (SqlConnection & SC,UUIDs & U,QString Key) ;
    virtual bool        FetchByTag   (SqlConnection & SC,UUIDs & U,QString Key) ;

  private:

  public slots:

    virtual void SearchPeople        (void) ;
    virtual void SearchNames         (void) ;
    virtual void SearchTags          (void) ;
    virtual void setVariable         (void) ;
    virtual void showVariables       (void) ;

  protected slots:

  private slots:

    void SendPeople                  (void) ;

  signals:

    void ViewPeople                  (void) ;
    void ListPeople                  (QString name,UUIDs & Uuids) ;

} ;

class Q_HUMAN_EXPORT PeopleProcessor : public InteractiveEditor
                                     , public Object
{
  Q_OBJECT
  public:

    explicit     PeopleProcessor (StandardConstructor) ;
    virtual     ~PeopleProcessor (void) ;

  protected:

    virtual void Configure       (void) ;

    virtual bool Interpret       (QString command) ;
    virtual void run             (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual void Enter           (void) ;
    virtual void Enter           (QString cmd) ;
    virtual void Help            (void) ;

  protected slots:

  private slots:

  signals:

    void Quit                    (void) ;

} ;

class Q_HUMAN_EXPORT SurnameOrganizer : public TreeWidget
{
  Q_OBJECT
  public:

    explicit SurnameOrganizer    (StandardConstructor) ;
    virtual ~SurnameOrganizer    (void);

    virtual QSize sizeHint       (void) const ;

  protected:

    QComboBox * SearchText ;

    virtual bool FocusIn         (void) ;
    virtual void Configure       (void) ;
    virtual void run             (void) ;

  private:

  public slots:

    virtual bool List            (void) ;
    virtual void DeleteSurname   (void) ;
    virtual void Guess           (void) ;
    virtual void Search          (void) ;
    virtual void Search          (QString text) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;

    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    virtual void selectLanguage  (QTreeWidgetItem * item,int column) ;
    virtual void selectSurname   (QTreeWidgetItem * item,int column) ;

    virtual void removeOldItem   (void) ;
    virtual void editingFinished (void) ;
    virtual void surnameChanged  (int index) ;
    virtual void languageChanged (int index) ;

    virtual void SearchReady      (void) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT SurnameEditor : public TreeDock
{
  Q_OBJECT
  public:

    explicit SurnameEditor               (StandardConstructor) ;
    virtual ~SurnameEditor               (void);

    virtual QSize sizeHint               (void) const ;

  protected:

    int         Method       ;
    int         OnlyLanguage ;
    bool        dropAction   ;
    QPoint      dragPoint    ;
    QComboBox * SearchText   ;

    virtual void Configure               (void) ;
    virtual bool FocusIn                 (void) ;

    virtual bool        hasItem          (void) ;
    virtual bool        startDrag        (QMouseEvent * event) ;
    virtual bool        fetchDrag        (QMouseEvent * event) ;
    virtual QMimeData * dragMime         (void) ;
    virtual void        dragDone         (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag       (QMouseEvent * event) ;

    virtual UUIDs LoadUuids              (SqlConnection & Connection) ;
    virtual void  LoadNames              (SqlConnection & Connection,UUIDs & Uuids) ;
    virtual QTreeWidgetItem * AppendItem (void) ;

    virtual void run                     (void) ;

    void Load                            (void) ;
    void Items                           (void) ;

  private:

  public slots:

    virtual bool List            (void) ;
    virtual void New             (void) ;
    virtual void Paste           (void) ;
    virtual void Location        (void) ;
    virtual void Search          (void) ;
    virtual void Search          (QString text) ;
    virtual void Rename          (void) ;
    virtual void AssignLanguage  (void) ;
    virtual void ImportMappings  (void) ;
    virtual void Statistics      (void) ;
    virtual void CopyToClipboard (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void editName        (QTreeWidgetItem * item,int column) ;
    virtual void selectLanguage  (QTreeWidgetItem * item,int column) ;

    virtual void SearchReady     (void) ;

    void removeOldItem           (void) ;
    void editingFinished         (void) ;
    void languageChanged         (int index) ;

  private slots:

  signals:

    void Translations            (QString name,SUID surname) ;
    void Documents               (QString name,SUID surname) ;
    void People                  (QString name,SUID surname,UUIDs people) ;

};

class Q_HUMAN_EXPORT SurnameTranslations : public TreeWidget
                                         , public Object
                                         , public Relation
                                         , public PeopleManager
{
  Q_OBJECT
  public:

    explicit SurnameTranslations (StandardConstructor) ;
    virtual ~SurnameTranslations (void) ;

    virtual QSize sizeHint       (void) const ;

  protected:

    bool   dropAction   ;
    QPoint dragPoint    ;
    UUIDs  suruuids     ;
    NAMEs  surnames     ;
    RMAPs  surlanguages ;

    virtual void Configure         (void) ;
    virtual bool FocusIn           (void) ;

    virtual bool        hasItem    (void) ;
    virtual bool        startDrag  (QMouseEvent * event) ;
    virtual bool        fetchDrag  (QMouseEvent * event) ;
    virtual QMimeData * dragMime   (void) ;
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag (QMouseEvent * event) ;

    virtual bool acceptDrop        (QWidget * source,const QMimeData * mime);
    virtual bool dropNew           (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropMoving        (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropAppend        (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual bool acceptPrivate     (const QMimeData * mime) ;
    virtual bool dropPrivate       (nDeclWidget,const QMimeData * mime,QPointF pos) ;

  private:

  public slots:

    virtual bool startup           (void) ;
    virtual void LoadSurnames      (void) ;
    virtual void LoadEquals        (void) ;
    virtual void New               (void) ;
    virtual void Detach            (void) ;
    virtual void DetachOneSide     (void) ;
    virtual void CopyToClipboard   (void) ;

    virtual void append            (UUIDs & Uuids) ;
    virtual void append            (UUIDs & Uuids,QPointF pos) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;
    virtual void singleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

    void editingFinished           (void) ;
    void languageChanged           (int index) ;
    void removeOldItem             (void) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT SexualityTree : public TreeDock
                                   , public PeopleManager
{
  Q_OBJECT
  public:

    explicit            SexualityTree (StandardConstructor) ;
    virtual            ~SexualityTree (void);

    QString             Gender        (SUID uuid) ;

  protected:

    virtual void        Configure     (void) ;
    virtual bool        FocusIn       (void) ;

    virtual QString     MimeType      (const QMimeData * mime);
    virtual UUIDs       MimeUuids     (const QMimeData * mime,QString mimetype) ;

    virtual QMimeData * dragMime      (void);

    virtual bool        acceptDrop    (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew       (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving    (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend    (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        removeDrop    (void) ;

  private:

  public slots:

    virtual void        List          (void) ;
    virtual void        Export        (void) ;
    virtual void        GetDocuments  (QTreeWidgetItem * item) ;
    virtual void        GetPeople     (QTreeWidgetItem * item) ;
    virtual void        JoinSexuality (QString name,SUID sexuality,UUIDs & People) ;

  protected slots:

    virtual bool        Menu          (QPoint pos) ;

  private slots:

  signals:

    void                ListPeople    (QString name,SUID uuid,UUIDs Uuids) ;
    void                Documents     (QString name,SUID uuid) ;

};

class Q_HUMAN_EXPORT SexualityList : public ComboBox
                                   , public PeopleManager
{
  Q_OBJECT
  public:

    explicit     SexualityList (StandardConstructor) ;
    virtual     ~SexualityList (void);

  protected:

    virtual void Configure     (void) ;

  private:

  public slots:

    virtual void List          (void) ;

  protected slots:

  private slots:

  signals:

};

class Q_HUMAN_EXPORT FaceScanner : public QThread
                                 , public PictureManager
                                 , public PictureRecognizer
{
  Q_OBJECT
  public:

    int            Mode     ;
    int            Chunk    ;

    explicit FaceScanner   (QObject * parent = NULL,Plan * plan = NULL) ;
    virtual ~FaceScanner   (void) ;

  protected:

    int    ProgressID    ;
    qint64 ProgressValue ;
    bool   ProgressGo    ;

    bool     FaceExists    (SqlConnection & Connection,SUID uuid,QRect Rect) ;
    bool     InsertFace    (SqlConnection & Connection,SUID uuid,FaceParaments & face) ;
    bool     FaceParament  (SqlConnection & Connection,SUID uuid,QString name,QList<QRect> Rects) ;
    SUID     AnalyzeFace   (SqlConnection & Connection,UUIDs & People,SUID uuid,QSize iSize,FaceParaments & face) ;
    void     run           (void) ;

    void     DetectFace    (SqlConnection & Connection,SUID uuid,IplImage * image) ;
    void     DetectDetail  (SqlConnection & Connection,SUID uuid,QSize iSize,IplImage * image) ;

  private:

  public slots:

    void Launch            (int mode = 0) ;
    void DetectFaces       (void) ;
    void DetectDetails     (void) ;

  protected slots:

  private slots:

  signals:

    void Finished         (void) ;

};

class Q_HUMAN_EXPORT FaceView : public ListDock
                              , public PictureManager
{
  Q_OBJECT
  public:

    SUID  Group        ;
    int   SourceType   ;
    int   Relationship ;
    int   Operation    ;
    int   StartId      ;
    int   Limit        ;
    UUIDs Fuids        ;

    explicit FaceView              (StandardConstructor) ;
    virtual ~FaceView              (void) ;

    virtual int Total              (void) ;

  protected:

    bool            loadingFaces ;
    int             Method       ;
    int             TotalFaces   ;
    CUIDs           KeyIDs       ;
    bool            dropAction   ;
    QPoint          dragPoint    ;
    ListWidgetItems Items        ;

    virtual bool        FocusIn    (void) ;
    virtual bool        FocusOut   (void) ;
    virtual void        Configure  (void) ;
    virtual void        run        (void) ;

    virtual UUIDs       MimeUuids  (const QMimeData * mime,QString mimetype) ;

    virtual bool        hasItem    (void);
    virtual bool        startDrag  (QMouseEvent * event);
    virtual bool        fetchDrag  (QMouseEvent * event);
    virtual QMimeData * dragMime   (void);
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag (QMouseEvent * event);

    virtual bool        acceptDrop (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew    (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool        dropMoving (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool        dropAppend (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool        removeDrop (void);

    virtual bool        dropPeople (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropFaces  (nDeclDrops,const UUIDs & Uuids) ;

    UUIDs        FaceLimits        (SqlConnection & Connection) ;
    UUIDs        FaceGroups        (SqlConnection & Connection) ;
    UUIDs        FailFaces         (SqlConnection & Connection) ;
    UUIDs        FaceUuids         (SqlConnection & Connection) ;
    virtual int  CountFaces        (SqlConnection & SC) ;
    virtual void MakeFaces         (UUIDs & Uuids,QMap<SUID,QListWidgetItem *> & Items) ;
    QListWidgetItem * uuidAt       (SUID uuid) ;

  private:

    bool FetchFaces                                  (
           SqlConnection                & Connection ,
           UUIDs                        & Uuids      ,
           QMap<SUID,QListWidgetItem *> & Items    ) ;

  public slots:

    virtual bool startup           (void) ;
    virtual bool List              (UUIDs & Uuids) ;

    virtual void ListFaces         (void) ;
    virtual void ListUuids         (void) ;

    virtual void Home              (void) ;
    virtual void End               (void) ;
    virtual void PageUp            (void) ;
    virtual void PageDown          (void) ;

    virtual void DeleteFaces       (void) ;
    virtual void JoinBadFaces      (void) ;
    virtual void AwayBadFaces      (void) ;

    virtual bool assignPeople      (SUID uuid,const UUIDs & people) ;
    virtual bool MoveFaces         (SUID uuid,const UUIDs & faces) ;
    virtual bool JoinFaces         (SUID uuid,const UUIDs & faces) ;
    virtual void Relocate          (void) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;

    virtual void setItemIcon       (QImage * image,QListWidgetItem * item) ;

  private slots:

  signals:

    void setIcon     (QImage * image,QListWidgetItem * item) ;
    void ViewPicture (SUID uuid) ;
    void ViewPicture (QString title,SUID group,int type,SUID uuid) ;
    void Features    (SUID people,UUIDs & Uuids) ;
    void Model       (SUID people,UUIDs & Uuids) ;

};

class Q_HUMAN_EXPORT PeopleBirthdays : public TreeDock
                                     , public PeopleManager
                                     , public HistoryManager
{
  Q_OBJECT
  public:

    explicit PeopleBirthdays    (StandardConstructor) ;
    virtual ~PeopleBirthdays    (void);

    virtual QSize sizeHint      (void) const ;

  protected:

    virtual void Configure      (void) ;
    virtual bool FocusIn        (void) ;

    virtual void run            (int type,ThreadData * data) ;

    virtual void setDateTime    (QTreeWidgetItem * item,QDateTime dt) ;

  private:

  public slots:

    virtual bool List           (void) ;
    virtual bool startup        (void) ;

  protected slots:

    virtual bool Menu           (QPoint pos) ;
    virtual void timeChanged    (void) ;
    virtual void doubleClicked  (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT PeopleSexuality : public TreeDock
                                     , public PeopleManager
{
  Q_OBJECT
  public:

    explicit PeopleSexuality      (StandardConstructor) ;
    virtual ~PeopleSexuality      (void);

    virtual QSize sizeHint        (void) const ;

  protected:

    NAMEs Sexualities ;

    virtual bool FocusIn          (void) ;
    virtual void Configure        (void) ;
    virtual void run              (void) ;

  private:

  public slots:

    virtual bool List             (void) ;

  protected slots:

    virtual void sexualityChanged (int index) ;
    virtual void doubleClicked    (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT EyesView : public ListDock
                              , public PeopleManager
{
  Q_OBJECT
  public:

    explicit EyesView                (StandardConstructor) ;
    virtual ~EyesView                (void);

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void Configure           (void) ;
    virtual bool FocusIn             (void) ;

    virtual bool        hasItem      (void);
    virtual bool        startDrag    (QMouseEvent * event);
    virtual bool        fetchDrag    (QMouseEvent * event);
    virtual QMimeData * dragMime     (void);
    virtual void        dragDone     (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag   (QMouseEvent * event);

    virtual QString     MimeType     (const QMimeData * mime);
    virtual UUIDs       MimeUuids    (const QMimeData * mime,QString mimetype) ;

    virtual bool        acceptDrop   (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving   (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend   (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual bool        dropTags     (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPictures (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPeople   (nDeclDrops,const UUIDs       & Uuids ) ;

  private:

  public slots:

    virtual bool startup             (void) ;
    virtual void Copy                (void) ;

  protected slots:

    virtual bool Menu                (QPoint pos) ;

  private slots:

  signals:

    void         People              (SUID uuid,QString name) ;
    void         Gallery             (QString name,SUID uuid) ;

};

class Q_HUMAN_EXPORT EyesList : public ComboBox
                              , public PeopleManager
{
  Q_OBJECT
  public:

    explicit     EyesList  (StandardConstructor) ;
    virtual     ~EyesList  (void);

  protected:

    virtual void Configure (void) ;

  private:

  public slots:

    virtual void List      (void) ;

  protected slots:

  private slots:

  signals:

};

class Q_HUMAN_EXPORT HairsView : public ListDock
                               , public PeopleManager
{
  Q_OBJECT
  public:

    explicit HairsView               (StandardConstructor) ;
    virtual ~HairsView               (void) ;

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void Configure           (void) ;
    virtual bool FocusIn             (void) ;

    virtual bool        hasItem      (void) ;
    virtual bool        startDrag    (QMouseEvent * event) ;
    virtual bool        fetchDrag    (QMouseEvent * event) ;
    virtual QMimeData * dragMime     (void) ;
    virtual void        dragDone     (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag   (QMouseEvent * event) ;

    virtual QString     MimeType     (const QMimeData * mime);
    virtual UUIDs       MimeUuids    (const QMimeData * mime,QString mimetype) ;

    virtual bool        acceptDrop   (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving   (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend   (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual bool        dropTags     (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPictures (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPeople   (nDeclDrops,const UUIDs       & Uuids ) ;

  private:

  public slots:

    virtual bool startup             (void) ;
    virtual void Copy                (void) ;

  protected slots:

    virtual bool Menu                (QPoint pos) ;

  private slots:

  signals:

    void         People              (SUID uuid,QString name) ;
    void         Gallery             (QString name,SUID uuid) ;

};

class Q_HUMAN_EXPORT HairsList : public ComboBox
                               , public PeopleManager
{
  Q_OBJECT
  public:

    explicit     HairsList (StandardConstructor) ;
    virtual     ~HairsList (void);

  protected:

    virtual void Configure (void) ;

  private:

  public slots:

    virtual void List      (void) ;

  protected slots:

  private slots:

  signals:

};

class Q_HUMAN_EXPORT OrganLists : public TreeDock
{
  Q_OBJECT
  public:

    explicit OrganLists          (StandardConstructor) ;
    virtual ~OrganLists          (void) ;

    virtual QSize sizeHint       (void) const ;

  protected:

    enum Qt::SortOrder sorting ;

    virtual bool  FocusIn        (void) ;
    virtual void  Configure      (void) ;

    virtual bool  acceptDrop     (QWidget * source,const QMimeData * mime);
    virtual bool  dropText       (QWidget * source,QPointF pos,const QString & text);

    virtual UUIDs LoadUuids      (SqlConnection & connection) ;
    virtual void  LoadNames      (SqlConnection & connection,UUIDs & Uuids) ;
    virtual SUID  AppendUuid     (SqlConnection & connection);
    virtual bool  AppendName     (SqlConnection & connection,SUID uuid,QString name);
    virtual QTreeWidgetItem * AppendItem (void) ;

    virtual void  setCheckable   (QTreeWidgetItem * item) ;

  private:

  public slots:

    virtual bool List            (void) ;
    virtual void New             (void) ;
    virtual void Paste           (void) ;
    virtual void Paste           (QString text);
    virtual void Export          (void) ;
    virtual void Copy            (void) ;
    virtual void Translation     (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    void removeOldItem           (void) ;
    void editingFinished         (void) ;

  private slots:

  signals:

    void Update                  (nDeclWidget,SUID uuid) ;
    void Translations            (QString title,UUIDs & Uuids);
    void BodyParts               (QString name,SUID organ) ;

};

class Q_HUMAN_EXPORT BodyParts : public TreeDock
{
  Q_OBJECT
  public:

    SUID organ ;

    explicit BodyParts           (StandardConstructor) ;
    virtual ~BodyParts           (void) ;

    virtual QSize sizeHint       (void) const ;

  protected:

    enum Qt::SortOrder sorting ;

    virtual bool  FocusIn        (void) ;
    virtual void  Configure      (void) ;

    virtual bool  acceptDrop     (QWidget * source,const QMimeData * mime);
    virtual bool  dropText       (QWidget * source,QPointF pos,const QString & text);

    virtual UUIDs LoadUuids      (SqlConnection & connection) ;
    virtual void  LoadNames      (SqlConnection & connection,UUIDs & Uuids) ;
    virtual SUID  AppendUuid     (SqlConnection & connection);
    virtual bool  AppendName     (SqlConnection & connection,SUID uuid,QString name);
    virtual QTreeWidgetItem * AppendItem (void) ;

    virtual void  setCheckable   (QTreeWidgetItem * item) ;

  private:

  public slots:

    virtual bool List            (void) ;
    virtual void New             (void) ;
    virtual void Paste           (void) ;
    virtual void Paste           (QString text);
    virtual void Export          (void) ;
    virtual void Copy            (void) ;
    virtual void Translation     (void) ;
    virtual void setHeader       (QString title,QString head) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    void removeOldItem           (void) ;
    void editingFinished         (void) ;

  private slots:

  signals:

    void Update                  (nDeclWidget,SUID uuid) ;
    void Translations            (QString title,UUIDs & Uuids);

};

class Q_HUMAN_EXPORT AcupunctureLists : public TreeDock
{
  Q_OBJECT
  public:

    explicit AcupunctureLists    (StandardConstructor) ;
    virtual ~AcupunctureLists    (void) ;

    virtual QSize sizeHint       (void) const ;

  protected:

    enum Qt::SortOrder sorting ;

    virtual bool  FocusIn        (void) ;
    virtual void  Configure      (void) ;

    virtual bool  acceptDrop     (QWidget * source,const QMimeData * mime);
    virtual bool  dropText       (QWidget * source,QPointF pos,const QString & text);

    virtual UUIDs LoadUuids      (SqlConnection & connection) ;
    virtual void  LoadNames      (SqlConnection & connection,UUIDs & Uuids) ;
    virtual SUID  AppendUuid     (SqlConnection & connection);
    virtual bool  AppendName     (SqlConnection & connection,SUID uuid,QString name);
    virtual QTreeWidgetItem * AppendItem (void) ;

    virtual void  setCheckable   (QTreeWidgetItem * item) ;

  private:

  public slots:

    virtual bool List            (void) ;
    virtual void New             (void) ;
    virtual void Paste           (void) ;
    virtual void Paste           (QString text);
    virtual void Export          (void) ;
    virtual void Copy            (void) ;
    virtual void Translation     (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    void removeOldItem           (void) ;
    void editingFinished         (void) ;

  private slots:

  signals:

    void Update                  (nDeclWidget,SUID uuid) ;
    void Translations            (QString title,UUIDs & Uuids);

};

class Q_HUMAN_EXPORT BodyLists : public TreeDock
{
  Q_OBJECT
  public:

    explicit BodyLists           (StandardConstructor) ;
    virtual ~BodyLists           (void) ;

    virtual QSize sizeHint       (void) const ;

  protected:

    enum Qt::SortOrder sorting ;

    virtual bool  FocusIn        (void) ;
    virtual void  Configure      (void) ;

    virtual bool  acceptDrop     (QWidget * source,const QMimeData * mime);
    virtual bool  dropText       (QWidget * source,QPointF pos,const QString & text);

    virtual UUIDs LoadUuids      (SqlConnection & connection) ;
    virtual void  LoadNames      (SqlConnection & connection,UUIDs & Uuids) ;
    virtual SUID  AppendUuid     (SqlConnection & connection);
    virtual bool  AppendName     (SqlConnection & connection,SUID uuid,QString name);
    virtual QTreeWidgetItem * AppendItem (void) ;

    virtual void  setCheckable   (QTreeWidgetItem * item) ;

  private:

  public slots:

    virtual bool List            (void) ;
    virtual void New             (void) ;
    virtual void Paste           (void) ;
    virtual void Paste           (QString text);
    virtual void Export          (void) ;
    virtual void Copy            (void) ;
    virtual void Translation     (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    void removeOldItem           (void) ;
    void editingFinished         (void) ;
    void organChanged            (int index) ;

  private slots:

  signals:

    void Update                  (nDeclWidget,SUID uuid) ;
    void Translations            (QString title,UUIDs & Uuids);

};

class Q_HUMAN_EXPORT Consanguinities : public TreeDock
{
  Q_OBJECT
  public:

    explicit Consanguinities    (StandardConstructor) ;
    virtual ~Consanguinities    (void) ;

  protected:

    NAMEs BloodRelations ;

    virtual void Configure      (void) ;
    virtual bool FocusIn        (void) ;

  private:

  public slots:

    virtual bool startup        (void) ;
    virtual void Export         (void) ;

  protected slots:

    virtual bool Menu           (QPoint pos) ;
    virtual void doubleClicked  (QTreeWidgetItem * item,int column) ;
    virtual void nameFinished   (void) ;

  private slots:

  signals:

};

class Q_HUMAN_EXPORT RaceLists : public ItemEditor
{
  Q_OBJECT
  public:

    explicit RaceLists     (StandardConstructor) ;
    virtual ~RaceLists     (void) ;

  protected:

    virtual void Configure (void) ;

  private:

  public slots:

  protected slots:

  private slots:

  signals:

};

class Q_HUMAN_EXPORT EyesShapes : public ListDock
                                , public PeopleManager
{
  Q_OBJECT
  public:

    explicit EyesShapes              (StandardConstructor) ;
    virtual ~EyesShapes              (void) ;

    virtual QSize sizeHint           (void) const ;

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void Configure           (void) ;
    virtual bool FocusIn             (void) ;

    virtual bool        hasItem      (void);
    virtual bool        startDrag    (QMouseEvent * event);
    virtual bool        fetchDrag    (QMouseEvent * event);
    virtual QMimeData * dragMime     (void);
    virtual void        dragDone     (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag   (QMouseEvent * event);

    virtual QString     MimeType     (const QMimeData * mime);
    virtual UUIDs       MimeUuids    (const QMimeData * mime,QString mimetype) ;

    virtual bool        acceptDrop   (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving   (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend   (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual bool        dropPictures (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPeople   (nDeclDrops,const UUIDs       & Uuids ) ;

  private:

  public slots:

    virtual bool startup             (void) ;
    virtual void New                 (void) ;
    virtual void Copy                (void) ;

  protected slots:

    virtual void editingFinished     (void) ;
    virtual bool Menu                (QPoint pos) ;

  private slots:

  signals:

    void         ShapeEquation       (SUID uuid,int type,QString name) ;
    void         People              (SUID uuid,int type,QString name) ;
    void         Gallery             (QString name,SUID uuid,int type) ;
    void         Translations        (QString title,UUIDs & Uuids);

};

class Q_HUMAN_EXPORT FaceEquation : public Object
{
  public:

    QString           Name          ;
    CUIDs             Sequences     ;
    QMap<int,QPointF> Points        ;
    IMAPs             ControlFirst  ;
    IMAPs             ControlSecond ;
    CMAPs             ControlUuids  ;
    EMAPs             ControlNames  ;

    explicit FaceEquation    (void) ;
    explicit FaceEquation    (SUID uuid) ;
    virtual ~FaceEquation    (void) ;

    void     StandardFace    (void) ;
    void     Reposition      (void) ;
    bool     Generate        (QByteArray & body) ;
    bool     Obtain          (QByteArray & body) ;
    bool     load            (PurePlan * plan,SqlConnection & Connection) ;
    bool     save            (PurePlan * plan,SqlConnection & Connection) ;
    bool     load            (PurePlan * plan) ;
    bool     save            (PurePlan * plan) ;

    QPointF  Quadratic       (double    t   ,
                              QPointF & P1  ,
                              QPointF & P2  ,
                              QPointF & P3) ;

    bool     Shape           (QPolygonF & face) ;

    QPointF  EyesCenter      (void) ;
    double   EyeLength       (void) ;
    double   CenterLength    (void) ; // Mouth and Eyes center
    double   EyeFactor       (QPointF Left,QPointF Right) ;
    double   NoseFactor      (QPointF Left,QPointF Right,QPointF Mouth) ;

    void     CleanIsolations (void) ;
    void     DoubleUp        (void) ;
    void     Dismantle       (int point) ;
    void     Delete          (int point) ;

  protected:

  private:

};

class Q_HUMAN_EXPORT FaceEquations : public QList<FaceEquation>
{
  public:

    explicit FaceEquations (void) ;
    virtual ~FaceEquations (void) ;

    bool     load         (PurePlan * plan,SqlConnection & Connection) ;
    bool     save         (PurePlan * plan,SqlConnection & Connection) ;
    bool     load         (PurePlan * plan) ;
    bool     save         (PurePlan * plan) ;

  protected:

  private:

};

class Q_HUMAN_EXPORT FaceShape : public Widget
                               , public Object
{
  Q_OBJECT
  public:

    FaceEquation equation ;

    explicit FaceShape                 (StandardConstructor) ;
    virtual ~FaceShape                 (void) ;

    virtual QSize sizeHint             (void) const ;

  protected:

    QImage   * image       ;
    double     ImageX      ;
    double     ImageY      ;
    double     ImageFactor ;
    double     Factor      ;
    BMAPs      Properties  ;
    VcfWidgets Widgets     ;

    virtual void Configure             (void) ;
    virtual bool FocusIn               (void) ;

    virtual void focusInEvent          (QFocusEvent       * event) ;
    virtual void focusOutEvent         (QFocusEvent       * event) ;
    virtual void closeEvent            (QCloseEvent       * event) ;
    virtual void paintEvent            (QPaintEvent       * event) ;
    virtual void resizeEvent           (QResizeEvent      * event) ;
    virtual void contextMenuEvent      (QContextMenuEvent * event) ;
    virtual void mouseDoubleClickEvent (QMouseEvent       * event) ;
    virtual void mouseMoveEvent        (QMouseEvent       * event) ;
    virtual void mousePressEvent       (QMouseEvent       * event) ;
    virtual void mouseReleaseEvent     (QMouseEvent       * event) ;

    virtual void PaintFace             (QPainter & painter) ;
    virtual void PaintAll              (QPainter & painter) ;

  private:

    QPointF      Quadratic             (double    t   ,
                                        QPointF & P1  ,
                                        QPointF & P2  ,
                                        QPointF & P3) ;
    int          Nearby                (QPoint pos) ;

  public slots:

    virtual bool startup               (void) ;
    virtual void relocation            (void) ;
    virtual void background            (void) ;
    virtual void Export                (void) ;
    virtual void Save                  (void) ;

    virtual void CleanIsolations       (void) ;
    virtual void EditPoints            (void) ;

  protected slots:

    virtual bool Menu                  (QPoint pos) ;

    virtual void Changed               (int id,QWidget * widget) ;
    virtual void Closed                (int id,QWidget * widget) ;

  private slots:

  signals:

    void Leave                         (QWidget * widget) ;

};

class Q_HUMAN_EXPORT FaceShapes : public ListDock
                                , public PeopleManager
{
  Q_OBJECT
  public:

    explicit FaceShapes              (StandardConstructor) ;
    virtual ~FaceShapes              (void);

    virtual QSize sizeHint           (void) const ;

  protected:

    bool     Commanding      ;
    QTimer * CommandSequence ;
    QPoint   dragPoint       ;
    bool     dropAction      ;

    virtual void Configure           (void) ;
    virtual bool FocusIn             (void) ;

    virtual bool        hasItem      (void);
    virtual bool        startDrag    (QMouseEvent * event);
    virtual bool        fetchDrag    (QMouseEvent * event);
    virtual QMimeData * dragMime     (void);
    virtual void        dragDone     (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag   (QMouseEvent * event);

    virtual QString     MimeType     (const QMimeData * mime);
    virtual UUIDs       MimeUuids    (const QMimeData * mime,QString mimetype) ;

    virtual bool        acceptDrop   (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving   (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend   (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual bool        dropPictures (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPeople   (nDeclDrops,const UUIDs       & Uuids ) ;

    virtual void        run          (int Type,ThreadData * data) ;
    virtual void        FetchUUIDs   (void) ;
    virtual void        ListIcons    (UUIDs & Uuids) ;
    virtual void        FetchIcons   (void) ;

  private:

  public slots:

    virtual bool startup             (void) ;
    virtual void List                (void) ;
    virtual void New                 (void) ;
    virtual void Copy                (void) ;

    virtual bool RunCommand          (VarArgs & arguments) ;

  protected slots:

    virtual void StartCommand        (void) ;
    virtual void CommandHandler      (void) ;
    virtual void editingFinished     (void) ;
    virtual bool Menu                (QPoint pos) ;

  private slots:

    void         setItemIcon         (QImage * image,QListWidgetItem * item) ;

  signals:

    void         setIcon             (QImage * image,QListWidgetItem * item) ;
    void         ShapeEquation       (SUID uuid,int type,QString name) ;
    void         People              (SUID uuid,int type,QString name) ;
    void         Gallery             (QString name,SUID uuid,int type) ;
    void         Translations        (QString title,UUIDs & Uuids);
    void         listItems           (ListWidgetItems & items) ;
    void         TriggerCommand      (void) ;

};

class Q_HUMAN_EXPORT EmotionLists : public ItemEditor
{
  Q_OBJECT
  public:

    explicit EmotionLists  (StandardConstructor) ;
    virtual ~EmotionLists  (void) ;

  protected:

    virtual void Configure (void) ;

  private:

  public slots:

  protected slots:

  private slots:

  signals:

};

/*****************************************************************************
 *                                                                           *
 *                              Linguistic widgets                           *
 *                                                                           *
 *****************************************************************************/

class Q_HUMAN_EXPORT PhonemeItems : public TreeDock
                                  , public Group
                                  , public GroupItems
{
  Q_OBJECT
  public:

    explicit PhonemeItems      (StandardConstructor) ;
    virtual ~PhonemeItems      (void) ;

    virtual QSize sizeHint     (void) const ;

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual bool FocusIn       (void) ;
    virtual void Configure     (void) ;

    virtual bool hasItem       (void) ;
    virtual bool startDrag     (QMouseEvent * event) ;
    virtual bool fetchDrag     (QMouseEvent * event) ;
    virtual QMimeData * dragMime (void) ;
    virtual void dragDone      (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool finishDrag    (QMouseEvent * event) ;

    virtual bool acceptDrop    (QWidget * source,const QMimeData * mime) ;
    virtual bool dropNew       (QWidget * source,const QMimeData * mime,QPoint pos) ;

    virtual bool dropPhonemes  (QWidget * widget,QPointF pos,const UUIDs & Uuids) ;

    virtual void run           (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void List          (void) ;
    virtual void New           (void) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;

    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

    virtual void nameChanged   (void) ;
    virtual void signChanged   (void) ;
    virtual void flagsChanged  (void) ;
    virtual void spinChanged   (void) ;

  private slots:

  signals:

    void Edit                  (QString name,SUID uuid) ;
    void Translations          (QString title,UUIDs & Uuids) ;

};

class Q_HUMAN_EXPORT PhonemeGroups : public TreeDock
                                   , public GroupItems
{
  Q_OBJECT
  public:

    explicit PhonemeGroups     (StandardConstructor) ;
    virtual ~PhonemeGroups     (void) ;

    virtual QSize sizeHint     (void) const ;

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual bool FocusIn       (void) ;
    virtual void Configure     (void) ;

    virtual bool acceptDrop    (QWidget * source,const QMimeData * mime) ;
    virtual bool dropNew       (QWidget * source,const QMimeData * mime,QPoint pos) ;

    virtual bool dropPhonemes  (QWidget * widget,QPointF pos,const UUIDs & Uuids) ;

    virtual void run           (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void List          (void) ;
    virtual void New           (void) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;

    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

    virtual void nameChanged   (void) ;
    virtual void identChanged  (void) ;
    virtual void langChanged   (int index) ;

  private slots:

  signals:

    void Edit                  (QString name,SUID uuid) ;
    void Translations          (QString title,UUIDs & Uuids) ;

};

class Q_HUMAN_EXPORT CrowdsShelf : public ViewShelf
{
  Q_OBJECT
  public:

    explicit     CrowdsShelf        (QObject * parent,Plan * plan) ;
    explicit     CrowdsShelf        (QObject * parent,Plan * plan,SceneDestructor * destructor) ;
    virtual     ~CrowdsShelf        (void) ;

    virtual bool Menu               (QWidget      * widget ,
                                     QPointF        pos    ,
                                     ControlPoint & From   ,
                                     ControlPoint & To   ) ;

    virtual bool Relocation         (QRectF viewport) ;

  protected:

    virtual void Configure          (void) ;

    virtual bool FocusIn            (void) ;
    virtual bool ItemRunner         (int type,ThreadData * data) ;

    virtual bool hasItem            (ControlPoint & From ,
                                     ControlPoint & To   ,
                                     ControlPoint & At ) ;
    virtual QMimeData * dragMime    (ControlPoint & From ,
                                     ControlPoint & To   ,
                                     ControlPoint & At ) ;
    virtual bool dropPeople         (QWidget     * widget  ,
                                     ControlPoint  pos     ,
                                     const UUIDs & Uuids ) ;
    virtual bool dropPictures       (QWidget     * widget  ,
                                     ControlPoint  pos     ,
                                     const UUIDs & Uuids ) ;
    virtual bool dropDivisions      (QWidget     * widget  ,
                                     ControlPoint  pos     ,
                                     const UUIDs & Uuids ) ;

    virtual void CrowdConstraints   (VarArgs & args) ;
    virtual void CrowdRules         (VarArgs & args) ;

    virtual void AllPeople          (void) ;

  private:

  public slots:

    virtual void SelectNothing      (void) ;
    virtual void SelectAll          (void) ;
    virtual void NewCrowd           (void) ;

    virtual int  Indexing           (RangeBar * rbu,int direction,QVariants & v) ;

  protected slots:

    virtual void atDoubleClicked    (SelectItem * item) ;

  private slots:

  signals:

    void         ShowAll            (void) ;

} ;

class Q_HUMAN_EXPORT PeopleShelf : public ViewShelf
{
  Q_OBJECT
  public:

    explicit     PeopleShelf         (QObject * parent,Plan * plan) ;
    explicit     PeopleShelf         (QObject * parent,Plan * plan,SceneDestructor * destructor) ;
    virtual     ~PeopleShelf         (void) ;

    virtual bool Menu                (QWidget      * widget ,
                                      QPointF        pos    ,
                                      ControlPoint & From   ,
                                      ControlPoint & To   ) ;

    virtual bool Relocation          (QRectF viewport) ;

  protected:

    virtual void Configure           (void) ;

    virtual bool FocusIn             (void) ;
    virtual bool ItemRunner          (int type,ThreadData * data) ;

  private:

  public slots:

    virtual void SelectNothing       (void) ;
    virtual void SelectAll           (void) ;

    virtual int  Indexing            (RangeBar * rbu,int direction,QVariants & v) ;

  protected slots:

    virtual void PeopleMenu          (MenuManager & mm,QMenu   * ms,SelectItem * item) ;
    virtual void ProfileMenu         (MenuManager & mm,QMenu   * ms,SelectItem * item) ;
    virtual void FeaturesMenu        (MenuManager & mm,QMenu   * ms,SelectItem * item) ;
    virtual void InternetMenu        (MenuManager & mm,QMenu   * ms,SelectItem * item) ;
    virtual void StatisticsMenu      (MenuManager & mm,QMenu   * ms,SelectItem * item) ;
    virtual bool PeopleProcessor     (MenuManager & mm,QAction * aa,SelectItem * item) ;
    virtual bool ProfileProcessor    (MenuManager & mm,QAction * aa,SelectItem * item) ;
    virtual bool FeaturesProcessor   (MenuManager & mm,QAction * aa,SelectItem * item) ;
    virtual bool InternetProcessor   (MenuManager & mm,QAction * aa,SelectItem * item) ;
    virtual bool StatisticsProcessor (MenuManager & mm,QAction * aa,SelectItem * item) ;

  private slots:

  signals:

    void         ShowAll             (void) ;
    void         PeopleGallery       (SelectItem * item) ;
    void         PeopleAlbums        (SelectItem * item) ;
    void         PeopleSubgroup      (SelectItem * item) ;

} ;

}

QT_END_NAMESPACE

#endif
