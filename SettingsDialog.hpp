/* ************************************************************************ */
/* Copyright 2017 Jiří Fatka. All rights reserved.                          */
/* ************************************************************************ */

#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

/* ************************************************************************ */

// Qt
#include <QDialog>
#include <QString>

/* ************************************************************************ */

namespace Ui {
class SettingsDialog;
}

/* ************************************************************************ */

/// Settings dialog
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();

    QString objdumpPath() const;
    void setObjdumpPath(const QString &path);

private slots:
    void on_toolButtonObjdump_clicked();

private:

    Ui::SettingsDialog* ui;
};

/* ************************************************************************ */

#endif // SETTINGSDIALOG_HPP

/* ************************************************************************ */
