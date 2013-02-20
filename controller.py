from PySide import QtGui


class Controller(QtGui.QWidget):

    def __init__(self):
        super(Controller, self).__init__()

        self.active = False

        self.layout = QtGui.QVBoxLayout()
        self.create_intervals_group()
        self.create_controls_group()
        self.setLayout(self.layout)

    def create_intervals_group(self):
        self.intervals = QtGui.QFormLayout()

        pb = QtGui.QPushButton('Add interval')
        pb.clicked.connect(self.add_interval)
        self.intervals.addRow(pb)

        self.add_interval()
        self.add_interval()

        group = QtGui.QGroupBox()
        group.setLayout(self.intervals)
        self.layout.addWidget(group)

    def create_controls_group(self):
        buttons = QtGui.QHBoxLayout()

        run = QtGui.QPushButton('Run')
        buttons.addWidget(run)

        stop = QtGui.QPushButton('Stop')
        stop.setDisabled(True)
        buttons.addWidget(stop)

        reset = QtGui.QPushButton('Reset')
        buttons.addWidget(reset)

        group = QtGui.QGroupBox()
        group.setLayout(buttons)
        self.layout.addWidget(group)
 
    def add_interval(self):
        sb = QtGui.QSpinBox()
        sb.setRange(0, 10800)
        sb.setSuffix(' s')
        label = ('A' if self.active else 'Ina') + 'ctive:'
        self.intervals.insertRow(self.intervals.rowCount()-1, label, sb)
        self.active = not self.active


if __name__ == '__main__':
    import sys
    app = QtGui.QApplication(sys.argv)
    controller = Controller()
    controller.show()
    sys.exit(app.exec_())
