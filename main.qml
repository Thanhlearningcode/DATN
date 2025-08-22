import QtQuick 6.0
import QtQuick.Controls 6.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Vibration Analyzer"

    // Main layout container
    Column {
        spacing: 20
        anchors.centerIn: parent

        // COM port and plot options
        Row {
            spacing: 20
            Text {
                text: "COM:"
            }
            ComboBox {
                id: comPortCombo
                model: ["ReCOM", "COM1", "COM2", "COM3"] // Add actual COM ports dynamically
            }
            Text {
                text: "Plot Top:"
            }
            ComboBox {
                id: plotTopCombo
                model: ["Ch1", "Ch2"]
            }
        }

        // Placeholder for plot area (use widgets/QCustomPlot in the Widgets build)
        Rectangle {
            width: parent.width * 0.9
            height: 300
            color: "#ffffff"
            border.color: "#cccccc"
            Text {
                anchors.centerIn: parent
                text: "Plot area (use Widgets/QCustomPlot)"
                color: "#666666"
            }
        }
    }
}
