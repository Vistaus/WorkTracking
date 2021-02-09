/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11
import QtQuick.Window 2.11

import ProjectsData 1.0

Dialog {
    id: workDialog
    title: qsTr("Add new Work")
    modal: true
    //standardButtons: DialogButtonBox.OK | DialogButtonBox.Cancel

    property ProgressModel workDataModel

    function formatNumber(number) {
        return number < 10 && number >= 0 ? "0" + number : number.toString()
    }

    onOpened: {
        projectTitle.text = ""
        hoursTumbler.currentIndex = 0
        minutesTumbler.currentIndex = 0
    }
    onAccepted: {
        var newDate = new Date(Date.now())
        newDate.setHours(hoursTumbler.currentIndex,minutesTumbler.currentIndex,0)
        workDataModel.append(projectTitle.text,"",newDate)
    }
    onRejected: workDialog.close()

    contentItem: ColumnLayout {
        RowLayout {
            id: description

            Label {
                id: gfhgfhs
                text: qsTr("title")
            }

            TextField {
                id: projectTitle
                text: ""
            }
        }

        RowLayout {
            id: rowTumbler

            Label {
                text: qsTr("time spent")
            }
            Tumbler {
                id: hoursTumbler
                model: 24
                delegate: TumblerDelegate {
                    text: formatNumber(modelData)
                }
            }
            Tumbler {
                id: minutesTumbler
                model: 60
                delegate: TumblerDelegate {
                    text: formatNumber(modelData)
                }
            }
        }

        RowLayout {
            Button {
                text: qsTr("Cancel")
                onClicked: close()
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                text: qsTr("OK")
                onClicked: accept()
            }
        }
    }
}