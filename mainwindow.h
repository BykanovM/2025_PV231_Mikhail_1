#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QString>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void browseSourceDirectory();
    void browseTargetDirectory();
    void copySelectedFiles();

private:
    QListWidget* fileListWidget;
    QPushButton* btnSourceDir;
    QPushButton* btnTargetDir;
    QPushButton* btnCopyFiles;

    QString sourceDir;
    QString targetDir;

    void loadFilesToList(const QString& dir);
    QString generateUniqueFilename(const QString& path);
};