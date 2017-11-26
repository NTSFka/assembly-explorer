/* ************************************************************************ */
/* Copyright 2017 Jiří Fatka. All rights reserved.                          */
/* ************************************************************************ */

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

/* ************************************************************************ */

#include <QMainWindow>
#include <QStandardItemModel>

/* ************************************************************************ */

namespace Ui {
class MainWindow;
}

/* ************************************************************************ */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void showFunction(const QModelIndex &index);

private slots:
    void on_actionOpen_triggered();

    void on_actionAbout_triggered();

    void on_treeViewStructure_pressed(const QModelIndex &index);

    void on_actionSettings_triggered();

    void on_treeViewStructure_activated(const QModelIndex &index);

private:

    /// Read settings
    void readSettings();

    /// Write settings
    void writeSettings();

    /// Process output into model
    void processResult(QIODevice& device);

private:
    Ui::MainWindow *ui;
    QStandardItemModel m_structureModel;
    QStandardItemModel m_functionModel;
    QString m_objdumpPath;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

/* ************************************************************************ */

#endif // MAINWINDOW_HPP

/* ************************************************************************ */
