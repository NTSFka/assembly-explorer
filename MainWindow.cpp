/* ************************************************************************ */
/* Copyright 2017 Jiří Fatka. All rights reserved.                          */
/* ************************************************************************ */

#include "MainWindow.hpp"
#include "ui_MainWindow.h"

/* ************************************************************************* */

// Qt
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QRegularExpression>
#include <QScopedPointerDeleteLater>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QFileInfo>
#include <QFontDatabase>
#include <QSettings>

// App
#include "SettingsDialog.hpp"

/* ************************************************************************* */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readSettings();

    ui->progressBar->hide();

    // Set model
    ui->treeViewStructure->setModel(&m_structureModel);
    ui->tableViewCode->setModel(&m_functionModel);

    ui->tableViewCode->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

/* ************************************************************************* */

MainWindow::~MainWindow()
{
    delete ui;
}

/* ************************************************************************* */

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this);

    if (filename.isEmpty())
        return;

    QProcess* process = new QProcess(this);

    connect(process, &QProcess::started, process, [this] {
        ui->progressBar->show();
        ui->progressBar->setValue(0);
        ui->progressBar->setMaximum(0);
        ui->actionOpen->setDisabled(true);

        m_structureModel.clear();
        m_functionModel.clear();
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), process, [this, process] (int code, QProcess::ExitStatus status) {
        QScopedPointer<QProcess, QScopedPointerDeleteLater> ptr(process);
        Q_UNUSED(ptr);

        if (status != QProcess::NormalExit)
        {
            QMessageBox::critical(this, tr("Failure"), tr("Symbol obtaining failed: %1").arg(process->errorString()));
            ui->progressBar->hide();
            ui->actionOpen->setEnabled(true);
            return;
        }

        if (code != 0)
        {
            QMessageBox::critical(this, tr("Failure"), tr("Symbol obtaining failed: %1").arg(QString(process->readAllStandardError())));
            ui->progressBar->hide();
            ui->actionOpen->setEnabled(true);
            return;
        }

        processResult(*process);

        ui->progressBar->hide();
        ui->actionOpen->setEnabled(true);
    });

    process->start(m_objdumpPath, {"-d", "-S", "-C", filename});
}

/* ************************************************************************* */

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, QStringLiteral("Assembly Explorer"), QStringLiteral("Copyright 2017 Jiří Fatka"));
}

/* ************************************************************************* */

void MainWindow::readSettings()
{
    QSettings settings;
    restoreGeometry(settings.value(QLatin1Literal("MainWindow/geometry")).toByteArray());
    restoreState(settings.value(QLatin1Literal("MainWindow/state")).toByteArray());

    m_objdumpPath = settings.value(QLatin1Literal("objdump-path"), QLatin1Literal("objdump")).toString();
}

/* ************************************************************************* */

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue(QLatin1Literal("MainWindow/geometry"), saveGeometry());
    settings.setValue(QLatin1Literal("MainWindow/state"), saveState());

    settings.setValue(QLatin1Literal("objdump-path"), m_objdumpPath);
}

/* ************************************************************************* */

void MainWindow::processResult(QIODevice &device)
{
    QTextStream is(&device);

    static QRegularExpression sectionRegex("^Disassembly of section\\s+(.*):$"); // Disassembly of section .text:
    static QRegularExpression functionRegex("^([0-9a-fA-F]+)\\s+<(.*)>:$");
    static QRegularExpression lineRegex("^\\s*([0-9a-fA-F]+):\\s+([0-9a-fA-F ]+)(\t(.*))?$");

    QStandardItem* sectionItem = nullptr;
    QStandardItem* functionItem = nullptr;
    QVariantList functionBody;

    while (!is.atEnd())
    {
        const QString line = is.readLine();

        const auto sectionMatch = sectionRegex.match(line);
        const auto functionMatch = functionRegex.match(line);

        if (sectionMatch.hasMatch())
        {
            // Store last section
            if (sectionItem)
            {
                // Store last function
                if (functionItem)
                {
                    functionItem->setData(functionBody, Qt::UserRole + 2);
                    sectionItem->appendRow(functionItem);
                    functionItem = nullptr;
                }

                m_structureModel.appendRow(sectionItem);
            }

            // Section name
            QString section = sectionMatch.captured(1);

            // Create section item
            sectionItem = new QStandardItem(section);
        }
        else if (sectionItem && functionMatch.hasMatch())
        {
            // Store last function
            if (functionItem)
            {
                functionItem->setData(functionBody, Qt::UserRole + 2);
                sectionItem->appendRow(functionItem);
            }

            // Function address
            qlonglong functionAddress = functionMatch.captured(1).toLongLong(nullptr, 16);
            QString functionName = functionMatch.captured(2);

            functionItem = new QStandardItem(functionName);
            functionItem->setData(functionAddress, Qt::UserRole + 1);
            functionBody.clear();
        }
        else if (functionItem)
        {
            auto assemblyMatch = lineRegex.match(line);

            if (!assemblyMatch.hasMatch())
            {
                functionBody.append(line);
                continue;
            }

            qlonglong address = assemblyMatch.captured(1).toLongLong(nullptr, 16);
            QString binary = assemblyMatch.captured(2);

            QVariantList data;
            data.append(address);
            data.append(binary);

            if (assemblyMatch.lastCapturedIndex() >= 4)
            {
                QString assembly = assemblyMatch.captured(4);
                data.append(assembly);
            }

            functionBody.append(QVariant(data));
        }
    }

    if (functionItem)
    {
        functionItem->setData(functionBody, Qt::UserRole + 2);
        sectionItem->appendRow(functionItem);
    }

    if (sectionItem)
        m_structureModel.appendRow(sectionItem);
}

/* ************************************************************************* */

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    QMainWindow::closeEvent(event);
}

/* ************************************************************************* */

void MainWindow::on_treeViewStructure_pressed(const QModelIndex &index)
{
    const auto lines = index.data(Qt::UserRole + 2).toList();

    m_functionModel.clear();

    for (const QVariant& line : lines)
    {
        QList<QStandardItem*> item;

        if (line.canConvert<QVariantList>())
        {
            const QVariantList parts = line.toList();
            const qlonglong address = parts.at(0).toLongLong();
            //const QString binary = parts.at(1).toString();
            const QString assembly = parts.size() > 2 ? parts.at(2).toString() : QString();

            auto* addressItem = new QStandardItem(QString::number(address, 16).rightJustified(8, '0'));
            addressItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

            item.append(addressItem);
            item.append(new QStandardItem(assembly));
        }
        else if (line.canConvert<QString>())
        {
            QString code = line.toString();

            // Only line
            item.append(new QStandardItem());
            item.append(new QStandardItem(code));
        }

        m_functionModel.appendRow(item);
    }
}

/* ************************************************************************* */

void MainWindow::on_actionSettings_triggered()
{
    QSettings settings;

    SettingsDialog dlg(this);
    dlg.setObjdumpPath(m_objdumpPath);

    if (dlg.exec() == QDialog::Accepted)
    {
        m_objdumpPath = dlg.objdumpPath();
    }
}

/* ************************************************************************* */
