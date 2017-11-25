/* ************************************************************************ */
/* Copyright 2017 Jiří Fatka. All rights reserved.                          */
/* ************************************************************************ */

#include "SettingsDialog.hpp"
#include "ui_SettingsDialog.h"

// Qt
#include <QFileDialog>

/* ************************************************************************ */

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

/* ************************************************************************ */

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

/* ************************************************************************ */

QString SettingsDialog::objdumpPath() const
{
    return ui->lineEditObjdump->text();
}

/* ************************************************************************ */

void SettingsDialog::setObjdumpPath(const QString& path)
{
    ui->lineEditObjdump->setText(path);
}

/* ************************************************************************ */

void SettingsDialog::on_toolButtonObjdump_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);

    if (filename.isEmpty())
        return;

    setObjdumpPath(filename);
}

/* ************************************************************************ */
