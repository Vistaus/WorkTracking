#ifndef PROGRESSMODEL_H
#define PROGRESSMODEL_H

#include <QObject>
#include <QtQuick>

class ProgressItem;

class ProgressEntry
{
public:
  ProgressEntry() = default;
  ProgressEntry(int id, QDateTime timestamp, QString name, QString description, bool active, bool doneHome, quint64 workInSeconds);
  ProgressEntry(int m_id, QString const &fromString);
  QString toString() const;

  int getId() const;

  QDateTime getTimeStamp() const;
  void setTimeStamp(const QDateTime &);

  QString getItemName() const;
  void setItemName(const QString &);

  QString getItemDescription() const;
  void setItemDescription(const QString &);

  bool getItemActive() const;
  void setItemActive(const bool &);

  bool getItemDoneHome() const;
  void setItemDoneHome(const bool &);

  quint64 getWorkInSeconds() const;
  void setWorkInSeconds(const quint64 &);
  void addWorkInSeconds(const quint64 &);

private:
  int m_id;
  QDateTime m_timeStamp;
  QString m_name;
  QString m_description;
  bool    m_active;
  bool    m_doneHome;
  quint64 m_workInSeconds;
};

class ProgressModel : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QDateTime actualDate READ actualDate WRITE setActualDate NOTIFY actualDateChanged)
  Q_PROPERTY(OperatingMode mode READ mode NOTIFY modeChanged)

  Q_PROPERTY(QString title READ title NOTIFY titleChanged)
  Q_PROPERTY(bool isChangeable READ isChangeable NOTIFY isChangeableChanged)
  Q_PROPERTY(bool alwaysShowWork READ alwaysShowWork WRITE setAlwaysShowWork NOTIFY alwaysShowWorkChanged)
  Q_PROPERTY(bool showHomeWorkOnly READ showHomeWorkOnly WRITE setShowHomeWorkOnly NOTIFY showHomeWorkOnlyChanged)
  Q_PROPERTY(QQmlListProperty<ProgressItem> itemList READ itemList NOTIFY itemListChanged)

  Q_PROPERTY(bool showSumInPercent READ showSumInPercent NOTIFY showSumInPercentChanged)
  Q_PROPERTY(QString totalTime READ totalTime NOTIFY totalTimeChanged)

public:
  enum OperatingMode
  {
    DisplayYear,
    DisplayMonth,
    DisplayWeek,
    DisplayRecordDay,
  };
  Q_ENUM(OperatingMode)

  explicit ProgressModel(QObject *parent = nullptr);
  virtual ~ProgressModel();

  QDateTime actualDate() const;
  void setActualDate(const QDateTime &);

  OperatingMode mode();

  QString title();

  bool isChangeable() const;

  bool alwaysShowWork() const;
  void setAlwaysShowWork(const bool &);

  bool showHomeWorkOnly() const;
  void setShowHomeWorkOnly(const bool &);

  QQmlListProperty<ProgressItem> itemList();

  bool showSumInPercent() const;
  void changeLanguage(int language);

  QString totalTime() const;

  Q_SLOT void itemStateChanged();
  Q_SLOT void itemNameChanged();
  Q_SLOT void itemDescriptionChanged();
  Q_SLOT void itemDoneHomeChanged();
  Q_SLOT void itemWorkInSecondsChanged();
  Q_SLOT void workingTimer();

  Q_SIGNAL void actualDateChanged();
  Q_SIGNAL void modeChanged();

  Q_SIGNAL void titleChanged();
  Q_SIGNAL void isChangeableChanged();
  Q_SIGNAL void alwaysShowWorkChanged();
  Q_SIGNAL void showHomeWorkOnlyChanged();
  Q_SIGNAL void itemListChanged();
  Q_SIGNAL void showSumInPercentChanged();
  Q_SIGNAL void totalTimeChanged();

  Q_SIGNAL void languageChanged();

  Q_INVOKABLE void previous();
  Q_INVOKABLE void next();

  Q_INVOKABLE void setMode(const OperatingMode &);
  Q_INVOKABLE void exportToClipboard(const QString &,const QString &);
  Q_INVOKABLE void changeSummary();

  Q_INVOKABLE QString getItemTitle(ProgressItem *);

  Q_INVOKABLE void append(const QString &name, const QString &description, const QDateTime &timeSpent);
  Q_INVOKABLE void remove(const int &index);

  Q_INVOKABLE void setLanguage(const int &);

  static void setQmlEngine(QQmlApplicationEngine &engine);

private:
  void updateItemsList();
  QString humanReadableMonth(int month);
  QString humanReadableWeekDay(int weekday);

  QString getSummary(ProgressEntry const &entry, quint64 totalWorkInSeconds) const;

  void saveData(QString const &filename, bool createBackup = false);

  QDateTime m_actualDate;
  QList<ProgressEntry> m_progressEntries;
  QList<ProgressItem*> m_progressItems;

  int m_nextId                    = 12345678;

  OperatingMode m_operatingMode   = DisplayRecordDay;
  bool m_isChangeable             = true;
  bool m_alwaysShowWork           = false;
  bool m_showHomeWorkOnly         = false;

  quint64 m_totalWorkSeconds      = 0;
  bool m_showSummariesInPercent   = false;

  quint64 m_runningSeconds        = 0;
  bool m_dataChanged              = false;
};

class ProgressItem : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString projectName READ projectName WRITE setProjectName NOTIFY projectNameChanged)
  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
  Q_PROPERTY(bool isActive READ isActive WRITE setIsActive NOTIFY isActiveChanged)
  Q_PROPERTY(bool isDoneHome READ isDoneHome WRITE setIsDoneHome NOTIFY isDoneHomeChanged)
  Q_PROPERTY(QDateTime timeStamp READ timeStamp WRITE setTimeStamp NOTIFY timeStampChanged)
  Q_PROPERTY(qint64 workInSeconds READ workInSeconds WRITE setWorkInSeconds NOTIFY workInSecondsChanged)
  Q_PROPERTY(QString summary READ summary WRITE setSummary NOTIFY summaryChanged)

public:
  explicit ProgressItem();
  //explicit ProgressItem(const ProgressItem &);
  virtual ~ProgressItem();

  int getId() const;
  void setId(const int &);

  QString projectName() const;
  void setProjectName(const QString &);

  QString description() const;
  void setDescription(const QString &);

  bool isActive() const;
  void setIsActive(const bool &);

  bool isDoneHome() const;
  void setIsDoneHome(const bool &);

  QDateTime timeStamp() const;
  void setTimeStamp(const QDateTime &);

  qint64 workInSeconds() const;
  void setWorkInSeconds(const qint64 seconds);

  QString summary() const;
  void setSummary(const QString &);

  Q_SIGNAL void projectNameChanged();
  Q_SIGNAL void descriptionChanged();
  Q_SIGNAL void isActiveChanged();
  Q_SIGNAL void isDoneHomeChanged();
  Q_SIGNAL void timeStampChanged();
  Q_SIGNAL void workInSecondsChanged();
  Q_SIGNAL void summaryChanged();

private:
  int m_id                = 0;
  QString m_projectName   = "";
  QString m_description   = "";
  bool m_isActive         = false;
  bool m_doneAtHome       = true;
  QDateTime m_timeStamp   = QDateTime::currentDateTime();
  qint64 m_workInSeconds  = 0;
  QString m_summaryText   = 0;
};

#endif // PROGRESSMODEL_H