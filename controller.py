from PySide import QtGui


class Controller(QtGui.QWidget):

    def __init__(self):
        super(Controller, self).__init__()

        self.layout = QtGui.QVBoxLayout()
        self.setLayout(self.layout)

        intervals_group = QtGui.QGroupBox()
        self.intervals = QtGui.QFormLayout()
        intervals_group.setLayout(self.intervals)
        
        self.layout.addWidget(intervals_group)
        self.intervals.addWidget(QtGui.QPushButton('Add interval'))

        self.active = False

        self.add_interval()
        self.add_interval()
 
    def add_interval(self):
        spinbox = QtGui.QSpinBox()
        spinbox.setRange(0, 10800)
        spinbox.setSuffix(' s')
        label = ('A' if self.active else 'Ina') + 'ctive:'
        self.intervals.insertRow(self.intervals.rowCount()-1, label, spinbox)
        self.active = not self.active


if __name__ == '__main__':
    import sys
    app = QtGui.QApplication(sys.argv)
    controller = Controller()
    controller.show()
    sys.exit(app.exec_())
