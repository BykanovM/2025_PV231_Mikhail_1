#include "mainwindow.h"
#include "smart_ptr.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    btnSourceDir = new QPushButton("Выбрать исходную папку");
    btnTargetDir = new QPushButton("Выбрать папку назначения");
    btnCopyFiles = new QPushButton("Копировать выбранные файлы");

    fileListWidget = new QListWidget;
    fileListWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    layout->addWidget(btnSourceDir);
    layout->addWidget(btnTargetDir);
    layout->addWidget(fileListWidget);
    layout->addWidget(btnCopyFiles);

    setCentralWidget(central);

    connect(btnSourceDir, &QPushButton::clicked, this, &MainWindow::browseSourceDirectory);
    connect(btnTargetDir, &QPushButton::clicked, this, &MainWindow::browseTargetDirectory);
    connect(btnCopyFiles, &QPushButton::clicked, this, &MainWindow::copySelectedFiles);
}

void MainWindow::browseSourceDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, "Исходная папка");
    if (!dir.isEmpty()) {
        sourceDir = dir;
        loadFilesToList(dir);
    }
}

void MainWindow::browseTargetDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, "Папка назначения");
    if (!dir.isEmpty()) {
        targetDir = dir;
    }
}

void MainWindow::loadFilesToList(const QString& dir) {
    fileListWidget->clear();
    QDir directory(dir);
    QFileInfoList files = directory.entryInfoList(QDir::Files);
    for (const QFileInfo& file : files) {
        fileListWidget->addItem(file.fileName());
    }
}

QString MainWindow::generateUniqueFilename(const QString& path) {
    QFileInfo info(path);
    QString base = info.completeBaseName();
    QString ext = info.completeSuffix();
    QString dir = info.path();
    int i = 1;
    QString newName;
    do {
        newName = QString("%1/%2 (%3)%4")
            .arg(dir)
            .arg(base)
            .arg(i++)
            .arg(ext.isEmpty() ? "" : "." + ext);
    } while (QFile::exists(newName));
    return newName;
}

void MainWindow::copySelectedFiles() {
    if (sourceDir.isEmpty() || targetDir.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Укажите обе директории.");
        return;
    }

    QList<QListWidgetItem*> selectedItems = fileListWidget->selectedItems();
    for (QListWidgetItem* item : selectedItems) {
        QString fileName = item->text();
        QString srcFile = sourceDir + "/" + fileName;
        QString dstFile = targetDir + "/" + fileName;

        if (QFile::exists(dstFile)) {
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "Файл уже существует",
                QString("Файл \"%1\" уже существует.\nПерезаписать?").arg(fileName),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

            if (reply == QMessageBox::Yes) {
                QFile::remove(dstFile);
            }
            else if (reply == QMessageBox::No) {
                continue;
            }
            else if (reply == QMessageBox::Cancel) {
                dstFile = generateUniqueFilename(dstFile);
            }
        }

        smart_ptr<QFile> src(new QFile(srcFile));
        smart_ptr<QFile> dst(new QFile(dstFile));

        if (!src->open(QIODevice::ReadOnly) || !dst->open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файлы: " + fileName);
            continue;
        }

        while (!src->atEnd()) {
            QByteArray data = src->read(8192);
            if (dst->write(data) != data.size()) {
                QMessageBox::warning(this, "Ошибка", "Ошибка записи файла: " + fileName);
                break;
            }
        }
    }

    QMessageBox::information(this, "Готово", "Файлы скопированы.");
}