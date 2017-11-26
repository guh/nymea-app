import QtQuick 2.4
import QtQuick.Controls 2.1
import Guh 1.0

Item {
    id: root

    property var model: null

    property color color: "grey"
    property string mode: "bezier" // "bezier" or "bars"

    Connections {
        target: model
        onCountChanged: canvas.requestPaint()
    }
    onModelChanged: canvas.requestPaint()

    Label {
        anchors.centerIn: parent
        width: parent.width - 2 * app.margins
        wrapMode: Text.WordWrap
        text: "Sorry, there isn't enough data to display a graph here yet!"
        visible: !root.model.busy && root.model.count <= 2
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: app.largeFont
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: model.busy
    }

    Canvas {
        id: canvas
        visible: root.model.count > 2

        anchors.fill: parent

        property int minTemp: {
            var lower = Math.floor(root.model.minimumValue - 2);
            var upper = Math.ceil(root.model.maximumValue + 2);
            print("upper", upper, "lower", lower)
            if (!lower || !upper) {
                return 0
            }

            while ((upper - lower) % 10 != 0) {
                lower -= 1;
                if ((upper - lower) % 10 != 0) {
                    upper += 1;
                }
            }
            return lower;
        }

        property int maxTemp: {
            var lower = Math.floor(root.model.minimumValue - 2);
            var upper = Math.ceil(root.model.maximumValue + 2);
            if (!lower || !upper) {
                return 0
            }
            while ((upper - lower) % 10 != 0) {
                lower -= 1;
                if ((upper - lower) % 10 != 0) {
                    upper += 1;
                }
            }
            return upper;
        }

        property int topMargins: app.margins
        property int bottomMargins: app.margins * 4
        property int leftMargins: app.margins * 3
        property int rightMargins: app.margins

        property color gridColor: "#d0d0d0"

        property int contentWidth: canvas.width - leftMargins - rightMargins
        property int contentHeight: canvas.height - topMargins - bottomMargins

        property int totalSections: Math.round((maxTemp - minTemp) / 10) * 10
        property int sections: {
            var tmp = totalSections;
            while (tmp >= 10) {
                tmp /= 2;
            }
            return tmp;
        }
        // Pixel per section
        property real pps: contentHeight / sections;

        onPaint: {

            print("painting canvas", totalSections, sections)
            var minTemp

            var ctx = canvas.getContext('2d');
            ctx.save();

            ctx.reset()

            ctx.translate(leftMargins, topMargins)

            ctx.globalAlpha = 1//canvas.alpha;
            //ctx.fillStyle = canvas.fillStyle;
            ctx.font = "" + app.smallFont + "px Ubuntu";

            paintGrid(ctx)
            enumerate(ctx)

            if (root.mode == "bezier") {
                paintGraph(ctx)
            } else {
                paintBars(ctx)
            }

            ctx.restore();

        }

        function paintGrid(ctx) {
            ctx.strokeStyle = canvas.gridColor;
            ctx.fillStyle = "black"
            ctx.lineWidth = 1;

            ctx.beginPath();
            ctx.rect(0, 0, contentWidth, contentHeight)
            ctx.stroke();
            ctx.closePath();

            // Horizontal lines
            var tempInterval = (maxTemp - minTemp) / sections;

            for (var i = 0; i <= sections; i++) {
                ctx.beginPath();
                ctx.lineWidth = 1;
                ctx.strokeStyle = canvas.gridColor
                ctx.moveTo(0, i * pps);
                ctx.lineTo(contentWidth, i * pps)
                ctx.stroke();
                ctx.closePath();

                ctx.beginPath();
                var label = maxTemp - (tempInterval * i).toFixed(0)
                var textSize = ctx.measureText(label)
                ctx.strokeStyle = "black"
                ctx.fillStyle = "black"
                ctx.lineWidth = 0;
                ctx.text(label, -textSize.width - app.margins, i * pps + 5)
//                ctx.stroke();
                ctx.fill()
                ctx.closePath()
            }

            ctx.beginPath();
            ctx.strokeStyle = "black"
            ctx.fillStyle = "black"
            ctx.lineWidth = 0;
            var label = "°C"
            var textSize = ctx.measureText(label)
            ctx.text(label, -textSize.width - 1, -1 * pps + 5)
//            ctx.stroke();
            ctx.fill()
            ctx.closePath()

        }

        function enumerate(ctx) {
            // enumate x axis
            ctx.beginPath();
            ctx.globalAlpha = 1;
            ctx.strokeStyle = "black"
            ctx.fillStyle = "black"
            ctx.lineWidth = 0;
            // enumerate Y axis

            var lastTextX = -1;
            for (var i = 0; i < model.count; i++) {
                var x = contentWidth / (model.count) * i;
                if (x < lastTextX) continue;

                var label = model.get(i).dayString
                var textSize = ctx.measureText(label)
                ctx.text(label.slice(0,2).concat("."), x, contentHeight + app.smallFont + app.margins / 2)

                switch (model.average) {
                case ValueLogsProxyModel.AverageQuarterHour:
                case ValueLogsProxyModel.AverageHourly:
                case ValueLogsProxyModel.AverageDayTime:
                    label = model.get(i).timeString
                    break;
                default:
                    label = model.get(i).dateString
                }

                textSize = ctx.measureText(label)
                ctx.text(label, x, contentHeight + app.smallFont * 2 + app.margins)
                lastTextX = x + textSize.width;
            }

//            ctx.stroke();
            ctx.fill()
            ctx.closePath();
        }

        function paintGraph(ctx) {
            if (model.count <= 1) {
                return;
            }

            var tempInterval = (maxTemp - minTemp) / sections;

            ctx.beginPath();
            ctx.globalAlpha = 1;
            ctx.lineWidth = 2;
            var graphStroke = root.color;
            var grapFill = Qt.rgba(root.color.r, root.color.g, root.color.b, .4);

            ctx.strokeStyle = graphStroke;
            ctx.fillStyle = grapFill;

            var points = new Array();
            for (var i = 0; i < model.count; i++) {
                var value = model.get(i).value;
                var point = new Object();
//                print("painting value", value)
                point.x = (i == 0) ? 0 : (contentWidth / (model.count - 2) * i);
                point.y = contentHeight - (value - minTemp) / tempInterval * pps;
                points.push(point);
            }

            paintBezier(ctx, points);
            ctx.stroke();
            ctx.closePath();

            ctx.beginPath();
            paintBezier(ctx, points)
            ctx.lineTo(contentWidth, contentHeight);
            ctx.lineTo(0, contentHeight);
            ctx.fill();
            ctx.closePath();


            ctx.beginPath();
            ctx.globalAlpha = 1;
            ctx.lineWidth = 2;
            ctx.strokeStyle = "green"
            ctx.fillStyle = "green"

            points = new Array();
            for (var i = 0; i < model.count; i++) {
                var dayMaxTemp = model.get(i).maxTemp;
                var point = new Object();
                point.x = (i == 0) ? 0 : (contentWidth / (model.count - 1) * i);
                point.y = - (dayMaxTemp - maxTemp) / tempInterval * pps;
                points.push(point);
            }

            paintBezier(ctx, points);

            ctx.stroke();
            ctx.closePath();
        }

        function paintBezier(ctx, points) {

            if (points.length == 2) {
                ctx.moveTo(points[0].x, points[0].y)
                ctx.lineTo(points[1].x, points[1].y)
            } else {
                var n = points.length - 1;
                points[0].rhsx = points[0].x + 2 * points[1].x;
                points[0].rhsy = points[0].y + 2 * points[1].y;
                for (var i = 1; i < n - 1; i++) {
                    points[i].rhsx = 4 * points[i].x + 2 * points[i+1].x;
                    points[i].rhsy = 4 * points[i].y + 2 * points[i+1].y;
                }
                points[n - 1].rhsx = (8 * points[n - 1].x + points[n].x) / 2;
                points[n - 1].rhsy = (8 * points[n - 1].y + points[n].y) / 2;

                var b = 2.0;
                n = points.length - 1;
                points[0].firstcontrolx = points[0].rhsx / b;
                points[0].firstcontroly = points[0].rhsy / b;

                for (var i = 1; i < n; i++) {
                    points[i].tmp = 1 / b;
                    b = (i < n - 1 ? 4.0 : 3.5) - points[i].tmp;
                    points[i].firstcontrolx = (points[i].rhsx - points[i - 1].firstcontrolx) / b;
                    points[i].firstcontroly = (points[i].rhsy - points[i - 1].firstcontroly) / b;
                }
                for (var i = 1; i < n; i++) {
                    points[n - i - 1].firstcontrolx -= points[n - i].tmp * points[n - i].firstcontrolx;
                    points[n - i - 1].firstcontroly -= points[n - i].tmp * points[n - i].firstcontroly;
                }

                n = points.length - 1;
                for (var i = 0; i < n; i++) {
                    points[i].secondcontrolx = 2 * points[i + 1].x - points[i + 1].firstcontrolx;
                    points[i].secondcontroly = 2 * points[i + 1].y - points[i + 1].firstcontroly;
                }
                points[n - 1].secondcontrolx = (points[n].x + points[n - 1].firstcontrolx) / 2;
                points[n - 1].secondcontroly = (points[n].x + points[n - 1].firstcontroly) / 2;

                ctx.moveTo(points[0].x, points[0].y);
                for (var i = 0; i < n - 1; i++) {
//                    ctx.lineTo(points[i].firstcontrolx, points[i].firstcontroly)
//                    ctx.lineTo(points[i].secondcontrolx, points[i].secondcontroly)
//                    ctx.lineTo(points[i+1].x, points[i+1].y)

                    ctx.bezierCurveTo(points[i].firstcontrolx, points[i].firstcontroly,
                                      points[i].secondcontrolx, points[i].secondcontroly,
                                      points[i + 1].x, points[i + 1].y)
                }
            }
        }

        function paintBars(ctx) {
            if (model.count <= 1) {
                return;
            }

            var tempInterval = (maxTemp - minTemp) / sections;

            ctx.globalAlpha = 1;
            ctx.lineWidth = 2;
            var graphStroke = root.color;
            var grapFill = Qt.rgba(root.color.r, root.color.g, root.color.b, .2);

            ctx.strokeStyle = graphStroke;
            ctx.fillStyle = grapFill;


            for (var i = 0; i < model.count; i++) {
                ctx.beginPath();
                var value = model.get(i).value;
                var x = contentWidth / (model.count) * i;
                var y = contentHeight - (value - minTemp) / tempInterval * pps;

                var slotWidth = contentWidth / model.count
                ctx.rect(x,y, slotWidth - 5, contentHeight - y)
                ctx.fillRect(x,y, slotWidth - 5, contentHeight - y);
                ctx.stroke();
                ctx.fill();
                ctx.closePath();
            }
        }

        function hourToX(hour) {
            var entries = root.day.count;
            return canvas.contentWidth / entries * hour
        }
    }
}
