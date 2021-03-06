import QtQuick 2.6
import QtQuick.Layouts 1.3
import JAGCS 1.0

import "qrc:/Controls" as Controls
import "../../Video"

Controls.Frame {
    id: root

    property int number: 0
    property int type: VideoSource.UnknownType
    property string source
    property bool changed: false

    signal save()
    signal restore()
    signal remove()
    signal setupPreview(QtObject preview)

    implicitHeight: grid.height + palette.margins * 2

    GridLayout {
        id: grid
        anchors.centerIn: parent
        width: parent.width - palette.margins * 2
        columns: 4
        rowSpacing: palette.spacing
        columnSpacing: palette.spacing

        Controls.Label {
            text: qsTr("Video") + " " + number
            Layout.columnSpan: 3
            Layout.fillWidth: true
        }

        Controls.Button {
            tipText: qsTr("Save")
            iconSource: "qrc:/icons/save.svg"
            onClicked: save()
            enabled: changed
        }

        Controls.Label {
            text: qsTr("Type")
            Layout.fillWidth: true
        }

        Controls.Label {
            id: typeLabel
            text: {
                switch (type) {
                case VideoSource.Stream: return qsTr("Stream video")
                case VideoSource.Device: return qsTr("Device")
                default: return qsTr("Unknown:")
                }
            }
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Controls.Button {
            tipText: qsTr("Restore")
            iconSource: "qrc:/icons/restore.svg"
            onClicked: restore()
            enabled: changed
        }

        Controls.Label {
            text: {
                switch (type) {
                case VideoSource.Stream: return qsTr("Stream URL:")
                case VideoSource.Device: return qsTr("Device file:")
                default: return qsTr("Unknown:")
                }
            }
            Layout.fillWidth: true
        }

        Controls.TextField {
            visible: type == VideoSource.Stream
            text: source
            placeholderText: qsTr("Enter stream url")
            onTextChanged: {
                source = text;
                changed = true;
            }
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Controls.ComboBox {
            visible: type == VideoSource.Device
            model: videoDevices
            currentIndex: videoDevices.indexOf(source);
            onCurrentTextChanged: {
                source = currentText;
                changed = true;
            }
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Controls.DelayButton {
            tipText: qsTr("Remove")
            iconSource: "qrc:/icons/remove.svg"
            onActivated: remove()
            iconColor: palette.dangerColor
        }

        Item {
            Layout.fillWidth: true
            Layout.columnSpan: preview.visible ? 1 : 3
        }

        VideoView {
            id: preview
            visible: false
            onVisibleChanged: visible ? setupPreview(preview) : setupPreview(null)
            Layout.fillWidth: true
            Layout.preferredHeight: width * ratio
            Layout.columnSpan: 2
        }

        Controls.Button {
            tipText: qsTr("Preview")
            iconSource: preview.visible ? "qrc:/icons/hide.svg" : "qrc:/icons/show.svg"
            onClicked: preview.visible = !preview.visible
            Layout.alignment: Qt.AlignTop
        }
    }
}
