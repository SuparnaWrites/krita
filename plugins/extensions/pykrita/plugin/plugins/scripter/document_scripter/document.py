from PyQt5.QtCore import QFile, QIODevice, QTextStream


class Document(object):

    def __init__(self, filePath):
        self._document = []
        self._filePath = filePath

    def open(self, filePath=''):
        if filePath:
            self._filePath = filePath

        with open(self._filePath, 'r') as pythonFile:
            self._document = pythonFile.read()

    def save(self):
        with open(self._filePath, 'w') as pythonFile:
            print(self._document, file=pythonFile)

    def compare(self, new_doc):
        if len(self._document) != len(new_doc):
            return False

        if new_doc != self._document:
            return False

        return True

    @property
    def data(self):
        return self._document

    @data.setter
    def data(self, data):
        self._document = data

    @property
    def filePath(self):
        return self._filePath
