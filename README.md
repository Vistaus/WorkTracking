# WorkTracking

This is a simple application for managing several working times with different locations (office / home).

The main features are:

- unlimited, individual work items per day
- distinguish betweeh mobile (home) and offfice work
- start / stop recording time spent for a specific item just by click
- full control over spent times: ability to correct wrong recordings afterwards
- managing a default list for new days

Extended features:

- automatic tracking of breaks during recording (more than 5 minutes)
- export of spent times per day / week / month / year into clipboard (EXCel format)
- ability of an optional automatic addition of break times in export if work exceeds a certain limit

- alerting with acoustical if mouse movement detected without active item
- alerting with acoustical signal given when recorded time exceeds a given limit
- recognizing mouse inactivity while item active: asking user what to do


[see complete documentation][1]

This application is based upon the QT 5.12 toolkit or above in its LGPL version.
Please refer to https://doc.qt.io/qt-5/licensing.html

This project is still in use but works quite well. So actually no need for changes!

--------------------------------------------

## Version History

### Version 2.1.7 (15.07.2022)

#### Fixed problems
- fixing several problems with new features


--------------------------------------------

### Version 2.1.6 (13.06.2022)

#### New features
+ adding visual alert function: yellow colored an red colored with given limits
  alert values specified within INI file
+ adding stop of recording after given time of inactivity
  inactivita period specified within INI file

#### Fixed problems
- now adjusting window position if it was stored outside current screen resolution


--------------------------------------------

### Version 2.1.5 (15.02.2022)

#### New features
+ now exporting break after either given threshold time or half of the recorded time if no recorded break found and automatic enabled for day
  (was fixed to always using 4 hours)

#### Fixed problems
- 


--------------------------------------------

### Version 2.1.4 (27.12.2021)

#### New features
+ adding -p option for changing data file path
+ adding ini file entry <> for data file path
+ now preselecting days for weekly report only when current work > 50% of total work

#### Fixed problems
- fixed crash problem with weekly export when no rest is detected automatically but day is excluded by mask
- fixing problem in month / week view when more than one year is stored


--------------------------------------------

## Version History

### Version 2.1.3 (20.11.2021)

#### New features

#### Fixed problems
- fixed persistency of language setting
- fixed non unique date format across menu and weekly work overview (was missing translation)


-------------------------
### Version 2.1.2 (9.10.2021)

#### New features
+ improved usability: using minimum of clicks
+ automatic disabling other recordings when a new recording is started
+ resetting current kind of work when idle for more than 5 minutes

#### Fixed problems
- also changing kind of work for corrections if current kind is changed
- fixing menu for current kind of work: resetting selection after 5 minutes idle


-------------------------
### Version 2.1.0 (2.10.2021)

#### New features
+ kind of work now unset by default
+ recording asks for kind orf work if not set
+ user may change recorded break times
+ adding day mask for weekly export: ignoring automatic addition of break

#### Fixed problems
- storing program settings in INI file instead of registry


-------------------------
### Version 2.0.5 (15.09.2021)

#### New features
+ recording recognizes and tracks breaks
+ export function now adding break times
+ new column in export for automazically added break times

#### Fixed problems
- moved selection for kind of work into navigation menu for faster access


-------------------------
### Version 2.0.1 (14.9.2021)

#### New features
+ restoring window position / size from settings

#### Fixed problems
- fixing problems with different font sizes windows / Mac


-------------------------
### Version 2.0.0 (30.8.2021)

#### New features
+ adding help button showing PDF

#### Fixed problems
- export function recognizes start of recording on an item. The earliest time per day is taken


-------------------------
### Version 1.5.4 (28.7.2021)

#### New features
+ better handling of time correction: more intelligent tumbler setings

#### Fixed problems
- fixing problem with underflow when correction times below 0


-------------------------
### Version 1.1.2 (31.5.2021)

#### New features
+ now able to use default items list per day
+ introducing new UI for generic account handling
+ adding tooltip language support for german, french


-------------------------
### Version 1.1.1 (26.2.2021)

#### New features
- enhancing UI for kind of work: preparing for future etensions of kind
- adding tooltips

#### Fixed problems
- changing file format to contain tags, several kind of work


-------------------------
### Version 1.0.0 (9.2.2021)

#### New features
+ first working version for recording data


  [1]: ressources/workTracking.pdf
