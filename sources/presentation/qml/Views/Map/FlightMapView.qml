import QtQuick 2.6
import QtLocation 5.6
import QtPositioning 5.6

MapView {
    id: root

    property var vehicleModel

    signal setHome(var coordinate)

    property bool settingHome: false

    MapItemView {
        id: tracks
        model: vehicleModel
        delegate: MapPolyline {
            line.width: 4
            line.color: palette.trackColor
            path: track
            smooth: true
        }
    }

    MapItemView {
        id: homePositions
        model: vehicleModel
        delegate: MapQuickItem {
            anchorPoint.x: homeImage.width / 2
            anchorPoint.y: homeImage.height / 2
            coordinate: homePosition

            sourceItem: Image {
                id: homeImage
                source: "qrc:/indicators/home_mark.svg"
            }
        }
    }

    MapItemView {
        id: vehicles
        model: vehicleModel
        autoFitViewport: true
        delegate: MapQuickItem {
            anchorPoint.x: markItem.width / 2
            anchorPoint.y: markItem.height / 2
            coordinate: position

            sourceItem: Item {
                id: markItem
                width: markImage.sourceSize.width
                height: markImage.sourceSize.height

                Image {
                    id: markImage
                    anchors.centerIn: parent
                    rotation: direction
                    source: mark
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            if (settingHome) {
                setHome(root.toCoordinate(Qt.point(mouseX, mouseY), true));
                settingHome = false;
            }
        }
    }

    MapActions {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 7
    }
}
