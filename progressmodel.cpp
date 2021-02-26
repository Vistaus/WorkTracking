#include "progressmodel.h"

QQmlEngine *m_qmlEngine;

ProgressEntry::ProgressEntry()
{
  QDateTime timestamp = m_timeStamp;
  timestamp.time().setHMS(0,0,0);
  m_timeStamp = timestamp;
}

ProgressEntry::ProgressEntry(int id, QDateTime timestamp, QString name, QString description, bool active, int currentAccount, QVector<quint64> workInSeconds)
  : ProgressEntry()
{
  m_id = id;
  m_timeStamp = timestamp;
  m_name = name;
  m_description = description;
  m_active = active;
  m_account = currentAccount;
  m_workInSeconds = workInSeconds;
  if( m_workInSeconds.size()<2 )
    m_workInSeconds.push_back(0);
}

ProgressEntry::ProgressEntry(int itemId, const QString &fromString)
  : ProgressEntry()
{
  int workIndex = 0;

  QStringList properties = fromString.split(";");
  if( properties[0].contains("=") )
  {
    for( const auto &item : qAsConst(properties) )
    {
      if( item.contains("itmid=") )
        m_id = (itemId==-1 ? item.midRef(6).toInt() : itemId);
      if( item.contains("title=") )
        m_name = item.mid(6);
      if( item.contains("descr=") )
        m_description = item.mid(6);
      if( item.contains("creat=") )
        m_timeStamp = QDateTime::fromSecsSinceEpoch(item.mid(6).toLongLong());;
      if( item.contains("spent=") )
      {
        if( workIndex==m_workInSeconds.size() )
          m_workInSeconds.push_back(item.midRef(6).toULongLong());
        else
          m_workInSeconds[workIndex] = item.midRef(6).toULongLong();
        workIndex++;
      }
    }
  }
  else
  {
    QDateTime time = QDateTime::fromSecsSinceEpoch(properties[3].toLongLong());
    m_id = (itemId==-1 ? properties[0].toInt() : itemId),
        m_timeStamp = time;
    m_name = properties[1];
    m_description = properties[2];
    m_active = false;
    m_workInSeconds[0] = properties[4].toULongLong();
  }
}

QString ProgressEntry::toString() const
{
  return    "itmid=" +    QString::number(m_id) + ";"
          + "title=" +                    m_name + ";"
          + "descr=" +                    m_description + ";"
          + "creat=" +    QString::number(m_timeStamp.toSecsSinceEpoch()) + ";"
          + "spent=" +    QString::number(m_workInSeconds[0]) + ";"
          + "spent=" +    QString::number(m_workInSeconds[1]);// + ";"
           //"" +                   (m_doneHome ? "h" : "o");
}

int ProgressEntry::getId() const
{
  return m_id;
}

QDateTime ProgressEntry::getTimeStamp() const
{
  return m_timeStamp;
}

void ProgressEntry::setTimeStamp(const QDateTime &time)
{
  m_timeStamp = time;
}

QString ProgressEntry::getItemName() const
{
  return m_name;
}

void ProgressEntry::setItemName(const QString &name)
{
  m_name = name;
}

QString ProgressEntry::getItemDescription() const
{
  return m_description;
}

void ProgressEntry::setItemDescription(const QString &descr)
{
  m_description = descr;
}

bool ProgressEntry::getItemActive() const
{
  return m_active;
}

void ProgressEntry::setItemActive(const bool &active)
{
  m_active = active;
}

int ProgressEntry::getItemCurrentAccount() const
{
  return m_account;
}

void ProgressEntry::setItemCurrentAccount(const int &account)
{
  m_account = account;
}

QVector<quint64> ProgressEntry::getAllWorkInSeconds() const
{
  return m_workInSeconds;
}

void ProgressEntry::addAllWorkInSeconds(const QVector<quint64> &work)
{
  for( int i=0; i<work.size(); i++ )
    m_workInSeconds[i] += work[i];
}

quint64 ProgressEntry::getWorkInSeconds(const int &typeOfWork) const
{
  if (typeOfWork<m_workInSeconds.size())
    return m_workInSeconds[typeOfWork];
  else
    return 0;
}

void ProgressEntry::setWorkInSeconds(const int &typeOfWork,const quint64 &work)
{
  while (m_workInSeconds.size()<typeOfWork) m_workInSeconds.push_back(0);
  m_workInSeconds[typeOfWork] = work;
}

void ProgressEntry::addWorkInSeconds(const int &typeOfWork,const quint64 &work)
{
  while (m_workInSeconds.size()<typeOfWork) m_workInSeconds.push_back(0);
  m_workInSeconds[typeOfWork] += work;
}

ProgressModel::ProgressModel(QObject *parent) : QObject(parent)
{
  saveData("workingOnProjects", true);

  connect(this,SIGNAL(languageChanged()),m_qmlEngine,SLOT(retranslate()));

  QSettings settings("VISolutions","project-tracker");
  if( settings.contains("language-id") )
    changeLanguage(settings.value("language-id").toInt());

  m_actualDate = QDateTime::currentDateTime();
  //m_operatingMode = DisplayRecordDay;
  //m_isChangeable = true;

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(workingTimer()));
  timer->start(1000);

  bool todayListIsEmpty = true;
  QFile file("workingOnProjects.csv");
  file.open(QFile::ReadOnly);
  QTextStream s(&file);
  while( !s.atEnd() )
  {
    m_progressEntries <<  ProgressEntry(-1,s.readLine());
    m_nextId = m_progressEntries.last().getId();
    if( m_progressEntries.last().getTimeStamp().date()==m_actualDate.date() )
      todayListIsEmpty = false;
  }
  m_nextId++;

  if( todayListIsEmpty )
  {
    m_progressEntries << ProgressEntry(m_nextId++,"itmid=0;title=Wideband");
    m_progressEntries << ProgressEntry(m_nextId++,"itmid=0;title=NRPxP");
    m_progressEntries << ProgressEntry(m_nextId++,"itmid=0;title=NRX Pflege");
    m_progressEntries << ProgressEntry(m_nextId++,"itmid=0;title=NRP Pflege");
    m_progressEntries << ProgressEntry(m_nextId++,"itmid=0;title=Audio");
    m_progressEntries << ProgressEntry(m_nextId++,"itmid=0;title=NRPM");
    m_progressEntries << ProgressEntry(m_nextId++,"itmid=0;title=Allgemein");
  }
  updateItemsList();
}

ProgressModel::~ProgressModel()
{
  if( m_dataChanged )
    saveData("workingOnProjects", false);
}

QDateTime ProgressModel::actualDate() const
{
  return m_actualDate;
}

void ProgressModel::setActualDate(const QDateTime &date)
{
  m_actualDate = date;
}

ProgressModel::OperatingMode ProgressModel::mode()
{
  return m_operatingMode;
}

void ProgressModel::setMode(const ProgressModel::OperatingMode &mode)
{
  m_operatingMode = mode;

  updateItemsList();
  emit titleChanged();
  emit totalTimeChanged();
}

void ProgressModel::exportToClipboard(const QString &additionalMinutes,const QString &thresholdHours)
{
  // this will ensure that m_totalWorkSeconds is correct
  updateItemsList();


  QClipboard *clipboard = QGuiApplication::clipboard();
  QString data;

  for( const auto item : qAsConst(m_progressItems) )
  {
    if( m_showSummariesInPercent || m_operatingMode!=DisplayWeek )
    {
      ProgressEntry summary;
      summary.addWorkInSeconds(0,item->workInSeconds());
      data += item->projectName() + "\t" + getSummaryText(summary,m_totalWorkSeconds,true) + "\n";
    }
    else
    {
      quint64 timespent = item->workInSeconds();
      if( timespent>(thresholdHours.toULong()*3600) )
        timespent += additionalMinutes.toULong()*60;

      timespent += 150; // round in range of 5 minutes
      timespent = (timespent / 300) * 300;

      QDateTime startDateTime = QDateTime::currentDateTime();
      QTime startTime = startDateTime.time();
      startTime.setHMS(8,0,0);
      QTime endTime = startTime.addSecs(timespent);

      data += item->projectName() + "\t" + "08:00" + "\t" + endTime.toString("hh:mm") + "\n";
    }
  }

  clipboard->setText(data);
}

QString ProgressModel::humanReadableMonth(int month)
{
  const QString s_months[] = {
    tr("January"),tr("February"),tr("March"),tr("April"),tr("May"),tr("June"),
    tr("July"),tr("August"),tr("September"),tr("October"),tr("November"),tr("December")
  };
  return s_months[month];
}
QString ProgressModel::humanReadableWeekDay(int weekday)
{
  const QString s_days[] = {
    tr("Monday"),tr("Tuesday"),tr("Wednesday"),tr("Thursday"),tr("Friday"),tr("Saturday"),tr("Sunday")
  };
  return s_days[weekday];
}

QString ProgressModel::title()
{
  switch( m_operatingMode )
  {
  case DisplayYear:
    return QString::number(m_actualDate.date().year());
    break;
  case DisplayMonth:
    return humanReadableMonth(m_actualDate.date().month()-1);
    break;
  case DisplayWeek:
    return tr("week") +" " + QString::number(m_actualDate.date().weekNumber()) + " " + tr("of year");
    break;
  case DisplayRecordDay:
    return humanReadableWeekDay(m_actualDate.date().dayOfWeek()-1) + m_actualDate.toString(" dd-MM-yyyy");
    break;
  }

  return "";
}

bool ProgressModel::isChangeable() const
{
  return m_operatingMode==DisplayRecordDay;
}

bool ProgressModel::alwaysShowWork() const
{
  return m_alwaysShowWork;
}

void ProgressModel::setAlwaysShowWork(const bool &alwaysWork)
{
  m_alwaysShowWork = alwaysWork;
  updateItemsList();
  emit alwaysShowWorkChanged();
}

bool ProgressModel::showHomeWorkOnly() const
{
  return m_showHomeWorkOnly;
}

void ProgressModel::setShowHomeWorkOnly(const bool &homeWorkOnly)
{
  m_showHomeWorkOnly = homeWorkOnly;
  updateItemsList();
  emit showHomeWorkOnlyChanged();
  emit totalTimeChanged();
}

QQmlListProperty<ProgressItem> ProgressModel::itemList()
{
  return QQmlListProperty<ProgressItem>(this, m_progressItems);
}

bool ProgressModel::showSumInPercent() const
{
  return m_showSummariesInPercent;
}

void ProgressModel::changeLanguage(int language)
{
  static QTranslator translator;
  bool loaded = false;

  QCoreApplication::removeTranslator(&translator);

  // 0 - english, just remove translation

  // 1 - german
  if( language==1 )
    loaded = translator.load(":/translations/workTracking_de.qm");

  // 2 - french
  if( language==2 )
    loaded = translator.load(":/translations/workTracking_fr.qm");

  if ( loaded )
    QCoreApplication::installTranslator(&translator);
}

QString ProgressModel::totalTime() const
{
  ProgressEntry summary;
  summary.setItemActive(false);
  summary.setWorkInSeconds(0,0);
  switch( m_operatingMode )
  {
  case DisplayYear:
  case DisplayMonth:
  case DisplayWeek:
    summary.setWorkInSeconds(0,m_totalWorkSeconds[0]);
    summary.setWorkInSeconds(1,m_totalWorkSeconds[1]);
    break;
  case DisplayRecordDay:
    for( int i=0; i<m_progressEntries.size(); i++ )
    {
      for( int j=0; j<m_progressItems.size(); j++ )
      {
        if( m_progressItems.at(j)->getId()==m_progressEntries[i].getId() )
          summary.addAllWorkInSeconds(m_progressEntries[i].getAllWorkInSeconds());
      }
    }
    break;
  }

  return getSummaryText(summary,QVector<quint64>({0,0}));
}

void ProgressModel::itemStateChanged()
{
  ProgressItem *item = static_cast<ProgressItem*>(sender());
  for( int i=0; i<m_progressEntries.size(); i++ )
  {
    if( m_progressEntries[i].getId()==item->getId() )
    {
      m_dataChanged = true;
      m_progressEntries[i].setItemActive(item->isActive());
      item->setSummary(getSummaryText(m_progressEntries[i],m_totalWorkSeconds));
    }
  }
}

void ProgressModel::itemNameChanged()
{
  ProgressItem *item = static_cast<ProgressItem*>(sender());
  for( int i=0; i<m_progressEntries.size(); i++ )
  {
    m_dataChanged = true;
    if( m_progressEntries[i].getId()==item->getId() )
      m_progressEntries[i].setItemName(item->projectName());
  }
}

void ProgressModel::itemDescriptionChanged()
{
  ProgressItem *item = static_cast<ProgressItem*>(sender());
  for( int i=0; i<m_progressEntries.size(); i++ )
  {
    m_dataChanged = true;
    if( m_progressEntries[i].getId()==item->getId() )
      m_progressEntries[i].setItemDescription(item->description());
  }
}

void ProgressModel::itemAccountChanged()
{
  ProgressItem *item = static_cast<ProgressItem*>(sender());
  for( int i=0; i<m_progressEntries.size(); i++ )
  {
    if( m_progressEntries[i].getId()==item->getId() )
      m_progressEntries[i].setItemCurrentAccount(item->selectedAccount());
  }
}

void ProgressModel::workingTimer()
{
  for( int i=0; i<m_progressEntries.size(); i++ )
  {
    if( m_progressEntries[i].getItemActive() )
    {
      m_dataChanged = true;
      for( int j=0; j<m_progressItems.size(); j++ )
      {
        if( m_progressItems.at(j)->getId()==m_progressEntries[i].getId() )
        {
          m_progressEntries[i].addWorkInSeconds(m_progressItems.at(j)->selectedAccount(),1);
          m_progressItems.at(j)->setSummary(getSummaryText(m_progressEntries[i],m_totalWorkSeconds));
        }
      }
    }
  }

  m_runningSeconds++;
  if( (m_runningSeconds % 300) == 0)
  {
    if( m_dataChanged )
    {
      saveData("workingOnProjects", false);
      m_dataChanged = false;
    }
  }

  emit totalTimeChanged();
}

void ProgressModel::previous()
{
  switch( m_operatingMode )
  {
  case DisplayYear:
    m_actualDate = m_actualDate.addYears(-1);
    break;
  case DisplayMonth:
    m_actualDate = m_actualDate.addMonths(-1);
    break;
  case DisplayWeek:
    m_actualDate = m_actualDate.addDays(-7);
    break;
  case DisplayRecordDay:
    m_actualDate = m_actualDate.addDays(-1);
    break;
  }

  updateItemsList();
  emit totalTimeChanged();
  emit actualDateChanged();
  emit titleChanged();
}

void ProgressModel::next()
{
  switch( m_operatingMode )
  {
  case DisplayYear:
    m_actualDate = m_actualDate.addYears(1);
    break;
  case DisplayMonth:
    m_actualDate = m_actualDate.addMonths(1);
    break;
  case DisplayWeek:
    m_actualDate = m_actualDate.addDays(7);
    break;
  case DisplayRecordDay:
    m_actualDate = m_actualDate.addDays(1);
    break;
  }

  updateItemsList();
  emit actualDateChanged();
  emit titleChanged();
  emit totalTimeChanged();
}

void ProgressModel::changeSummary()
{
  m_showSummariesInPercent = !m_showSummariesInPercent;
  updateItemsList();
  emit showSumInPercentChanged();
}

QString ProgressModel::getItemTitle(ProgressItem *item)
{
  return item->projectName();
}

void ProgressModel::append(const QString &name, const QString &description, const QDateTime &timeSpent)
{
  QDateTime newTimeStamp = m_actualDate;
  newTimeStamp.setTime(timeSpent.time());

  ProgressEntry item {m_nextId++,newTimeStamp,name,description,true,0,{0,0}};

  item.setWorkInSeconds(0,timeSpent.time().hour()*3600 + timeSpent.time().minute()*60);
  m_progressEntries << item;

  updateItemsList();
  emit totalTimeChanged();
}

void ProgressModel::remove(const int &index)
{
  int id = m_progressItems.at(index)->getId();

  QList<ProgressEntry>::iterator item = m_progressEntries.begin();
  while( item!=m_progressEntries.end() )
  {
    if( item->getId()==id )
      item = m_progressEntries.erase(item);
    else
      ++item;
  }

  updateItemsList();
  emit totalTimeChanged();
}

void ProgressModel::addSeconds(const int &index, const int &diff)
{
  qDebug("addseconds");
  int id = m_progressItems.at(index)->getId();

  QList<ProgressEntry>::iterator item = m_progressEntries.begin();
  while( item!=m_progressEntries.end() )
  {
    if( item->getId()==id )
    {
      m_dataChanged = true;
      item->addWorkInSeconds(m_progressItems.at(index)->selectedAccount(),diff);
      m_progressItems.at(index)->setSummary(getSummaryText(*item,QVector<quint64>({0,0})));
    }
    ++item;
  }

  emit totalTimeChanged();
}

void ProgressModel::setLanguage(const int &lang)
{
  QSettings settings("VISolutions","project-tracker");

  changeLanguage(lang);
  settings.setValue("language-id",lang);

  updateItemsList();
  emit languageChanged();
}

void ProgressModel::setQmlEngine(QQmlApplicationEngine &engine)
{
  m_qmlEngine = &engine;
}

void ProgressModel::updateItemsList()
{
  m_progressItems.clear();

  QMap<QString,ProgressEntry> projectsMap;
  QMap<int,ProgressEntry> monthsMap;
  QMap<int,ProgressEntry> weekDayMap;
  QList<ProgressEntry> dayList;

  m_totalWorkSeconds.clear();
  m_totalWorkSeconds.push_back(0);
  m_totalWorkSeconds.push_back(0);
  for( const auto &item : qAsConst(m_progressEntries) )
  {
    switch( m_operatingMode )
    {
    case DisplayYear:
      if( item.getTimeStamp().date().year()!=m_actualDate.date().year() )
        continue;
      if( m_alwaysShowWork )
      {
        if( !projectsMap.contains(item.getItemName()) )
          projectsMap[item.getItemName()] = ProgressEntry{0,item.getTimeStamp(), item.getItemName(),item.getItemDescription(),false,0,{0,0}};
        projectsMap[item.getItemName()].addAllWorkInSeconds(item.getAllWorkInSeconds());
      }
      else
      {
        if( !monthsMap.contains(item.getTimeStamp().date().month()) )
          monthsMap[item.getTimeStamp().date().month()] = ProgressEntry{0,item.getTimeStamp(), item.getItemName(),item.getItemDescription(),false,0,{0,0}};
        monthsMap[item.getTimeStamp().date().month()].addAllWorkInSeconds(item.getAllWorkInSeconds());
      }
      break;
    case DisplayMonth:
      if( item.getTimeStamp().date().month()!=m_actualDate.date().month() )
        continue;
      if( !projectsMap.contains(item.getItemName()) )
        projectsMap[item.getItemName()] = ProgressEntry{0,item.getTimeStamp(), item.getItemName(),item.getItemDescription(),false,0,{0,0}};
      projectsMap[item.getItemName()].addAllWorkInSeconds(item.getAllWorkInSeconds());
      break;
    case DisplayWeek:
      if( item.getTimeStamp().date().weekNumber()!=m_actualDate.date().weekNumber())
        continue;
      if( m_alwaysShowWork )
      {
        if( !projectsMap.contains(item.getItemName()) )
          projectsMap[item.getItemName()] = ProgressEntry{0,item.getTimeStamp(), item.getItemName(),item.getItemDescription(),false,0,{0,0}};
        projectsMap[item.getItemName()].addAllWorkInSeconds(item.getAllWorkInSeconds());
      }
      else
      {
        if( !weekDayMap.contains(item.getTimeStamp().date().dayOfWeek()) )
          weekDayMap[item.getTimeStamp().date().dayOfWeek()] = ProgressEntry{0,item.getTimeStamp(), item.getItemName(),item.getItemDescription(),false,0,{0,0}};
        weekDayMap[item.getTimeStamp().date().dayOfWeek()].addAllWorkInSeconds(item.getAllWorkInSeconds());
      }
      break;
    case DisplayRecordDay:
      if( item.getTimeStamp().date()!=m_actualDate.date() )
        continue;
      dayList << ProgressEntry(item.getId(),item.getTimeStamp(), item.getItemName(),item.getItemDescription(),item.getItemActive(),item.getItemCurrentAccount(),item.getAllWorkInSeconds());
      break;
    }
    m_totalWorkSeconds[0] += item.getWorkInSeconds(0);
    m_totalWorkSeconds[1] += item.getWorkInSeconds(1);
  }

  if( projectsMap.size()>0 )
  {
    QList<QString> keys = projectsMap.keys();
    for( const auto &key : qAsConst(keys) )
    {
      ProgressEntry &item = projectsMap[key];
      ProgressItem *entry = new ProgressItem();
      entry->setId(item.getId());
      entry->setProjectName(key);// item.m_projectName;
      //entry->m_description = item.m_description;
      //entry->m_isActive = item.m_active;
      //entry->m_timeStamp = item.m_timeStamp;
      entry->setWorkInSeconds(getSummaryWorkInSeconds(item));
      entry->setSummary(getSummaryText(item,m_totalWorkSeconds));

      //entry->connect(entry,SIGNAL(isActiveChanged()),this,SLOT(itemStateChanged()));
      entry->connect(entry,SIGNAL(projectNameChanged()),this,SLOT(itemNameChanged()));
      entry->connect(entry,SIGNAL(descriptionChanged()),this,SLOT(itemDescriptionChanged()));
      m_progressItems << entry;
    }
  }
  if( monthsMap.size()>0 )
  {
    for( int month=0; month<12; month++ )
    {
      if( monthsMap.contains(month+1) )
      {
        ProgressEntry &item = monthsMap[month+1];

        ProgressItem *entry = new ProgressItem();
        entry->setId(item.getId());
        entry->setProjectName(humanReadableMonth(month));
        //entry->m_description = "";
        //entry->m_isActive = false;
        entry->setTimeStamp(item.getTimeStamp());
        entry->setWorkInSeconds(getSummaryWorkInSeconds(item));
        entry->setSummary(getSummaryText(item,m_totalWorkSeconds));

        m_progressItems << entry;
      }
    }
  }
  if( weekDayMap.size()>0 )
  {
    for( int day=0; day<7; day++ )
    {
      if( weekDayMap.contains(day+1) )
      {
        ProgressEntry &item = weekDayMap[day+1];

        ProgressItem *entry = new ProgressItem();
        entry->setId(item.getId());
        entry->setProjectName(humanReadableWeekDay(day));
        //entry->m_description = "";
        //entry->m_isActive = false;
        entry->setTimeStamp(item.getTimeStamp());
        entry->setWorkInSeconds(getSummaryWorkInSeconds(item));
        entry->setSummary(getSummaryText(item,m_totalWorkSeconds));

        m_progressItems << entry;
      }
    }
  }
  if( dayList.size()>0 )
  {
    for( const auto &item : qAsConst(dayList) )
    {
      ProgressItem *entry = new ProgressItem();
      entry->setId(item.getId());
      entry->setProjectName(item.getItemName());
      entry->setDescription(item.getItemDescription());
      entry->setIsActive(item.getItemActive());
      entry->setSelectedAccount(item.getItemCurrentAccount());
      entry->setTimeStamp(item.getTimeStamp());
      entry->setWorkInSeconds(getSummaryWorkInSeconds(item));
      entry->setSummary(getSummaryText(item,m_totalWorkSeconds));

      entry->connect(entry,SIGNAL(isActiveChanged()),this,SLOT(itemStateChanged()));
      entry->connect(entry,SIGNAL(projectNameChanged()),this,SLOT(itemNameChanged()));
      entry->connect(entry,SIGNAL(descriptionChanged()),this,SLOT(itemDescriptionChanged()));
      entry->connect(entry,SIGNAL(selectedAccountChanged()),this,SLOT(itemAccountChanged()));
      //entry->connect(entry,SIGNAL(workInSecondsChanged()),this,SLOT(itemWorkInSecondsChanged()));
      m_progressItems << entry;
    }
  }

  emit itemListChanged();
}

quint64 ProgressModel::getSummaryWorkInSeconds(const ProgressEntry &entry) const
{
  quint64 value = 0;
  if( m_showHomeWorkOnly )
    value = entry.getWorkInSeconds(0);
  else
    value = entry.getWorkInSeconds(0) + entry.getWorkInSeconds(1);

  return value;
}

QString ProgressModel::getSummaryText(const ProgressEntry &entry, QVector<quint64> totalWorkInSeconds, bool clipboardFormat) const
{
  double percent = 0.0;
  QString infotext;
  bool percentEnabled = totalWorkInSeconds[0]>0 || totalWorkInSeconds[1]>0;

  quint64 value = getSummaryWorkInSeconds(entry);

  if( percentEnabled )
    percent = (double)value*100.0/(double)(totalWorkInSeconds[0]+totalWorkInSeconds[1]); // \todo selection of menu?
  bool showinPercent = m_showSummariesInPercent && (percentEnabled);

  int seconds = value % 60; value /= 60;
  int minutes = value % 60; value /= 60;
  int hours = value;

  switch( m_operatingMode )
  {
  case DisplayYear:
    //break;
  case DisplayMonth:
    infotext = showinPercent ? QString::asprintf(clipboardFormat ? "%1.3f" : "%.0f %%",percent) : QString::asprintf("%4d.%02d h",hours,minutes*10/6);
    break;
  case DisplayWeek:
    infotext = showinPercent ? QString::asprintf(clipboardFormat ? "%1.3f" : "%.1f %%",percent) : QString::asprintf("%02d:%02d",hours,minutes);
    break;
  case DisplayRecordDay:
    if( entry.getItemActive() )
      infotext = QString::asprintf("%02d:%02d:%02d",hours,minutes,seconds);
    else
      infotext = QString::asprintf("%02d:%02d",hours,minutes);
    break;
  }

  return infotext.replace(".",",");
}

void ProgressModel::saveData(const QString &filename, bool createBackup)
{
  QString dataFile = filename+".csv";
  QString dataBackup = filename+"-backup.csv";

  if( createBackup )
  {
    QFileInfo fi(dataBackup);
    if( !fi.exists() || fi.lastModified().daysTo(QDateTime::currentDateTime())>=1 )
    {
      QFile file(dataBackup);
      if( !file.rename(filename+"-"+QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".csv"))
        ;
      QFile file2(dataFile);
      if( !file2.copy(dataBackup) )
        ;
    }
  }
  else
  {
    QFile file(dataFile);
    file.open(QFile::WriteOnly);
    QTextStream s(&file);
    for( const auto &entry : qAsConst(m_progressEntries) )
    {
      s << entry.toString() << "\n";
    }
  }
}

ProgressItem::ProgressItem()
{
}

ProgressItem::~ProgressItem()
{
}

int ProgressItem::getId() const
{
  return m_id;
}

void ProgressItem::setId(const int &id)
{
  m_id = id;
}

QString ProgressItem::projectName() const
{
  return m_projectName;
}

void ProgressItem::setProjectName(const QString &name)
{
  m_projectName = name;
  emit projectNameChanged();
}

QString ProgressItem::description() const
{
  return m_description;
}

void ProgressItem::setDescription(const QString &description)
{
  m_description = description;
  emit descriptionChanged();
}

bool ProgressItem::isActive() const
{
  return m_isActive;
}

void ProgressItem::setIsActive(const bool &active)
{
  m_isActive = active;
  emit isActiveChanged();
}

int ProgressItem::selectedAccount() const
{
  return m_selectedAccount;
}

void ProgressItem::setSelectedAccount(const int &home)
{
  m_selectedAccount = home;
  emit selectedAccountChanged();
}

QDateTime ProgressItem::timeStamp() const
{
  return m_timeStamp;
}

void ProgressItem::setTimeStamp(const QDateTime &time)
{
  m_timeStamp = time;
  emit timeStampChanged();
}

qint64 ProgressItem::workInSeconds() const
{
  return m_workInSeconds;
}

void ProgressItem::setWorkInSeconds(const qint64 seconds)
{
  if( seconds>=0 )
    m_workInSeconds = seconds;
  else
    m_workInSeconds = 0;
  emit workInSecondsChanged();
}

QString ProgressItem::summary() const
{
  return m_summaryText;
}

void ProgressItem::setSummary(const QString &text)
{
  m_summaryText = text;
  emit summaryChanged();
}
